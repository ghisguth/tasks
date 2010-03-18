
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <string>

#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>


void zero_args_fn()
{}

void test_case_1()
{
	boost::fiber f( zero_args_fn, boost::fiber::default_stacksize);
	BOOST_CHECK_EQUAL( 0, f.priority() );

	f.priority( 5);
	BOOST_CHECK_EQUAL( 5, f.priority() );

	f.priority( -5);
	BOOST_CHECK_EQUAL( -5, f.priority() );
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: priority test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );

	return test;
}
