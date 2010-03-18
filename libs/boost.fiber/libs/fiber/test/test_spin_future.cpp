//  (C) Copyright 2008-9 Anthony Williams 
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "boost/fiber.hpp"
#include <utility>
#include <memory>
#include <string>

#include <boost/move/move.hpp>
#include <boost/test/unit_test.hpp>

template<typename T>
BOOST_RV_REF( T) cast_to_rval(T& t)
{
    return boost::move(t);
}

struct X
{
private:
    
   BOOST_MOVABLE_BUT_NOT_COPYABLE(X);

public:
    
    int i;
    
    X():
        i(42)
    {}
    X( BOOST_RV_REF( X) other):
        i(other.i)
    {
        other.i=0;
    }
    ~X()
    {}
};

int make_int()
{
    return 42;
}

int throw_runtime_error()
{
    throw std::runtime_error("42");
}

void set_promise_thread(boost::fibers::spin::promise<int>* p)
{
    p->set_value(42);
}

struct my_exception
{};

void set_promise_exception_thread(boost::fibers::spin::promise<int>* p)
{
    p->set_exception(boost::copy_exception(my_exception()));
}


void test_store_value_from_thread()
{
    boost::fibers::spin::promise<int> pi2;
    boost::fibers::spin::unique_future<int> fi2=pi2.get_future();
    boost::fibers::scheduler<> sched;
	sched.make_fiber(set_promise_thread,&pi2, boost::fiber::default_stacksize);
	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}
    int j=fi2.get();
    BOOST_CHECK(j==42);
    BOOST_CHECK(fi2.is_ready());
    BOOST_CHECK(fi2.has_value());
    BOOST_CHECK(!fi2.has_exception());
    BOOST_CHECK(fi2.get_state()==boost::fibers::future_state::ready);
}


void test_store_exception()
{
    boost::fibers::spin::promise<int> pi3;
    boost::fibers::spin::unique_future<int> fi3=pi3.get_future();
    boost::fibers::scheduler<> sched;
	sched.make_fiber(set_promise_exception_thread,&pi3, boost::fiber::default_stacksize);
	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}
    try
    {
        fi3.get();
        BOOST_CHECK(false);
    }
    catch(my_exception)
    {
        BOOST_CHECK(true);
    }
    
    BOOST_CHECK(fi3.is_ready());
    BOOST_CHECK(!fi3.has_value());
    BOOST_CHECK(fi3.has_exception());
    BOOST_CHECK(fi3.get_state()==boost::fibers::future_state::ready);
}

void test_initial_state()
{
    boost::fibers::spin::unique_future<int> fi;
    BOOST_CHECK(!fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::uninitialized);
    int i;
    try
    {
        i=fi.get();
        BOOST_CHECK(false);
    }
    catch(boost::fibers::future_uninitialized)
    {
        BOOST_CHECK(true);
    }
}

void test_waiting_future()
{
    boost::fibers::spin::promise<int> pi;
    boost::fibers::spin::unique_future<int> fi;
    fi=pi.get_future();

    int i=0;
    BOOST_CHECK(!fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::waiting);
    BOOST_CHECK(i==0);
}

void test_cannot_get_future_twice()
{
    boost::fibers::spin::promise<int> pi;
    pi.get_future();

    try
    {
        pi.get_future();
        BOOST_CHECK(false);
    }
    catch(boost::fibers::future_already_retrieved&)
    {
        BOOST_CHECK(true);
    }
}

void test_set_value_updates_future_state()
{
    boost::fibers::spin::promise<int> pi;
    boost::fibers::spin::unique_future<int> fi;
    fi=pi.get_future();

    pi.set_value(42);
    
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
}

void test_set_value_can_be_retrieved()
{
    boost::fibers::spin::promise<int> pi;
    boost::fibers::spin::unique_future<int> fi;
    fi=pi.get_future();

    pi.set_value(42);
    
    int i=0;
    BOOST_CHECK(i=fi.get());
    BOOST_CHECK(i==42);
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
}

void test_set_value_can_be_moved()
{
//     boost::fibers::spin::promise<int> pi;
//     boost::fibers::spin::unique_future<int> fi;
//     fi=pi.get_future();

//     pi.set_value(42);
    
//     int i=0;
//     BOOST_CHECK(i=fi.get());
//     BOOST_CHECK(i==42);
//     BOOST_CHECK(fi.is_ready());
//     BOOST_CHECK(fi.has_value());
//     BOOST_CHECK(!fi.has_exception());
//     BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
}

