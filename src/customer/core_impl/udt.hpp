/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_UDT_HPP_
#define SRC_CUSTOMER_COREIMPL_UDT_HPP_
#include <stdint.h>
#include <iostream>
#include <vector>

namespace UDIMPL {

/// Define your UDT here and then include this header by UDF and GSE loader/POST

//SearchPath express different search paths, which will be used in user_search UDF
enum SearchPath { 
    PathOntoUser      = 0,  //ontology -> user 
    PathItemUser      = 1,  //item -> behavior -> user
    PathOntoItemUser  = 2   //ontology -> item -> behavior -> user
};

//operator used in ontology and behavior conditions
enum Operator {
   GT       = 0,
   LT       = 1,
   GE       = 2,
   LE       = 3,
   NE       = 4,
   EQ       = 5,
   LIKE     = 6, //not support yet
   DIS_LIKE = 7  //not support yet
};

//search condition: ontology -> user 
struct OntologyCond {
  std::vector<uint64_t> startId;
  Operator op; 
  double weight;
  friend std::ostream& operator<<(std::ostream& os, const OntologyCond& obj);
};

//search condition: (ontology ->) item -> behavior -> user 
struct BehaviorCond {
  SearchPath sp;
  std::vector<uint64_t> startId;
  Operator op;
  uint32_t itemT;
  uint32_t ontoT;
  uint32_t times;
  uint64_t timeStart;
  uint64_t timeEnd;
  friend std::ostream& operator<<(std::ostream& os, const BehaviorCond& obj);
};

struct ItemBehaviorCond: public BehaviorCond {
  friend std::ostream& operator<<(std::ostream& os, const ItemBehaviorCond& obj);
};

struct OntoBehaviorCond: public BehaviorCond {
  friend std::ostream& operator<<(std::ostream& os, const OntoBehaviorCond& obj);
};

}// namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_UDT_HPP_
