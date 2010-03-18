
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/condition.hpp"

#include <boost/fiber/utility.hpp>

namespace boost {
namespace fibers {

condition::~condition()
{ strategy_->unregister_object( id_); }

void
condition::notify_one()
{
	enter_mtx_.lock();

	if ( 0 == waiters_)
	{
		enter_mtx_.unlock();
		return;
	}

	cmd_ = NOTIFY_ONE;

	strategy_->object_notify_one( id_);
}

void
condition::notify_all()
{
	enter_mtx_.lock();

	if ( 0 == waiters_)
	{
		enter_mtx_.unlock();
		return;
	}

	cmd_ = NOTIFY_ALL;

	strategy_->object_notify_all( id_);
}

}}
