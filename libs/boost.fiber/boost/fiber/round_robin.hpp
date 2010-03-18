
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_ROUND_ROBIN_H
#define BOOST_FIBERS_ROUND_ROBIN_H

#include <cstddef>
#include <list>
#include <map>
#include <queue>

#include <boost/config.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/object/id.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/strategy.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

# if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable:4251 4275)
# endif

namespace boost {
namespace fibers {

class BOOST_FIBER_DECL round_robin : private noncopyable,
									 public strategy
{
private:
	struct schedulable
	{
		fiber					f;
		std::list< fiber::id >	joining_fibers;
		optional< fiber::id >	waiting_on_fiber;
		optional< object::id >	waiting_on_object;

		schedulable() :
			f(), joining_fibers(),
			waiting_on_fiber(), waiting_on_object()
		{}

		schedulable( fiber f_) :
			f( f_), joining_fibers(),
			waiting_on_fiber(), waiting_on_object()
		{}

		schedulable( schedulable const& other) :
			f( other.f),
			joining_fibers( other.joining_fibers),
			waiting_on_fiber( other.waiting_on_fiber),
			waiting_on_object( other.waiting_on_object)
		{}	

		schedulable &
		operator=( schedulable const& other)
		{
			if ( this == & other) return * this;
			f = other.f;
			joining_fibers = other.joining_fibers;
			waiting_on_fiber = other.waiting_on_fiber;
			waiting_on_object = other.waiting_on_object;
			return * this;
		}
	};

	typedef std::list< fiber::id >					fiber_id_list;
	typedef std::map< object::id, fiber_id_list >	object_map;
	typedef std::map< fiber::id, schedulable >		fiber_map;
	typedef std::list< fiber::id >					runnable_queue;
	typedef std::queue< fiber::id >					terminated_queue;

	fiber_map			fibers_;
	object_map			objects_;
	runnable_queue		runnable_fibers_;
	terminated_queue	terminated_fibers_;

public:
	round_robin();

	void add( fiber);

	void join( fiber::id const&);

	void interrupt( fiber::id const&);

	void reschedule( fiber::id const&);

	void cancel( fiber::id const&);

	void yield();

	void register_object( object::id const&);

	void unregister_object( object::id const&);

	void wait_for_object( object::id const&);

	void object_notify_one( object::id const&);

	void object_notify_all( object::id const&);

	fiber release( fiber::id const&);

	void migrate( fiber);

	void detach_all();

	bool run();

	bool empty() const;

	std::size_t size() const;

	std::size_t ready() const;
};

}}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_ROUND_ROBIN_H
