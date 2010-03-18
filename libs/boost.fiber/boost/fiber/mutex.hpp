
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//  based on boost::interprocess::sync::interprocess_mutex

#ifndef BOOST_FIBERS_MUTEX_H
#define BOOST_FIBERS_MUTEX_H

#include <boost/config.hpp>
#include <boost/thread/locks.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/object/id.hpp>
#include <boost/fiber/scheduler.hpp>
#include <boost/fiber/strategy.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

# if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable:4355 4251 4275)
# endif

namespace boost {
namespace fibers {

class BOOST_FIBER_DECL mutex : private noncopyable
{
private:
	enum state
	{
		LOCKED = 0,
		UNLOCKED
	};

	state					state_;
	object::id				id_;
	strategy::ptr			strategy_;

public:
	typedef unique_lock< mutex >			scoped_lock;

	template< typename Strategy >
	mutex( scheduler< Strategy > & sched) :
		state_( UNLOCKED),
		id_( * this),
		strategy_( sched.strategy_)
	{ strategy_->register_object( id_); }

	~mutex();

	void lock();

	bool try_lock();

	void unlock();
};

typedef mutex try_mutex;

}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_MUTEX_H
