
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/ref.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>

#include <boost/fiber.hpp>

int value = 0;

void notify_one_fn( boost::fibers::spin::condition & cond)
{
	cond.notify_one();
}

void notify_all_fn( boost::fibers::spin::condition & cond)
{
	cond.notify_all();
}

void wait_fn(
	boost::fibers::spin::mutex & mtx,
	boost::fibers::spin::condition & cond)
{
	boost::fibers::spin::mutex::scoped_lock lk( mtx);
	cond.wait( lk);
	++value;
}

void test_case_1()
{
	value = 0;
	boost::fibers::spin::mutex mtx;
	boost::fibers::spin::condition cond;
	boost::fibers::scheduler<> sched;

	sched.make_fiber(
		wait_fn,
		boost::ref( mtx),
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	sched.make_fiber(
		notify_one_fn,
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 1, value);
}

void test_case_2()
{
	value = 0;
	boost::fibers::spin::mutex mtx;
	boost::fibers::spin::condition cond;
	boost::fibers::scheduler<> sched;

	sched.make_fiber(
		wait_fn,
		boost::ref( mtx),
		boost::ref( cond),
		boost::fiber::default_stacksize);

	sched.make_fiber(
		wait_fn,
		boost::ref( mtx),
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	sched.make_fiber(
		notify_one_fn,
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 1, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 1, value);
}

void test_case_3()
{
	value = 0;
	boost::fibers::spin::mutex mtx;
	boost::fibers::spin::condition cond;
	boost::fibers::scheduler<> sched;

	sched.make_fiber(
		wait_fn,
		boost::ref( mtx),
		boost::ref( cond),
		boost::fiber::default_stacksize);

	sched.make_fiber(
		wait_fn,
		boost::ref( mtx),
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	sched.make_fiber(
		notify_all_fn,
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 3), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 2), sched.size() );
	BOOST_CHECK_EQUAL( 0, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 1, value);

	sched.make_fiber(
		wait_fn,
		boost::ref( mtx),
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 2, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 2, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 2, value);

	sched.make_fiber(
		notify_all_fn,
		boost::ref( cond),
		boost::fiber::default_stacksize);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 1), sched.size() );
	BOOST_CHECK_EQUAL( 2, value);

	BOOST_CHECK( sched.run() );
	BOOST_CHECK_EQUAL( std::size_t( 0), sched.size() );
	BOOST_CHECK_EQUAL( 3, value);
}

boost::unit_test::test_suite * init_unit_test_suite( int, char* [])
{
	boost::unit_test::test_suite * test =
		BOOST_TEST_SUITE("Boost.Fiber: spin-condition test suite");

	test->add( BOOST_TEST_CASE( & test_case_1) );
	test->add( BOOST_TEST_CASE( & test_case_2) );
	test->add( BOOST_TEST_CASE( & test_case_3) );

	return test;
}
