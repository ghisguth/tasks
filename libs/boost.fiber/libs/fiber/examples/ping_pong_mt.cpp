#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/assert.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/ref.hpp>
#include <boost/optional.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread.hpp>

#include <boost/fiber.hpp>

typedef boost::fibers::spin::unbounded_channel< std::string >	fifo_t;

inline
void ping(
		fifo_t & recv_buf,
		fifo_t & send_buf)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();
	std::stringstream fss;
	fss << boost::this_fiber::get_id();

	fprintf( stderr, "start ping fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );

	boost::optional< std::string > value;

	send_buf.put("ping");
	BOOST_ASSERT( recv_buf.take( value) );
	fprintf( stderr, "ping fiber %s in thread %s recevied %s\n", fss.str().c_str(), tss.str().c_str(), value->c_str());
	value.reset();

	send_buf.put("ping");
	BOOST_ASSERT( recv_buf.take( value) );
	fprintf( stderr, "ping fiber %s in thread %s recevied %s\n", fss.str().c_str(), tss.str().c_str(), value->c_str());
	value.reset();

	send_buf.put("ping");
	BOOST_ASSERT( recv_buf.take( value) );
	fprintf( stderr, "ping fiber %s in thread %s recevied %s\n", fss.str().c_str(), tss.str().c_str(), value->c_str());
	value.reset();

	send_buf.deactivate();

	fprintf( stderr, "end ping fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );
}

inline
void pong(
		fifo_t & recv_buf,
		fifo_t & send_buf)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();
	std::stringstream fss;
	fss << boost::this_fiber::get_id();

	fprintf( stderr, "start pong fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );

	boost::optional< std::string > value;

	BOOST_ASSERT( recv_buf.take( value) );
	fprintf( stderr, "pong fiber %s in thread %s recevied %s\n", fss.str().c_str(), tss.str().c_str(), value->c_str());
	value.reset();
	send_buf.put("pong");

	BOOST_ASSERT( recv_buf.take( value) );
	fprintf( stderr, "pong fiber %s in thread %s recevied %s\n", fss.str().c_str(), tss.str().c_str(), value->c_str());
	value.reset();
	send_buf.put("pong");

	BOOST_ASSERT( recv_buf.take( value) );
	fprintf( stderr, "pong fiber %s in thread %s recevied %s\n", fss.str().c_str(), tss.str().c_str(), value->c_str());
	value.reset();
	send_buf.put("pong");

	send_buf.deactivate();

	fprintf( stderr, "end pong fiber %s in thread %s\n", fss.str().c_str(), tss.str().c_str() );
}

void f(
		fifo_t & recv_buf,
		fifo_t & send_buf)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();

	fprintf( stderr, "start thread %s\n", tss.str().c_str() );

	boost::fibers::scheduler<> sched;
	
	sched.make_fiber( & ping, recv_buf, send_buf, boost::fiber::default_stacksize);

	for (;;)
	{
		while ( sched.run() );
		if ( sched.empty() ) break;
	}

	fprintf( stderr, "end thread %s\n", tss.str().c_str() );
}

void g(
		fifo_t & recv_buf,
		fifo_t & send_buf)
{
	std::stringstream tss;
	tss << boost::this_thread::get_id();

	fprintf( stderr, "start thread %s\n", tss.str().c_str() );

	boost::fibers::scheduler<> sched;
	
	sched.make_fiber( & pong, recv_buf, send_buf, boost::fiber::default_stacksize);

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
		fifo_t buf1;
		fifo_t buf2;

		std::cout << "start" << std::endl;

		boost::thread th1( boost::bind( & f, buf1, buf2) );
		boost::thread th2( boost::bind( & g, buf2, buf1) );

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
