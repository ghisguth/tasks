
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/spin/mutex.hpp>

#include <boost/thread/thread.hpp>

#include <boost/fiber/utility.hpp>

namespace boost {
namespace fibers {
namespace spin {

mutex::mutex() :
	state_( UNLOCKED)
{}

void
mutex::lock()
{
	for (;;)
	{
		state expected = UNLOCKED;
		if ( state_.compare_exchange_strong( expected, LOCKED) )
			break;
		if ( this_fiber::runs_as_fiber() )
			this_fiber::yield();
		else
			this_thread::yield();
	}
}

bool
mutex::try_lock()
{
	state expected = UNLOCKED;
	return state_.compare_exchange_strong( expected, LOCKED);
}

void
mutex::unlock()
{ state_.store( UNLOCKED); }

}}}
