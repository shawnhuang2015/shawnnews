/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ConditionLibM.hpp: a system library of token conversion function declaration. 
 *
 * - It is an M-tokens-in boolean function.
 * - All functions must have the same funcion signature,
 *   you can have different function name but you must follow the name convetion
 *   using snake case gsql_funcname (...). E.g.  gsql_find_max_digit (...).
 *   see http://en.wikipedia.org/wiki/Snake_case
 *
 *     extern "C" bool conditionFuncName (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum)
 *     
 *      @param: iToken: 1 or M input tokens, each pointed by one char pointer 
 *      @param: iTokenLen: each input token's length
 *
 *      Note: extern "C" make C++ compiler not change/mangle the function name.
 *
 *   The UDF token conversion functions will take M input char array and do a customized check.
 *   Then, return true or false depending on the tokens pass the condition or not. 
 *
 * - All functions can be used in the loading job definition, in the WHERE caluse.
 *    e.g. Let a function named SumGreaterThan3(), we can use it in the DDL shell as below
 *
 *     load "xx" into vertex abc values($1,$2) where SumGreaterThan3($0,$1) using seperator=",";
 *
 * - This header file will be included in the ConditionBankM.cpp
 *
 * - The implementation in ConditionLibM.cpp
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: Zixuan Zhuang, Mingxi Wu
 ******************************************************************************/

#ifndef CONDITIONLIBM_HPP_
#define CONDITIONLIBM_HPP_

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <stdbool.h>
#include <cstdlib>
#include <string>

/**
 * This funtion compares two string (case sensitive), and returns true if they are equal.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_token_equal($1, "test");
 */

extern "C" bool gsql_token_equal(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * This funtion compares two string (case insensitive), and returns true if they are equal.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_token_ignore_case_equal($1, "Test");
 */

extern "C" bool gsql_token_ignore_case_equal(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum);

/**
 * Predicate testing helper function. 
 *    E.g. WHERE $1 IS NUMERIC 
 *    Will call this function.
 *
 * IsNumeric test whether a string is a numeric number.
 * 
 * Numeric number is defined as follows in regular expression:
 *      (+/-)?[0-9][0-9]*(.[0-9])?[0-9]*     
 *
 * Any space appear in between will not be taken as numeric number. 
 * Space in front and at the end is OK.
 */

extern "C" bool IsNumeric (std::string s);

#endif /* CONDITIONLIBM_HPP_ */
