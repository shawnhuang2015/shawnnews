/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenLibM.cpp: a system library of token conversion function implementation. 
 *
 * - It is an M-tokens-in, one-token-out function.
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
 *   1. string[] -> string 
 *
 *   The UDF token conversion functions will take M input char 
 *   array and do a customized conversion. Then, put the 
 *   converted char array to the output char buffer.
 *
 *     extern "C" void funcName (const char* const iToken[], uint32_t iTokenLen[], 
 *     uint32_t iTokenNum, char* const oToken, uint32_t& oTokenLen);
 *     
 *      @param: iToken: 1 or M input tokens, each pointed by one char pointer 
 *      @param: iTokenLen: each input token's length
 *      @param: iTokenNum: how many input tokens 
 *      @param: oToken: the output token buffer; caller will prepare this buffer.
 *      @param: oTokenLen: the output token length 
 *
 *      Note: extern "C" make C++ compiler not change/mangle the function name.
 *      Note: To avoid array out of boundary issue in oToken buffer, it is 
 *            recommended to add semantic check to ensure oToken length does 
 *            not exceed  OutputTokenBufferSize parameter specified in the 
 *            shell config.
 *
 *  
 *   2. string[] -> int/bool/float
 *
 *     extern "C" uint64_t funcName (const char* const iToken[], 
 *     uint32_t iTokenLen[], uint32_t iTokenNum)
 *
 *     extern "C" bool funcName (const char* const iToken[], uint32_t iTokenLen[], 
 *     uint32_t iTokenNum)
 *
 *     extern "C" float funcName (const char* const iToken[], uint32_t iTokenLen[], 
 *     uint32_t iTokenNum)
 *
 *      @param: iToken: 1 or M input tokens, each pointed by one char pointer 
 *      @param: iTokenLen: each input token's length
 *      @param: iTokenNum: how many input tokens 
 *
 *   Think token function as a UDF designed to combine M specific columns into 
 *   one column before we load them into graph store.
 * 
 * - All functions can be used in the loading job definition, in the VALUES caluse.
 *    e.g. Let a function named Concat(), we can use it in the DDL shell as below
 *      values( $1, Concat($2,$3), $3...)
 *
 *
 * - The declaration is in TokenLibM.hpp
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

#include <TokenLibM.hpp>


 /**
  * this function concatenates all input tokens into one big token
  *
  */
extern "C" void gsql_concat(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
    char* const oToken, uint32_t& oTokenLen) {
      
  int k = 0;
  for (int i=0; i < iTokenNum; i++) {
    for (int j =0; j < iTokenLen[i]; j++) {
           oToken[k++] = iToken[i][j];
    }
  }
  oTokenLen = k;
}

 /**
  * This function replace the specified seperator to '\30'.
  */

extern "C" void gsql_replace_separator (const char* const iToken[], uint32_t iTokenLen[], 
    uint32_t iTokenNum, char* const oToken, uint32_t& oTokenLen) {

  uint32_t j,k;
  oTokenLen = 0;
  for (uint32_t i = 0; i < iTokenLen[0];) {
    for (j = 0, k = i; j < iTokenLen[1] && k < iTokenLen[0]; 
        j ++, k++)  {
      if (iToken[0][k] != iToken[1][j]) {
        break;
      }
    }
    // If there is a match
    if (j == iTokenLen[1]) {
      // Should not be:
      // 1. Heading '\30'
      // 2. consecutive '\30'
      if (oTokenLen != 0 && oToken[oTokenLen-1] != '\30') {
        oToken[oTokenLen++] = '\30';
      }
      i += iTokenLen[1];
    } 
    // If there is no match 
    else {
      oToken[oTokenLen++] = iToken[0][i++];
    }
  }
  // trim trailing '\30'
  if (oTokenLen > 0) {
    oTokenLen --;
    while (oTokenLen > 0 && oToken[oTokenLen] == '\30') {
      oTokenLen --;
    }
    // end
    oToken[++oTokenLen] = '\0';
  }
}
