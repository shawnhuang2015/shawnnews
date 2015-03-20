/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ReducerLib.cpp: a library of reudce function declaration and implementation.
 *
 * - It takes a list of values, reduce them to one value. 
 *   The reducer return type and the values type must be the same
 *   and match the targeting attribute type.

 * - All functions must use one of the following signatures, 
 *   but different function name.
 *
 *   1. vector<string> -> string 
 *
 *      The UDF reduce functions will take an input char array and do 
 *      a customized reduction. Then, put the scalar output is returned to a 
 *      char buffer. 
 *
 *      extern "C" void funcName (const vector<const char*> iValueList, vector<uint32_t> iValueLen, 
 *                           char *const oValue, uint32_t& oValueLen);
 *
 *       @param: iValueList: A list of tokens subject to reduce.
 *       @param: iValueLen: each input token length 
 *       @param: oValue: the output token buffer; caller will prepare this buffer
 *       @param: oValueLen: the output token length 
 *
 *       Note: extern "C" make C++ compiler not change/mangle the function name.
 *       Note: To avoid array out of boundary issue in oToken buffer, it is 
 *             recommended to add semantic check to ensure oToken length does not exceed 
 *             OutputTokenBufferSize parameter value specified in the shell config.
 *             Default is 2000 chars. 
 *
 *
 *   2. vector<bool/uint/float> -> bool/uint/float
 *
 *      extern "C" bool funcName (const vector<bool> iValueList);
 *      extern "C" uint64_t funcName (const vector<uint64_t> iValueList);
 *      extern "C" float funcName (const vector<float> iValueList);
 *
 *       @param: iValueList: A list of values subject to reduce.
 *       @return: reduced scalar value
 *
 *
 * - All functions can be used in the loading job definition, in the VALUES caluse.
 *    e.g. Let a function named MyReducer(), we can use it in the DDL shell as below
 *      values( $1, reduce(MyReducer($2), $3...). 
 *
 * - The group by key to get a value list for a reducer is the object id. 
 *   That is, for vertex, it's the vertex primary_id. For edge, it's the (from, to).
 *
 * - Once defined UDF, run the follow to compile a shared libary.
 *
 *    ReducerBank/compile
 *
 *   GraphSQL loader binary will automatically use the library at runtime.
 *
 * - You can unit test your reducer function in the main.cpp
 *   To run your test, you can do 
 *
 *     g++ -I./ ReducerLib.cpp  main.cpp
 *     ./a.out
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 10, 2015
 * Author: Mingxi Wu
 ******************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <cfloat>
#include <vector>
#include <string>
#include <ReducerLib.hpp>

using namespace std;

/**
 * sum a list of uint
 */
extern "C" uint64_t gsql_sum_uint(const vector<uint64_t> iValueList) {
  uint64_t sum = 0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      sum += iValueList[i];
    }
  }

  return sum;
}

/**
 * sum a list of float 
 */
extern "C" float gsql_sum_real(const vector<float> iValueList) {
  float sum = 0.0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      sum += iValueList[i];
    }
  }
  return sum;
}

/**
 * find max of a list of uint
 */
extern "C" uint64_t gsql_max_uint(const vector<uint64_t> iValueList) {
  uint64_t max = 0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      if (iValueList[i] > max){
        max = iValueList[i];
      }
    }
  }
  return max;
}

/**
 * find max of a list of real
 */
extern "C" float gsql_max_real(const vector<float> iValueList) {
  float max = 0.0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      if (iValueList[i] > max){
        max = iValueList[i];
      }
    }
  }
  return max;
}

/**
 * find min of a list of uint
 */
extern "C" uint64_t gsql_min_uint(const vector<uint64_t> iValueList) {
  uint64_t min = UINT64_MAX;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      if (iValueList[i] < min){
        min = iValueList[i];
      }
    }
  }
  return min;
}

/**
 * find min of a list of real
 */
extern "C" float gsql_min_real(const vector<float> iValueList) {
  float min = FLT_MAX;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      if (iValueList[i] < min){
        min = iValueList[i];
      }
    }
  }
  return min;
}


/**
 * find average of a list of real
 */
extern "C" float gsql_avg_real(const vector<float> iValueList) {
  float avg = 0.0;
  if (!iValueList.empty()) {
    avg = gsql_sum_real(iValueList)/iValueList.size();
  }
  return avg;
}

/**
 * find average of a list of uint
 */
extern "C" uint64_t gsql_avg_uint(const vector<uint64_t> iValueList) {
  double avg = 0.0;
  if (!iValueList.empty()) {
    avg = gsql_sum_uint(iValueList)/iValueList.size();
  }
  return (uint64_t)avg;
}

/**
 * find count of a list of uint
 */
extern "C" uint64_t gsql_cnt_uint(const vector<uint64_t> iValueList) {

  return iValueList.size();
}


/**
 * bit OR a list of uint
 */
extern "C" uint64_t gsql_or_uint(const vector<uint64_t> iValueList) {
  uint64_t result = 0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      result |= iValueList[i];
    }
  }
  return result;
}

/**
 * bit OR  a list of bool
 */
extern "C" bool gsql_or_bool(const vector<bool> iValueList) {
  bool result = 0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      result = result || iValueList[i];
    }
  }
  return result;
}

/**
 * bit AND a list of uint
 */
extern "C" uint64_t gsql_and_uint(const vector<uint64_t> iValueList) {
  uint64_t result = 0;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      result &= iValueList[i];
    }
  }
  return result;
}

/**
 * bit AND a list of bool
 */
extern "C" bool gsql_and_bool(const vector<bool> iValueList) {
  bool result = 1;
  if (!iValueList.empty()) {
    for (int i = 0; i < iValueList.size(); i++) {
      result = result && iValueList[i];
    }
  }
  return result;
}

/**
 * concatenate a list of char string
 */
extern "C" void gsql_concat_str( const vector<const char*> iValueList, 
    vector<uint32_t> iValueLen, char *const oValue, uint32_t& oValueLen) {

  if (!iValueList.empty()) {
    int index = 0;
    for (int i = 0; i < iValueList.size(); i++) {
      for (int j = 0; j< iValueLen[i]; j ++) {
        oValue[index++]= iValueList[i][j];
      }
    }
    oValueLen = index;
  } else {
    oValue[0]='\0';
    oValueLen =0;
  }
}




