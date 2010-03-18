
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>

std::vector< int > vec;
bool set = false;

void at_exit_1()
{ vec.push_back( 1); }

void at_exit_2()
{ vec.push_back( 2); }

void fn()
{
	boost::this_fiber::at_fiber_exit( at_exit_1);
	boost::this_fiber::at_fiber_exit( at_exit_2);
	set = true;
}

void test_case_1()
{
	boost::fibers::scheduler<> sched;

	sched.make_fiber( fn, boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( std::size_t( 0), vec.size() );
	BOOST_CHECK( vec.empty() );
	BOOST_CHECK_EQUAL( false, set);

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	BOOST_CHECK_EQUAL( std::size_t( 2), vec.size() );
	BOOST_CHECK( ! vec.empty() );
	BOOST_CHECK_EQUAL( 2, vec[0]);
	BOOST_CHECK_EQUAL( 1, vec[1]);
	BOOST_CHECK_EQUAL( true, set);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: at-exit test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );

	return test;
}
