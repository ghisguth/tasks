
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/mutex.hpp>

#include <boost/fiber/utility.hpp>

namespace boost {
namespace fibers {

mutex::~mutex()
{ strategy_->unregister_object( id_); }

void
mutex::lock()
{
	while ( LOCKED == state_)
		strategy_->wait_for_object( id_);
	state_ = LOCKED;
}

bool
mutex::try_lock()
{
	if ( UNLOCKED == state_)
	{
		state_ = LOCKED;
		return true;
	}
	return false;
}

void
mutex::unlock()
{
	state_ = UNLOCKED;
	strategy_->object_notify_one( id_);
}

}}
