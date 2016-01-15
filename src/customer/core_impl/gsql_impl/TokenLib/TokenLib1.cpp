/***************************************************************************//**
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenBank1.cpp: a system library of token conversion function implementation. 
 *
 * - It is a one-token-in, one-token-out function.
 * - All functions must use one of the following signatures, 
 *   you can have different function name but you must follow the name convetion
 *   using snake case gsql_funcname (...). E.g.  gsql_find_max_digit (...).
 *   see http://en.wikipedia.org/wiki/Snake_case
 *
 * - A token function can have nested other token function;
 *   The out-most token function should return the same type
 *   as the targeted attribute type specified in the 
 *   vertex/edge schema.
 *
 *   1. string -> string 
 *
 *      The UDF token conversion functions will take an input char array and do 
 *      a customized conversion. Then, put the converted char array to output 
 *      char buffer. 
 *
 *      extern "C" void funcName (const char* const iToken, uint32_t iTokenLen, 
 *                           char *const oToken, uint32_t& oTokenLen);
 *
 *       @param: iToken: 1 input token pointed by one char pointer.
 *       @param: iTokenLen: the input token length 
 *       @param: oToken: the output token buffer; caller will prepare this buffer
 *       @param: oTokenLen: the output token length 
 *
 *       Note: extern "C" make C++ compiler not change/mangle the function name.
 *       Note: To avoid array out of boundary issue in oToken buffer, it is 
 *             recommended to add semantic check to ensure oToken length does not exceed 
 *             OutputTokenBufferSize parameter value specified in the shell config.
 *
 *
 *   2. string -> bool/uint/float
 *
 *      extern "C" bool funcName (const char* const iToken, uint32_t iTokenLen);
 *      extern "C" uint64_t funcName (const char* const iToken, uint32_t iTokenLen);
 *      extern "C" float funcName (const char* const iToken, uint32_t iTokenLen);
 *
 *       @param: iToken: 1 input token pointed by one char pointer.
 *       @param: iTokenLen: the input token length 
 *       @return: converted value
 *
 *
 *   3. string -> string 
 *
 *      The UDF token conversion functions will take an input char array and do 
 *      a customized conversion. Then, put the converted char array to output 
 *      char buffer. This converted char array is a concatenation of multiple
 *      tuples.
 *
 *      extern "C" void funcName (const char* const iToken, uint32_t iTokenLen, 
 *                           char *const oToken, uint32_t& oTokenLen);
 *
 *       @param: iToken: 1 input token pointed by one char pointer.
 *       @param: iTokenLen: the input token length 
 *       @param: oToken: the output token buffer; caller will prepare this buffer
 *       @param: oTokenLen: the output token length 
 *
 *     Note: each tuple must be end with (char)30. For field separator within tuple,
 *     we use (char)31. See below example ratingMovie()
 * 
 *  Think token function as a UDF designed to transform a specific column before we load 
 *  it in graph store.
 *
 * - All functions can be used in the loading job definition, in the VALUES caluse.
 *    e.g. Let a function named Reverse (), we can use it in the DDL shell as below
 *      values( $1, Reverse($2), $3...)
 *
 * - The declaration is in TokenLib1.hpp
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

#include <TokenLib1.hpp>

using namespace std;

/**
 * \brief  time convert utility
 */
class GtimeConv {
 public:

  /**
   * Only support 3 types
   * "%Y-%m-%d %H:%M:%S"
   * "%Y/%m/%d %H:%M:%S"
   * "%Y-%m-%dT%H:%M:%S.000z" // ignores text after .
   * e.g.
   * "2014-4-17 12:22:23"
   * "2014/4/17 12:22:23"
   *
   * @param s
   * @return
   */
  uint32_t seconds_from_epoch(const std::string& ymd_hms) {
    return seconds_from_epoch(ymd_hms.c_str(), ymd_hms.length());
  }

