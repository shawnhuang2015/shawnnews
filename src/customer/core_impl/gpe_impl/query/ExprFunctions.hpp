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

#ifndef UDF_HPP_
#define UDF_HPP_

#include <stdlib.h>
typedef std::string string; //XXX DON'T REMOVE

/****** BIULT-IN FUNCTIONS **************/
/****** XXX DON'T REMOVE ****************/
inline int str_to_int (string str) {
  return atoi(str.c_str());
}

inline int float_to_int (float val) {
  return (int) val;
}

inline string to_string (float val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}
/****************************************/

#endif /* UDF_HPP_ */
