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
 *             Default is 2000 chars. 
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
#include <vector>
#include <string>

#include <TokenLib1.hpp>

using namespace std;
/**
 * This function convert iToken char array of size iTokenLen, reverse order 
 * and put it in oToken.
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
 * This function convert iToken of size iTokenLen to a zero integer
 *
 */
extern "C"  uint64_t Zero(const char* const iToken, uint32_t iTokenLen) {
  return 0;
}


/**
 * Suppose I have a schema, and the third column is 
 * rating:movieId1;movieId2..|rating:movieId3,movieId4..
 *
 * id,name, p_rate_movie
 * 1,Charlie Sheen,3:1;4|9:2
 *
 * The below function will take token from the p_rate_movie column.
 * And produce 
 * 3 (char)31 1 (char)30 3 (char)31 4 (char)30 9 (char)31 2 (char)30
 *
 * where 31 and 30 are ascii invisible chars. 31 is field separator, 
 * 30 is record separator.
 */
extern "C" void ratingMovie(const char* const iToken, uint32_t iTokenLen, 
    char *const oToken, uint32_t& oTokenLen) {

  int i = 0; //start point

  int j = 0; //current cursor

  oTokenLen = 0;

  while (j<iTokenLen) {
    //find one field
    while (j < iTokenLen ) {

      if (iToken[j] != '|'){
        j ++;
      } else {
        break; 
      }
    }

    //start is i, end is j, find rating
    int r;
    for (int m = i; m < j; m++) {
      if (iToken[m] == ':') {
        r = m;
        break;
      }
    }

    //from r+1 to j, construct tuples
    bool newTuple = true;
    for (int m = r+1; m < j; m++) {

      //create rating field
      if(newTuple){
        newTuple = false ;
        //get first field
        for (int k = i; k<r; k++){
          oToken[oTokenLen ++] = iToken[k];
        }
        //append field separator
        oToken[oTokenLen++] = (char)31;
      } 

      if (iToken[m] != ';') {
        oToken[oTokenLen ++] = iToken[m];
      } else {
        oToken[oTokenLen ++] = (char)30;
        newTuple = true;
      }
    }

    oToken[oTokenLen ++] = (char)30;

    i = j+1;
    j++;
  }

}


/**
 *  Unit testing of the token bank functions
 */ 
/*
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

  char c[14] = {'4',':','1',',','2','|','5',':','3','|','1','0',':','4'};

  uint32_t alen = 0; 
//ABC(c,14, b, alen);

  cout<<"alen= "<<alen<<endl;

  for (int i = 0; i < alen; i++) {
     if (b[i] == (char) 31) {
         cout <<","; 
     } else if (b[i] == (char) 30){
         cout <<"|"; 
     } else {
       cout <<b[i];
     }
  }
  cout <<endl;

}

  */
