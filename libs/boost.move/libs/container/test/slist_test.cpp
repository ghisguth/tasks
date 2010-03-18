//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2007. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <boost/container/detail/config_begin.hpp>
#include <boost/container/slist.hpp>
#include "dummy_test_allocator.hpp"
#include "list_test.hpp"
#include "movable_int.hpp"
#include "emplace_test.hpp"

using namespace boost::container;

//Explicit instantiation to detect compilation errors
template class boost::container::slist<test::movable_and_copyable_int, 
   test::dummy_test_allocator<test::movable_and_copyable_int> >;

typedef slist<int> MyList;
typedef slist<volatile int> MyVolatileList;
typedef slist<test::movable_int> MyMoveList;
typedef slist<test::movable_and_copyable_int> MyCopyMoveList;

class recursive_slist
{
public:
   int id_;
   slist<recursive_slist> slist_;
};

void recursive_slist_test()//Test for recursive types
{
   slist<recursive_slist> recursive_list_list;
}

int main ()
{
   recursive_slist_test();
   {
      //Now test move semantics
      slist<recursive_slist> original;
      slist<recursive_slist> move_ctor(boost::move(original));
      slist<recursive_slist> move_assign;
      move_assign = boost::move(move_ctor);
      move_assign.swap(original);
   }

   if(test::list_test<MyList, false>())
      return 1;

   if(test::list_test<MyMoveList, false>())
      return 1;

   if(test::list_test<MyCopyMoveList, false>())
      return 1;

   if(test::list_test<MyVolatileList, false>())
      return 1;

   const test::EmplaceOptions Options = (test::EmplaceOptions)
      (test::EMPLACE_FRONT | test::EMPLACE_AFTER | test::EMPLACE_BEFORE  | test::EMPLACE_AFTER);

   if(!boost::container::test::test_emplace
      < slist<test::EmplaceInt>, Options>())
      return 1;
}

#include <boost/container/detail/config_end.hpp>

