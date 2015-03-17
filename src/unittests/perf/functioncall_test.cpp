#include <gtest/gtest.h>
#include <gutil/gtimer.hpp>
#include "functioncall.hpp"

TEST(FunctionCall, Run) {
  size_t count = 1 << 30;
  std::cout << "Test " << count << " number of function call (add 2 integers)\n";
  size_t sum = 0;
  {
    gutil::GTimer timer;
    FunctionCall obj;
    for(size_t i = 0; i < count; ++i)
      sum += obj.inline_func(i, i);
    ASSERT_EQ(sum, count * (count - 1));
    timer.Stop("inline_func");
  }
  {
    sum = 0;
    gutil::GTimer timer;
    FunctionCall obj;
    for(size_t i = 0; i < count; ++i)
      sum += obj.direct_func(i, i);
    ASSERT_EQ(sum, count * (count - 1));
    timer.Stop("direct_func");
  }
  {
    sum = 0;
    gutil::GTimer timer;
    FunctionCall obj;
    for(size_t i = 0; i < count; ++i)
      sum += obj.virtual_func(i, i);
    ASSERT_EQ(sum, 0ul);
    timer.Stop("virtual on base");
  }
  {
    sum = 0;
    gutil::GTimer timer;
    FunctionCallSub obj;
    for(size_t i = 0; i < count; ++i)
      sum += obj.virtual_func(i, i);
    ASSERT_EQ(sum, count * (count - 1));
    timer.Stop("virtual on sub");
  }
  {
    sum = 0;
    gutil::GTimer timer;
    int (*foo)(int, int);
    foo = &FunctionCall::functionptr_func;
    for(size_t i = 0; i < count; ++i)
      sum += foo(i, i);
    ASSERT_EQ(sum, count * (count - 1));
    timer.Stop("function ptr");
  }
}
