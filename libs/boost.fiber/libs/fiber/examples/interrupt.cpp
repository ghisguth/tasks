#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>

#include <boost/fiber.hpp>

int value1 = 0;
int value2 = 0;
int value3 = 0;

void fn_1()
{
	for ( int i = 0; i < 10; ++i)
	{
		++value1;
		std::cout << "fn_1() increment value1 " << value1 << std::endl;
		boost::this_fiber::interruption_point();
		boost::this_fiber::yield();
	}
}

void fn_2()
{
	boost::this_fiber::disable_interruption disabled;
	for ( int i = 0; i < 10; ++i)
	{
		++value2;
		std::cout << "fn_2() increment value2 " << value2 << std::endl;
		boost::this_fiber::interruption_point();
		boost::this_fiber::yield();
	}
}

void fn_3()
{
	boost::this_fiber::disable_interruption disabled;
	for ( int i = 0; i < 10; ++i)
	{
		++value3;
		std::cout << "fn_3() increment value3 " << value3 << std::endl;
		boost::this_fiber::restore_interruption restored( disabled);
		boost::this_fiber::interruption_point();
		boost::this_fiber::yield();
	}
}

void fn_4( boost::fiber f)
{
	for ( int i = 0; i < 10; ++i)
	{
		if ( i == 1)
		{
			std::cout << "fn_4() interrupt fiber " << f.get_id() << std::endl;
			f.interrupt();
			break;
		}
		boost::this_fiber::yield();
	}
}

int main()
{
	try
	{
		boost::fibers::scheduler<> sched;

		boost::fiber f1( fn_1, boost::fiber::default_stacksize);
		sched.submit_fiber( f1);
		sched.make_fiber( fn_4, f1, boost::fiber::default_stacksize);

		std::cout << "start: interrupt fn_1()" << std::endl;

		for (;;)
		{
			while ( sched.run() );
			if ( sched.empty() ) break;
		}

		std::cout << "finish: value1 == " << value1 << std::endl;

		boost::fiber f2( fn_2, boost::fiber::default_stacksize);
		sched.submit_fiber( f2);
		sched.make_fiber( fn_4, f2, boost::fiber::default_stacksize);
		std::cout << "start: interrupt fn_2()" << std::endl;

		for (;;)
		{
			while ( sched.run() );
			if ( sched.empty() ) break;
		}

		std::cout << "finish: value2 == " << value2 << std::endl;

		boost::fiber f3( fn_3, boost::fiber::default_stacksize);
		sched.submit_fiber( f3);
		sched.make_fiber( fn_4, f3, boost::fiber::default_stacksize);
		std::cout << "start: interrupt fn_3()" << std::endl;

		for (;;)
		{
			while ( sched.run() );
			if ( sched.empty() ) break;
		}

		std::cout << "finish: value3 == " << value3 << std::endl;

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
