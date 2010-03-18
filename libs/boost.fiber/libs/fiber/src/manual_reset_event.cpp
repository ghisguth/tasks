
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/manual_reset_event.hpp"

#include <boost/assert.hpp>

namespace boost {
namespace fibers {

manual_reset_event::~manual_reset_event()
{ strategy_->unregister_object( id_); }

void
manual_reset_event::set()
{
	enter_mtx_.lock();

	if ( SET == state_)
	{
		enter_mtx_.unlock();
		return;
	}

	state_ = SET;

	if ( 0 == waiters_)
		enter_mtx_.unlock();
	else
		strategy_->object_notify_all( id_);
}

void
manual_reset_event::reset()
{
	mutex::scoped_lock	lk( enter_mtx_);
	BOOST_ASSERT( lk);

	state_ = RESET;
}

void
manual_reset_event::wait()
{
	{
		mutex::scoped_lock lk( enter_mtx_);
		BOOST_ASSERT( lk);
		++waiters_;
	}

	while ( RESET == state_)
		strategy_->wait_for_object( id_);

	if ( 0 == --waiters_)
		enter_mtx_.unlock();
}

bool
manual_reset_event::try_wait()
{
	{
		mutex::scoped_lock lk( enter_mtx_);
		BOOST_ASSERT( lk);
		++waiters_;
	}

	bool result = SET == state_;

	if ( 0 == --waiters_)
		enter_mtx_.unlock();

	return result;
}

}}
