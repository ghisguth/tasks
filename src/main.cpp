/* main.cpp
 * This file is a part of tasks library
 * Copyright (c) tasks authors (see file `COPYRIGHT` for the license)
 */
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>
#include <boost/task.hpp>

typedef boost::shared_ptr<class Request> RequestPtr;
class Request
{
public:
	Request(const std::string & data)
		: data_(data)
	{}

	const std::string & Read() const
	{
		std::cout<<"\tRequest "<<this<<" Read "<<data_<<"\n";
		return data_;
	}

	void Write(const std::string & answer)
	{
		std::cout<<"\tRequest "<<this<<" Write "<<answer<<"\n";
	}

private:
	std::string data_;
};

class ExternalRequestHandler
{
public:
	ExternalRequestHandler(boost::asio::io_service & io_service)
		: timer_(io_service)
	{}

	void PerformExternalReqeust(const std::string & what, boost::tasks::spin::auto_reset_event * ev)
	{
		what_ = what;
		std::cout<<"\texternal request "<<what_<<" begin\n";
		timer_.expires_from_now(boost::posix_time::milliseconds(rand()%100 + 10));
		timer_.async_wait(boost::bind(&ExternalRequestHandler::HandleTimeout, this, ev));
	}

private:

	void HandleTimeout(boost::tasks::spin::auto_reset_event * ev)
	{
		std::cout<<"\texternal request "<<what_<<" end\n";
		ev->set();
	}

private:
	boost::asio::deadline_timer timer_;
	std::string what_;
};

typedef boost::shared_ptr<class RequestHandler> RequestHandlerPtr;
class RequestHandler
{
public:
	RequestHandler(boost::asio::io_service & io_service, const RequestPtr & request)
		: io_service_(io_service)
		, request_(request)
	{}

	void Process() const
	{
		std::cout<<"\tRequestHandler "<<request_.get()<<" exec begin\n";

		std::string data = request_->Read();

		Request("query1");

		boost::tasks::handle< void > subtask1(
			boost::tasks::async(
				boost::tasks::make_task( &RequestHandler::Subtask1, this )));
		boost::tasks::handle< void > subtask2(
			boost::tasks::async(
				boost::tasks::make_task( &RequestHandler::Subtask2, this )));

		boost::tasks::waitfor_all( subtask1, subtask2);

		Request("query5");

		request_->Write("some answer");

		std::cout<<"\tRequestHandler "<<request_.get()<<" exec end\n";
	}

private:

	void Subtask1() const
	{
		Request("query2");
		ExternalRequest("extquery1");
	}

	void Subtask2() const
	{
		Request("query3");
		ExternalRequest("extquery2");
		Request("query4");
	}

	void Request(const std::string & what) const
	{
		std::cout<<"\trequest "<<what<<" begin\n";
		boost::this_thread::sleep(boost::posix_time::milliseconds(rand()%100 + 10));
		std::cout<<"\trequest "<<what<<" end\n";
	}

	void ExternalRequest(const std::string & what) const
	{
		std::cout<<"\text request "<<what<<" begin\n";
		ExternalRequestHandler external_handler(io_service_);
		boost::tasks::spin::auto_reset_event ev;
		external_handler.PerformExternalReqeust(what, &ev);
		std::cout<<"\text request "<<what<<" wait for completion\n";
		ev.wait();
		std::cout<<"\text request "<<what<<" end\n";
	}

private:
	boost::asio::io_service & io_service_;
	RequestPtr request_;
};

void dummy_handler(const boost::system::error_code&) {}

int main()
{
	std::cout<<" * main\n";

	boost::tasks::static_pool< boost::tasks::unbounded_fifo > pool( boost::tasks::poolsize( 5) );

	std::cout<<" * creating asio::io_service\n";
	boost::asio::io_service io_service;

	// keep io_service busy
	boost::asio::deadline_timer dummy_timer(io_service);
	dummy_timer.expires_from_now(boost::posix_time::seconds(10));
	dummy_timer.async_wait(&dummy_handler);

	std::cout<<" * starting asio::io_service threads\n";
	boost::thread_group io_service_thread_pool;
	for(int i = 0; i < 3; ++i)
		io_service_thread_pool.create_thread(
			boost::bind(&boost::asio::io_service::run, &io_service)
		);

	std::cout<<" * creating request\n";
	RequestPtr request(new Request("some data"));
	std::cout<<" * creating handler\n";
	RequestHandlerPtr handler(new RequestHandler(io_service, request));


	std::cout<<" * creating task for handling request\n";
	boost::tasks::handle< void > request_processing(
		boost::tasks::async(
			boost::tasks::make_task( &RequestHandler::Process, handler ),
			pool));

	std::cout<<" * waiting task for colmpletion\n";
	request_processing.wait();

	std::cout<<" * terminationg io_service threads\n";
	io_service.stop();
	io_service_thread_pool.join_all();

	std::cout<<" * done\n";

	return 0;
}

