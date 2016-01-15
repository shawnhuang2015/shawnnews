/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ConditionLibM.hpp: a system library of token conversion function implementation. 
 *
 * - It is a one-token-in boolean function.
 * - All functions must have the same funcion signature,
 *   you can have different function name but you must follow the name convetion
 *      gsql_funcName
 *
 *     extern "C" bool conditionFuncName(const char* const iToken, uint32_t iTokenLen);
 *  
 *      @param: iToken: 1 input token pointed by one char pointer.
 *      @param: iTokenLen: the input token length.
 *      @return: true if condition met, false otherwise.
 *
 *      Note: extern "C" make C++ compiler not change/mangle the function name.
 *        
 *     The UDF token functions will take an input char array and do a customized check.
 *     Then, return true or false depending on the token passes the condition or not.
 *           
 *            
 * - All functions can be used in the loading job definition, in the WHERE caluse.
 *    e.g. Let a function named GreaterThan3(), we can use it in the DDL shell as below
 *               
 *     load "xx" into vertex abc values($1,$2) where Greater($5) using seperator=",";
 *
 * - The declaration is in ConditionLib1.hpp
 *
 * - You can unit test your token function in the main.cc
 *   To run your test, you can do 
 *
 *     g++ -I./ main.cpp TokenLib1.cpp TokenLibM.cpp ConditionLib1.cpp ConditionLibM.cpp
 *     ./a.out
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: Zixuan Zhuang, Mingxi Wu
 ******************************************************************************/

#include <ConditionLib1.hpp>

/**
 * This funtion compares two strings, and returns true if they are equal.
 * This is used internally in this file by other condition functions.
 */
static inline bool compareStrIgnoreCase (std::string s1, std::string s2) {

  int len = s1.length();
  if (len != s2.length()) {
    return false;
  }

  for (int i = 0; i < len; i++) {
    if (tolower(s1[i]) != tolower(s2[i])) {
      return false;
    }
  }

  return true;
}

/**
 * This function check whether the input string is "true" case insensitive;
 * return true if it is "true" or "t".
 */
extern "C"  bool gsql_is_true(const char* const iToken, uint32_t iTokenLen) {

  if (iTokenLen == 1 && (iToken[0] == 'T' || iToken[0] == 't')) {
    return true;
  }
 
  std::string trueString = "true";
  std::string token (iToken, iTokenLen);
  return compareStrIgnoreCase(token, trueString);
}

/**
 * This function check whether the input string is "false" case insensitive;
 * return true if it is "false" or "f".
 */
extern "C"  bool gsql_is_false(const char* const iToken, uint32_t iTokenLen) {

  if (iTokenLen == 1 && (iToken[0] == 'F' || iToken[0] == 'f')) {
    return true;
  }
  std::string falseString = "false";
  std::string token (iToken, iTokenLen);
  return compareStrIgnoreCase(token, falseString);
}

/**
 * This function checks whether the input string is empty 
 * after removing white space.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_not_empty_string($2);
 */
extern "C"  bool gsql_is_not_empty_string(const char* const iToken, uint32_t iTokenLen) {
  if (iTokenLen <= 0) {
    return false;
  }

  for (int i = 0; i < iTokenLen; i++) {
         if (iToken[i] != ' '){
           return true;
         }
  }
  //empty or all space, return false
  return false;
}

/**
 * Get a int from a timestamp string year, month or day.
 *
 * We only supports 3 formats.
 *
 * "%Y-%m-%d %H:%M:%S"
 * "%Y/%m/%d %H:%M:%S"
 * "%Y-%m-%dT%H:%M:%S.000z" // ignores text after .
 * e.g.
 * "2014-4-17 12:22:23"
 * "2014/4/17 12:22:23"
 *
 * Since  each field range is 
 *  YYYY should be [0,10000]
 *  MM   should be [0,12]
 *  DD   should be [0,31]
 *  HH   should be [0,24]
 *  MM   should be [0,60]
 *  SS   should be [0,60]
 *
 *  So, we should make nextInt fall between [0,10000]
 *
 */
static inline uint32_t nextTSInt(char*& timestamp_ptr, 
    const char* timestamp_end_ptr) {

  uint32_t int_value = 0;
  while (*timestamp_ptr != ' ' && *timestamp_ptr != ':' &&
      *timestamp_ptr != '-' && *timestamp_ptr != '/' &&
      *timestamp_ptr != 'T' && *timestamp_ptr != '.' &&
      *timestamp_ptr != '\n' && *timestamp_ptr != '\r' &&
      timestamp_ptr < timestamp_end_ptr) {

    //1. digit validity check.
    if (*timestamp_ptr < '0' || *timestamp_ptr > '9') {
      return std::numeric_limits<uint32_t>::max();
    } 

    int_value = int_value * 10 + *timestamp_ptr - '0';

    //2. validity check.
    if (int_value > 10000) {
      return std::numeric_limits<uint32_t>::max();
    } 

    timestamp_ptr++;
  }
  timestamp_ptr++;  // jump over separator
  return int_value;
}

/**
 * This function checks if a given token is a valid timestamp 
 * format.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) 
 *           where gsql_is_valid_timestamp($2);
 */
extern "C"  bool gsql_is_valid_timestamp(const char* const iToken, 
    uint32_t iTokenLen) {

  if (iTokenLen <= 0) {
    return false;
  }

  char* timestamp_ptr_ = const_cast<char*>(iToken);
  char* timestamp_end_ptr_ = timestamp_ptr_ + iTokenLen;

  uint32_t _year = nextTSInt(timestamp_ptr_, timestamp_end_ptr_);
  if (_year == std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  uint32_t _mon = nextTSInt(timestamp_ptr_, timestamp_end_ptr_);
  if (_mon == std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  uint32_t _day = nextTSInt(timestamp_ptr_, timestamp_end_ptr_);
  if (_day == std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  uint32_t _hour = nextTSInt(timestamp_ptr_, timestamp_end_ptr_);
  if (_hour == std::numeric_limits<uint32_t>::max()) {
    return false;
  }


  uint32_t _min = nextTSInt(timestamp_ptr_, timestamp_end_ptr_);
  if (_min == std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  uint32_t _sec = nextTSInt(timestamp_ptr_, timestamp_end_ptr_);
  if (_sec == std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  //empty or all space, return false
  return false;
}



