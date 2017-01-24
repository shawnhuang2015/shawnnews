/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: ntietz
 ******************************************************************************/
#include "udt.hpp"

namespace UDIMPL {
  /// Implement your UDT here.
std::ostream& operator<<(std::ostream& os, const OntologyCond& obj){
  os << "OntologyCond startId Size = " << obj.startId.size()  << ", "
     << "op = " << obj.op << ", "
     << "weight = " << obj.weight << "\n"; 
  return os;
}

std::ostream& operator<<(std::ostream& os, const BehrUserCond& obj) {
  os << "sp = " << obj.sp << ", "
     << "op = " << obj.op << ", "
     << "behrT = " << obj.behrT << ", "
     << "times = " << obj.times << ", "
     << "timeStart = " << obj.timeStart << ", "
     << "timeEnd = " << obj.timeEnd << "\n";
}

std::ostream& operator<<(std::ostream& os, const BehaviorCond& obj) {
  os << "sp = " << obj.sp << ", "
     << "startId Size = " << obj.startId.size() << ", "
     << "op = " << obj.op << ", "
     << "itemT = " << obj.itemT << ", "
     << "ontoT = " << obj.ontoT << ", "
     << "times = " << obj.times << ", "
     << "timeStart = " << obj.timeStart << ", "
     << "timeEnd = " << obj.timeEnd << "\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const ItemBehaviorCond& obj) {
  os << "ItemBehaviorCond " << (BehaviorCond)obj;
  return os;
}

std::ostream& operator<<(std::ostream& os, const OntoBehaviorCond& obj) {
  os << "OntoBehaviorCond " << (BehaviorCond)obj;
  return os;
}

} // namespace UDIMPL