void test_future_from_packaged_task_is_waiting()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();
    int i=0;
    BOOST_CHECK(!fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::waiting);
    BOOST_CHECK(i==0);
}

void test_invoking_a_packaged_task_populates_future()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();

    pt();

    int i=0;
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(i=fi.get());
    BOOST_CHECK(i==42);
}

void test_invoking_a_packaged_task_twice_throws()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);

    pt();
    try
    {
        pt();
        BOOST_CHECK(false);
    }
    catch(boost::fibers::task_already_started)
    {
        BOOST_CHECK(true);
    }
}


void test_cannot_get_future_twice_from_task()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);
    pt.get_future();
    try
    {
        pt.get_future();
        BOOST_CHECK(false);
    }
    catch(boost::fibers::future_already_retrieved)
    {
        BOOST_CHECK(true);
    }
}

void test_task_stores_exception_if_function_throws()
{
    boost::fibers::spin::packaged_task<int> pt(throw_runtime_error);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();

    pt();

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
    try
    {
        fi.get();
        BOOST_CHECK(false);
    }
    catch(std::exception&)
    {
        BOOST_CHECK(true);
    }
    catch(...)
    {
        BOOST_CHECK(!"Unknown exception thrown");
    }
    
}

void test_void_promise()
{
    boost::fibers::spin::promise<void> p;
    boost::fibers::spin::unique_future<void> f=p.get_future();
    p.set_value();
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_value());
    BOOST_CHECK(!f.has_exception());
    BOOST_CHECK(f.get_state()==boost::fibers::future_state::ready);
    f.get();
}

void test_reference_promise()
{
    boost::fibers::spin::promise<int&> p;
    boost::fibers::spin::unique_future<int&> f=p.get_future();
    int i=42;
    p.set_value(i);
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_value());
    BOOST_CHECK(!f.has_exception());
    BOOST_CHECK(f.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(&f.get()==&i);
}

void do_nothing()
{}

void test_task_returning_void()
{
    boost::fibers::spin::packaged_task<void> pt(do_nothing);
    boost::fibers::spin::unique_future<void> fi=pt.get_future();

    pt();

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
}

int global_ref_target=0;

int& return_ref()
{
    return global_ref_target;
}

void test_task_returning_reference()
{
    boost::fibers::spin::packaged_task<int&> pt(return_ref);
    boost::fibers::spin::unique_future<int&> fi=pt.get_future();

    pt();

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::ready);
    int& i=fi.get();
    BOOST_CHECK(&i==&global_ref_target);
}

void test_shared_future()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();

    boost::fibers::spin::shared_future<int> sf(::cast_to_rval(fi));
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::uninitialized);

    pt();

    int i=0;
    BOOST_CHECK(sf.is_ready());
    BOOST_CHECK(sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(i=sf.get());
    BOOST_CHECK(i==42);
}

void test_copies_of_shared_future_become_ready_together()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();

    boost::fibers::spin::shared_future<int> sf(::cast_to_rval(fi));
    boost::fibers::spin::shared_future<int> sf2(sf);
    boost::fibers::spin::shared_future<int> sf3;
    sf3=sf;
    BOOST_CHECK(sf.get_state()==boost::fibers::future_state::waiting);
    BOOST_CHECK(sf2.get_state()==boost::fibers::future_state::waiting);
    BOOST_CHECK(sf3.get_state()==boost::fibers::future_state::waiting);

    pt();

    int i=0;
    BOOST_CHECK(sf.is_ready());
    BOOST_CHECK(sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(i=sf.get());
    BOOST_CHECK(i==42);
    i=0;
    BOOST_CHECK(sf2.is_ready());
    BOOST_CHECK(sf2.has_value());
    BOOST_CHECK(!sf2.has_exception());
    BOOST_CHECK(sf2.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(i=sf2.get());
    BOOST_CHECK(i==42);
    i=0;
    BOOST_CHECK(sf3.is_ready());
    BOOST_CHECK(sf3.has_value());
    BOOST_CHECK(!sf3.has_exception());
    BOOST_CHECK(sf3.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(i=sf3.get());
    BOOST_CHECK(i==42);
}

void test_shared_future_can_be_move_assigned_from_unique_future()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();

    boost::fibers::spin::shared_future<int> sf;
    sf=::cast_to_rval(fi);
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::uninitialized);

    BOOST_CHECK(!sf.is_ready());
    BOOST_CHECK(!sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::fibers::future_state::waiting);
}

