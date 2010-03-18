
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <string>

#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>

int value1 = 0;
int value2 = 0;
int value3 = 0;
bool interrupted = false;

void fn_1()
{
	for ( int i = 0; i < 5; ++i)
	{
		++value1;
		boost::this_fiber::yield();
	}
}

void fn_2( boost::fiber f)
{
	try
	{
		for ( int i = 0; i < 5; ++i)
		{
			++value2;
			if ( i == 1) f.join();
			boost::this_fiber::yield();
		}
	}
	catch ( boost::fibers::fiber_interrupted const&)
	{ interrupted = true; }
}

void fn_3( boost::fiber f)
{
	for ( int i = 0; i < 5; ++i)
	{
		++value3;
		if ( i == 3) f.cancel();
		boost::this_fiber::yield();
	}
}

void fn_4( boost::fiber f)
{
	for ( int i = 0; i < 5; ++i)
	{
		++value3;
		if ( i == 3) f.interrupt();
		boost::this_fiber::yield();
	}
}

void test_case_1()
{
	value1 = 0;
	value2 = 0;

	boost::fibers::scheduler<> sched;

	boost::fiber f( fn_1, boost::fiber::default_stacksize);
	sched.submit_fiber( f);
	sched.make_fiber( fn_2, f, boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( 0, value1);
	BOOST_CHECK_EQUAL( 0, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 0, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 1, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 1, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 3, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 4, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 5, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 5, value2);

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 5, value2);
}

void test_case_2()
{
	value1 = 0;
	value2 = 0;
	value3 = 0;

	boost::fibers::scheduler<> sched;

	boost::fiber f( fn_1, boost::fiber::default_stacksize);
	sched.submit_fiber( f);
	sched.make_fiber( fn_2, f, boost::fiber::default_stacksize);
	sched.make_fiber( fn_3, f, boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( 0, value1);
	BOOST_CHECK_EQUAL( 0, value2);
	BOOST_CHECK_EQUAL( 0, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 0, value2);
	BOOST_CHECK_EQUAL( 0, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 1, value2);
	BOOST_CHECK_EQUAL( 0, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 1, value2);
	BOOST_CHECK_EQUAL( 1, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 1, value2);
	BOOST_CHECK_EQUAL( 1, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 1, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 2, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 2, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 3, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 3, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 4, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 4, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 5, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 3, value2);
	BOOST_CHECK_EQUAL( 5, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 3, value2);
	BOOST_CHECK_EQUAL( 5, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 4, value2);
	BOOST_CHECK_EQUAL( 5, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 5, value2);
	BOOST_CHECK_EQUAL( 5, value3);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 5, value2);
	BOOST_CHECK_EQUAL( 5, value3);

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
	BOOST_CHECK_EQUAL( 5, value2);
	BOOST_CHECK_EQUAL( 5, value3);
}

void test_case_3()
{
	value1 = 0;
	value2 = 0;
	value3 = 0;

	boost::fibers::scheduler<> sched;

	boost::fiber f1( fn_1, boost::fiber::default_stacksize);
	sched.submit_fiber( f1);
	boost::fiber f2( fn_2, f1, boost::fiber::default_stacksize);
	sched.submit_fiber( f2);
	sched.make_fiber( fn_4, f2, boost::fiber::default_stacksize);

	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );

	BOOST_CHECK_EQUAL( 0, value1);
	BOOST_CHECK_EQUAL( 0, value2);
	BOOST_CHECK_EQUAL( 0, value3);
	BOOST_CHECK_EQUAL( false, interrupted);

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 2, value2);
	BOOST_CHECK_EQUAL( 5, value3);
	BOOST_CHECK_EQUAL( true, interrupted);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: join test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );
	test->add( BOOST_TEST_CASE( & test_case_2) );
	test->add( BOOST_TEST_CASE( & test_case_3) );

	return test;
}
