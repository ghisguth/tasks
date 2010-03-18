//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#include <boost/container/detail/config_begin.hpp>
#include <boost/container/detail/workaround.hpp>
//[doc_recursive_containers
#include <boost/container/vector.hpp>
#include <boost/container/list.hpp>
#include <boost/container/map.hpp>
#include <boost/container/stable_vector.hpp>

using namespace boost::container;

struct data
{
   int               i_;
   vector<data>      v_;
   list<data>        l_;
   map<data, data>   m_;

   friend bool operator <(const data &l, const data &r)
   { return l.i_ < r.i_; }
};

int main()
{
   stable_vector<data> sv;
   sv.resize(100);
   return 0;
}
//]
#include <boost/container/detail/config_end.hpp>
