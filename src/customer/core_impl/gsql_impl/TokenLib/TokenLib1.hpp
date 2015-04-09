/***************************************************************************//**
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenLib1.hpp: a system library of token conversion function declaration. 
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
 *             Default Outputbuffersize is 2000 chars.
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
 * - This header file will be included in the TokenBank1.cpp
 *
 * - The implementation is in TokenLib1.cpp
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: Zixuan Zhuang, Mingxi Wu
 ******************************************************************************/

#ifndef TOKENLIB1_HPP_
#define TOKENLIB1_HPP_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>


using namespace std;

/**
 * This function will convert iToken char array of size iTokenLen, reverse order 
 * and put it in oToken.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, gsql_reverse($2)); 
 */
extern "C"  void gsql_reverse (const char* const iToken, uint32_t iTokenLen, 
    char *const oToken, uint32_t& oTokenLen);

/**
 * This funcion will convert timestamp to epoch seconds. 
 *
 * It only supports 3 types of input:
 *  - "%Y-%m-%d %H:%M:%S"<br>
 *  - "%Y/%m/%d %H:%M:%S"<br>
 *  - "%Y-%m-%dT%H:%M:%S.000z"<br> 
 * e.g.<br>
 *  - "2014-4-17 12:22:23"<br>
 *  - "2014/4/17 12:22:23"<br>
 *
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, gsql_ts_to_epoch_seconds($2)); 
 */
extern "C" uint64_t gsql_ts_to_epoch_seconds(const char* const iToken, uint32_t iTokenLen);

/**
 * This funcion will replace space with ((char)30).
 * @par  Example 
 *       load "source_file" to to temp_table t1(pid,pname,rating,mid) values($0, $1, flatten(gsql_split_by_space($2)));<br>
 *       load temp_table t1 to vertex v values ($pid, $pid, $pname);<br>
 *       load temp_table t1 to edge e values ($pid, $mid, $rating);
 */
extern "C" void gsql_split_by_space (const char* const iToken, uint32_t iTokenLen,
         char *const oToken, uint32_t& oTokenLen);

/**
 * This function converts a string to boolean.
 * - If the string is "true" or "t", return TRUE.
 * - O/w return FALSE.
 * - String is compared ignoring case considerations.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1, gsql_to_bool($2));
 */
extern "C"  bool gsql_to_bool(const char* const iToken, uint32_t iTokenLen);

#endif /* TOKENLIB1_HPP_ */
