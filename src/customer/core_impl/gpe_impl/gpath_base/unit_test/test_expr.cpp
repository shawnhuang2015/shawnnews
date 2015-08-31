#include "expression.hpp"
#include "contextTable.hpp"

using namespace UDIMPL;
using namespace UDIMPL::GPATH_TOOLS;

void evaluate(bool result, bool expect, std::string exprstr) {
  if (result == expect) {
    std::cout << ".";
  } else {
    std::cout << std::endl << "failed " << exprstr << std::endl;
  }
}

Expression newExpr(std::string expStr) {
  Expression expr(expStr);
  return expr;
}

int main() {
  //========================= Test Single Expression ==========================
  ContextTuple fullTuple;
  ContextTable ct;
  VECTOR_STRING vs;
  vs.push_back("i_0");
  vs.push_back("i_1");
  vs.push_back("i_2");
  vs.push_back("i_3");
  vs.push_back("i_4");
  vs.push_back("f_0");
  vs.push_back("f_1");
  vs.push_back("f_2");
  vs.push_back("f_3");
  vs.push_back("f_4");
  vs.push_back("s_0");
  vs.push_back("s_00");
  vs.push_back("s_src");
  vs.push_back("s_1");
  vs.push_back("b_t");
  vs.push_back("b_f");
  VECTOR_ANY vc;
  vc.push_back(0);
  vc.push_back(1);
  vc.push_back(2);
  vc.push_back(3);
  vc.push_back(4);
  vc.push_back(0.0);
  vc.push_back(1.0);
  vc.push_back(2.0);
  vc.push_back(3.0);
  vc.push_back(4.0);
  std::string s = "0";
  vc.push_back(s);
  s = "00";
  vc.push_back(s);
  s = "src";
  vc.push_back(s);
  s = "1";
  vc.push_back(s);
  vc.push_back(true);
  vc.push_back(false);
  ct.addColumns(vs, vc);
  ct.getContextTuple(0, fullTuple);
  std::string expStr;

  //--------------------- empty filter --------------------------
  expStr = "";
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  //-------------------------- < --------------------------------
  expStr = "C(2,int) C(3,int) O(<)";              // 2 < 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "C(4,int) C(3,int) O(<)";              // 4 < 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(f_3,float) O(<)";          // 2 < 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(i_4,int) C(3,float) O(<)";          // 4 < 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_t,bool) O(<)";           // 2 < true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(<)";           // 2 < false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(s_0,string) O(<)";         // 2.0 < "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(<)";         // 0.0 < "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(<)";            // 2.0 < 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "C(4,float) C(3,int) O(<)";            // 4.0 < 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(f_3,float) O(<)";        // 2.0 < 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(f_4,float) C(3,float) O(<)";        // 4.0 < 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_t,bool) O(<)";         // 2.0 < true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(<)";         // 2.0 < false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(<)";       // 2.0 < "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,float) A(s_1,string) O(<)";       // 0.0 < "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(<)";           // true < 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(<)";         // false < 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(<)";     // true < "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(<)";       // true < false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(<)";      // false < false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(<)";       // true < true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(<)";      // false < true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(<)";   // "0" < "00"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_00,string) A(s_0,string) O(<)";   // "00" < "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,int) O(<)";         // "0" < 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(<)";       // "0" < 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(<)";      // "0" < true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(<)";      // "0" < false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //-------------------------- <= --------------------------------
  expStr = "C(2,int) C(3,int) O(<=)";             // 2 <= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,int) C(3,int) O(<=)";             // 3 <= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "C(4,int) C(3,int) O(<=)";             // 4 <= 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(f_3,float) O(<=)";         // 2 <= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,int) A(f_3,float) O(<=)";         // 3 <= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(i_4,int) C(3,float) O(<=)";         // 4 <= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_t,bool) O(<=)";          // 2 <= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(<=)";          // 2 <= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(s_0,string) O(<=)";        // 2.0 <= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(<=)";        // 0.0 <= "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(<=)";           // 2.0 <= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,float) C(3,int) O(<=)";           // 3.0 <= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(4,float) C(3,int) O(<=)";           // 4.0 <= 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(f_3,float) O(<=)";       // 2.0 <= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,float) A(f_3,float) O(<=)";       // 3.0 <= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(f_4,float) C(3,float) O(<=)";       // 4.0 < 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_t,bool) O(<=)";        // 2.0 <= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(<=)";        // 2.0 <= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(<=)";      // 2.0 <= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,float) A(s_1,string) O(<=)";      // 0.0 <= "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(<=)";          // true <= 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(<=)";        // false <= 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(<=)";    // true <= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(<=)";      // true <= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(<=)";     // false <= false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(<=)";      // true <= true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(<=)";     // false <= true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(<=)";  // "0" <= "00"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) A(s_0,string) O(<=)";  // "0" <= "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(s_00,string) A(s_0,string) O(<=)";  // "00" <= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,int) O(<=)";        // "0" <= 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(<=)";      // "0" <= 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(<=)";     // "0" <= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(<=)";     // "0" <= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //-------------------------- > --------------------------------
  expStr = "C(2,int) C(3,int) O(>)";              // 2 > 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(4,int) C(3,int) O(>)";              // 4 > 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(f_3,float) O(>)";          // 2 > 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(i_4,int) C(3,float) O(>)";          // 4 > 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(b_t,bool) O(>)";           // 2 > true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(>)";           // 2 > false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(s_0,string) O(>)";         // 2.0 > "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(>)";         // 0.0 > "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(>)";            // 2.0 > 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(4,float) C(3,int) O(>)";            // 4.0 > 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(f_3,float) O(>)";        // 2.0 > 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(f_4,float) C(3,float) O(>)";        // 4.0 > 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(b_t,bool) O(>)";         // 2.0 > true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(>)";         // 2.0 > false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(>)";       // 2.0 > "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,float) A(s_1,string) O(>)";       // 0.0 > "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(>)";           // true > 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(>)";         // false > 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(>)";     // true > "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(>)";       // true > false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(>)";      // false > false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(>)";       // true > true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(>)";      // false > true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(>)";   // "0" > "00"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_00,string) A(s_0,string) O(>)";   // "00" > "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) C(1,int) O(>)";         // "0" > 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(>)";       // "0" > 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(>)";      // "0" > true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(>)";      // "0" > false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //-------------------------- >= --------------------------------
  expStr = "C(2,int) C(3,int) O(>=)";             // 2 >= 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,int) C(3,int) O(>=)";             // 3 >= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "C(4,int) C(3,int) O(>=)";             // 4 >= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(f_3,float) O(>=)";         // 2 >= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,int) A(f_3,float) O(>=)";         // 3 >= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(i_4,int) C(3,float) O(>=)";         // 4 >= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(b_t,bool) O(>=)";          // 2 >= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(>=)";          // 2 >= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(s_0,string) O(>=)";        // 2.0 >= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(>=)";        // 0.0 >= "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(>=)";           // 2.0 >= 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,float) C(3,int) O(>=)";           // 3.0 >= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(4,float) C(3,int) O(>=)";           // 4.0 >= 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(f_3,float) O(>=)";       // 2.0 >= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,float) A(f_3,float) O(>=)";       // 3.0 >= 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(f_4,float) C(3,float) O(>=)";       // 4.0 > 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(b_t,bool) O(>=)";        // 2.0 >= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(>=)";        // 2.0 >= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(>=)";      // 2.0 >= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,float) A(s_1,string) O(>=)";      // 0.0 >= "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(>=)";          // true >= 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(>=)";        // false >= 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(>=)";    // true >= "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(>=)";      // true >= false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(>=)";     // false >= false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(>=)";      // true >= true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(>=)";     // false >= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(>=)";  // "0" >= "00"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) A(s_0,string) O(>=)";  // "0" >= "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(s_00,string) A(s_0,string) O(>=)";  // "00" >= "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) C(1,int) O(>=)";        // "0" >= 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(>=)";      // "0" >= 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(>=)";     // "0" >= true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(>=)";     // "0" >= false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //-------------------------- == --------------------------------
  expStr = "C(2,int) C(3,int) O(==)";             // 2 == 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,int) C(3,int) O(==)";             // 3 == 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "C(4,int) C(3,int) O(==)";             // 4 == 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(f_3,float) O(==)";         // 2 == 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,int) A(f_3,float) O(==)";         // 3 == 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(i_4,int) C(3,float) O(==)";         // 4 == 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_t,bool) O(==)";          // 2 == true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(==)";          // 2 == false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(s_0,string) O(==)";        // 2.0 == "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(==)";        // 0.0 == "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(==)";           // 2.0 == 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,float) C(3,int) O(==)";           // 3.0 == 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(4,float) C(3,int) O(==)";           // 4.0 == 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(f_3,float) O(==)";       // 2.0 == 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(3,float) A(f_3,float) O(==)";       // 3.0 == 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(f_4,float) C(3,float) O(==)";       // 4.0 == 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_t,bool) O(==)";        // 2.0 == true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(==)";        // 2.0 == false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(==)";      // 2.0 == "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,float) A(s_1,string) O(==)";      // 0.0 == "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(==)";          // true == 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(==)";        // false == 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(==)";    // true == "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(==)";      // true == false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(==)";     // false == false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(==)";      // true == true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(==)";     // false == true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(==)";  // "0" == "00"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) A(s_0,string) O(==)";  // "0" == "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
  
  expStr = "A(s_00,string) A(s_0,string) O(==)";  // "00" == "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,int) O(==)";        // "0" == 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(==)";      // "0" == 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(==)";     // "0" == true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(==)";     // "0" == false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //-------------------------- != --------------------------------
  expStr = "C(2,int) C(3,int) O(!=)";             // 2 != 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,int) C(3,int) O(!=)";             // 3 != 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);
  
  expStr = "C(4,int) C(3,int) O(!=)";             // 4 != 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(f_3,float) O(!=)";         // 2 != 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,int) A(f_3,float) O(!=)";         // 3 != 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);
  
  expStr = "A(i_4,int) C(3,float) O(!=)";         // 4 != 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(b_t,bool) O(!=)";          // 2 != true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(b_f,bool) O(!=)";          // 2 != false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) A(s_0,string) O(!=)";        // 2.0 != "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(0,int) A(s_1,string) O(!=)";        // 0.0 != "1"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,int) O(!=)";           // 2.0 != 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,float) C(3,int) O(!=)";           // 3.0 != 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(4,float) C(3,int) O(!=)";           // 4.0 != 3
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(f_3,float) O(!=)";       // 2.0 != 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,float) A(f_3,float) O(!=)";       // 3.0 != 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);
  
  expStr = "A(f_4,float) C(3,float) O(!=)";       // 4.0 != 3.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(b_t,bool) O(!=)";        // 2.0 != true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(b_f,bool) O(!=)";        // 2.0 != false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) A(s_0,string) O(!=)";      // 2.0 != "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(0,float) A(s_1,string) O(!=)";      // 0.0 != "1"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(b_t,bool) C(1,int) O(!=)";          // true != 1
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(b_f,bool) C(1,float) O(!=)";        // false != 1.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(true,bool) A(s_0,string) O(!=)";    // true != "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(!=)";      // true != false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(!=)";     // false != false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(!=)";      // true != true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(!=)";     // false != true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(!=)";  // "0" != "00"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) A(s_0,string) O(!=)";  // "0" != "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);
  
  expStr = "A(s_00,string) A(s_0,string) O(!=)";  // "00" != "0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) C(1,int) O(!=)";        // "0" != 1
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) C(1,float) O(!=)";      // "0" != 1.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(!=)";     // "0" != true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(!=)";     // "0" != false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  //------------------------- and -------------------------------
  expStr = "C(2,int) C(3,int) O(&&)";            // 2 and 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(f_3,float) O(&&)";        // 2 and 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_t,bool) O(&&)";         // 2 and true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(&&)";         // 2 and false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(&&)";       // 0.0 and "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(&&)";          // 2.0 and 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(f_3,float) O(&&)";      // 2.0 and 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_t,bool) O(&&)";       // 2.0 and true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(&&)";       // 2.0 and false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(&&)";     // 2.0 and "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(&&)";         // true and 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,int) O(&&)";         // false and 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,float) O(&&)";       // true and 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(&&)";       // false and 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(&&)";   // true and "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(&&)";     // true and false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(&&)";    // false and false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(&&)";     // true and true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(&&)";    // false and true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(&&)"; // "0" and "00"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_00,string) A(s_0,string) O(&&)"; // "00" and "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,int) O(&&)";       // "0" and 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(&&)";     // "0" and 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(&&)";    // "0" and true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(&&)";    // "0" and false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //------------------------- or -------------------------------
  expStr = "C(2,int) C(3,int) O(||)";             // 2 or 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(f_3,float) O(||)";         // 2 or 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_t,bool) O(||)";          // 2 or true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,int) A(b_f,bool) O(||)";          // 2 or false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(0,int) A(s_1,string) O(||)";        // 0.0 or "1"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) C(3,int) O(||)";           // 2.0 or 3
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(f_3,float) O(||)";       // 2.0 or 3.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_t,bool) O(||)";        // 2.0 or true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(b_f,bool) O(||)";        // 2.0 or false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) A(s_0,string) O(||)";      // 2.0 or "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,int) O(||)";          // true or 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,int) O(||)";          // false or 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) C(1,float) O(||)";        // true or 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_f,bool) C(1,float) O(||)";        // false or 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(s_0,string) O(||)";    // true or "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_f,bool) O(||)";      // true or false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_f,bool) O(||)";     // false or false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(true,bool) A(b_t,bool) O(||)";      // true or true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(false,bool) A(b_t,bool) O(||)";     // false or true
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_0,string) A(s_00,string) O(||)";  // "0" or "00"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_00,string) A(s_0,string) O(||)";  // "00" or "0"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,int) O(||)";        // "0" or 1
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(1,float) O(||)";      // "0" or 1.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_t,bool) O(||)";     // "0" or true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_0,string) C(b_f,bool) O(||)";     // "0" or false
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //------------------------- not -------------------------------
  expStr = "C(2,int) O(!)";                     // !2
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(2,float) O(!)";                   // !2.0
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(b_t,bool) O(!)";                  // !true
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "C(false,bool) O(!)";                // !false
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_src,string) O(!)";              // !"src"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  expStr = "A(s_00,string) O(!)";               // !"00"
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  //--------------------------- + --------------------------------
  expStr = "C(2,int) C(3,int) O(+) C(5,int) O(==)";               // (2 + 3) == 5
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) C(3,float) O(+) C(5,float) O(==)";           // (2 + 3.0) == 5.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,int) O(+) C(5,float) O(==)";           // (2.0 + 3) == 5
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,float) O(+) C(5,float) O(==)";         // (2.0 + 3.0) == 5.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);
 
  expStr = "C(false,bool) C(true,bool) O(+) C(1,int) O(==)"; // (false + true) == 1
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "A(s_src,string) A(s_0,string) O(+) C(\'src0\',string) O(==)"; // ("src" + "0") == "src0"
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  //--------------------------- - --------------------------------
  expStr = "C(2,int) C(3,int) O(-) C(-1,int) O(==)";              // (2 - 3) == -1
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) C(3,float) O(-) C(-1.0,float) O(==)";        // (2 - 3.0) == -1.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,int) O(-) C(-1,float) O(==)";          // (2.0 - 3) == -1.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,float) O(-) C(-1,float) O(==)";        // (2.0 - 3.0) == -1.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  //--------------------------- * --------------------------------
  expStr = "C(2,int) C(3,int) O(*) C(6,int) O(==)";               // (2 * 3) == 6
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,int) C(3,float) O(*) C(6.0,float) O(==)";         // (2 * 3.0) == 6.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,int) O(*) C(6,float) O(==)";           // (2.0 * 3) == 6.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(2,float) C(3,float) O(*) C(6,float) O(==)";         // (2.0 * 3.0) == 6.0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  //--------------------------- / --------------------------------
  expStr = "C(2,int) C(3,int) O(/) C(0,int) O(==)";               // (2 / 3) == 0
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,int) C(2,float) O(/) C(1.5,float) O(==)";         // (3 / 2.0) == 1.5
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,float) C(2,int) O(/) C(1.5,float) O(==)";         // (3.0 / 2) == 1.5
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(3,float) C(2,float) O(/) C(1.5,float) O(==)";       // (3.0 / 2.0) == 1.5
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  //----------------------- complex -----------------------------
  expStr = "C(1.2,float) C(2,int) O(*) A(f_2,float) O(<) O(!)";// not(1.2*2 < @(f_2,float))
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  expStr = "C(1.2,float) C(2,int) O(*) A(f_3,float) O(<) O(!)";// not(1.2*2 < @(f_3,float))
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  // ((2 * 4 + 5 <  3 * @(i_1,int) and @(f_3,float) < 4) or (true == true))
  expStr = "C(2,int) C(4,int) O(*) C(5,int) O(+) C(3,int) A(i_1,int) O(*) O(<) A(f_3,float) C(4,int) O(<) O(&&) C(true,bool) C(true,bool) O(==) O(||)";
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  // ((2 * 4 + 1 <  3 * @(i_4,int) and @(f_3,float) < 4) or (true == false))
  expStr = "C(2,int) C(4,int) O(*) C(1,int) O(+) C(3,int) A(i_4,int) O(*) O(<) A(f_3,float) C(4,int) O(<) O(&&) C(true,bool) C(false,bool) O(==) O(||)";
  evaluate(newExpr(expStr).pass(fullTuple), true, expStr);

  // ((2 * 4 + 5 <  3 * @(i_4,int) and @(f_3,float) < 4) or (true == false))
  expStr = "C(2,int) C(4,int) O(*) C(5,int) O(+) C(3,int) A(i_4,int) O(*) O(<) A(f_3,float) C(4,int) O(<) O(&&) C(true,bool) C(false,bool) O(==) O(||)";
  evaluate(newExpr(expStr).pass(fullTuple), false, expStr);

  std::cout << std::endl;  
  return 0;
}