  uint32_t seconds_from_epoch(const char * ymd_hms, uint32_t length) {
    const int mon_days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273,
        304, 334, 365 };
    uint32_t tyears, tdays, leaps;
    char *timestamp_ptr_ = (char *) ymd_hms;
    char *timestamp_end_ptr_ = timestamp_ptr_ + length;
    struct tm t = { 0 };  // Initalize to all 0's
    t.tm_year = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_mon = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_mday = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_hour = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_min = nextInt(timestamp_ptr_, timestamp_end_ptr_);
    t.tm_sec = nextInt(timestamp_ptr_, timestamp_end_ptr_);

    tyears = t.tm_year - 1970;  // base time is 1970
    leaps = (tyears + 2) / 4;  // no of next two lines until year 2100.
    tdays = mon_days[t.tm_mon - 1];
    tdays += t.tm_mday - 1;  // days of month passed.
    tdays = tdays + (tyears * 365) + leaps;
    if (t.tm_mon <= 2 && ((tyears + 2)%4 == 0)) // leaf year: the 1st two months need -1 day.
      tdays--;
    return (tdays * 86400) + (t.tm_hour * 3600) + (t.tm_min * 60)
        + t.tm_sec;
  }
 private:
  uint32_t nextInt(char *&timestamp_ptr, const char *timestamp_end_ptr) {
    uint32_t int_value = 0;
    while (*timestamp_ptr != ' ' && *timestamp_ptr != ':'
        && *timestamp_ptr != '-' && *timestamp_ptr != '/'
        && *timestamp_ptr != 'T' && *timestamp_ptr != '.'
        && *timestamp_ptr != '\n' && *timestamp_ptr != '\r'
        && timestamp_ptr < timestamp_end_ptr) {
      int_value = int_value * 10 + *timestamp_ptr - '0';
      timestamp_ptr++;
    }
    timestamp_ptr++;  // jump over separator
    return int_value;
  }
};

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
 * This function convert iToken char array of size iTokenLen, reverse order 
 * and put it in oToken.
 *
 * @param kdlsjfl
 * @return skdjfkl
 */

extern "C"  void gsql_reverse (const char* const iToken, uint32_t iTokenLen, 
    char *const oToken, uint32_t& oTokenLen) {

  uint32_t j = 0;
  for (uint32_t i = iTokenLen; i>0; i--){
    oToken[j++] = iToken[i-1];     
  }
  oTokenLen = j;
}

extern "C" uint64_t gsql_ts_to_epoch_seconds(const char* const iToken, uint32_t iTokenLen) {
  GtimeConv converter;
  return converter.seconds_from_epoch(iToken,iTokenLen);
}

extern "C" void gsql_split_by_space (const char* const iToken, uint32_t iTokenLen,
         char *const oToken, uint32_t& oTokenLen) {
  for(int i = 0; i < iTokenLen; i++) {
    if(iToken[i] == ' '){
      oToken[i] = (char)30; 
    }else{
      oToken[i] = iToken[i]; 
    }
  }
  oToken[iTokenLen] = (char)30;
  oTokenLen = iTokenLen+1;
  
}

extern "C"  bool gsql_to_bool(const char* const iToken, uint32_t iTokenLen) {

  if (iTokenLen == 1 && (iToken[0] == 'T' || iToken[0] == 't')) {
    return true;
  }
 
  std::string trueString = "true";
  std::string token (iToken, iTokenLen);
  return compareStrIgnoreCase(token, trueString);
}

// TODO: uncomment after upgrade to 4.3.3
//extern "C" int64_t gsql_to_int(const char* const iToken, uint32_t iTokenLen) {
//  return (int64_t) atof(std::string(iToken, iTokenLen).c_str());
//}

extern "C" uint64_t gsql_to_uint(const char* const iToken, uint32_t iTokenLen) {
  return (uint64_t) atof(std::string(iToken, iTokenLen).c_str());
}

extern "C" uint64_t gsql_current_time_epoch(const char* const iToken, uint32_t iTokenLen) {
    return time(0);
}


