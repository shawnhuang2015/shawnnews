/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPath
 *
 *  Created on: 6/29/2015
 *  Author: Renchu
 *  Description: Condition Expression Evalutator
 ******************************************************************************/
#include <gpe/serviceimplbase.hpp>
#include <gutil/gsqlcontainer.hpp>
#include "boost/variant.hpp"
#include "gpathTypeDefines.hpp"

#ifndef SRC_CUSTOMER_COREIMPL_EXPRESSION_HPP_
#define SRC_CUSTOMER_COREIMPL_EXPRESSION_HPP_


namespace UDIMPL {
  namespace GPATH_TOOLS{
    enum TOKEN_TYPE {CONST, REF, OP};
    enum OP_TYPE {LT, LTE, GT, GTE, EQ, NEQ, PLUS, MINUS, MUL, DIV, AND, OR, NOT, OTHER};

    class ColumnMeta;//forward declaration

    struct Token {
      TOKEN_TYPE tokenType;
    
      OP_TYPE opType;
      std::string refName;
      VAL value;
    
      Token() {}
      Token(char tokenTypeChar, std::string& first, std::string& second);
      OP_TYPE getOpType(std::string& str);
      int getValueType(std::string& str);
    
      friend std::ostream& operator<< (std::ostream& os, Token& data);
    };//End Token
    
    class Expression {
    private:
      typedef std::pair<uint32_t, uint32_t> INDEX;
      GUnorderedMap<uint32_t, GVector<INDEX>::T >::T refToToken_;  // From ref attr int to token index
      int numOfOperand(OP_TYPE opType);
      uint32_t getMatchedParam(std::string& str, uint32_t start, std::string& param);
      void set(std::string& condition);
    public:
      bool emptyExpr_;
      GVector<Token>::T tokens_;
      uint32_t minLayerIndex_;                                   // minimal layer number
      Expression();
      Expression(std::string condition);
      Expression(std::string& condition, ColumnMeta& attrMeta);
      bool pass();
      void setColumnMeta(ColumnMeta& attrMeta);
      VAL evaluate();
      void update(uint32_t layer, VECTOR_ANY& vec);
      friend std::ostream& operator<< (std::ostream& os, Expression& data); 
    };//end Expression
  }//namespae GPATH_TOOLS
} // namespace UDIMPL
#endif  // SRC_CUSTOMER_COREIMPL_EXPRESSION_HPP_

