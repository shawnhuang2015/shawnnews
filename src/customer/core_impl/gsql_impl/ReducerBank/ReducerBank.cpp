/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ReducerBank.cpp: a library of reudce function declaration and implementation.
 *
 * - It takes a list of token, reduce them to one value. 
 * - All functions must use one of the following signatures, 
 *   but different function name.
 *
 *   1. vector<string> -> string 
 *
 *      The UDF reduce functions will take an input char array and do 
 *      a customized reduction. Then, put the scalar output is returned to a 
 *      char buffer. 
 *
 *      extern "C" void funcName ( const vector<const char*> iTokenList, vector<uint32_t> iTokenLen, 
 *                           char *const oToken, uint32_t& oTokenLen);
 *
 *       @param: iTokenList: A list of tokens subject to reduce.
 *       @param: iTokenLen: each input token length 
 *       @param: oToken: the output token buffer; caller will prepare this buffer
 *       @param: oTokenLen: the output token length 
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
 *      extern "C" bool funcName (const vector<bool> iTokenList);
 *      extern "C" uint64_t funcName (const vector<uint64_t> iTokenList);
 *      extern "C" float funcName (const vector<float> iTokenList);
 *
 *       @param: iTokenList: A list of values subject to reduce.
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
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <set>

using namespace std;
/**
 * This function convert iToken char array of size iTokenLen, reverse order 
 * and put it in oToken.
 *
 */

extern "C" bool OR(const vector<bool> iTokenList) {
  int size = iTokenList.size();
  for (int i = 0 ; i < size; i++) {
    if (iTokenList[i]) {
      return true;
    }
  }
  return false;
}

extern "C" uint64_t MySum(const vector<uint64_t> iTokenList) {

    int size = iTokenList.size();
    uint64_t sum=0;

    for (int i =0 ; i < size; i++){
          sum += iTokenList[i]; 
    }

    return sum;
}


extern "C" void _Concat( const vector<const char*> iTokenList, vector<uint32_t> iTokenLen, 
                          char *const oToken, uint32_t& oTokenLen){


    int size = iTokenList.size();
    int index = 0;

    for (int i =0 ; i < size; i++){
      for (int j = 0; j< iTokenLen[i]; j++){
          oToken[index++] = iTokenList[i][j];
      }
    }

    oTokenLen = index;
}


/**
 *  Unit testing of the token bank functions
 */ 
int main(){

  vector<uint64_t> input;

  input.push_back(1);
  input.push_back(2);
  input.push_back(3);
  input.push_back(4);

  cout<<MySum(input)<<endl;


  vector<const char*> input2;
  vector<uint32_t> inputLen;

  char a[3]= {'a','b','c'};
  char b[2]= {'d','e'};
  char c[1]= {'f'};

  input2.push_back(a);
  input2.push_back(b);
  input2.push_back(c);

  inputLen.push_back(3);
  inputLen.push_back(2);
  inputLen.push_back(1);

  uint32_t outputLen;
  char outputBuffer[2000];

   _Concat(input2, inputLen, outputBuffer, outputLen);

   for (int i=0; i<outputLen; i ++){
     cout<<outputBuffer[i]<<"," ;
   }
   cout<<endl;


}

