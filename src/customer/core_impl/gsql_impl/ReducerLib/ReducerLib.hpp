/**************************************************************************//**
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ReducerLib.cpp: a library of reudce function declaration and implementation.
 *
 * - It takes a list of values, reduce them to one value. 
 *   The reducer return type and the values type must be the same
 *   and match the targeting attribute type.
 *
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
 * - You can unit test your reducer function in the main function in this file.
 *   To run your test, you can do 
 *
 *     g++ ReducerBank.cpp 
 *     ./a.out
 *
 * Created on: Mar 10, 2015
 * Author: Mingxi Wu
 *//*****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

/**
 * Sum a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_sum_uint($2))); 
 */
extern "C" uint64_t gsql_sum_uint(const vector<uint64_t> iValueList);

/**
 * Sum a list of real numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_sum_real($2))); 
 */
extern "C" float gsql_sum_real(const vector<float> iValueList);

/**
 * Find max of a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_max_uint($2))); 
 */
extern "C" uint64_t gsql_max_uint(const vector<uint64_t> iValueList);

/**
 * Find max of a list of real numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_max_real($2))); 
 */
extern "C" float gsql_max_real(const vector<float> iValueList);

/**
 * Find min of a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_min_uint($2))); 
 */
extern "C" uint64_t gsql_min_uint(const vector<uint64_t> iValueList);

/**
 * Find min of a list of real numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_min_real($2))); 
 */
extern "C" float gsql_min_real(const vector<float> iValueList);

/**
 * Find average of a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_avg_uint($2))); 
 */
extern "C" uint64_t gsql_avg_uint(const vector<uint64_t> iValueList);

/**
 * Find average of a list of real numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_avg_real($2))); 
 */
extern "C" float gsql_avg_real(const vector<float> iValueList);

/**
 * Find count of a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_cnt_uint($2))); 
 */
extern "C" uint64_t gsql_cnt_uint(const vector<uint64_t> iValueList);


/**
 * Bit OR a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_and_bool($2))); 
 */
extern "C" uint64_t gsql_or_uint(const vector<uint64_t> iValueList);

/**
 * OR  a list of bool values.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_or_bool($2))); 
 */
extern "C" bool gsql_or_bool(const vector<bool> iValueList);

/**
 * Bit AND a list of uint numbers.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_and_uint($2))); 
 */
extern "C" uint64_t gsql_and_uint(const vector<uint64_t> iValueList);

/**
 * AND a list of bool values.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_and_bool($2))); 
 */
extern "C" bool gsql_and_bool(const vector<bool> iValueList);

/**
 * Concatenate a list of char string.
 * @par  Example 
 *       load "source_file" to vertex v values ($0, $1, reduce(gsql_concat_str($2))); 
 */
extern "C" void gsql_concat_str(const vector<const char*> iValueList, vector<uint32_t> iValueLen, char *const oValue, uint32_t& oValueLen);

