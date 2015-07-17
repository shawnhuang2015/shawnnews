/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Loader
 * ConditionLibM.hpp: a system library of token conversion function declaration. 
 *
 * - It is an M-tokens-in boolean function.
 * - All functions must have the same funcion signature,
 *   you can have different function name but you must follow the name convetion
 *   using snake case gsql_funcname (...). E.g.  gsql_find_max_digit (...).
 *   see http://en.wikipedia.org/wiki/Snake_case
 *
 *     extern "C" bool conditionFuncName (const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum)
 *     
 *      @param: iToken: 1 or M input tokens, each pointed by one char pointer 
 *      @param: iTokenLen: each input token's length
 *
 *      Note: extern "C" make C++ compiler not change/mangle the function name.
 *
 *   The UDF token conversion functions will take M input char array and do a customized check.
 *   Then, return true or false depending on the tokens pass the condition or not. 
 *
 * - All functions can be used in the loading job definition, in the WHERE caluse.
 *    e.g. Let a function named SumGreaterThan3(), we can use it in the DDL shell as below
 *
 *     load "xx" into vertex abc values($1,$2) where SumGreaterThan3($0,$1) using seperator=",";
 *
 * - The declaration is in ConditionLibM.hpp
 *
 * - You can unit test your token function in the main.cc
 *   To run your test, you can do 
 *
 *     g++ -I./ main.cpp TokenLib1.cpp TokenLibM.cpp ConditionLib1.cpp ConditionLibM.cpp
 *     ./a.out
 *
 * - You must use GTEST to test the UDFs before deliver to customers
 *
 * Created on: Mar 05, 2015
 * Author: Zixuan Zhuang, Mingxi Wu
 ******************************************************************************/

#include <ConditionLibM.hpp>


/**
 * This funtion compares two string, and returns true if they are equal.
 *   - if there isn't two string, return false.
 *   - if two string are not equal length, return false.
 *   - if any character is not equal, return false.
 */

extern "C" bool gsql_token_equal(const char* const iToken[], uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenNum != 2) {
    return false;
  }

  if (iTokenLen[0] != iTokenLen[1]) {
    return false;
  }

  for (int i =0; i < iTokenLen[0]; i++) {
    if (iToken[0][i] != iToken[1][i]) {
      return false;
    }
  }

  return true;
}

/**
 * This funtion compares two string in case insensitive, and returns true if they are equal.
 *   - if there isn't two string, return false.
 *   - if two string are not equal length, return false.
 *   - if any character is not equal, return false.
 *
 */

extern "C" bool gsql_token_ignore_case_equal(const char* const iToken[], 
    uint32_t iTokenLen[], uint32_t iTokenNum) {

  if (iTokenNum != 2) {
    return false;
  }

  if (iTokenLen[0] != iTokenLen[1]) {
    return false;
  }

  for (int i =0; i < iTokenLen[0]; i++) {
    if (tolower(iToken[0][i]) != tolower(iToken[1][i])) {
      return false;
    }
  }

  return true;
}

/**
 * Predicate testing helper function. 
 *    E.g. WHERE $1 IS NUMERIC 
 *    Will call this function.
 *
 * IsNumeric test whether a string is a numeric number, implemented as an FSM.
 * 
 * Numeric number is defined as follows in regular expression:
 *      (+/-)?  [0-9]   [0-9]*    (.[0-9])?   [0-9]*  ((e/E) (+/-)?  [0-9]    [0-9]*)?    
 *  ^     ^       ^       ^        ^   ^         ^       ^     ^       ^         ^         ^
 *  |     |       |       |        |   |         |       |     |       |         |         |
 * INIT  SIGN   START   MIDDLE    DOT  TAIL     TAIL    EXP  EXPSIGN EXPDGT  EXPDGTAIL    END
 *
 * Any space appear in between will not be taken as numeric number. 
 * Space in front and at the end is OK.
 *
 * Author: Chengjie Qin
 * Created: July, 2015
 */

extern "C" bool IsNumeric (std::string s) { 
  enum State {INIT, SIGN, START, MIDDLE, DOT, 
    TAIL, EXP, EXPSIGN, EXPDGT, EXPDGTAIL, END};
  size_t i = 0;
  State current = INIT;

  while (i < s.size()) {
    switch (current) {
      case INIT:
        if (s[i] == ' ') {
          // stays same
        } else if ( std::isdigit(s[i]) ) {
          current = START;
        } else if (s[i] == '-' || s[i] == '+') {
          current = SIGN;
        } else {
          return false;
        }
        break;
      case SIGN:
        if ( std::isdigit(s[i]) ) {
          current = START;
        } else if (s[i] == ' ') {
          current = END;
        } else {
          return false;
        }
        break;
      case START:
        if ( std::isdigit(s[i]) ) {
          current = MIDDLE;
        } else if (s[i] == '.') {
          current = DOT;
        } else if (s[i] == ' ') {
          current = END;
        } else if (s[i] == 'e' ||
            s[i] == 'E') {
          current = EXP;
        } else {
          return false;
        }
        break;
      case MIDDLE:
        if ( std::isdigit(s[i]) ) {
          // stays same
        } else if (s[i] == '.') {
          current = DOT;
        } else if (s[i] == ' ') {
          current = END;
        } else if (s[i] == 'e' ||
            s[i] == 'E') {
          current = EXP;
        } else {
          return false;
        }
        break;
      case DOT:
        if ( std::isdigit(s[i]) ) {
          current = TAIL;
        } else {
          return false;
        }
        break;
      case TAIL:
        if ( std::isdigit(s[i]) ) {
          // stays same
        } else if (s[i] == ' ') {
          current = END;
        } else if (s[i] == 'e' ||
            s[i] == 'E') {
          current = EXP;
        } else {
          return false;
        }
        break;
      case EXP:
        if ( std::isdigit(s[i]) ) {
          current = EXPDGT;
        } else if (s[i] == '-' ||
            s[i] == '+') {
          current = EXPSIGN;
        } else {
          return false;
        }
        break;
      case EXPSIGN:
        if ( std::isdigit(s[i]) ) {
          current = EXPDGT;
        } else {
          return false;
        }
        break;
      case EXPDGT:
        if ( std::isdigit(s[i]) ) {
          current = EXPDGTAIL;
        } else if (s[i] == ' ') {
          current = END;
        } else {
          return false;
        }
        break;
      case EXPDGTAIL:
        if ( std::isdigit(s[i]) ) {
            ;// stays the same
        } else if (s[i] == ' ') {
          current = END;
        } else {
          return false;
        }
        break;
      case END:
        if (s[i] == ' ') {
          ;// stays the same
        } else {
          return false;
        }
        break;
      default:
        return false;
    }
    i ++;
  }

  if (current == TAIL || current == START 
      || current == MIDDLE || current == END
      || current == EXPDGT || current == EXPDGTAIL) {
    return true;
  }
  return false;
}
