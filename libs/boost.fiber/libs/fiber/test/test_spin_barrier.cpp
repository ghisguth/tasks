
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

void fn_1( boost::fibers::spin::barrier & b)
{
	++value1;
	boost::this_fiber::yield();

	b.wait();

	++value1;
	boost::this_fiber::yield();
	++value1;
	boost::this_fiber::yield();
	++value1;
	boost::this_fiber::yield();
	++value1;
}

void fn_2( boost::fibers::spin::barrier & b)
{
	++value2;
	boost::this_fiber::yield();
	++value2;
	boost::this_fiber::yield();
	++value2;
	boost::this_fiber::yield();

	b.wait();

	++value2;
	boost::this_fiber::yield();
	++value2;
}

void test_case_1()
{
	value1 = 0;
	value2 = 0;

	boost::fibers::scheduler<> sched;

	boost::fibers::spin::barrier b( 2);
	sched.make_fiber( fn_1, boost::ref( b), boost::fiber::default_stacksize);
	sched.make_fiber( fn_2, boost::ref( b), boost::fiber::default_stacksize);

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
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 1, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 2, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 3, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 3, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 1, value1);
	BOOST_CHECK_EQUAL( 4, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 4, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 2, value1);
	BOOST_CHECK_EQUAL( 5, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 3, value1);
	BOOST_CHECK_EQUAL( 5, value2);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( ! sched.empty() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 4, value1);
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

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: spin::barrier test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );

	return test;
}
