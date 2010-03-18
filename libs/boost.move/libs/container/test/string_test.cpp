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
#include <boost/container/vector.hpp>
#include <boost/container/string.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstddef>
#include <new>
#include "dummy_test_allocator.hpp"
#include "check_equal_containers.hpp"
#include "expand_bwd_test_allocator.hpp"
#include "expand_bwd_test_template.hpp"

using namespace boost::container;

typedef test::dummy_test_allocator<char>           DummyCharAllocator; 
typedef basic_string<char, std::char_traits<char>, DummyCharAllocator> DummyString;
typedef test::dummy_test_allocator<DummyString>    DummyStringAllocator;
typedef test::dummy_test_allocator<wchar_t>              DummyWCharAllocator; 
typedef basic_string<wchar_t, std::char_traits<wchar_t>, DummyWCharAllocator> DummyWString;
typedef test::dummy_test_allocator<DummyWString>         DummyWStringAllocator;

//Explicit instantiations of container::basic_string
template class basic_string<char, std::char_traits<char>, DummyCharAllocator>;
template class basic_string<wchar_t, std::char_traits<wchar_t>, DummyWCharAllocator>;
//Explicit instantiation of container::vectors of container::strings
template class vector<DummyString, DummyStringAllocator>;
template class vector<DummyWString, DummyWStringAllocator>;

struct StringEqual
{
   template<class Str1, class Str2>
   bool operator ()(const Str1 &string1, const Str2 &string2) const
   {
      if(string1.size() != string2.size())
         return false;
      return std::char_traits<typename Str1::value_type>::compare
         (string1.c_str(), string2.c_str(), string1.size()) == 0;
   }
};

//Function to check if both lists are equal
template<class StrVector1, class StrVector2>
bool CheckEqualStringVector(StrVector1 *strvect1, StrVector2 *strvect2)
{
   StringEqual comp;
   return std::equal(strvect1->begin(), strvect1->end(), 
                     strvect2->begin(), comp);
}

