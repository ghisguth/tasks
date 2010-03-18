
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include <sstream>
#include <string>

#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>

bool runs_as_fiber = false;
std::string id;
int value = 0;

void runs_as_fn()
{ runs_as_fiber = boost::this_fiber::runs_as_fiber(); }

void get_id_fn()
{
	std::ostringstream os;
	os << boost::this_fiber::get_id();
	id = os.str();
}

void yield_fn( int n)
{
	for ( int i = 0; i < n; ++i)
	{
		++value;
		boost::this_fiber::yield();
	}	
}

void cancel_fn( int n)
{
	if ( n < 3) throw std::invalid_argument("must be greater than 3");
	for ( int i = 0; i < n; ++i)
	{
		if ( i == 2) boost::this_fiber::cancel();
		++value;
		boost::this_fiber::yield();
	}	
}

void submit_fn( int n)
{
	boost::this_fiber::submit_fiber(
			boost::fibers::make_fiber(
				& yield_fn, n, boost::fiber::default_stacksize) );
}

void test_case_1()
{
	boost::fibers::scheduler<> sched;
	sched.make_fiber( runs_as_fn, boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );

	BOOST_CHECK( runs_as_fiber);
}

void test_case_2()
{
	boost::fibers::scheduler<> sched;
	sched.make_fiber( get_id_fn, boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );

	BOOST_CHECK( ! id.empty() );
	BOOST_CHECK( std::string("{not-a-fiber}") != id);
}

void test_case_3()
{
	value = 0;
	boost::fibers::scheduler<> sched;
	sched.make_fiber( yield_fn, 3, boost::fiber::default_stacksize);
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( 1, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( 2, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( 3, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( 3, value);

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK_EQUAL( 3, value);
}

void test_case_4()
{
	value = 0;
	boost::fibers::scheduler<> sched;
	sched.make_fiber( cancel_fn, 5, boost::fiber::default_stacksize);
	BOOST_CHECK_EQUAL( 0, value);
	
	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( 1, value);
	
	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( 2, value);
	
	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK_EQUAL( 2, value);
}

void test_case_5()
{
	value = 0;
	boost::fibers::scheduler<> sched;
	sched.make_fiber( submit_fn, 5, boost::fiber::default_stacksize);
	BOOST_CHECK_EQUAL( 0, value);
	
	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}
	BOOST_CHECK_EQUAL( 5, value);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: utility test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );
	test->add( BOOST_TEST_CASE( & test_case_2) );
	test->add( BOOST_TEST_CASE( & test_case_3) );
	test->add( BOOST_TEST_CASE( & test_case_4) );
	test->add( BOOST_TEST_CASE( & test_case_5) );

	return test;
}