void test_shared_future_void()
{
    boost::fibers::spin::packaged_task<void> pt(do_nothing);
    boost::fibers::spin::unique_future<void> fi=pt.get_future();

    boost::fibers::spin::shared_future<void> sf(::cast_to_rval(fi));
    BOOST_CHECK(fi.get_state()==boost::fibers::future_state::uninitialized);

    pt();

    BOOST_CHECK(sf.is_ready());
    BOOST_CHECK(sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::fibers::future_state::ready);
    sf.get();
}

void test_shared_future_ref()
{
    boost::fibers::spin::promise<int&> p;
    boost::fibers::spin::shared_future<int&> f(p.get_future());
    int i=42;
    p.set_value(i);
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_value());
    BOOST_CHECK(!f.has_exception());
    BOOST_CHECK(f.get_state()==boost::fibers::future_state::ready);
    BOOST_CHECK(&f.get()==&i);
}

void test_can_get_a_second_future_from_a_moved_promise()
{
    boost::fibers::spin::promise<int> pi;
    boost::fibers::spin::unique_future<int> fi=pi.get_future();
    
    boost::fibers::spin::promise<int> pi2(::cast_to_rval(pi));
    boost::fibers::spin::unique_future<int> fi2=pi.get_future();

    pi2.set_value(3);
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(!fi2.is_ready());
    BOOST_CHECK(fi.get()==3);
    pi.set_value(42);
    BOOST_CHECK(fi2.is_ready());
    BOOST_CHECK(fi2.get()==42);
}

void test_can_get_a_second_future_from_a_moved_void_promise()
{
    boost::fibers::spin::promise<void> pi;
    boost::fibers::spin::unique_future<void> fi=pi.get_future();
    
    boost::fibers::spin::promise<void> pi2(::cast_to_rval(pi));
    boost::fibers::spin::unique_future<void> fi2=pi.get_future();

    pi2.set_value();
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(!fi2.is_ready());
    pi.set_value();
    BOOST_CHECK(fi2.is_ready());
}

void test_unique_future_for_move_only_udt()
{
    boost::fibers::spin::promise<X> pt;
    boost::fibers::spin::unique_future<X> fi=pt.get_future();

    pt.set_value(X());
    X res(fi.get());
    BOOST_CHECK(res.i==42);
}

void test_unique_future_for_string()
{
    boost::fibers::spin::promise<std::string> pt;
    boost::fibers::spin::unique_future<std::string> fi=pt.get_future();

    pt.set_value(std::string("hello"));
    std::string res(fi.get());
    BOOST_CHECK(res=="hello");

    boost::fibers::spin::promise<std::string> pt2;
    fi=pt2.get_future();

    std::string const s="goodbye";
    
    pt2.set_value(s);
    res=fi.get();
    BOOST_CHECK(res=="goodbye");

    boost::fibers::spin::promise<std::string> pt3;
    fi=pt3.get_future();

    std::string s2="foo";
    
    pt3.set_value(s2);
    res=fi.get();
    BOOST_CHECK(res=="foo");
}

boost::fibers::spin::mutex callback_mutex;
unsigned callback_called=0;

void wait_callback(boost::fibers::spin::promise<int>& pi)
{
    boost::lock_guard<boost::fibers::spin::mutex> lk(callback_mutex);
    ++callback_called;
    try
    {
        pi.set_value(42);
    }
    catch(...)
    {
    }
}

void do_nothing_callback(boost::fibers::spin::promise<int>& pi)
{
    boost::lock_guard<boost::fibers::spin::mutex> lk(callback_mutex);
    ++callback_called;
}

void test_wait_callback()
{
    callback_called=0;
    boost::fibers::spin::promise<int> pi;
    boost::fibers::spin::unique_future<int> fi=pi.get_future();
    pi.set_wait_callback(wait_callback);
    fi.wait();
    BOOST_CHECK(callback_called);
    BOOST_CHECK(fi.get()==42);
    fi.wait();
    fi.wait();
    BOOST_CHECK(callback_called==1);
}

