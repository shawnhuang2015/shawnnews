/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Query Language
 * udf.hpp: a library of user defined functions used in queries.
 *
 * - Supported type of return value and parameters
 *     - int
 *     - float
 *     - bool
 *     - string (don't use std::string)
 *
 * - Function names are case sensitive, unique, and can't be conflict with
 *   built-in math functions and reserve keywords.
 *
 * - Please don't remove necessary codes in this file
 *
 * Created on: Nov 05, 2015
 * Author: Zixuan Zhuang
 ******************************************************************************/

#ifndef EXPRFUNCTIONS_HPP_
#define EXPRFUNCTIONS_HPP_

#include <stdlib.h>
#include <string>
#include <sstream>
//#include "acclib/MapAccum.hpp"
//#include "acclib/ListAccum.hpp"
#include <gutil/gsqlcontainer.hpp>

typedef std::string string; //XXX DON'T REMOVE

/****** BIULT-IN FUNCTIONS **************/
/****** XXX DON'T REMOVE ****************/
//inline MapAccum<string,ListAccum<string> > MapNeighborToMap (boost::unordered_multiset<string> &set, string id) {

//  MapAccum<string,ListAccum<string> > map;

//  typename boost::unordered_multiset<std::string>::const_iterator it;
//  for (it = set.begin(); it != set.end(); ++it) {
//    std::cout << "*it" << *it << std::cout;
//    map.data_.insert({*it,ListAccum<string>(id)});
//  }

//  return map;
//}


inline string to_string (float val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}



inline int str_to_int (string str) {
  return atoi(str.c_str());
}

inline int float_to_int (float val) {
  return (int) val;
}

//inline string GroupOrderAndPrintMap (MapAccum<string,ListAccum<string> > map, int topK) {

//  // modify topK value
//  if (topK < 0 || topK > map.size())
//    topK = map.size();

//    std::vector<sort_map> v;
//    sort_map sm;
//    // prepare vector
//    typename GUnorderedMap<string, ListAccum<string> >::T::const_iterator it;
//    for (it = map.data_.begin(); it != map.data_.end(); ++it) {
//    sm.key = it->first;
//    sm.val = it->second;
//    v.push_back(sm);
//  }
//  // sort vector
//  std::sort(v.begin(),v.end());

//  // get result
//  std::stringstream ret;
//  for (int i = 0; i < topK; ++i) {
//    ret << "(" << v[i].key << ":" << v[i].val << ") ";
//  }
//  std::string s = ret.str ();
//  return *(new std::string (s));
//}

/****************************************/

#endif /* EXPRFUNCTIONS_HPP_ */
