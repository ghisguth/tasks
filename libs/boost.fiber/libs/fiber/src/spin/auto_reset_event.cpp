
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/spin/auto_reset_event.hpp"

#include <boost/thread/thread.hpp>

#include <boost/fiber/utility.hpp>

namespace boost {
namespace fibers {
namespace spin {

auto_reset_event::auto_reset_event( bool isset) :
	state_( isset ? SET : RESET)
{}

void
auto_reset_event::set()
{ state_.store( SET); }

void
auto_reset_event::wait()
{
	state expected = SET;
	while ( ! state_.compare_exchange_strong( expected, RESET) )
	{
		if ( this_fiber::runs_as_fiber() )
			this_fiber::yield();
		else
			this_thread::yield();
		expected = SET;
	}
}

bool
auto_reset_event::try_wait()
{
	state expected = SET;
	return state_.compare_exchange_strong( expected, RESET);
}

}}}
