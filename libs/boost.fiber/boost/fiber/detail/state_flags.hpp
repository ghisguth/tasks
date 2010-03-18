
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_DETAIL_STATE_FLAGS_H
#define BOOST_FIBERS_DETAIL_STATE_FLAGS_H

#include <boost/config.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace fibers {
namespace detail {

enum state_t_
{
	STATE_MASTER			= 1 << 0,
	STATE_NOT_STARTED		= 1 << 1,
	STATE_READY				= 1 << 2,
	STATE_RUNNING			= 1 << 3,
	STATE_WAIT_FOR_FIBER	= 1 << 4,
	STATE_WAIT_FOR_OBJECT	= 1 << 5,
	STATE_TERMINATED		= 1 << 6
};

typedef char state_type;

#define IS_ALIVE_BIT_MASK 0x3C

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_DETAIL_STATE_FLAGS_H
