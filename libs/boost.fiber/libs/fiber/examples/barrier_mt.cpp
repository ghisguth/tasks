#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/assert.hpp>
#include <boost/ref.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread.hpp>

#include <boost/fiber.hpp>

int value1 = 0;
int value2 = 0;

inline
void fn1( boost::fibers::spin::barrier & b)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();
	std::stringstream fss;
	fss << boost::this_fiber::get_id();

	fprintf( stderr, "start fn1 fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );

	++value1;
	fprintf( stderr, "fn1 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value1);

	fprintf( stderr, "fn1 fiber %s in thread %s before wait for barrier\n", fss.str().c_str(), tss.str().c_str());
	b.wait();
	fprintf( stderr, "fn1 fiber %s in thread %s after wait for barrier\n", fss.str().c_str(), tss.str().c_str());

	++value1;
	fprintf( stderr, "fn1 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value1);

	++value1;
	fprintf( stderr, "fn1 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value1);

	++value1;
	fprintf( stderr, "fn1 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value1);

	++value1;
	fprintf( stderr, "fn1 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value1);

	fprintf( stderr, "end fn1 fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );
}

inline
void fn2( boost::fibers::spin::barrier & b)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();
	std::stringstream fss;
	fss << boost::this_fiber::get_id();

	fprintf( stderr, "start fn2 fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );

	++value2;
	fprintf( stderr, "fn2 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value2);

	++value2;
	fprintf( stderr, "fn2 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value2);

	++value2;
	fprintf( stderr, "fn2 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value2);

	++value2;
	fprintf( stderr, "fn2 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value2);

	fprintf( stderr, "fn2 fiber %s in thread %s before wait for barrier\n", fss.str().c_str(), tss.str().c_str());
	b.wait();
	fprintf( stderr, "fn2 fiber %s in thread %s after wait for barrier\n", fss.str().c_str(), tss.str().c_str());

	++value2;
	fprintf( stderr, "fn2 fiber %s in thread %s increment %d\n", fss.str().c_str(), tss.str().c_str(), value2);

	fprintf( stderr, "end fn2 fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );
}

void f(
	boost::barrier & tb,
	boost::fibers::spin::barrier & fb)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();

	fprintf( stderr, "start thread %s\n", tss.str().c_str() );

	boost::fibers::scheduler<> sched;
	
	sched.make_fiber( & fn1, boost::ref( fb), boost::fiber::default_stacksize);

	tb.wait();

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	fprintf( stderr, "end thread %s\n", tss.str().c_str() );
}

void g(
	boost::barrier & tb,
	boost::fibers::spin::barrier & fb)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();

	fprintf( stderr, "start thread %s\n", tss.str().c_str() );

	boost::fibers::scheduler<> sched;
	
	sched.make_fiber( & fn2, boost::ref( fb), boost::fiber::default_stacksize);

	tb.wait();

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	fprintf( stderr, "end thread %s\n", tss.str().c_str() );
}

int main()
{
	try
	{
		boost::barrier tb( 2);
		boost::fibers::spin::barrier fb( 2);

		std::cout << "start" << std::endl;

		boost::thread th1( boost::bind( & f, boost::ref( tb), boost::ref( fb) ) );
		boost::thread th2( boost::bind( & g, boost::ref( tb), boost::ref( fb) ) );

		th1.join();
		th2.join();

		std::cout << "finish" << std::endl;

		return EXIT_SUCCESS;
	}
	catch ( boost::system::system_error const& e)
	{ std::cerr << "system_error: " << e.code().value() << std::endl; }
	catch ( boost::fibers::scheduler_error const& e)
	{ std::cerr << "scheduler_error: " << e.what() << std::endl; }
	catch ( std::exception const& e)
	{ std::cerr << "exception: " << e.what() << std::endl; }
	catch (...)
	{ std::cerr << "unhandled exception" << std::endl; }
	return EXIT_FAILURE;
}
