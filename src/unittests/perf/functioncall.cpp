#include "functioncall.hpp"

int FunctionCall::direct_func(int a, int b){
  return a + b;
}


int FunctionCall::virtual_func(int a, int b){
  return a - b;
}


int FunctionCall::functionptr_func(int a, int b){
  return a + b;
}

int FunctionCallSub::virtual_func(int a, int b){
  return a + b;
}
