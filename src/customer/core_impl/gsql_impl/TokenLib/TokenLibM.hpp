/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenLibM.hpp: a system library of token conversion function declaration. 
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
 * - This header file will be included in the TokenBank1.cpp
 *
 * - The implementation is in TokenLibM.cpp
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: Zixuan Zhuang, Mingxi Wu
 ******************************************************************************/

#ifndef TOKENLIBM_HPP_
#define TOKENLIBM_HPP_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>


 /**
  * This function concatenates all input tokens into one string.
  * @par  Example 
  *       load "source_file" to vertex v values ($0, $1, gsql_concat($1, $2)); 
  */
extern "C" void gsql_concat(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
    char* const oToken, uint32_t& oTokenLen);


 /**
  * This function replace the specified seperator to '\30'.
  * @par  Example 
  *       load "source_file" to vertex v values ($0, $1, gsql_replace_separator($1, $2)); 
  */
extern "C" void gsql_replace_separator (const char* const iToken[], uint32_t iTokenLen[], 
    uint32_t iTokenNum, char* const oToken, uint32_t& oTokenLen);

#endif /* TOKENLIBM_HPP_ */
