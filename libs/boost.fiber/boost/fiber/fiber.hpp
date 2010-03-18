
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_FIBER_H
#define BOOST_FIBERS_FIBER_H

#include <cstddef>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/preprocessor/repetition.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/detail/config.hpp>
#include <boost/fiber/detail/info.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

# if defined(BOOST_MSVC)
# pragma warning(push)
# pragma warning(disable:4251)
# endif

namespace boost {
namespace fibers {

template< typename Strategy >
class scheduler;
class strategy;

class BOOST_FIBER_DECL fiber
{
private:
	template< typename Strategy >
	friend class scheduler;
	friend class strategy;

	static void convert_thread_to_fiber();
	static void convert_fiber_to_thread();

	BOOST_COPYABLE_AND_MOVABLE( fiber);

	detail::info_base::ptr	info_;

	explicit fiber( detail::info_base::ptr);

	void init_();

	void switch_to_( fiber &);

	template< typename Fn >
	static detail::info_base::ptr make_info_(
		Fn fn, std::size_t stack_size)
	{
		return detail::info_base::ptr(
			new detail::info< Fn >( fn, stack_size) );
	}

	template< typename Fn >
	static detail::info_base::ptr make_info_(
		BOOST_RV_REF( Fn) fn, std::size_t stack_size)
	{
		return detail::info_base::ptr(
			new detail::info< Fn >( fn, stack_size) );
	}

public:
	static std::size_t default_stacksize;

	class id;

	fiber();

	template< typename Fn >
	explicit fiber( Fn fn, std::size_t stack_size) :
		info_( make_info_( fn, stack_size) )
	{ init_(); }

#define BOOST_FIBER_ARG(z, n, unused) \
   BOOST_PP_CAT(A, n) BOOST_PP_CAT(a, n)
#define BOOST_ENUM_FIBER_ARGS(n) BOOST_PP_ENUM(n, BOOST_FIBER_ARG, ~)

#define BOOST_FIBER_FIBER_CTOR(z, n, unused) \
	template< typename Fn, BOOST_PP_ENUM_PARAMS(n, typename A) > \
	fiber( Fn fn, BOOST_ENUM_FIBER_ARGS(n), std::size_t stack_size) : \
		info_( \
			make_info_( \
				boost::bind( boost::type< void >(), fn, BOOST_PP_ENUM_PARAMS(n, a) ), \
			   	stack_size) ) \
	{ init_(); } \

#ifndef BOOST_FIBER_MAX_ARITY
#define BOOST_FIBER_MAX_ARITY 10
#endif

BOOST_PP_REPEAT_FROM_TO( 1, BOOST_FIBER_MAX_ARITY, BOOST_FIBER_FIBER_CTOR, ~)

#undef BOOST_FIBER_FIBER_CTOR

	template< typename Fn >
	explicit fiber( BOOST_RV_REF( Fn) fn, std::size_t stack_size) :
		info_( make_info_( fn, stack_size) )
	{ init_(); }

	fiber( fiber const& other);

	fiber & operator=( BOOST_COPY_ASSIGN_REF( fiber) other);

	fiber( BOOST_RV_REF( fiber) other);

	fiber & operator=( BOOST_RV_REF( fiber) other);

	typedef detail::info_base::ptr::unspecified_bool_type	unspecified_bool_type;

	operator unspecified_bool_type() const;

	bool operator!() const;

	void swap( fiber & other);

	id get_id() const;

	bool operator==( fiber const& other) const;
	bool operator!=( fiber const& other) const;

	bool is_alive() const;

	int priority() const;

	void priority( int);

	void interrupt();

	bool interruption_requested() const;

	void cancel();

	void join();
};

class BOOST_FIBER_DECL fiber::id
{
private:
	friend class fiber;

	detail::info_base::ptr	info_;

	explicit id( detail::info_base::ptr info) :
		info_( info)
	{}

public:
	id() :
		info_()
	{}

	bool operator==( id const& other) const
	{ return info_ == other.info_; }

	bool operator!=( id const& other) const
	{ return info_ != other.info_; }
	
	bool operator<( id const& other) const
	{ return info_ < other.info_; }
	
	bool operator>( id const& other) const
	{ return other.info_ < info_; }
	
	bool operator<=( id const& other) const
	{ return !( other.info_ < info_); }
	
	bool operator>=( id const& other) const
	{ return ! ( info_ < other.info_); }

	template< typename charT, class traitsT >
	friend std::basic_ostream< charT, traitsT > &
	operator<<( std::basic_ostream< charT, traitsT > & os, id const& other)
	{
		if ( other.info_)
			return os << other.info_;
		else
			return os << "{not-a-fiber}";
	}
};

template< typename Fn >
fiber make_fiber( Fn fn, std::size_t stack_size)
{ return fiber( fn, stack_size); }

#define BOOST_FIBER_MAKE_FIBER_FUNCTION(z, n, unused) \
template< typename Fn, BOOST_PP_ENUM_PARAMS(n, typename A) > \
fiber make_fiber( Fn fn, BOOST_ENUM_FIBER_ARGS(n), std::size_t stack_size) \
{ return fiber( fn, BOOST_PP_ENUM_PARAMS(n, a), stack_size); }

BOOST_PP_REPEAT_FROM_TO( 1, BOOST_FIBER_MAX_ARITY, BOOST_FIBER_MAKE_FIBER_FUNCTION, ~)

#undef BOOST_FIBER_MAKE_FIBER_FUNCTION
#undef BOOST_ENUM_FIBER_ARGS
#undef BOOST_FIBER_ARG
#undef BOOST_FIBER_MAX_ARITY

}

using fibers::fiber;

inline
void swap( fiber & l, fiber & r)
{ return l.swap( r); }

}

# if defined(BOOST_MSVC)
# pragma warning(pop)
# endif

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_FIBER_H
