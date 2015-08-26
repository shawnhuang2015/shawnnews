#include "../context_table.hpp"

using namespace UDIMPL::GPATH_TOOLS;

int main() {
  CONTEXT_TABLE_PTR nullPtr((ContextTable*)NULL);
  CONTEXT_TABLE_PTR pt1 = CONTEXT_TABLE_PTR(new ContextTable(nullPtr));
  CONTEXT_TABLE_PTR pt2 = CONTEXT_TABLE_PTR(new ContextTable(nullPtr));
  CONTEXT_TABLE_PTR pt3 = CONTEXT_TABLE_PTR(new ContextTable(nullPtr));
  CONTEXT_TABLE_PTR pt4 = CONTEXT_TABLE_PTR(new ContextTable(nullPtr));
  CONTEXT_TABLE_PTR pt5 = CONTEXT_TABLE_PTR(new ContextTable(nullPtr));
  ContextTableMessage cm1(pt1);
  ContextTableMessage cm2(pt2);
  ContextTableMessage cm3(pt3);
  ContextTableMessage cm4(pt4);
  ContextTableMessage cm5(pt5);
  
  std::cout << cm1 << std::endl;
  std::cout << cm2 << std::endl;
  std::cout << cm3 << std::endl;
  std::cout << cm4 << std::endl;
  std::cout << cm5 << std::endl;
  
  ContextTableMessage cm6 = cm1;
  cm6 += cm2;
  cm6 += cm3;
  std::cout << cm6 << std::endl;

  ContextTableMessage cm7 = cm1;
  cm7 += cm3;
  cm7 += cm4;
  std::cout << cm6 << std::endl;
  
  cm6 += cm7;
  std::cout << cm6 << std::endl;

  cm6 += cm7;
  std::cout << cm6 << std::endl;
  
  cm6 += cm2;
  std::cout << cm6 << std::endl;
  return 0;
}
