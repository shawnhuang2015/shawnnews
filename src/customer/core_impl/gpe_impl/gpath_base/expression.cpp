/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPath
 *
 *  Created on: 6/29/2015
 *  Author: Renchu
 *  Description: Condition Expression Evalutator
 ******************************************************************************/
#include "column_meta.hpp"
#include "variant_numerical_operators.hpp"
#include "expression.hpp"

namespace UDIMPL {

namespace GPATH_TOOLS {
  Token::Token(char tokenTypeChar, std::string& first, std::string& second) {
    switch (tokenTypeChar) {
      case 'C':
        tokenType = CONST;
        switch (getValueType(second)) {
          case INT:
            value = std::atoi(first.c_str());
            break;
          case DOUBLE:
            value = std::atof(first.c_str());
            break;
          case STRING:
            value = first;
            break;
          case BOOL:
            if (first == "true") {
              value = true;
            } else {  // if (first == "false")
              value = false;
            }
            break;
        }
        break;
      case 'A':
        tokenType = REF;
        refName = first;
        break;
      case 'O':
        tokenType = OP;
        opType = getOpType(first);
        break;
    }
  }

  OP_TYPE Token::getOpType(std::string& str) {
    if (str == "<") {
      return LT;
    } else if (str == "<=") {
      return LTE;
    } else if (str == ">") {
      return GT;
    } else if (str == ">=") {
      return GTE;
    } else if (str == "==") {
      return EQ;
    } else if (str == "!=") {
      return NEQ;
    } else if (str == "+") {
      return PLUS;
    } else if (str == "-") {
      return MINUS;
    } else if (str == "*") {
      return MUL;
    } else if (str == "/") {
      return DIV;
    } else if (str == "&&") {
      return AND;
    } else if (str == "||") {
      return OR;
    } else if (str == "!") {
      return NOT;
    }
    return OTHER;
  }

  int Token::getValueType(std::string& str) {
    if (str == "int") {
      return INT;
    } else if (str == "float") {
      return DOUBLE;
    } else if (str == "string") {
      return STRING;
    } else if (str == "bool") {
      return BOOL;
    }
    return -1;
  }

  std::ostream& operator<< (std::ostream& os, Token& data) {
    if (data.tokenType == CONST) os << "C(";
    if (data.tokenType == REF) os << "A(";
    if (data.tokenType == OP) os << "O(";
    if (data.tokenType == REF) os << data.refName << " : " << data.value << ") ";
    else if (data.tokenType == OP){
      if (data.opType == LT) os << "<) ";
      else if (data.opType == LTE) os << "<=) ";
      else if (data.opType == GT) os << ">) ";
      else if (data.opType == GTE) os << ">=) ";
      else if (data.opType == EQ) os << "==) ";
      else if (data.opType == NEQ) os << "!=) ";
      else if (data.opType == PLUS) os << "+) ";
      else if (data.opType == MINUS) os << "-) ";
      else if (data.opType == MUL) os << "*) ";
      else if (data.opType == DIV) os << "/) ";
      else if (data.opType == AND) os << "&&) ";
      else if (data.opType == OR) os << "||) ";
      else if (data.opType == NOT) os << "!) ";
    }
    else if (data.tokenType == CONST) {
      os << data.value << ")";
    }
    return os;
  }
//=====================Expression======================================

  Expression::Expression(): emptyExpr_(true), minLayerIndex_(std::numeric_limits<uint32_t>::max()) {}

  int Expression::numOfOperand(OP_TYPE opType) {
    switch(opType) {
      case LT:
        return 2;
      case LTE:
        return 2;
      case GT:
        return 2;
      case GTE:
        return 2;
      case EQ:
        return 2;
      case NEQ:
        return 2;
      case PLUS:
        return 2;
      case MINUS:
        return 2;
      case MUL:
        return 2;
      case DIV:
        return 2;
      case AND:
        return 2;
      case OR:
        return 2;
      case NOT:
        return 1;
      default:
        return 0;
    }
    return 0;
  }

  uint32_t Expression::getMatchedParam(std::string& str, uint32_t start, std::string& param) {
    uint32_t i;
    if (str.at(start) != '\'') {
      // Normal token
      for (i = start; i < str.length(); i++) {
        if (str.at(i) == ',' || str.at(i) == ')') {
          param = str.substr(start, i - start);
          return i;
        }
      }
    } else {
      // Quoted string
      for (i = start + 1; i < str.length(); i++) {
        if (str.at(i) == '\'' && str.at(i - 1) != '\\') {
          param = str.substr(start + 1, i - start - 1);
          return i + 1;
        }
      }
    }
    param = str.substr(start);
    return i;
  }

