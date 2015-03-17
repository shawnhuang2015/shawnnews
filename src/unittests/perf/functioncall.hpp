#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <gutil/gpelibidtypes.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

class FunctionCall{
public:
  ALWAYS_INLINE int inline_func(int a, int b){
    return a + b;
  }

  int direct_func(int a, int b);

  virtual int virtual_func(int a, int b);

  static int functionptr_func(int a, int b);
};

class FunctionCallSub: public FunctionCall{
public:
  virtual int virtual_func(int a, int b);
};

#endif // FUNCTIONCALL_HPP

