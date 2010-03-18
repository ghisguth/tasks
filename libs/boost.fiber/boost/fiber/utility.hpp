
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_THIS_FIBER_UTILITY_H
#define BOOST_THIS_FIBER_UTILITY_H

#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include <boost/move/move.hpp>

#include <boost/fiber/fiber.hpp>
#include <boost/fiber/strategy.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace this_fiber {

inline
bool runs_as_fiber()
{ return fibers::strategy::runs_as_fiber_(); }

inline
fiber::id get_id()
{ return fibers::strategy::get_id_(); }

inline
int priority()
{ return fibers::strategy::priority_(); }

inline
void priority( int prio)
{ fibers::strategy::priority_( prio); }

inline
void interruption_point()
{ fibers::strategy::interruption_point_(); }

inline
bool interruption_requested()
{ return fibers::strategy::interruption_requested_(); }

inline
bool interruption_enabled()
{ return fibers::strategy::interruption_enabled_(); }

inline
void at_fiber_exit( function< void() > ca)
{ fibers::strategy::at_fiber_exit_( ca); }

template< typename Callable >
void at_fiber_exit( Callable ca)
{ fibers::strategy::at_fiber_exit_( boost::bind( boost::type< void >(), ca) ); }

inline
void at_fiber_exit( void ( * ca)() )
{ fibers::strategy::at_fiber_exit_( boost::bind( boost::type< void >(), ca) ); }

inline
void yield()
{ fibers::strategy::yield_(); }

inline
void cancel()
{ fibers::strategy::cancel_(); }

inline
void submit_fiber( fiber f)
{ fibers::strategy::submit_fiber_( f); }

inline
void submit_fiber( BOOST_RV_REF( fiber) f)
{ fibers::strategy::submit_fiber_( f); }

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_THIS_FIBER_UTILITY_H
