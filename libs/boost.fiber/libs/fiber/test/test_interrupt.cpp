
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>

int value1 = 0;
int value2 = 0;
bool interrupted = false;

void fn_1()
{
	try
	{
		for ( int i = 0; i < 5; ++i)
		{
			++value1;
			boost::this_fiber::interruption_point();
			boost::this_fiber::yield();
		}
	}
	catch ( boost::fibers::fiber_interrupted const&)
	{ interrupted = true; }
}

void fn_2()
{
	boost::this_fiber::disable_interruption disabler;
	if ( boost::this_fiber::interruption_enabled() )
		throw std::logic_error("interruption enabled");
	for ( int i = 0; i < 5; ++i)
	{
		++value1;
		boost::this_fiber::interruption_point();
		boost::this_fiber::yield();
	}
}

void fn_3()
{
	try
	{
		boost::this_fiber::disable_interruption disabler;
		if ( boost::this_fiber::interruption_enabled() )
			throw std::logic_error("interruption enabled");
		for ( int i = 0; i < 5; ++i)
		{
			++value1;
			boost::this_fiber::restore_interruption restorer( disabler);
			boost::this_fiber::interruption_point();
			boost::this_fiber::yield();
		}
	}
	catch ( boost::fibers::fiber_interrupted const&)
	{ interrupted = true; }
}

void fn_5( boost::fiber f)
{
	for ( int i = 0; i < 5; ++i)
	{
		++value2;
		if ( i == 1) f.interrupt();
		if ( i >= 1)
		{
			if ( ! f.interruption_requested() )
				throw std::logic_error("");
		}
		boost::this_fiber::yield();
	}
}

void test_case_1()
{
	value1 = 0;
	value2 = 0;
	interrupted = false;

	boost::fibers::scheduler<> sched;

	boost::fiber f( fn_1, boost::fiber::default_stacksize);
	sched.submit_fiber( f);
	sched.make_fiber( fn_5, f, boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( 0, value1);
	BOOST_CHECK_EQUAL( 0, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( false, interrupted);
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 0, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( false, interrupted);
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 1, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( false, interrupted);
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 1, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( false, interrupted);
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( true, interrupted);
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( true, interrupted);
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 3, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( true, interrupted);
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 4, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( true, interrupted);
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 5, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( true, interrupted);
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 5, value2);

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( true, interrupted);
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 5, value2);
}

void test_case_2()
{
	value1 = 0;
	value2 = 0;
	interrupted = false;

	boost::fibers::scheduler<> sched;

	boost::fiber f( fn_2, boost::fiber::default_stacksize);
	sched.submit_fiber( f);
	sched.make_fiber( fn_5, f, boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( 0, value1);
	BOOST_CHECK_EQUAL( 0, value2);
	BOOST_CHECK_EQUAL( false, interrupted);
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 5, value1);
	BOOST_CHECK_EQUAL( 5, value2);
	BOOST_CHECK_EQUAL( false, interrupted);
}

void test_case_3()
{
	value1 = 0;
	value2 = 0;
	interrupted = false;

	boost::fibers::scheduler<> sched;

	boost::fiber f( fn_3, boost::fiber::default_stacksize);
	sched.submit_fiber( f);
	sched.make_fiber( fn_5, f, boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( 0, value1);
	BOOST_CHECK_EQUAL( 0, value2);
	BOOST_CHECK_EQUAL( false, interrupted);
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	BOOST_CHECK( ! sched.run() );
	BOOST_CHECK( sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 5, value2);
	BOOST_CHECK_EQUAL( true, interrupted);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: interrupt test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );
	test->add( BOOST_TEST_CASE( & test_case_2) );
	test->add( BOOST_TEST_CASE( & test_case_3) );

	return test;
}
