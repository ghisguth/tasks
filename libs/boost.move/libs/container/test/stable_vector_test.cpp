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
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <functional>

#include <boost/container/stable_vector.hpp>
#include "check_equal_containers.hpp"
#include "movable_int.hpp"
#include "expand_bwd_test_allocator.hpp"
#include "expand_bwd_test_template.hpp"
#include "dummy_test_allocator.hpp"
#include "vector_test.hpp"

using namespace boost::container;

//Explicit instantiation to detect compilation errors
//template class stable_vector<test::movable_and_copyable_int, 
   //test::dummy_test_allocator<test::movable_and_copyable_int> >;

class recursive_vector
{
   public:
   int id_;
   stable_vector<recursive_vector> vector_;
};

void recursive_vector_test()//Test for recursive types
{
   stable_vector<recursive_vector> recursive_vector_vector;
}

int main()
{
   recursive_vector_test();
   {
      //Now test move semantics
      stable_vector<recursive_vector> original;
      stable_vector<recursive_vector> move_ctor(boost::move(original));
      stable_vector<recursive_vector> move_assign;
      move_assign = boost::move(move_ctor);
      move_assign.swap(original);
   }
   typedef stable_vector<int> MyVector;
   typedef stable_vector<test::movable_int> MyMoveVector;
   typedef stable_vector<test::movable_and_copyable_int> MyCopyMoveVector;

   if(test::vector_test<MyVector>())
      return 1;

   if(test::vector_test<MyMoveVector>())
      return 1;

   if(test::vector_test<MyCopyMoveVector>())
      return 1;

   const test::EmplaceOptions Options = (test::EmplaceOptions)(test::EMPLACE_BACK | test::EMPLACE_BEFORE);
   if(!boost::container::test::test_emplace
      < stable_vector<test::EmplaceInt>, Options>())
      return 1;

   return 0;
}

#include <boost/container/detail/config_end.hpp>
