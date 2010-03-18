#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread.hpp>

#include <boost/fiber.hpp>
#include <boost/fiber/spin/future.hpp>

inline
std::string helloworld_fn()
{ return "Hello World"; }

int main()
{
	try
	{
		boost::fibers::scheduler<> sched;

		boost::fibers::spin::packaged_task< std::string > pt( helloworld_fn);
		boost::fibers::spin::unique_future< std::string > fu = pt.get_future();
		sched.make_fiber( boost::move( pt), boost::fiber::default_stacksize);

		for (;;)
		{
			while ( sched.run() );
			if ( sched.empty() ) break;
		}

		std::cout << fu.get() << std::endl;

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
