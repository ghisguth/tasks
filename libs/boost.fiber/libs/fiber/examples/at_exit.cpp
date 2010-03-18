#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>

#include <boost/fiber.hpp>

void at_exit()
{ std::cout << "at_exit()" << std::endl; }

void fn()
{
	boost::this_fiber::at_fiber_exit( at_exit);
	std::cout << "fn()" << std::endl;
}

int main()
{
	try
	{
		boost::fibers::scheduler<> sched;

		sched.make_fiber( fn, boost::fiber::default_stacksize);

		std::cout << "start" << std::endl;

		for (;;)
		{
			while ( sched.run() );
			if ( sched.empty() ) break;
		}

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
