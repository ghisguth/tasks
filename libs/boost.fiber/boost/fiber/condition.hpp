
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//  based on boost::interprocess::sync::interprocess_condition

#ifndef BOOST_FIBERS_CONDITION_H
#define BOOST_FIBERS_CONDITION_H

#include <cstddef>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/thread/locks.hpp>

#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/mutex.hpp>
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

class BOOST_FIBER_DECL condition : private noncopyable
{
private:
	enum command
	{
		SLEEPING = 0,
		NOTIFY_ONE,
		NOTIFY_ALL
	};

	command			cmd_;
	std::size_t		waiters_;
	mutex			enter_mtx_;
	mutex			check_mtx_;
	object::id		id_;
	strategy::ptr	strategy_;

public:
	template< typename Strategy >
	condition( scheduler< Strategy > & sched) :
		cmd_( SLEEPING),
		waiters_( 0),
		enter_mtx_( sched),
		check_mtx_( sched),
		id_( * this),
		strategy_( sched.strategy_)
	{ strategy_->register_object( id_); }

	~condition();

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
			++waiters_;
			lt.unlock();
		}

		bool unlock_enter_mtx = false;
		for (;;)
		{
			while ( SLEEPING == cmd_)
				strategy_->wait_for_object( id_);

			mutex::scoped_lock lk( check_mtx_);
			BOOST_ASSERT( lk);

			if ( SLEEPING == cmd_)
				continue;
			else if ( NOTIFY_ONE == cmd_)
			{
				cmd_ = SLEEPING;	
				unlock_enter_mtx = true;
				--waiters_;
				break;
			}
			else
			{
				unlock_enter_mtx = 0 == --waiters_;
				if ( unlock_enter_mtx)
					cmd_ = SLEEPING;
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

}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_CONDITION_H
