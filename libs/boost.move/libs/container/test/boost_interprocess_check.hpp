//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2007. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_TEST_HEADER
#define BOOST_CONTAINER_TEST_HEADER

#include <boost/container/detail/config_begin.hpp>
#include <boost/container/exceptions.hpp>
#include <iostream>

namespace boost { namespace container { namespace test {

#define BOOST_INTERPROCES_CHECK( P )  \
   if(!(P)) do{  std::cout << "Failed: " << #P << " file: " << __FILE__ << " line : " << __LINE__ << std::endl; throw boost::container::container_exception();}while(0)

}}}   //namespace boost { namespace container { namespace test {

#include <boost/container/detail/config_end.hpp>

#endif   //BOOST_CONTAINER_TEST_HEADER
