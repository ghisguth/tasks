
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_AUTO_RESET_EVENT_H
#define BOOST_FIBERS_AUTO_RESET_EVENT_H

#include <boost/config.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/detail/config.hpp>
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

class BOOST_FIBER_DECL auto_reset_event : private noncopyable
{
private:
	enum state
	{
		SET = 0,
		RESET
	};

	state			state_;
	object::id		id_;
	strategy::ptr	strategy_;

public:
	template< typename Strategy >
	auto_reset_event( scheduler< Strategy > & sched, bool isset = false) :
		state_( isset ? SET : RESET),
		id_( * this),
		strategy_( sched.strategy_)
	{ strategy_->register_object( id_); }

	~auto_reset_event();

	void set();

	void wait();

	bool try_wait();
};

}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_AUTO_RESET_EVENT_H
