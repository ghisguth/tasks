
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_SCHEDULER_H
#define BOOST_FIBERS_SCHEDULER_H

#include <cstddef>
#include <memory>

#include <boost/config.hpp>
#include <boost/preprocessor/repetition.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/move/move.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/round_robin.hpp>
#include <boost/fiber/strategy.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace fibers {

class auto_reset_event;
class condition;
class count_down_event;
class manual_reset_event;
class mutex;

template< typename Strategy = round_robin >
class scheduler : private noncopyable
{
private:
	friend class auto_reset_event;
	friend class condition;
	friend class count_down_event;
	friend class manual_reset_event;
	friend class mutex;

	intrusive_ptr< strategy >	strategy_;

public:
	scheduler() :
		strategy_( new Strategy() )
	{}

	~scheduler()
	{ strategy_->detach_all(); } 

	bool run()
	{ return strategy_->run(); }

	bool empty() const
	{ return strategy_->empty(); }

	std::size_t size() const
	{ return strategy_->size(); }

	std::size_t ready() const
	{ return strategy_->ready(); }

	void submit_fiber( fiber f)
	{ strategy_->add( f); }

	template< typename Fn >
	void make_fiber( Fn fn, std::size_t stack_size)
	{ strategy_->add( fiber( fn, stack_size) ); }

	template< typename Fn >
	void make_fiber( BOOST_RV_REF( Fn) fn, std::size_t stack_size)
	{ strategy_->add( fiber( fn, stack_size) ); }

#ifndef BOOST_FIBER_MAX_ARITY
#define BOOST_FIBER_MAX_ARITY 10
#endif

#define BOOST_FIBER_ARG(z, n, unused) \
   BOOST_PP_CAT(A, n) BOOST_PP_CAT(a, n)
#define BOOST_ENUM_FIBER_ARGS(n) BOOST_PP_ENUM(n, BOOST_FIBER_ARG, ~)

#define BOOST_FIBER_MAKE_FIBER_FUNCTION(z, n, unused) \
	template< typename Fn, BOOST_PP_ENUM_PARAMS(n, typename A) > \
	void make_fiber( Fn fn, BOOST_ENUM_FIBER_ARGS(n), std::size_t stack_size) \
	{ strategy_->add( fiber( fn, BOOST_PP_ENUM_PARAMS(n, a), stack_size) ); }

BOOST_PP_REPEAT_FROM_TO( 1, BOOST_FIBER_MAX_ARITY, BOOST_FIBER_MAKE_FIBER_FUNCTION, ~)

#undef BOOST_FIBER_MAKE_FIBER_FUNCTION
#undef BOOST_FIBER_MAX_ARITY

	void migrate_fiber( fiber f)
	{
		if ( ! f) throw fiber_moved();

		strategy * st( f.info_->st);
		if ( ! st) throw fiber_error("fiber not attached");
		strategy_->migrate( f.info_->st->release( f.get_id() ) );
	}

	template< typename OtherStrategy >
	void migrate_fiber( fiber::id const& id, scheduler< OtherStrategy > & other)
	{ strategy_->migrate( other.strategy_->release( id) ); }
};

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_SCHEDULER_H
