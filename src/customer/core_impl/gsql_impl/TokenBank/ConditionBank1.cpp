/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenBank1.cpp: a library of token conversion function declaration. 
 *
 * - It is a one-token-in boolean function.
 * - All functions must have the same funcion signature, but different func name.
 *
 *      extern "C" bool conditionFuncName(const char* const iToken, uint32_t iTokenLen);
 *
 *       @param: iToken: 1 input token pointed by one char pointer.
 *       @param: iTokenLen: the input token length.
 *       @return: true if condition met, false otherwise.
 *       Note: extern "C" make C++ compiler not change/mangle the function name.
 *
 *    The UDF token functions will take an input char array and do a customized check.
 *    Then, return true or false depending on the token passes the condition or not. 
 *
 *
 * - All functions can be used in the loading job definition, in the WHERE caluse.
 *    e.g. Let a function named GreaterThan3(), we can use it in the DDL shell as below
 *
 *     load "xx" into vertex abc values($1,$2) where Greater($5) using seperator=",";
 *
 * - Once defined UDF, run the following script to compile it to a shared libary.
 *
 *    ./compile
 *
 *    GraphSQL loader binary will automatically use the library at runtime.
 *
 * - You can unit test your condition function in the main function in this file.
 *   To run your test, you can do 
 *
 *     g++ ConditionBank1.cpp 
 *     ./a.out
 *
 * Created on: Dec 11, 2014
 * Author: Mingxi Wu
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <stdbool.h>
#include <cstdlib>

#include <ConditionLib1.hpp>


/**
 * This function convert iToken to integer and compare with 3.
 * If it is greater than 3, return true. Otherwise, return false.
 *
 */
extern "C"  bool GreaterThan3(const char* const iToken, uint32_t iTokenLen) {

  int tmp = atoi(iToken);

  if (tmp >3) {
    return true;
  }
  return false;
}

extern "C" bool NotNull(const char* const iToken, uint32_t iTokenLen) {
  return iTokenLen != 0;
}

extern "C"  bool IsSSNIDType(const char* const iToken, uint32_t iTokenLen) {

  int tmp = atoi(iToken);

  if (tmp == 7 || tmp == 0) {
    return true;
  }
  return false;
}


/**
 *  Unit testing of the token bank functions
 */ 
int main(){

  char a[3]={'1','2','3'};

  if (GreaterThan3(a,3)){
    std::cout<<"condition func return true"<<std::endl;
  } else{
    std::cout<<"condition func return false"<<std::endl;
  }

}

