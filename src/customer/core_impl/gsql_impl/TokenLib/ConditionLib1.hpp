/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ConditionLib1.hpp: a system library of token conversion function declaration. 
 *
 * - It is a one-token-in boolean function.
 * - All functions must have the same funcion signature,
 *   you can have different function name but you must follow the name convetion
 *   using snake case gsql_funcname (...). E.g.  gsql_find_max_digit (...).
 *   see http://en.wikipedia.org/wiki/Snake_case
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
 * - This header file will be included in the ConditionBankM.cpp
 *
 * - The implementation is in TokenLib1.cpp
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: Zixuan Zhuang, Mingxi Wu
 ******************************************************************************/

#ifndef CONDITIONLIB1_HPP_
#define CONDITIONLIB1_HPP_

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <stdbool.h>
#include <cstdlib>


/**
 * This function checks whether the input string is "true" or "t" (case insensitive),
 * returns true if it is "true" or "t".
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_true($2);
 */
extern "C"  bool gsql_is_true(const char* const iToken, uint32_t iTokenLen);

/**
 * This function checks whether the input string is "false" or "f" (case insensitive),
 * and returns true if it is "false" or "f".
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_false($2);
 */
extern "C"  bool gsql_is_false(const char* const iToken, uint32_t iTokenLen);

/**
 * This function checks whether the input string is empty 
 * after removing white space.
 *
 * @par  Example
 *       load "source_file" to vertex v values ($0, $1) where gsql_is_not_empty_string($2);
 */
extern "C"  bool gsql_is_not_empty_string(const char* const iToken, uint32_t iTokenLen);

#endif /* CONDITIONLIB1_HPP_ */
