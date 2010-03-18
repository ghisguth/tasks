
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/auto_reset_event.hpp"

namespace boost {
namespace fibers {

auto_reset_event::~auto_reset_event()
{ strategy_->unregister_object( id_); }

void
auto_reset_event::set()
{
	state_ = SET;
	strategy_->object_notify_one( id_);
}

void
auto_reset_event::wait()
{
	while ( RESET == state_)
		strategy_->wait_for_object( id_);
	state_ = RESET;
}

bool
auto_reset_event::try_wait()
{
	if ( SET == state_)
	{
		state_ = RESET;
		return true;
	}
	return false;
}

}}
