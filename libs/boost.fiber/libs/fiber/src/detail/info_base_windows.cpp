
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/detail/info_base_windows.hpp>

#include <cerrno>

#include <boost/assert.hpp>
#include <boost/system/system_error.hpp>

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace fibers {
namespace detail {

info_base::info_base() :
	use_count( 0),
	stack_size( 0),
	priority( 0),
	uctx(),
	state( STATE_MASTER),
	interrupt( INTERRUPTION_DISABLED),
	at_exit(),
	st( 0)
{
	uctx = ::GetCurrentFiber();
	if ( ! uctx)
		throw system::system_error(
			system::error_code(
				::GetLastError(),
				system::system_category) );
}

info_base::~info_base()
{ if ( state != STATE_MASTER) ::DeleteFiber( uctx); }

info_base::info_base( std::size_t stack_size_) :
	use_count( 0),
	stack_size( stack_size_),
	priority( 0),
	uctx(),
	state( STATE_NOT_STARTED),
	interrupt( INTERRUPTION_DISABLED)
{}

}}}

#include <boost/config/abi_suffix.hpp>
