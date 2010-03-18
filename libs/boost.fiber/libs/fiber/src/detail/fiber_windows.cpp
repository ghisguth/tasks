
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/fiber.hpp>

extern "C" {

#include <windows.h>

}

#include <cerrno>

#include <boost/system/system_error.hpp>

#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/utility.hpp>

#include <boost/config/abi_prefix.hpp>

namespace {

bool is_thread_a_fiber()
{
#if ( _WIN32_WINNT > 0x0600)
	return ::IsThreadAFiber() == TRUE;
#else
	LPVOID current = ::GetCurrentFiber();
	return 0 != current && current != reinterpret_cast< LPVOID >( 0x1E00);
#endif
}

}

namespace boost {
namespace fibers {

VOID CALLBACK trampoline( LPVOID vp)
{
	detail::info_base * self(
			static_cast< detail::info_base * >( vp) );
	BOOST_ASSERT( self);
	try
	{ self->run(); }
	catch ( fiber_interrupted const&) {}
	catch (...) {}
	while ( ! self->at_exit.empty() )
	{
		detail::info_base::callable_type ca;
		self->at_exit.top().swap( ca);
		self->at_exit.pop();
		ca();
	}
 	this_fiber::cancel();
}

void
fiber::init_()
{
	if ( ! info_) throw fiber_moved();

	info_->uctx = ::CreateFiber(
		info_->stack_size,
		static_cast< LPFIBER_START_ROUTINE >( & trampoline),
		static_cast< LPVOID >( info_.get() ) );
}

void
fiber::switch_to_( fiber & to)
{
	if ( ! info_ || ! to.info_) throw fiber_moved();

	::SwitchToFiber( to.info_->uctx);
}

void
fiber::convert_thread_to_fiber()
{
	if ( ! is_thread_a_fiber() )
		if ( ::ConvertThreadToFiber( 0) == 0)
			throw system::system_error(
				system::error_code(
					GetLastError(),
					system::system_category) );
}

void
fiber::convert_fiber_to_thread()
{
	if ( ! this_fiber::runs_as_fiber() && is_thread_a_fiber() )
		if ( ::ConvertFiberToThread() == 0)
			throw system::system_error(
				system::error_code(
					GetLastError(),
					system::system_category) );
}

}}

#include <boost/config/abi_suffix.hpp>
