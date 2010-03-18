
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FIBERS_BOUNDED_CHANNEL_H
#define BOOST_FIBERS_BOUNDED_CHANNEL_H

#include <cstddef>
#include <stdexcept>

#include <boost/intrusive_ptr.hpp>
#include <boost/config.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

#include <boost/fiber/condition.hpp>
#include <boost/fiber/exceptions.hpp>
#include <boost/fiber/mutex.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace fibers {
namespace detail {

template< typename T >
class bounded_channel_base : private noncopyable
{
public:
	typedef optional< T >	value_type;

private:
	struct node
	{
		typedef intrusive_ptr< node >	ptr;

		std::size_t		use_count;
		value_type		va;
		ptr				next;

		node() :
			use_count( 0),
			va(),
			next()
		{}

		friend void intrusive_ptr_add_ref( node * p)
		{ ++p->use_count; }
		
		friend void intrusive_ptr_release( node * p)
		{ if ( --p->use_count == 0) delete p; }
	};

	enum state
	{
		ACTIVE = 0,
		DEACTIVE
	};

	state				state_;
	std::size_t			count_;
	typename node::ptr	head_;
	mutable mutex		head_mtx_;
	typename node::ptr	tail_;
	mutable mutex		tail_mtx_;
	condition			not_empty_cond_;
	condition			not_full_cond_;
	std::size_t			hwm_;
	std::size_t			lwm_;
	std::size_t			use_count_;

	bool active_() const
	{ return ACTIVE == state_; }

	void deactivate_()
	{ state_ = DEACTIVE; }

	std::size_t size_() const
	{ return count_; }

	bool empty_() const
	{ return head_ == get_tail_(); }

	bool full_() const
	{ return size_() >= hwm_; }

	typename node::ptr get_tail_() const
	{
		mutex::scoped_lock lk( tail_mtx_);	
		typename node::ptr tmp = tail_;
		return tmp;
	}

	typename node::ptr pop_head_()
	{
		typename node::ptr old_head = head_;
		head_ = old_head->next;
		--count_;
		return old_head;
	}

public:
	template< typename Strategy >
	bounded_channel_base(
			scheduler< Strategy > & sched,
			std::size_t hwm,
			std::size_t lwm) :
		state_( ACTIVE),
		count_( 0),
		head_( new node() ),
		head_mtx_( sched),
		tail_( head_),
		tail_mtx_( sched),
		not_empty_cond_( sched),
		not_full_cond_( sched),
		hwm_( hwm),
		lwm_( lwm),
		use_count_( 0)
	{
		if ( hwm_ < lwm_)
			throw invalid_watermark();
	}

	template< typename Strategy >
	bounded_channel_base(
			scheduler< Strategy > & sched,
			std::size_t wm) :
		state_( ACTIVE),
		count_( 0),
		head_( new node() ),
		head_mtx_( sched),
		tail_( head_),
		tail_mtx_( sched),
		not_empty_cond_( sched),
		not_full_cond_( sched),
		hwm_( wm),
		lwm_( wm),
		use_count_( 0)
	{}

	std::size_t upper_bound() const
	{ return hwm_; }

	std::size_t lower_bound() const
	{ return lwm_; }

	void deactivate()
	{
		mutex::scoped_lock head_lk( head_mtx_);
		mutex::scoped_lock tail_lk( tail_mtx_);
		deactivate_();
		not_empty_cond_.notify_all();
		not_full_cond_.notify_all();
	}

	bool empty() const
	{
		mutex::scoped_lock lk( head_mtx_);
		return empty_();
	}

	void put( T const& t)
	{
		typename node::ptr new_node( new node() );
		{
			mutex::scoped_lock lk( tail_mtx_);

			if ( full_() )
			{
				while ( active_() && full_() )
					not_full_cond_.wait( lk);
			}

			if ( ! active_() )
				throw std::runtime_error("queue is not active");

			tail_->va = t;
			tail_->next = new_node;
			tail_ = new_node;
			++count_;
		}
		not_empty_cond_.notify_one();
	}

	bool take( value_type & va)
	{
		mutex::scoped_lock lk( head_mtx_);
		bool empty = empty_();
		if ( ! active_() && empty)
			return false;
		if ( empty)
		{
			try
			{
				while ( active_() && empty_() )
					not_empty_cond_.wait( lk);
			}
			catch ( fiber_interrupted const&)
			{ return false; }
		}
		if ( ! active_() && empty_() )
			return false;
		swap( va, head_->va);
		pop_head_();
		if ( size_() <= lwm_)
		{
			if ( lwm_ == hwm_)
				not_full_cond_.notify_one();
			else
				// more than one producer could be waiting
				// for submiting an action object
				not_full_cond_.notify_all();
		}
		return va;
	}

	bool try_take( value_type & va)
	{
		mutex::scoped_lock lk( head_mtx_);
		if ( empty_() )
			return false;
		swap( va, head_->va);
		pop_head_();
		bool valid = va;
		if ( valid && size_() <= lwm_)
		{
			if ( lwm_ == hwm_)
				not_full_cond_.notify_one();
			else
				// more than one producer could be waiting
				// in order to submit an task
				not_full_cond_.notify_all();
		}
		return valid;
	}

	friend void intrusive_ptr_add_ref( bounded_channel_base * p)
	{ ++( p->use_count_); }

	friend void intrusive_ptr_release( bounded_channel_base * p)
	{ if ( --( p->use_count_) == 1) delete p; }
};

}

template< typename T >
class bounded_channel
{
private:
	intrusive_ptr< detail::bounded_channel_base< T > >	base_;

public:
	typedef typename detail::bounded_channel_base< T >::value_type	value_type;

	template< typename Strategy >
	bounded_channel(
			scheduler< Strategy > & sched,
			unsigned int hwm,
			unsigned int lwm) :
		base_( new detail::bounded_channel_base< T >( sched, hwm, lwm) )
	{}
	
	template< typename Strategy >
	bounded_channel(
			scheduler< Strategy > & sched,
			unsigned int wm) :
		base_( new detail::bounded_channel_base< T >( sched, wm) )
	{}
	
	void upper_bound( std::size_t hwm)
	{ base_->upper_bound( hwm); }
	
	std::size_t upper_bound() const
	{ return base_->upper_bound(); }
	
	void lower_bound( std::size_t lwm)
	{ base_->lower_bound( lwm); }
	
	std::size_t lower_bound() const
	{ return base_->lower_bound(); }
	
	void deactivate()
	{ base_->deactivate(); }
	
	bool empty() const
	{ return base_->empty(); }
	
	void put( T const& t)
	{ base_->put( t); }
	
	bool take( value_type & va)
	{ return base_->take( va);}
	
	bool try_take( value_type & va)
	{ return base_->try_take( va); }
};

}}

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_FIBERS_BOUNDED_CHANNEL_H
