
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_TASKS_DETAIL_BIND_PROCESSOR_SOLARIS_H
#define BOOST_TASKS_DETAIL_BIND_PROCESSOR_SOLARIS_H

extern "C"
{
#include <sys/types.h>
#include <sys/processor.h>
#include <sys/procset.h>
}

#include <boost/assert.hpp>
#include <boost/thread.hpp>
#include <boost/system/system_error.hpp>

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace this_thread {

inline
void bind_to_processor( unsigned int n)
{
	BOOST_ASSERT( n >= 0);
	BOOST_ASSERT( n < boost::thread::hardware_concurrency() );

	if ( ::processor_bind( P_LWPID, P_MYID, static_cast< processorid_t >( n), 0) == -1)
		throw boost::system::system_error(
				boost::system::error_code(
					errno,
					boost::system::system_category) );
}

inline
void bind_to_any_processor()
{
	if ( ::processor_bind( P_LWPID, P_MYID, PBIND_NONE, 0) == -1)
		throw boost::system::system_error(
				boost::system::error_code(
					errno,
					boost::system::system_category) );
}

}}

#include <boost/config/abi_suffix.hpp>

#endif // BOOST_TASKS_DETAIL_BIND_PROCESSOR_SOLARIS_H
