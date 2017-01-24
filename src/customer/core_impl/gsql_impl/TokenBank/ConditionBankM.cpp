/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenBankM.cpp: a library of token conversion function declaration. 
 *
 * - It is an M-tokens-in boolean function.
 *     which means in ddl, it should be load .... where funcName($0,$1)...;
 *     statements like load .... where funcName($0)...; will search funcName in ConditionBank1.cpp
 * - All functions must have the same funcion signature, but different func name.
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
 * Author: Mingxi
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <stdbool.h>
#include <cstdlib>

#include <ConditionLibM.hpp>

/**
 * This function convert each input token to an integer, sum them and compare 
 * the sum with 3. If it is greater than 3, return true. Otherwise, return false.
 *
 */
extern "C" bool SumGreaterThan3(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) 
{
  int k = 0;
  int sum = 0;

  for (int i=0; i < iTokenNum; i++){
    int tmp = atoi(iToken[i]);
    sum += tmp;
  }

  if (sum > 3) {
    return true;
  }

  return false;

}

extern "C"  bool IsSSNIDTypeAndNotNull(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) {

  int tmp = atoi(iToken[0]);

  if (tmp != 7 && tmp != 0) {
    return false;
  }
  return iTokenLen[1] != 0;
}



/**
 *  Unit testing of the token bank functions
 */ 
/*
int main(){

  char* a[2];
  char d[100]={'0','1','1'};

  a[0]=&d[0];
  a[1]=&d[2];

  uint32_t len[2];

  len[0] = 2;
  len[1] = 1;

  if (SumGreaterThan3(a, len, 2)){
    std::cout<<"condition func return true"<<std::endl;
  } else{
    std::cout<<"condition func return false"<<std::endl;
  }

}
*/

