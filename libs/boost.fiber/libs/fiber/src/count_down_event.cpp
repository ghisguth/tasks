
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/count_down_event.hpp"

namespace boost {
namespace fibers {

count_down_event::~count_down_event()
{ strategy_->unregister_object( id_); }

std::size_t
count_down_event::initial() const
{ return initial_; }

std::size_t
count_down_event::current() const
{ return current_; }

bool
count_down_event::is_set() const
{ return 0 == current_; }

void
count_down_event::set()
{
	if ( 0 == current_) return;
	if ( 0 == --current_)
		strategy_->object_notify_all( id_);
}

void
count_down_event::wait()
{
	while ( 0 != current_)
		strategy_->wait_for_object( id_);
}

}}
