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

/* This funtion compares two strings, and returns true if they are equal */
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
  std::string token (iToken);
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
  std::string token (iToken);
  return compareStrIgnoreCase(token, falseString);
}
