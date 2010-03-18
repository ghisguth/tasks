
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/fiber.hpp>

#include <boost/assert.hpp>

#include <boost/fiber/detail/interrupt_flags.hpp>
#include <boost/fiber/detail/state_flags.hpp>
#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/strategy.hpp>

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace fibers {

std::size_t
fiber::default_stacksize = 65536;

fiber::fiber() :
	info_()
{}

fiber::fiber( detail::info_base::ptr info_base) :
	info_( info_base)
{}

fiber::fiber( fiber const& other) :
	info_( other.info_)
{}

fiber &
fiber::operator=( BOOST_COPY_ASSIGN_REF( fiber) other)
{
	if ( this == & other) return * this;
	info_ = other.info_;
	return * this;
}

fiber::fiber( BOOST_RV_REF( fiber) other)
{
	info_ = other.info_;
	other.info_.reset();
}

fiber &
fiber::operator=( BOOST_RV_REF( fiber) other)
{
	fiber tmp( other);
	swap( tmp);
	return * this;
}

fiber::operator unspecified_bool_type() const
{ return info_; }

bool
fiber::operator!() const
{ return ! info_; }

bool
fiber::operator==( fiber const& other) const
{ return get_id() == other.get_id(); }

bool
fiber::operator!=( fiber const& other) const
{ return !( get_id() == other.get_id() ); }

void
fiber::swap( fiber & other)
{ info_.swap( other.info_); }

fiber::id
fiber::get_id() const
{ return fiber::id( info_); }

bool
fiber::is_alive() const
{
	if ( ! info_) throw fiber_moved();
	return ( info_->state & IS_ALIVE_BIT_MASK) != 0;
}

int
fiber::priority() const
{
	if ( ! info_) throw fiber_moved();
	return info_->priority;
}

void
fiber::priority( int prio)
{
	if ( ! info_) throw fiber_moved();
	info_->priority = prio;
	if ( is_alive() )
		info_->st->reschedule( get_id() );
}

void
fiber::interrupt()
{
	if ( ! info_) throw fiber_moved();
	info_->st->interrupt( get_id() );
}

bool
fiber::interruption_requested() const
{
	if ( ! info_) throw fiber_moved();
	return ( info_->interrupt & detail::INTERRUPTION_ENABLED) != 0;
}

void
fiber::cancel()
{
	if ( ! info_) throw fiber_moved();
	info_->st->cancel( get_id() );
}

void
fiber::join()
{
	if ( ! info_) throw fiber_moved();
	info_->st->join( get_id() );
}

}}

#include <boost/config/abi_suffix.hpp>
