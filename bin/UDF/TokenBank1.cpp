/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenBank1.cpp: a library of token conversion function declaration. 
 *
 * - It is a one-token-in, one-token-out function.
 * - All functions must have the same funcion signature, but different func name.
 *
 *      extern "C" void funcName (const char* const iToken, uint32_t iTokenLen, 
 *                           char *const oToken, uint32_t& oTokenLen);
 *
 *       @param: iToken: 1 input token pointed by one char pointer.
 *       @param: iTokenLen: the input token length 
 *       @param: oToken: the output token buffer; maximum 200 characters.
 *       @param: oTokenLen: the output token length 
 *
 *       Note: extern "C" make C++ compiler not change/mangle the function name.
 *
 *    The UDF token conversion functions will take an input char array and do a customized conversion.
 *    Then, put the converted char array to output char buffer.
 *
 *     Think it as a UDF designed to transform a specific column before we load it in GStore.
 *
 * - For 1 loading job, one can use at most *20* token UDFs from this file.
 * - All functions can be used in the loading job definition, in the VALUES caluse.
 *    e.g. Let a function named Reverse (), we can use it in the DDL shell as below
 *      values( $1, Reverse($2), $3...)
 *
 * - Once defined UDF, run the following script to compile it to a shared libary.
 *
 *    ./compileTokenBank.sh
 *
 *    The shared library path can be specified in run.sh, e.g.
 *
 *     export LD_LIBRARY_PATH='/vagrant/repo/product/bin/'
 *
 *    GraphSQL loader binary will automatically use the library at runtime.
 *
 * - You can unit test your token function in the main function in this file.
 *   To run your test, you can do 
 *
 *     g++ TokenBank1.cpp 
 *     ./a.out
 *
 * Created on: Dec 11, 2014
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
extern "C"  void Zero(const char* const iToken, uint32_t iTokenLen, 
    char *const oToken, uint32_t& oTokenLen){
  memset (oToken, '0', iTokenLen);
  oTokenLen = iTokenLen;
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
  Zero(a,3,b,len);

  for(int i =0; i<3; i++){

    std::cout<<b[i]<<",";
  }
  std::cout<<len<<std::endl;

}