  void Expression::set(std::string& condition) {
    emptyExpr_ = false;
    if (condition == "") {
      emptyExpr_ = true;
      return;
    }
    char tokenTypeChar;
    std::string first, second;
    for (uint32_t i = 0; i < condition.length();) {
      tokenTypeChar = condition.at(i);
      i = getMatchedParam(condition, i + 2, first);
      if (condition.at(i) == ',') {
        i = getMatchedParam(condition, i + 1, second);
      }
      Token token(tokenTypeChar, first, second);
      tokens_.push_back(token);
      i += 2;
    }
  }

  Expression::Expression(std::string condition) {
    set(condition);
  }

  void Expression::setColumnMeta(ColumnMeta& attrMeta) {
   // this function is used to reset the refToToken and minLayerIndex_
    refToToken_.clear();
    minLayerIndex_ = std::numeric_limits<uint32_t>::max();
    for (uint32_t i = 0; i < tokens_.size(); ++i) {
      Token& token = tokens_[i];
      if (token.tokenType == REF) {
        uint32_t layer = 0, bias = 0;
        attrMeta.getAttrLayerAndBias(token.refName, layer, bias);
        refToToken_[layer].push_back(INDEX(bias, i));//for REF token, push the last index into the map
        if (layer < minLayerIndex_) {
          minLayerIndex_ = layer;//update minLayer to the lower layer number
        }
      }
    }
  }

  Expression::Expression(std::string& condition, ColumnMeta& attrMeta) {
    set(condition);
    setColumnMeta(attrMeta);
  }

  void Expression::update(uint32_t layer, VECTOR_ANY& vec) {
    if (!refToToken_.count(layer)) {
      return;//if layer is not in any token return
    }
    GVector<INDEX>::T& tk = refToToken_[layer];
    for (uint32_t i = 0; i < tk.size(); ++i) {
      tokens_[tk[i].second].value = vec[tk[i].first];//update the value of the corresponding token
    }
  }

  bool Expression::pass() {
    if (emptyExpr_) {
      return true;
    }
    VAL val = evaluate();
    return boost::get<bool>(val);
  }

  VAL Expression::evaluate() {
    GDeque<VAL>::T stk; // Local stack is used to guarantee multi-thread access
    for (GVector<Token>::T::iterator it = tokens_.begin();
        it != tokens_.end(); it++) {
      if (it->tokenType == OP) {
        // Evaluate operator
        VAL v1, v2 = stk.back();
        stk.pop_back();
        if (numOfOperand(it->opType) == 2) {
          v1 = stk.back();
          stk.pop_back();
        }
        switch(it->opType) {
          case LT:
            stk.push_back(boost::apply_visitor(LessThan(), v1, v2));
            break;
          case LTE:
            stk.push_back(boost::apply_visitor(LessEqual(), v1, v2));
            break;
          case GT:
            stk.push_back(boost::apply_visitor(GreaterThan(), v1, v2));
            break;
          case GTE:
            stk.push_back(boost::apply_visitor(GreaterEqual(), v1, v2));
            break;
          case EQ:
            stk.push_back(boost::apply_visitor(Equal(), v1, v2));
            break;
          case NEQ:
            stk.push_back(boost::apply_visitor(NotEqual(), v1, v2));
            break;
          case PLUS:
            stk.push_back(boost::apply_visitor(Plus(), v1, v2));
            break;
          case MINUS:
            stk.push_back(boost::apply_visitor(Minus(), v1, v2));
            break;
          case MUL:
            stk.push_back(boost::apply_visitor(Mul(), v1, v2));
            break;
          case DIV:
            stk.push_back(boost::apply_visitor(Div(), v1, v2));
            break;
          case AND:
            stk.push_back(boost::apply_visitor(And(), v1, v2));
            break;
          case OR:
            stk.push_back(boost::apply_visitor(Or(), v1, v2));
            break;
          case NOT:
            stk.push_back(boost::apply_visitor(Not(), v2));
            break;
          default:
            break;
        }
      }
      else {
          // Get const value from token vector
          stk.push_back(it->value);
      }
    }

    return stk.back();
  }

  std::ostream& operator<< (std::ostream& os, Expression& data) {
    for (uint32_t i = 0; i < data.tokens_.size(); ++i) {
      os << data.tokens_[i];
    }
    os << std::endl << "Layer and bias" << std::endl;
    for (GUnorderedMap<uint32_t, GVector<Expression::INDEX>::T >::T::iterator it = data.refToToken_.begin();
      it != data.refToToken_.end(); ++it) {
      os << it->first << " : " << std::endl;
      for (uint32_t j = 0; j < it->second.size(); ++j) {
        os << it->second[j].first << ", " << it->second[j].second << "   ";
      }
      os << std::endl;
    }
    os << "Min Layer: " << data.minLayerIndex_ << std::endl;
    return os;
  }
 }//end gpath
} // namespace UDIMPL

