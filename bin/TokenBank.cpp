/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GS-1
 * TokenBank.hpp: a library of token conversion function declaration.
 *
 * All functions must have the same argument signature, but different function 
 * name.
 *
 * funcName(const char* const input, uint32_t input_len, char *const output, uint32_t& output_len);
 *  @param: const char* const input: const pointer, const data
 *  @param: input_len: input char array's length
 *  @param: char* const output: const pointer, non-const data
 *  @param: output_len: output char array's length 
 *
 *  The func will take an input char array and do a customized conversion,
 *  then, put the converted char array to output char buffer.
 *
 *  The function can be unit tested in the main function.
 *
 *  All functions can be used in the loading job definition, in the values caluse.
 *
 *  e.g. 
 *  values( $1, func($2), $3...)
 *
 *
 *  Once defined UDF, run  the following script to compile it to a shared libary.
 *
 *  ./compileTokenBank.sh
 *
 *  The shared library path is specified in run.sh 
 *
 *  export LD_LIBRARY_PATH='/vagrant/repo2/product/bin/'
 *
 *  The loader binary will automatically use the library at runtime.
 *
 *  Created on: Nov 13, 2014
 *  Author: Mingxi
 ******************************************************************************/
#include <graphsqltokefuncs>

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>

/**
 * This function convert input char array of size input_len in reverse order and 
 * put it in output.
 *
 */
extern "C"  void Reverse (const char* const input, uint32_t input_len, 
    char *const output, uint32_t& output_len) {

  uint32_t j =0;
   for (uint32_t i = input_len; i>0; i--){
    output[j++]= input[i-1];     
  }
  output_len =j;
}

/**
 * This function convert input char array of size input_len in reverse order and 
 * put it in output.
 *
 */
extern "C"  void Zero(const char* const input, uint32_t input_len, 
    char *const output, uint32_t& output_len){
  memset (output, '1', input_len );
  output_len = input_len;
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

