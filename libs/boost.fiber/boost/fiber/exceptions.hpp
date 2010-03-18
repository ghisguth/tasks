
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_EXCEPTIONS_H
#define BOOST_FIBERS_EXCEPTIONS_H

#include <stdexcept>
#include <string>

#include <boost/config.hpp>

#include <boost/fiber/detail/config.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace fibers {

class fiber_error : public std::runtime_error
{
public:
	fiber_error( std::string const& msg) :
		std::runtime_error( msg)
	{}
};

class fiber_interrupted
{};

class fiber_moved : public std::logic_error
{
public:
    fiber_moved() :
		std::logic_error("fiber moved")
	{}
};

class invalid_watermark : public std::runtime_error
{
public:
    invalid_watermark() :
		std::runtime_error("invalid watermark")
	{}
};

class lock_error : public std::logic_error
{
public:
    lock_error() :
		std::logic_error("lock invalid")
	{}
};

class scheduler_error : public std::runtime_error
{
public:
    scheduler_error( std::string const& msg) :
		std::runtime_error( msg)
	{}
};

class future_uninitialized :
	public std::logic_error
{
public:
    future_uninitialized() :
        std::logic_error("Future Uninitialized")
    {}
};

class broken_promise :
    public std::logic_error
{
public:
    broken_promise() :
        std::logic_error("Broken promise")
    {}
};

class future_already_retrieved :
    public std::logic_error
{
public:
    future_already_retrieved() :
        std::logic_error("Future already retrieved")
    {}
};

class promise_already_satisfied :
    public std::logic_error
{
public:
    promise_already_satisfied() :
        std::logic_error("Promise already satisfied")
    {}
};

class task_already_started :
    public std::logic_error
{
public:
    task_already_started() :
        std::logic_error("Task already started")
    {}
};

class task_moved :
    public std::logic_error
{
public:
    task_moved() :
        std::logic_error("Task moved")
    {}
};

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_EXCEPTIONS_H
