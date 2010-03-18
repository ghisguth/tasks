
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_BARRIER_H
#define BOOST_FIBERS_BARRIER_H

#include <cstddef>
#include <stdexcept>

#include <boost/config.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/condition.hpp>
#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/mutex.hpp>
#include <boost/fiber/scheduler.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

# if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable:4275)
# endif

namespace boost {
namespace fibers {

class BOOST_FIBER_DECL barrier : private noncopyable
{
private:
	std::size_t		initial_;
	std::size_t		current_;
	bool			cycle_;
	mutex			mtx_;
	condition		cond_;

public:
	template< typename Strategy >
	barrier( scheduler< Strategy > & sched, std::size_t initial) :
		initial_( initial),
		current_( initial_),
		cycle_( true),
		mtx_( sched),
		cond_( sched)
	{ if ( initial == 0) throw std::invalid_argument("invalid barrier count"); }

	bool wait();
};

}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_BARRIER_H