template<class CharType>
int string_test()
{
   typedef std::string        StdString;
   typedef vector<StdString>  StdStringVector;
   typedef basic_string<CharType> ShmString;
   typedef vector<ShmString> ShmStringVector;

   const int MaxSize = 100;

   //Create shared memory
   {
      ShmStringVector *shmStringVect = new ShmStringVector;
      StdStringVector *stdStringVect = new StdStringVector;
      ShmString auxShmString;
      StdString auxStdString(StdString(auxShmString.begin(), auxShmString.end() ));

      CharType buffer [20];

      //First, push back
      for(int i = 0; i < MaxSize; ++i){
         auxShmString = "String";
         auxStdString = "String";
         std::sprintf(buffer, "%i", i);
         auxShmString += buffer;
         auxStdString += buffer;
         shmStringVect->push_back(auxShmString);
         stdStringVect->push_back(auxStdString);
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)){
         return 1;
      }

      //Now push back moving 
      for(int i = 0; i < MaxSize; ++i){
         auxShmString = "String";
         auxStdString = "String";
         std::sprintf(buffer, "%i", i);
         auxShmString += buffer;
         auxStdString += buffer;
         shmStringVect->push_back(boost::move(auxShmString));
         stdStringVect->push_back(auxStdString);
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)){
         return 1;
      }

      //push front
      for(int i = 0; i < MaxSize; ++i){
         auxShmString = "String";
         auxStdString = "String";
         std::sprintf(buffer, "%i", i);
         auxShmString += buffer;
         auxStdString += buffer;
         shmStringVect->insert(shmStringVect->begin(), auxShmString);
         stdStringVect->insert(stdStringVect->begin(), auxStdString);
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)){
         return 1;
      }

      //Now push front moving 
      for(int i = 0; i < MaxSize; ++i){
         auxShmString = "String";
         auxStdString = "String";
         std::sprintf(buffer, "%i", i);
         auxShmString += buffer;
         auxStdString += buffer;
         shmStringVect->insert(shmStringVect->begin(), boost::move(auxShmString));
         stdStringVect->insert(stdStringVect->begin(), auxStdString);
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)){
         return 1;
      }

      //Now test long and short representation swapping
      auxShmString = "String";
      auxStdString = "String";
      ShmString shm_swapper;
      StdString std_swapper;
      shm_swapper.swap(auxShmString);
      std_swapper.swap(auxStdString);
      if(!StringEqual()(auxShmString, auxStdString))
         return 1;   
      if(!StringEqual()(shm_swapper, std_swapper))
         return 1;   

      shm_swapper.swap(auxShmString);
      std_swapper.swap(auxStdString);
      if(!StringEqual()(auxShmString, auxStdString))
         return 1;   
      if(!StringEqual()(shm_swapper, std_swapper))
         return 1;   

      auxShmString = "LongLongLongLongLongLongLongLongLongLongLongLongLongString";
      auxStdString = "LongLongLongLongLongLongLongLongLongLongLongLongLongString";
      shm_swapper = ShmString();
      std_swapper = StdString();
      shm_swapper.swap(auxShmString);
      std_swapper.swap(auxStdString);
      if(!StringEqual()(auxShmString, auxStdString))
         return 1;   
      if(!StringEqual()(shm_swapper, std_swapper))
         return 1;   

      shm_swapper.swap(auxShmString);
      std_swapper.swap(auxStdString);
      if(!StringEqual()(auxShmString, auxStdString))
         return 1;   
      if(!StringEqual()(shm_swapper, std_swapper))
         return 1;   

      //No sort
      std::sort(shmStringVect->begin(), shmStringVect->end());
      std::sort(stdStringVect->begin(), stdStringVect->end());
      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      const CharType prefix []    = "Prefix";
      const int  prefix_size  = sizeof(prefix)/sizeof(prefix[0])-1;
      const CharType sufix []     = "Suffix";

      for(int i = 0; i < MaxSize; ++i){
         (*shmStringVect)[i].append(sufix);
         (*stdStringVect)[i].append(sufix);
         (*shmStringVect)[i].insert((*shmStringVect)[i].begin(), 
                                    prefix, prefix + prefix_size);
         (*stdStringVect)[i].insert((*stdStringVect)[i].begin(), 
                                    prefix, prefix + prefix_size);
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      for(int i = 0; i < MaxSize; ++i){
         std::reverse((*shmStringVect)[i].begin(), (*shmStringVect)[i].end());
         std::reverse((*stdStringVect)[i].begin(), (*stdStringVect)[i].end());
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      for(int i = 0; i < MaxSize; ++i){
         std::reverse((*shmStringVect)[i].begin(), (*shmStringVect)[i].end());
         std::reverse((*stdStringVect)[i].begin(), (*stdStringVect)[i].end());
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      for(int i = 0; i < MaxSize; ++i){
         std::sort(shmStringVect->begin(), shmStringVect->end());
         std::sort(stdStringVect->begin(), stdStringVect->end());
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      for(int i = 0; i < MaxSize; ++i){
         (*shmStringVect)[i].replace((*shmStringVect)[i].begin(), 
                                    (*shmStringVect)[i].end(),
                                    "String");
         (*stdStringVect)[i].replace((*stdStringVect)[i].begin(), 
                                    (*stdStringVect)[i].end(),
                                    "String");
      }

      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      shmStringVect->erase(std::unique(shmStringVect->begin(), shmStringVect->end()),
                           shmStringVect->end());
      stdStringVect->erase(std::unique(stdStringVect->begin(), stdStringVect->end()),
                           stdStringVect->end());
      if(!CheckEqualStringVector(shmStringVect, stdStringVect)) return 1;

      //When done, delete vector
      delete shmStringVect;
      delete stdStringVect;
   }
   return 0;
}

bool test_expand_bwd()
{
   //Now test all back insertion possibilities
   typedef test::expand_bwd_test_allocator<char>
      allocator_type;
   typedef basic_string<char, std::char_traits<char>, allocator_type>
      string_type;
   return  test::test_all_expand_bwd<string_type>();
}

int main()
{
   if(string_test<char>()){
      return 1;
   }

   if(!test_expand_bwd())
      return 1;

   return 0;
}

#include <boost/container/detail/config_end.hpp>
