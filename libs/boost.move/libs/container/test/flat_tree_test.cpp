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
#include <set>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include "print_container.hpp"
#include "dummy_test_allocator.hpp"
#include "movable_int.hpp"
#include "set_test.hpp"
#include "map_test.hpp"
#include "emplace_test.hpp"

/////////////////////////////////////////////////////////////////
//                                                               
//  This example repeats the same operations with std::set and   
//  shmem_set using the node allocator                           
//  and compares the values of both containers                   
//                                                               
/////////////////////////////////////////////////////////////////

using namespace boost::container;
/*
//Explicit instantiation to detect compilation errors
template class boost::container::flat_set
   <test::movable_and_copyable_int
   ,std::less<test::movable_and_copyable_int>
   ,test::dummy_test_allocator<test::movable_and_copyable_int> >;

template class boost::container::flat_map
   <test::movable_and_copyable_int
   ,test::movable_and_copyable_int
   ,std::less<test::movable_and_copyable_int>
   ,test::dummy_test_allocator<std::pair<test::movable_and_copyable_int
                                        ,test::movable_and_copyable_int> > >;

template class boost::container::flat_multiset
   <test::movable_and_copyable_int
   ,std::less<test::movable_and_copyable_int>
   ,test::dummy_test_allocator<test::movable_and_copyable_int> >;

template class boost::container::flat_multimap
   <test::movable_and_copyable_int
   ,test::movable_and_copyable_int
   ,std::less<test::movable_and_copyable_int>
   ,test::dummy_test_allocator<std::pair<test::movable_and_copyable_int
                                        ,test::movable_and_copyable_int> > >;
*/
//Alias allocator type
typedef std::allocator<int> shmem_allocator_t;
typedef std::allocator<test::movable_int> 
   shmem_movable_allocator_t;
typedef std::allocator<std::pair<int, int> > 
   shmem_pair_allocator_t;
typedef std::allocator<std::pair<test::movable_int, test::movable_int> > 
   shmem_movable_pair_allocator_t;
typedef std::allocator<test::movable_and_copyable_int > 
   shmem_move_copy_allocator_t;
typedef std::allocator<std::pair<test::movable_and_copyable_int, test::movable_and_copyable_int> > 
   shmem_move_copy_pair_allocator_t;

//Alias set types
typedef std::set<int>                                                   MyStdSet;
typedef std::multiset<int>                                              MyStdMultiSet;
typedef std::map<int, int>                                              MyStdMap;
typedef std::multimap<int, int>                                         MyStdMultiMap;

typedef flat_set<int, std::less<int>, shmem_allocator_t>                MyShmSet;
typedef flat_multiset<int, std::less<int>, shmem_allocator_t>           MyShmMultiSet;
typedef flat_map<int, int, std::less<int>, shmem_pair_allocator_t>      MyShmMap;
typedef flat_multimap<int, int, std::less<int>, shmem_pair_allocator_t> MyShmMultiMap;

typedef flat_set<test::movable_int, std::less<test::movable_int>
                ,shmem_movable_allocator_t>                             MyMovableShmSet;
typedef flat_multiset<test::movable_int,std::less<test::movable_int>
                     ,shmem_movable_allocator_t>                        MyMovableShmMultiSet;
typedef flat_map<test::movable_int, test::movable_int
                ,std::less<test::movable_int>
                ,shmem_movable_pair_allocator_t>                        MyMovableShmMap;
typedef flat_multimap<test::movable_int, test::movable_int
                ,std::less<test::movable_int>
                ,shmem_movable_pair_allocator_t>                        MyMovableShmMultiMap;

typedef flat_set<test::movable_and_copyable_int, std::less<test::movable_and_copyable_int>
                ,shmem_move_copy_allocator_t>                             MyMoveCopyShmSet;
typedef flat_multiset<test::movable_and_copyable_int,std::less<test::movable_and_copyable_int>
                     ,shmem_move_copy_allocator_t>                        MyMoveCopyShmMultiSet;
typedef flat_map<test::movable_and_copyable_int, test::movable_and_copyable_int
                ,std::less<test::movable_and_copyable_int>
                ,shmem_move_copy_pair_allocator_t>                        MyMoveCopyShmMap;
typedef flat_multimap<test::movable_and_copyable_int, test::movable_and_copyable_int
                ,std::less<test::movable_and_copyable_int>
                ,shmem_move_copy_pair_allocator_t>                        MyMoveCopyShmMultiMap;

//Test recursive structures
class recursive_flat_set
{
   public:
   recursive_flat_set(const recursive_flat_set &c)
      : id_(c.id_), flat_set_(c.flat_set_)
   {}

   recursive_flat_set & operator =(const recursive_flat_set &c)
   {
      id_ = c.id_;
      flat_set_= c.flat_set_;
      return *this;
   }
   int id_;
   flat_set<recursive_flat_set> flat_set_;
   friend bool operator< (const recursive_flat_set &a, const recursive_flat_set &b)
   {  return a.id_ < b.id_;   }
};

class recursive_flat_map
{
   public:
   recursive_flat_map(const recursive_flat_map &c)
      : id_(c.id_), map_(c.map_)
   {}

