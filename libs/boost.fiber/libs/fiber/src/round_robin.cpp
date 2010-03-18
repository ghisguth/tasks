
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/round_robin.hpp>

#include <memory>
#include <utility>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/detail/move.hpp>

#include <boost/fiber/exceptions.hpp>

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace fibers {

round_robin::round_robin() :
	fibers_(),
	objects_(),
	runnable_fibers_(),
	terminated_fibers_()
{}

void
round_robin::add( fiber f)
{
	if ( ! f) throw fiber_moved();

	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( in_state_not_started( f) );

	// set state to ready
	set_state_ready( f);

	// attach to this scheduler
	attach( f);

	// insert fiber to fiber-list
	std::pair< std::map< fiber::id, schedulable >::iterator, bool > result(
		fibers_.insert(
			std::make_pair(
				f.get_id(),
				schedulable( f) ) ) );

	// check result
	if ( ! result.second) throw scheduler_error("inserting fiber failed");

	// put fiber to runnable-queue
	runnable_fibers_.push_back( result.first->first);
}

void
round_robin::join( fiber::id const& id)
{
	BOOST_ASSERT( active_fiber.get() );
	fiber_map::iterator i = fibers_.find( id);
	if ( i == fibers_.end() ) return;
	schedulable s( i->second);
	fiber f( s.f);
	if ( ! f) return;
	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( ! in_state_not_started( f) );

	// nothing to do for a terminated fiber
	if ( in_state_terminated( f) ) return;

	// prevent self-join
	if ( active_fiber->get_id() == id)
		throw scheduler_error("self-join denied");

	// register on fiber to be joined
	fibers_[id].joining_fibers.push_back( active_fiber->get_id() );

	// set state waiting
	set_state_wait_for_fiber( ( * active_fiber) );

	// set fiber-id waiting-on
	fibers_[active_fiber->get_id()].waiting_on_fiber = id;

	// switch to master-fiber
	switch_between( * active_fiber, master_fiber);

	// fiber returned
	BOOST_ASSERT( in_state_running( ( * active_fiber) ) );
	BOOST_ASSERT( ! fibers_[active_fiber->get_id()].waiting_on_fiber);

	// check if interruption was requested
	if ( interruption_enabled( * active_fiber) )
		throw fiber_interrupted();
}

void
round_robin::interrupt( fiber::id const& id)
{
	fiber_map::iterator i = fibers_.find( id);
	if ( i == fibers_.end() ) return;
	schedulable s( i->second);
	fiber f( s.f);
	BOOST_ASSERT( f);
	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( ! in_state_not_started( f) );

	// nothing to do for al terminated fiber
	if ( in_state_terminated( f) ) return;

	enable_interruption( f);

	// if fiber is waiting
	if ( in_state_wait_for_fiber( f) )
	{
		// fiber is waiting (joining) on another fiber
		// remove it from the waiting-queue, reset waiting-on
		// and reset the waiting state
		BOOST_ASSERT( s.waiting_on_fiber);
		fibers_[* s.waiting_on_fiber].joining_fibers.remove( id);
		fibers_[id].waiting_on_fiber.reset();
		set_state_ready( f);
		runnable_fibers_.push_back( id);
	} else if ( in_state_wait_for_object( f) ) {
		// fiber is waiting on an object
		// remove it from the waiting-queue, reset waiting-on
		// and reset the waiting state
		BOOST_ASSERT( s.waiting_on_object);
		objects_[* s.waiting_on_object].remove( id);
		fibers_[id].waiting_on_object.reset();
		set_state_ready( f);
		runnable_fibers_.push_back( id);
	}
}	

void
round_robin::reschedule( fiber::id const& id)
{
	fiber_map::iterator i = fibers_.find( id);
	if ( i == fibers_.end() ) return;
	fiber f( i->second.f);
	BOOST_ASSERT( f);
	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( ! in_state_not_started( f) );
	BOOST_ASSERT( ! in_state_terminated( f) );
}

void
round_robin::cancel( fiber::id const& id)
{
	BOOST_ASSERT( active_fiber.get() );
	fiber_map::iterator i = fibers_.find( id);
	if ( i == fibers_.end() ) return;
	schedulable s( i->second);
	fiber f( s.f);
	BOOST_ASSERT( f);
	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( ! in_state_not_started( f) );

	// nothing to do for al terminated fiber
	if ( in_state_terminated( f) ) return;

	// invoke each fiber waiting on this fiber
	BOOST_FOREACH( fiber::id id__, s.joining_fibers)
	{
		schedulable s__( fibers_[id__]);
		fiber f__( s__.f);
		BOOST_ASSERT( s__.waiting_on_fiber);
		BOOST_ASSERT( in_state_wait_for_fiber( f__) );

		// clear waiting-on
		fibers_[id__].waiting_on_fiber.reset();

		// put fiber on runnable-queue
		set_state_ready( f__);
		runnable_fibers_.push_back( id__);
	}
	// clear waiting-queue
	fibers_[id].joining_fibers.clear();

	// if fiber is ready remove it from the runnable-queue
	// and put it to terminated-queue
	if ( in_state_ready( f) )
	{
		set_state_terminated( f);
		runnable_fibers_.remove( id);
		terminated_fibers_.push( id);	
	}
	// if fiber is running (== active fiber)
	// reset active fiber
	// put it to terminated-queue and switch
	// to master fiber
	else if ( in_state_running( f) )
	{
		BOOST_ASSERT( active_fiber->get_id() == id);
		set_state_terminated( f);
		terminated_fibers_.push( id);
		switch_between( f, master_fiber);
	}
	// if fiber is waiting then remove it from the
	// waiting-queue and put it to terminated-queue
	else if ( in_state_wait_for_fiber( f) )
	{
		BOOST_ASSERT( s.waiting_on_fiber);
		set_state_terminated( f);
		fibers_[* s.waiting_on_fiber].joining_fibers.remove( id);
		terminated_fibers_.push( id);	
	}
	else
		BOOST_ASSERT( ! "should never reached");
}

void
round_robin::yield()
{
	BOOST_ASSERT( active_fiber.get() );
	BOOST_ASSERT( in_state_running( ( * active_fiber) ) );
	BOOST_ASSERT( ! fibers_[active_fiber->get_id()].waiting_on_fiber);

	// set state ready
	set_state_ready( ( * active_fiber) );

	// put fiber to runnable-queue
	runnable_fibers_.push_back( active_fiber->get_id() );

	// switch to master-fiber
	switch_between( * active_fiber, master_fiber);
}

void
round_robin::register_object( object::id const& oid)
{
	std::pair< object_map::iterator, bool > p(
			objects_.insert(
				std::make_pair( oid, fiber_id_list() ) ) );
	BOOST_ASSERT( p.second);
}

void
round_robin::unregister_object( object::id const& oid)
{
	BOOST_ASSERT( objects_.end() != objects_.find( oid) );
	objects_.erase( oid);
}

void
round_robin::wait_for_object( object::id const& oid)
{
	BOOST_ASSERT( active_fiber.get() );
	fiber::id id = active_fiber->get_id();
	fiber_map::iterator i = fibers_.find( id);
	if ( i == fibers_.end() ) return;
	schedulable s( i->second);
	fiber f( s.f);
	BOOST_ASSERT( f);
	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( active_fiber->get_id() == id);
	BOOST_ASSERT( in_state_running( f) );
	BOOST_ASSERT( ! s.waiting_on_fiber);
	BOOST_ASSERT( ! s.waiting_on_object);

	// register on object to be waiting on
	objects_[oid].push_back( id);
	
	// set state waiting
	set_state_wait_for_object( f);
	
	// set object-id waiting-on
	fibers_[id].waiting_on_object = oid;

	// switch to master-fiber
	switch_between( * active_fiber, master_fiber);

	// fiber returned
	BOOST_ASSERT( active_fiber->get_id() == id);
	BOOST_ASSERT( in_state_running( fibers_[id].f) );
	BOOST_ASSERT( ! fibers_[id].waiting_on_fiber);
	BOOST_ASSERT( ! fibers_[id].waiting_on_object);

	// check if interruption was requested
	if ( interruption_enabled( f) )
		throw fiber_interrupted();
}

void
round_robin::object_notify_one( object::id const& oid)
{
	object_map::iterator oi( objects_.find( oid) );
	BOOST_ASSERT( oi != objects_.end() );
	if ( oi->second.empty() ) return;
	fiber::id id( oi->second.front() );
	oi->second.pop_front();

	fiber_map::iterator fi = fibers_.find( id);
	BOOST_ASSERT( fi != fibers_.end() );
	schedulable s( fi->second);
	fiber f( s.f);
	BOOST_ASSERT( f);
	BOOST_ASSERT( ! is_master( f) );
	BOOST_ASSERT( ! s.waiting_on_fiber);
	BOOST_ASSERT( s.waiting_on_object);
	BOOST_ASSERT( * s.waiting_on_object == oid);

	// remove object waiting for
	fibers_[id].waiting_on_object.reset();
	
	// set state ready
	set_state_ready( f);

	// put fiber to runnable-queue
	runnable_fibers_.push_back( id);
}

void
round_robin::object_notify_all( object::id const& oid)
{
	object_map::iterator oi( objects_.find( oid) );
	BOOST_ASSERT( oi != objects_.end() );
	fiber_id_list::iterator fe( oi->second.end() );
	for (
			fiber_id_list::iterator fi( oi->second.begin() );
			fi != fe;
			++fi)
	{
		fiber::id id( * fi);

		fiber_map::iterator i = fibers_.find( id);
		BOOST_ASSERT( i != fibers_.end() );
		schedulable s( i->second);
		fiber f( s.f);
		BOOST_ASSERT( f);
		BOOST_ASSERT( ! is_master( f) );
		BOOST_ASSERT( ! s.waiting_on_fiber);
		BOOST_ASSERT( s.waiting_on_object);
		BOOST_ASSERT( * s.waiting_on_object == oid);

		// remove object waiting for
		fibers_[id].waiting_on_object.reset();
		
		// set state ready
		set_state_ready( f);

		// put fiber to runnable-queue
		runnable_fibers_.push_back( id);
	}
	oi->second.clear();
}

fiber
round_robin::release( fiber::id const& id)
{
	fiber_map::iterator fi = fibers_.find( id);
	if ( fi == fibers_.end() )
		throw scheduler_error("fiber not managed by scheduler");
	schedulable s( fi->second);
	fiber f( s.f);
	BOOST_ASSERT( f);
	BOOST_ASSERT( ! is_master( f) );
	if ( ! in_state_ready( f) || ! s.joining_fibers.empty() )
		throw fiber_error("fiber can not be released");
	BOOST_ASSERT( ! s.waiting_on_fiber);
	BOOST_ASSERT( ! s.waiting_on_object);

	runnable_fibers_.remove( id);
	fibers_.erase( id);

	return f;	
}

void
round_robin::migrate( fiber f)
{
	if ( ! f) throw fiber_moved();

	BOOST_ASSERT( in_state_ready( f) );

	// attach to this scheduler
	attach( f);

	// insert fiber to fiber-list
	std::pair< std::map< fiber::id, schedulable >::iterator, bool > result(
		fibers_.insert(
			std::make_pair(
				f.get_id(),
				schedulable( f) ) ) );

	// check result
	if ( ! result.second) throw scheduler_error("migrating fiber failed");

	// put fiber to runnable-queue
	runnable_fibers_.push_back( result.first->first);
}

void
round_robin::detach_all()
{
	BOOST_FOREACH( fiber_map::value_type va, fibers_)
	{ detach( va.second.f); }
}

bool
round_robin::run()
{
	bool result( false);
	if ( ! runnable_fibers_.empty() )
	{
		schedulable s = fibers_[runnable_fibers_.front()];
		runnable_fibers_.pop_front();
		std::auto_ptr< fiber > orig( active_fiber.release() );
		active_fiber.reset( new fiber( s.f) );
		BOOST_ASSERT( ! s.waiting_on_fiber);
		BOOST_ASSERT( ! s.waiting_on_object);
		BOOST_ASSERT( in_state_ready( ( * active_fiber) ) );
		set_state_running( ( * active_fiber) );
		switch_between( master_fiber, * active_fiber);
		active_fiber.reset( orig.release() );
		result = true;
	}

	while ( ! terminated_fibers_.empty() )
	{
		fiber_map::iterator i( fibers_.find( terminated_fibers_.front() ) );
		BOOST_ASSERT( i != fibers_.end() );
		schedulable s( i->second);
		fiber f( s.f);
		terminated_fibers_.pop();
		BOOST_ASSERT( s.joining_fibers.empty() );	
		BOOST_ASSERT( ! s.waiting_on_fiber);
		BOOST_ASSERT( ! s.waiting_on_object);
		BOOST_ASSERT( in_state_terminated( f) );	
		fibers_.erase( i);
	}
	return result;
}

bool
round_robin::empty() const
{ return fibers_.empty(); }

std::size_t
round_robin::size() const
{ return fibers_.size(); }

std::size_t
round_robin::ready() const
{ return runnable_fibers_.size(); }

}}

#include <boost/config/abi_suffix.hpp>
