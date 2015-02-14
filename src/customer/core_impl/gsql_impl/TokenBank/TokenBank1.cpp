/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenBank1.cpp: a library of token conversion function declaration. 
 *
 * - It is a one-token-in, one-token-out function.
 * - All functions must use one of the following signatures, 
 *   but different function name.
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
 *  Think token function as a UDF designed to transform a specific column before we load 
 *  it in graph store.
 *
 * - All functions can be used in the loading job definition, in the VALUES caluse.
 *    e.g. Let a function named Reverse (), we can use it in the DDL shell as below
 *      values( $1, Reverse($2), $3...)
 *
 * - Once defined UDF, run the follow to compile a shared libary.
 *
 *    TokenBank/compile
 *
 *   GraphSQL loader binary will automatically use the library at runtime.
 *
 * - You can unit test your token function in the main function in this file.
 *   To run your test, you can do 
 *
 *     g++ TokenBank1.cpp 
 *     ./a.out
 *
 * Created on: Dec 11, 2014
 * Updated on: Feb 7, 2014
 * Author: Mingxi Wu
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>

/**
 * This function convert iToken char array of size iTokenLen, reverse order and 
 * put it in oToken.
 *
 */
extern "C"  void Reverse (const char* const iToken, uint32_t iTokenLen, 
    char *const oToken, uint32_t& oTokenLen) {

  uint32_t j = 0;
   for (uint32_t i = iTokenLen; i>0; i--){
    oToken[j++] = iToken[i-1];     
  }
  oTokenLen = j;
}

/**
 * This function convert iToken of size iTokenLen to zero array 
 * and put it in oToken.
 *
 */
extern "C"  uint64_t Zero(const char* const iToken, uint32_t iTokenLen) {
  return 0;
}


/**
 *  Unit testing of the token bank functions
 */ 
int main(){

  char a[3]={'a','b','c'};
  char b[100];

  uint32_t len =0;
  Reverse(a,3,b, len);

  for(int i =0; i<3; i++){
    std::cout<<b[i]<<",";
  }

  std::cout<<len <<std::endl;

  len =0;
  Zero(a,3);

  for(int i =0; i<3; i++){

    std::cout<<b[i]<<",";
  }
  std::cout<<len<<std::endl;

}