   recursive_flat_map & operator =(const recursive_flat_map &c)
   {
      id_ = c.id_;
      map_= c.map_;
      return *this;
   }

   int id_;
   flat_map<recursive_flat_map, recursive_flat_map> map_;

   friend bool operator< (const recursive_flat_map &a, const recursive_flat_map &b)
   {  return a.id_ < b.id_;   }
};

//Test recursive structures
class recursive_flat_multiset
{
   public:
   recursive_flat_multiset(const recursive_flat_multiset &c)
      : id_(c.id_), flat_set_(c.flat_set_)
   {}

   recursive_flat_multiset & operator =(const recursive_flat_multiset &c)
   {
      id_ = c.id_;
      flat_set_= c.flat_set_;
      return *this;
   }
   int id_;
   flat_multiset<recursive_flat_multiset> flat_set_;
   friend bool operator< (const recursive_flat_multiset &a, const recursive_flat_set &b)
   {  return a.id_ < b.id_;   }
};

class recursive_flat_multimap
{
public:
   recursive_flat_multimap(const recursive_flat_multimap &c)
      : id_(c.id_), map_(c.map_)
   {}

   recursive_flat_multimap & operator =(const recursive_flat_multimap &c)
   {
      id_ = c.id_;
      map_= c.map_;
      return *this;
   }
   int id_;
   flat_map<recursive_flat_multimap, recursive_flat_multimap> map_;
   friend bool operator< (const recursive_flat_multimap &a, const recursive_flat_multimap &b)
   {  return a.id_ < b.id_;   }
};

template<class C>
void test_move()
{
   //Now test move semantics
   C original;
   C move_ctor(boost::move(original));
   C move_assign;
   move_assign = boost::move(move_ctor);
   move_assign.swap(original);
}

int main()
{
   using namespace boost::container::test;

   //Now test move semantics
   {
      test_move<flat_set<recursive_flat_set> >();
      test_move<flat_multiset<recursive_flat_multiset> >();
      test_move<flat_map<recursive_flat_map, recursive_flat_map> >();
      test_move<flat_multimap<recursive_flat_multimap, recursive_flat_multimap> >();
   }


   if (0 != set_test<
                  MyShmSet
                  ,MyStdSet
                  ,MyShmMultiSet
                  ,MyStdMultiSet>()){
      std::cout << "Error in set_test<MyShmSet>" << std::endl;
      return 1;
   }

   if (0 != set_test_copyable<
                  MyShmSet
                  ,MyStdSet
                  ,MyShmMultiSet
                  ,MyStdMultiSet>()){
      std::cout << "Error in set_test<MyShmSet>" << std::endl;
      return 1;
   }

   if (0 != set_test<
                  MyMovableShmSet
                  ,MyStdSet
                  ,MyMovableShmMultiSet
                  ,MyStdMultiSet>()){
      std::cout << "Error in set_test<MyMovableShmSet>" << std::endl;
      return 1;
   }

   if (0 != set_test<
                  MyMoveCopyShmSet
                  ,MyStdSet
                  ,MyMoveCopyShmMultiSet
                  ,MyStdMultiSet>()){
      std::cout << "Error in set_test<MyMoveCopyShmSet>" << std::endl;
      return 1;
   }

   if (0 != map_test<
                  MyShmMap
                  ,MyStdMap
                  ,MyShmMultiMap
                  ,MyStdMultiMap>()){
      std::cout << "Error in set_test<MyShmMap>" << std::endl;
      return 1;
   }

   if (0 != map_test_copyable<
                  MyShmMap
                  ,MyStdMap
                  ,MyShmMultiMap
                  ,MyStdMultiMap>()){
      std::cout << "Error in set_test<MyShmMap>" << std::endl;
      return 1;
   }

//   if (0 != map_test<
//                  MyMovableShmMap
//                  ,MyStdMap
//                  ,MyMovableShmMultiMap
//                  ,MyStdMultiMap>()){
//      return 1;
//   }

   if (0 != map_test<
                  MyMoveCopyShmMap
                  ,MyStdMap
                  ,MyMoveCopyShmMultiMap
                  ,MyStdMultiMap>()){
      std::cout << "Error in set_test<MyMoveCopyShmMap>" << std::endl;
      return 1;
   }

   const test::EmplaceOptions SetOptions = (test::EmplaceOptions)(test::EMPLACE_HINT | test::EMPLACE_ASSOC);
   const test::EmplaceOptions MapOptions = (test::EmplaceOptions)(test::EMPLACE_HINT_PAIR | test::EMPLACE_ASSOC_PAIR);

   if(!boost::container::test::test_emplace<flat_map<test::EmplaceInt, test::EmplaceInt>, MapOptions>())
      return 1;
   if(!boost::container::test::test_emplace<flat_multimap<test::EmplaceInt, test::EmplaceInt>, MapOptions>())
      return 1;
   if(!boost::container::test::test_emplace<flat_set<test::EmplaceInt>, SetOptions>())
      return 1;
   if(!boost::container::test::test_emplace<flat_multiset<test::EmplaceInt>, SetOptions>())
      return 1;
   return 0;
}

#include <boost/container/detail/config_end.hpp>
