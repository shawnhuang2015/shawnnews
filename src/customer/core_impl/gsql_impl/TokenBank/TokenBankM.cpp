/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * TokenBankM.cpp: a library of token conversion function declaration. 
 *
 * - It is an M-tokens-in, one-token-out function.
 * - All functions must use one of the following signatures, 
 *   but different function name.
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
 *            shell config. Default is 2000 chars.
 *             
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
 * - Once defined UDF, run the follow to compile a shared libary.
 *
 *    TokenBank/compile
 *
 *   GraphSQL loader binary will automatically use the library at runtime.
 *
 * - You can unit test your token function in the main function in this file.
 *   To run your test, you can do 
 *
 *     g++ TokenBankM.cpp 
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

#include <TokenLibM.hpp>

 /**
  * this function concatenate all input tokens into one big token
  *
  */
extern "C" void _Concat(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum,
    char* const oToken, uint32_t& oTokenLen){
      
  int k = 0;
  for (int i=0; i < iTokenNum; i++){
    for (int j =0; j < iTokenLen[i]; j++) {
           oToken[k++]=iToken[i][j];
    }
  }
  oTokenLen = k;
}


/**
 *  Unit testing of the token bank functions
 */ 
int main(){

  char* a[2];
  char d[100]={'a','b','c','d','e','f'};

  a[0]=&d[0];
  a[1]=&d[3];

  uint32_t len[2];

  len[0] = 3;
  len[1] = 3;


  char b[100];
  uint32_t  outlen;
  _Concat(a,len,2, b, outlen);
  for(int i =0; i<outlen; i++){
    std::cout<<b[i]<<",";
  }
  std::cout<<std::endl;

}

