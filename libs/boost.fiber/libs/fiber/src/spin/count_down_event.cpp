
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/spin/count_down_event.hpp"

#include <boost/thread/thread.hpp>

#include <boost/fiber/spin/mutex.hpp>
#include <boost/fiber/utility.hpp>

namespace boost {
namespace fibers {
namespace spin {

count_down_event::count_down_event( std::size_t initial) :
	initial_( initial),
	current_( initial_)
{}

std::size_t
count_down_event::initial() const
{ return initial_; }

std::size_t
count_down_event::current() const
{ return current_.load(); }

bool
count_down_event::is_set() const
{ return 0 == current_.load(); }

void
count_down_event::set()
{
	for (;;)
	{
		if ( 0 == current_.load() )
			return;
		std::size_t expected = current_.load();
		if ( current_.compare_exchange_strong( expected, expected - 1) )
			return;
	}
}

void
count_down_event::wait()
{
	while ( 0 != current_.load() )
	{
		if ( this_fiber::runs_as_fiber() )
			this_fiber::yield();
		else
			this_thread::yield();
	}
}

}}}
