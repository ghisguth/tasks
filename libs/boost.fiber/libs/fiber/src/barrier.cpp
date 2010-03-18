
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber/barrier.hpp"

#include <boost/assert.hpp>

namespace boost {
namespace fibers {

bool
barrier::wait()
{
	mutex::scoped_lock lk( mtx_);
	bool cycle( cycle_);
	if ( 0 == --current_)
	{
		cycle_ = ! cycle_;
		current_ = initial_;
		cond_.notify_all();
		return true;
	}
	else
	{
		while ( cycle == cycle_)
			cond_.wait( lk);
	}
	return false;
}

}}
