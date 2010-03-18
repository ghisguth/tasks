#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread.hpp>

#include <boost/fiber.hpp>

void g( std::string const& str, int n)
{
	for ( int i = 0; i < n; ++i)
	{
		std::ostringstream os1;
		std::ostringstream os2;
		os1	<< boost::this_thread::get_id();
		os2 << boost::this_fiber::get_id();
		fprintf( stderr, "(thread: %s, fiber: %s) %d: %s\n", os1.str().c_str(), os2.str().c_str(), i, str.c_str() );
		boost::this_fiber::yield();
	}
}

void fn1(
		boost::fiber::id const& id,
		boost::barrier & b,
		boost::fibers::scheduler<> & sched2,
		std::string const& msg, int n)
{
		std::ostringstream os;
		os << boost::this_thread::get_id();
		fprintf( stderr, "start (thread1: %s)\n", os.str().c_str() );

		boost::fibers::scheduler<> sched1;
		sched1.make_fiber( & g, msg, n, boost::fiber::default_stacksize);

		for ( int i = 0; i < 2; ++i)
			sched1.run();

		b.wait();

		sched1.migrate_fiber( id, sched2);

		b.wait();

		std::ostringstream id_os;
		id_os << id;

		fprintf( stderr, "thread1: fiber %s migrated\n", id_os.str().c_str() );
		fprintf( stderr, "thread1: scheduler runs %d fibers\n", static_cast< int >( sched1.size() ) );

		for (;;)
		{
			while ( sched1.run() );
			if ( sched1.empty() ) break;
		}

		fprintf( stderr, "finish (thread1: %s)\n", os.str().c_str() );
}

void fn2(
		boost::fiber & f,
		boost::barrier & b,
		boost::fibers::scheduler<> & sched)
{
		std::ostringstream os;
		os << boost::this_thread::get_id();
		fprintf( stderr, "start (thread2: %s)\n", os.str().c_str() );

		sched.submit_fiber( f);

		sched.run();
		sched.run();

		b.wait();
		b.wait();

		fprintf( stderr, "thread2: scheduler runs %d fibers\n", static_cast< int >( sched.size() ) );

		fprintf( stderr, "finish (thread2: %s)\n", os.str().c_str() );
}

int main()
{
	try
	{
		boost::fibers::scheduler<> sched;
		boost::barrier b( 2);

		boost::fiber f( & g, "xyz", 4, boost::fiber::default_stacksize);

		std::cout << "start" << std::endl;

		boost::thread th1(
				fn1,
				f.get_id(),
				boost::ref( b),
				boost::ref( sched),
				"abc", 5);
		boost::thread th2(
				fn2,
				f,
				boost::ref( b),
				boost::ref( sched) );

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
