
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_TASKS_DETAIL_BIND_PROCESSOR_FREEBSD_H
#define BOOST_TASKS_DETAIL_BIND_PROCESSOR_FREEBSD_H

extern "C"
{
#include <sys/param.h>
#include <sys/cpuset.h>
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

	cpuset_t cpuset;
	CPU_ZERO( & cpuset);
	CPU_SET( n, & cpuset);

	if ( ::cpuset_setaffinity(  CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof( cpuset), & cpuset) == -1)
		throw boost::system::system_error(
				boost::system::error_code(
					errno,
					boost::system::system_category) );
}

inline
void bind_to_any_processor()
{
	cpuset_t cpuset;
	CPU_ZERO( & cpuset);

	unsigned int max( boost::thread::hardware_concurrency() );
	for ( unsigned int i( 0); i < max; ++i)
		CPU_SET( i, & cpuset);

	if ( ::cpuset_setaffinity(  CPU_LEVEL_WHICH, CPU_WHICH_TID, -1, sizeof( cpuset), & cpuset) == -1)
		throw boost::system::system_error(
				boost::system::error_code(
					errno,
					boost::system::system_category) );
}

}}

#include <boost/config/abi_suffix.hpp>

#endif // BOOST_TASKS_DETAIL_BIND_PROCESSOR_FREEBSD_H
