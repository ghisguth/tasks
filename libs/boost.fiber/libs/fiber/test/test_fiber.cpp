
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <string>

#include <boost/assert.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>

void zero_args_fn()
{ BOOST_ASSERT( boost::this_fiber::runs_as_fiber() ); }

void one_args_fn( int)
{ BOOST_ASSERT( boost::this_fiber::runs_as_fiber() ); }

void two_args_fn( int, std::string const&)
{ BOOST_ASSERT( boost::this_fiber::runs_as_fiber() ); }

void test_case_1()
{
	boost::fiber f1( one_args_fn, 10, boost::fiber::default_stacksize);
	boost::fiber f2;
	BOOST_CHECK( f1);
	BOOST_CHECK( ! f2);
	BOOST_CHECK( ! f1.is_alive() );
}

void test_case_2()
{
	boost::fiber f1;
	f1 = boost::fibers::make_fiber( zero_args_fn, boost::fiber::default_stacksize);
	boost::fiber f2 = boost::fibers::make_fiber( one_args_fn, 1, boost::fiber::default_stacksize);
	boost::fiber f3;
	f3 = boost::fibers::make_fiber( two_args_fn, 1, "abc", boost::fiber::default_stacksize);
}

void test_case_3()
{
	boost::fiber f1( & zero_args_fn, boost::fiber::default_stacksize);
	f1 = boost::fibers::make_fiber( zero_args_fn, boost::fiber::default_stacksize);
	boost::fiber f2( one_args_fn, 1, boost::fiber::default_stacksize);
	f2 = boost::fibers::make_fiber( one_args_fn, 1, boost::fiber::default_stacksize);
}

void test_case_4()
{
	boost::fiber f1( one_args_fn, 10, boost::fiber::default_stacksize);
	BOOST_CHECK( f1);
	boost::fiber f2( boost::move( f1) );
	BOOST_CHECK( ! f1);
	BOOST_CHECK( f2);
	boost::fiber f3;
	BOOST_CHECK( ! f3);
	f3 = f2;
	BOOST_CHECK( f3);
	BOOST_CHECK_EQUAL( f2, f3);
}

void test_case_5()
{
	boost::fiber f1( zero_args_fn, boost::fiber::default_stacksize);
	boost::fiber f2( zero_args_fn, boost::fiber::default_stacksize);
	boost::fiber f3;
	BOOST_CHECK( f1);
	BOOST_CHECK( f2);
	BOOST_CHECK( ! f3);

	BOOST_CHECK( f1 != f2);
	BOOST_CHECK( f1 != f3);
	BOOST_CHECK( f2 != f3);

	std::ostringstream os1;
	os1 << f1.get_id();
	std::ostringstream os2;
	os2 << f2.get_id();
	std::ostringstream os3;
	os3 << f3.get_id();

	std::string not_a_fiber("{not-a-fiber}");
	BOOST_CHECK( os1.str() != os2.str() );
	BOOST_CHECK( os1.str() != os3.str() );
	BOOST_CHECK( os2.str() != os3.str() );
	BOOST_CHECK( os1.str() != not_a_fiber);
	BOOST_CHECK( os2.str() != not_a_fiber);
	BOOST_CHECK( os3.str() == not_a_fiber);
}

void test_case_6()
{
	boost::fiber f1( zero_args_fn, boost::fiber::default_stacksize);
	boost::fiber f2( zero_args_fn, boost::fiber::default_stacksize);

	boost::fiber::id id1 = f1.get_id();
	boost::fiber::id id2 = f2.get_id();

	f1.swap( f2);

	BOOST_CHECK_EQUAL( f1.get_id(), id2);
	BOOST_CHECK_EQUAL( f2.get_id(), id1);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: fiber test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );
	test->add( BOOST_TEST_CASE( & test_case_2) );
	test->add( BOOST_TEST_CASE( & test_case_3) );
	test->add( BOOST_TEST_CASE( & test_case_4) );
	test->add( BOOST_TEST_CASE( & test_case_5) );
	test->add( BOOST_TEST_CASE( & test_case_6) );

	return test;
}