void wait_callback_for_task(boost::fibers::spin::packaged_task<int>& pt)
{
    boost::lock_guard<boost::fibers::spin::mutex> lk(callback_mutex);
    ++callback_called;
    try
    {
        pt();
    }
    catch(...)
    {
    }
}


void test_wait_callback_for_packaged_task()
{
    callback_called=0;
    boost::fibers::spin::packaged_task<int> pt(make_int);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();
    pt.set_wait_callback(wait_callback_for_task);
    fi.wait();
    BOOST_CHECK(callback_called);
    BOOST_CHECK(fi.get()==42);
    fi.wait();
    fi.wait();
    BOOST_CHECK(callback_called==1);
}

void test_packaged_task_can_be_moved()
{
    boost::fibers::spin::packaged_task<int> pt(make_int);

    boost::fibers::spin::unique_future<int> fi=pt.get_future();

    BOOST_CHECK(!fi.is_ready());
    
    boost::fibers::spin::packaged_task<int> pt2(::cast_to_rval(pt));

    BOOST_CHECK(!fi.is_ready());
    try
    {
        pt();
        BOOST_CHECK(!"Can invoke moved task!");
    }
    catch(boost::fibers::task_moved&)
    {
    }

    BOOST_CHECK(!fi.is_ready());

    pt2();
    
    BOOST_CHECK(fi.is_ready());
}

void test_destroying_a_promise_stores_broken_promise()
{
    boost::fibers::spin::unique_future<int> f;
    
    {
        boost::fibers::spin::promise<int> p;
        f=p.get_future();
    }
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_exception());
    try
    {
        f.get();
    }
    catch(boost::fibers::broken_promise&)
    {
    }
}

void test_destroying_a_packaged_task_stores_broken_promise()
{
    boost::fibers::spin::unique_future<int> f;
    
    {
        boost::fibers::spin::packaged_task<int> p(make_int);
        f=p.get_future();
    }
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_exception());
    try
    {
        f.get();
    }
    catch(boost::fibers::broken_promise&)
    {
    }
}

int make_int_slowly()
{
//    boost::this_thread::sleep(boost::posix_time::seconds(1));
    return 42;
}

void test_wait_for_either_of_two_futures_1()
{
    boost::fibers::spin::packaged_task<int> pt(make_int_slowly);
    boost::fibers::spin::unique_future<int> f1(pt.get_future());
    boost::fibers::spin::packaged_task<int> pt2(make_int_slowly);
    boost::fibers::spin::unique_future<int> f2(pt2.get_future());
    
    boost::fibers::scheduler<> sched;
	sched.make_fiber(::cast_to_rval(pt), boost::fiber::default_stacksize);
	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}
    
    unsigned const future=boost::fibers::spin::wait_for_any(f1,f2);
    
    BOOST_CHECK(future==0);
    BOOST_CHECK(f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(f1.get()==42);
}
/*
void test_wait_for_either_invokes_callbacks()
{
    callback_called=0;
    boost::fibers::spin::packaged_task<int> pt(make_int_slowly);
    boost::fibers::spin::unique_future<int> fi=pt.get_future();
    boost::fibers::spin::packaged_task<int> pt2(make_int_slowly);
    boost::fibers::spin::unique_future<int> fi2=pt2.get_future();
    pt.set_wait_callback(wait_callback_for_task);

    boost::thread(::cast_to_rval(pt));
    
    boost::fibers::spin::wait_for_any(fi,fi2);
    BOOST_CHECK(callback_called==1);
    BOOST_CHECK(fi.get()==42);
}

void test_wait_for_any_from_range()
{
    unsigned const count=10;
    for(unsigned i=0;i<count;++i)
    {
        boost::fibers::spin::packaged_task<int> tasks[count];
        boost::fibers::spin::unique_future<int> futures[count];
        for(unsigned j=0;j<count;++j)
        {
            tasks[j]=boost::fibers::spin::packaged_task<int>(make_int_slowly);
            futures[j]=tasks[j].get_future();
        }
        boost::thread(::cast_to_rval(tasks[i]));
    
        boost::fibers::spin::unique_future<int>* const future=boost::fibers::spin::wait_for_any(futures,futures+count);
    
        BOOST_CHECK(future==(futures+i));
        for(unsigned j=0;j<count;++j)
        {
            if(j!=i)
            {
                BOOST_CHECK(!futures[j].is_ready());
            }
            else
            {
                BOOST_CHECK(futures[j].is_ready());
            }
        }
        BOOST_CHECK(futures[i].get()==42);
    }
}

void test_wait_for_all_from_range()
{
    unsigned const count=10;
    boost::fibers::spin::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::fibers::spin::packaged_task<int> task(make_int_slowly);
        futures[j]=task.get_future();
        boost::thread(::cast_to_rval(task));
    }
    
    boost::fibers::spin::wait_for_all(futures,futures+count);
    
    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}

void test_wait_for_all_two_futures()
{
    unsigned const count=2;
    boost::fibers::spin::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::fibers::spin::packaged_task<int> task(make_int_slowly);
        futures[j]=task.get_future();
        boost::thread(::cast_to_rval(task));
    }
    
    boost::fibers::spin::wait_for_all(futures[0],futures[1]);
    
    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}
*/

