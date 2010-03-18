
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/spin/manual_reset_event.hpp"

#include <boost/assert.hpp>
#include <boost/thread/thread.hpp>

#include <boost/fiber/utility.hpp>

namespace boost {
namespace fibers {
namespace spin {

manual_reset_event::manual_reset_event( bool isset) :
	state_( isset ? SET : RESET),
	waiters_( 0),
	enter_mtx_()
{}

void
manual_reset_event::set()
{
	enter_mtx_.lock();

	state expected = RESET;
	if ( ! state_.compare_exchange_strong( expected, SET) ||
			0 == waiters_.load() )
		enter_mtx_.unlock();
}

void
manual_reset_event::reset()
{
	mutex::scoped_lock lk( enter_mtx_);
	BOOST_ASSERT( lk);

	state_.store( RESET);
}

void
manual_reset_event::wait()
{
	{
		mutex::scoped_lock lk( enter_mtx_);
		BOOST_ASSERT( lk);
		waiters_.fetch_add( 1);
	}

	while ( RESET == state_.load() )
	{
		if ( this_fiber::runs_as_fiber() )
			this_fiber::yield();
		else
			this_thread::yield();
	}

	if ( 1 == waiters_.fetch_sub( 1) )
		enter_mtx_.unlock();
}

bool
manual_reset_event::try_wait()
{
	{
		mutex::scoped_lock lk( enter_mtx_);
		BOOST_ASSERT( lk);
		waiters_.fetch_add( 1);
	}

	bool result = SET == state_.load();

	if ( 1 == waiters_.fetch_sub( 1) )
		enter_mtx_.unlock();

	return result;
}

}}}
