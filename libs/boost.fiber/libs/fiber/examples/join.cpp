#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>

#include <boost/fiber.hpp>

int value1 = 0;
int value2 = 0;

void fn_1()
{
	for ( int i = 0; i < 5; ++i)
	{
		++value1;
		std::cout << "fn_1() increment value1 " << value1 << std::endl;
		boost::this_fiber::yield();
	}
}

void fn_2( boost::fiber f)
{
	for ( int i = 0; i < 5; ++i)
	{
		++value2;
		std::cout << "fn_2() increment value2 " << value2 << std::endl;
		if ( i == 1)
		{
			std::cout << "fn_2() join fiber " << f.get_id() << std::endl;
			f.join();
			std::cout << "fn_2() fiber " << f.get_id() << " joined" << std::endl;
		}
		boost::this_fiber::yield();
	}
}

int main()
{
	try
	{
		boost::fibers::scheduler<> sched;

		boost::fiber f( fn_1, boost::fiber::default_stacksize);
		sched.submit_fiber( f);
		sched.make_fiber( fn_2, f, boost::fiber::default_stacksize);

		std::cout << "start" << std::endl;
		std::cout << "fiber to be joined " << f.get_id() << std::endl;

		for (;;)
		{
			while ( sched.run() );
			if ( sched.empty() ) break;
		}

		std::cout << "finish: value1 == " << value1 << ", value2 == " << value2 << std::endl;

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