boost::unit_test_framework::test_suite* init_unit_test_suite(int, char*[])
{
    boost::unit_test_framework::test_suite* test =
        BOOST_TEST_SUITE("Boost.Threads: futures test suite");

    test->add(BOOST_TEST_CASE(test_initial_state));
    test->add(BOOST_TEST_CASE(test_waiting_future));
    test->add(BOOST_TEST_CASE(test_cannot_get_future_twice));
    test->add(BOOST_TEST_CASE(test_set_value_updates_future_state));
    test->add(BOOST_TEST_CASE(test_set_value_can_be_retrieved));
    test->add(BOOST_TEST_CASE(test_set_value_can_be_moved));
    test->add(BOOST_TEST_CASE(test_store_value_from_thread));
    test->add(BOOST_TEST_CASE(test_store_exception));
    test->add(BOOST_TEST_CASE(test_future_from_packaged_task_is_waiting));
    test->add(BOOST_TEST_CASE(test_invoking_a_packaged_task_populates_future));
    test->add(BOOST_TEST_CASE(test_invoking_a_packaged_task_twice_throws));
    test->add(BOOST_TEST_CASE(test_cannot_get_future_twice_from_task));
    test->add(BOOST_TEST_CASE(test_task_stores_exception_if_function_throws));
    test->add(BOOST_TEST_CASE(test_void_promise));
    test->add(BOOST_TEST_CASE(test_reference_promise));
    test->add(BOOST_TEST_CASE(test_task_returning_void));
    test->add(BOOST_TEST_CASE(test_task_returning_reference));
    test->add(BOOST_TEST_CASE(test_shared_future));
    test->add(BOOST_TEST_CASE(test_copies_of_shared_future_become_ready_together));
    test->add(BOOST_TEST_CASE(test_shared_future_can_be_move_assigned_from_unique_future));
    test->add(BOOST_TEST_CASE(test_shared_future_void));
    test->add(BOOST_TEST_CASE(test_shared_future_ref));
    test->add(BOOST_TEST_CASE(test_can_get_a_second_future_from_a_moved_promise));
    test->add(BOOST_TEST_CASE(test_can_get_a_second_future_from_a_moved_void_promise));
    test->add(BOOST_TEST_CASE(test_unique_future_for_move_only_udt));
    test->add(BOOST_TEST_CASE(test_unique_future_for_string));
    test->add(BOOST_TEST_CASE(test_wait_callback));
    test->add(BOOST_TEST_CASE(test_wait_callback_for_packaged_task));
    test->add(BOOST_TEST_CASE(test_packaged_task_can_be_moved));
    test->add(BOOST_TEST_CASE(test_destroying_a_promise_stores_broken_promise));
    test->add(BOOST_TEST_CASE(test_destroying_a_packaged_task_stores_broken_promise));
//  test->add(BOOST_TEST_CASE(test_wait_for_either_of_two_futures_1));
//  test->add(BOOST_TEST_CASE(test_wait_for_either_invokes_callbacks));
//  test->add(BOOST_TEST_CASE(test_wait_for_any_from_range));
//  test->add(BOOST_TEST_CASE(test_wait_for_all_from_range));
//  test->add(BOOST_TEST_CASE(test_wait_for_all_two_futures));

    return test;
}
