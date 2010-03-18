
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//  based on boost::interprocess::sync::interprocess_condition

#ifndef BOOST_FIBERS_SPIN_CONDITION_H
#define BOOST_FIBERS_SPIN_CONDITION_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/atomic.hpp>
#include <boost/config.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/spin/mutex.hpp>
#include <boost/fiber/utility.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

# if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable:4355 4251 4275)
# endif

namespace boost {
namespace fibers {
namespace spin {

class BOOST_FIBER_DECL condition : private noncopyable
{
private:
	enum command
	{
		SLEEPING = 0,
		NOTIFY_ONE,
		NOTIFY_ALL
	};

	atomic< command >		cmd_;
	atomic< std::size_t >	waiters_;
	mutex					enter_mtx_;
	mutex					check_mtx_;

	void notify_( command);

public:
	condition();

	void notify_one();

	void notify_all();

	void wait( unique_lock< mutex > & lk)
	{
		if ( ! lk)
			throw lock_error();
		wait( * lk.mutex() );
	}

	template< typename Pred >
	void wait( unique_lock< mutex > & lk, Pred pred)
	{
		if ( ! lk)
			throw lock_error();

		while ( ! pred() )
			wait( * lk.mutex() );
	}

	template< typename LockType >
	void wait( LockType & lt)
	{
		{
			mutex::scoped_lock lk( enter_mtx_);
			BOOST_ASSERT( lk);
			waiters_.fetch_add( 1);
			lt.unlock();
		}

		bool unlock_enter_mtx = false;
		for (;;)
		{
			while ( SLEEPING == cmd_.load() )
			{
				if ( this_fiber::runs_as_fiber() )
					this_fiber::yield();
				else
					this_thread::yield();
			}

			mutex::scoped_lock lk( check_mtx_);
			BOOST_ASSERT( lk);

			command expected = NOTIFY_ONE;
			cmd_.compare_exchange_strong( expected, SLEEPING);
			if ( SLEEPING == expected)
				continue;
			else if ( NOTIFY_ONE == expected)
			{
				unlock_enter_mtx = true;
				waiters_.fetch_sub( 1);
				break;
			}
			else
			{
				unlock_enter_mtx = 1 == waiters_.fetch_sub( 1);
				if ( unlock_enter_mtx)
				{
					expected = NOTIFY_ALL;
					cmd_.compare_exchange_strong( expected, SLEEPING);
				}
				break;
			}
		}

		if ( unlock_enter_mtx)
			enter_mtx_.unlock();

		lt.lock();
	}

	template<
		typename LockType,
		typename Pred
	>
	void wait( LockType & lt, Pred pred)
	{
		while ( ! pred() )
			wait( lt);
	}
};

}}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_SPIN_CONDITION_H
