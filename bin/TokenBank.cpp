/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GS-1
 * TokenBank.hpp: a set of token conversion function declaration.
 *
 * All functions must have the same argument signature, but different function 
 * name.
 *
 * funcName(const char* const input, uint32_t input_len, char *const output);
 *  @param: const char* const input: const pointer, const data
 *  @param: input_len: input char array's length
 *  @param: char* const output: const pointer, non-const data
 *
 *  The func will take a input char array and do a customized conversion,
 *  then, put the converted char array to output char buffer.
 *
 *  Created on: Nov 13, 2014
 *  Author: Mingxi
 ******************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <cstring>

/**
 * This function convert input char array of size input_len in reverse order and 
 * put it in output.
 *
 */
extern "C"  void Reverse(const char* const input, uint32_t input_len, char *const output){
  uint32_t j =0;
  for (uint32_t i = input_len; i>0; i--){
    output[j++]= input[i-1];     
  }
}

/**
 * This function convert input char array of size input_len in reverse order and 
 * put it in output.
 *
 */
extern "C"  void Zero(const char* const input, uint32_t input_len, char *const output){
  uint32_t j =0;
  memset (output, '1', input_len );
}

int main(){

  char a[3]={'a','b','c'};
  char b[100];

  Reverse(a,3,b);

  for(int i =0; i<3; i++){
    std::cout<<b[i]<<",";
  }

  std::cout<<std::endl;

  Zero(a,3,b);

  for(int i =0; i<3; i++){

    std::cout<<b[i]<<",";
  }
  std::cout<<std::endl;

}

