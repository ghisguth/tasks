
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBER_DETAIL_INFO_BASE_H
#define BOOST_FIBER_DETAIL_INFO_BASE_H

#include <boost/fiber/detail/config.hpp>

# if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#include <boost/fiber/detail/info_base_windows.hpp>
# elif defined(_POSIX_VERSION)
#include <boost/fiber/detail/info_base_posix.hpp>
# endif

#endif // BOOST_FIBER_DETAIL_INFO_BASE_H
