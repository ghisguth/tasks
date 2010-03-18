
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_TASKS_DETAIL_BIND_PRCESSOR_LINUX_H
#define BOOST_TASKS_DETAIL_BIND_PRCESSOR_LINUX_H

extern "C"
{
#include <pthread.h>
#include <sched.h>
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
	BOOST_ASSERT( n < CPU_SETSIZE);
	BOOST_ASSERT( n < boost::thread::hardware_concurrency() );

	cpu_set_t cpuset;
	CPU_ZERO( & cpuset);
	CPU_SET( n, & cpuset);

	int errno_( ::pthread_setaffinity_np( ::pthread_self(), sizeof( cpuset), & cpuset) );
	if ( errno_ != 0)
		throw boost::system::system_error(
				boost::system::error_code(
					errno_,
					boost::system::system_category) );
}

inline
void bind_to_any_processor()
{
	cpu_set_t cpuset;
	CPU_ZERO( & cpuset);

	unsigned int max( boost::thread::hardware_concurrency() );
	for ( unsigned int i( 0); i < max; ++i)
		CPU_SET( i, & cpuset);

	int errno_( ::pthread_setaffinity_np( ::pthread_self(), sizeof( cpuset), & cpuset) );
	if ( errno_ != 0)
		throw boost::system::system_error(
				boost::system::error_code(
					errno_,
					boost::system::system_category) );
}

}}

#include <boost/config/abi_suffix.hpp>

#endif // BOOST_TASKS_DETAIL_BIND_PRCESSOR_LINUX_H
