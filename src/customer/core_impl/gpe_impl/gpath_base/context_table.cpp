/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: Generic path query, contextTable module
 *
 *      Author: Renchu & DanHu
 *      Date: Aug 7th 2015
 ******************************************************************************/

#include "expression.hpp"
#include "context_table.hpp"
#include "dag_traversal.hpp"
//#include <boost/functional/hash/extensions.hpp>
#include "ragraph.hpp"

namespace UDIMPL {
  namespace GPATH_TOOLS {
    ContextTableMessage::ContextTableMessage() { }

    ContextTableMessage::ContextTableMessage(GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents)
      :parents_(parents) { }

    ContextTableMessage::ContextTableMessage(const ContextTableMessage& cpy):
      parents_(cpy.parents_) { }

    ContextTableMessage::ContextTableMessage(CONTEXT_TABLE_PTR ptr) {
      if (ptr->isSkipped) {
        for (uint32_t i = 0; i < ptr->parents.size(); ++i) {
          parents_[ptr->parents[i]] = ptr->dupNumFromParents[i];
        }
      }
      else {
        parents_[ptr] = 1;
      }
    }

    ContextTableMessage& ContextTableMessage::operator+=(const ContextTableMessage& other) {
      for (GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T::const_iterator it = other.parents_.begin();
           it != other.parents_.end(); ++it) {
        parents_[it->first]  += it->second;//if does not exist, default value for  parents[it->first] = 0
      }
      return *this;
    }
    std::ostream& operator<<(std::ostream& os, const ContextTableMessage& data) {
      for (GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T::const_iterator it = data.parents_.begin();
           it != data.parents_.end(); ++it) {
        os << it->first << ": " << it->first->validPaths.size() << " " << it->second << std::endl;
      }
      return os;
    }
//============================ContextTable====================================================
    ContextTable::ContextTable() { }

    ContextTable::ContextTable(const CONTEXT_TABLE_PTR ptrNull): layerIndex_(0), isSkipped(false) {
      parents.push_back(ptrNull);
      dupNumFromParents.push_back(1);
      numPathsFromParents.push_back(1);
      validPaths.push_back(0);
    }

    ContextTable::ContextTable(uint32_t layerIndex, const CONTEXT_TABLE_PTR ptr): layerIndex_(layerIndex), isSkipped(false) {
      addContextTablePointer(ptr);
    }

    ContextTable::ContextTable(uint32_t layerIndex, const CONTEXT_TABLE_PTR ptr, 
                               GVector<Expression>::T& filter, VECTOR_ANY& attr): layerIndex_(layerIndex), isSkipped(false) {
      ContextTable();
      if (layerIndex_ < filter[0].minLayerIndex_) {//if filter is empty
        return;//return an empty contextTable
      }

      //filter only uses the filter[0] element
      filter[0].update(layerIndex_, attr);//update for current layer

      //if the filter only depends on the current layer
      if (layerIndex_ == filter[0].minLayerIndex_) {
        if (filter[0].pass()) {//pass the filter all paths are valid
          addContextTablePointer(ptr);
          this->attr = attr;//update attr
        } 
        return;
      }
      //if the filter also depends on vertex node, then do the traversal on vertex node and check the filter
      //initialize the validPaths as all possible paths
      for (uint32_t i = 0; i < ptr->validPaths.size(); ++i) {
        validPaths.push_back(i);
      }
      StackNode sNode(ptr, validPaths);
      this->applyFilter(filter, sNode);
      //only use this parent pointer, if it passes the filter
      if (!sNode.traversalPaths.empty()) {//if the sNode.traversalPaths is not empty then it is a valid parent, we need add this into contextTable
        addContextTablePointer(ptr);
        this->validPaths = sNode.traversalPaths;//renew the validPaths with the selected one
        this->attr = attr;
      }
    }
    ContextTable::ContextTable(const ContextTable& ct)
      : layerIndex_(ct.layerIndex_),
        attr(ct.attr),
        parents(ct.parents),
        dupNumFromParents(ct.dupNumFromParents),
        numPathsFromParents(ct.numPathsFromParents),
        validPaths(ct.validPaths),
        isSkipped(false) {}

    ContextTable::ContextTable(uint32_t layerIndex, 
                               const GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents)
      : attr(), isSkipped(false) {
       reset(layerIndex, parents);
    }

    void ContextTable::reset(uint32_t layerIndex, const GUnorderedMap<CONTEXT_TABLE_PTR, 
                             uint32_t>::T& parents) {
      this->layerIndex_ = layerIndex;
      this->parents.clear();
      this->numPathsFromParents.clear();
      this->dupNumFromParents.clear();
      this->attr.clear();
      this->validPaths.clear();

      for (GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T::const_iterator it = parents.begin();
           it != parents.end(); ++it) {
        this->parents.push_back(it->first);
        if (it->first == CONTEXT_TABLE_NULL_PTR) {
          this->numPathsFromParents.push_back(1);
        } else {
          this->numPathsFromParents.push_back(it->first->validPaths.size());
        }
        this->dupNumFromParents.push_back(it->second);
      }
      //change the numPathsParents into the accumulated format
      uint32_t sum = 0;
      for (uint32_t i = 0; i < this->numPathsFromParents.size(); ++i) {
        sum += numPathsFromParents[i];
        numPathsFromParents[i] = sum;
      }
      //set all possible paths as valid here, the applyFilter will get update on the valid path
      for (uint32_t i = 0; i < sum; ++i) {
        this->validPaths.push_back(i);//set all possible paths as valid
      }
    }

    ContextTable::ContextTable(uint32_t layerIndex, 
                               const GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents, 
                               GVector<Expression>::T& filter, VECTOR_ANY& attr)
      : layerIndex_(layerIndex), isSkipped(false) {
      //filter only uses the filter[0] element
      filter[0].update(layerIndex_, attr);//update for current layer

      //if the filter only depends on the current layer
      if (layerIndex_ == filter[0].minLayerIndex_) {
        if (filter[0].pass()) {//pass the filter all paths are valid
          this->reset(layerIndex, parents);//call normal constructor
          this->attr = attr;//update attr
        } 
        return;
      }

      uint32_t offSet = 0;
      //if the filter also depends on its parent, then do the traversal for each parent pointer and check the filter
      for (GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T::const_iterator it = parents.begin();
           it != parents.end(); ++it) {
        //create a traversalPath for each parent pointer, [0,1,2,3]
        GVector<uint32_t>::T traversalPaths(it->first->validPaths.size(), 0);
        for (uint32_t i = 1; i < traversalPaths.size(); ++i) {
          traversalPaths[i] = i;//update the value, such that [0,1,2,3]
        }
        StackNode sNode(it->first, traversalPaths);
        this->applyFilter(filter, sNode);
        //only use this parent pointer, if it passes the filter
        if (!sNode.traversalPaths.empty()) {//if the sNode.traversalPaths is not empty then it is a valid parent, we need add this into contextTable
          this->parents.push_back(it->first);
          this->dupNumFromParents.push_back(it->second);
          uint32_t oldsize = this->validPaths.size();
          this->validPaths.insert(this->validPaths.end(), sNode.traversalPaths.begin(), sNode.traversalPaths.end());
          //Because the sNode.traversalPaths is still local index on parent pointer, we need add the offSet to it;
          for (uint32_t i = oldsize; i < this->validPaths.size(); ++i) {
            this->validPaths[i] += offSet;//add the offSet to the newly added elements
          }
          offSet += it->first->validPaths.size();//set the offSet for the next round;
          this->numPathsFromParents.push_back(offSet);//the offSet is accumulated numPathsFromParent, which already skipped the parent pointer with empty traversalPaths
          this->attr = attr;
        }
      }
    }


    void ContextTable::addColumns(VECTOR_ANY& cols) {
      this->attr = cols;
    }

    void ContextTable::applyFilter(GVector<Expression>::T& filter, StackNode& sNode) {
      //filter only use filter[0] to store the filter
      DAGTraversal traversal;
      traversal.minLayerIndex_ = filter[0].minLayerIndex_;
      //traversal.lastVisitedNode_ = sNode;
      traversal.stack_.push(sNode);

      GVector<uint32_t>::T newValidPaths;
      uint32_t start = 0;
      uint32_t jump = 0;
      while (traversal.next(filter, jump)) {
        if (filter[0].pass()) {//if it passes the filter, then paths between start and start + jump are all valid paths
          for (uint32_t i = start; i < start + jump; ++i) {
            newValidPaths.push_back(sNode.traversalPaths[i]);
          }
        }
        start += jump;
      }
      sNode.traversalPaths = newValidPaths;//update the sNode.traversalPaths with paths passing the filter.
    }

    bool ContextTable::isValid() {
      return !this->validPaths.empty();
    }

    void ContextTable::setSkipped() {
      this->isSkipped = true;
    }
    // Get context table, just for virtual
    ContextTable& ContextTable::getContextTable() {
      return *this;
    }
    // Combine context table
    ContextTable& ContextTable::operator+=(const ContextTable& other) {
      ContextTable* left = this, *right = &(const_cast<ContextTable&>(other));
      /*if (left->validPaths.size() <= right->validPaths.size()) {
        std::swap(left, right);
      }*/
      // Layer Index
      left->layerIndex_ = std::max(left->layerIndex_, right->layerIndex_);
      // Parents
      left->parents.insert(left->parents.end(), right->parents.begin(), right->parents.end());
      // Duplication number from parents
      left->dupNumFromParents.insert(left->dupNumFromParents.end(),
                                     right->dupNumFromParents.begin(),
                                     right->dupNumFromParents.end());
      // Path number from parents
      uint32_t bias = 0;
      if (left->numPathsFromParents.size() > 0) {
        bias = left->numPathsFromParents[left->numPathsFromParents.size() - 1];
      }
      for (uint32_t i = 0; i < right->numPathsFromParents.size(); ++i) {
        right->numPathsFromParents[i] += bias;
      }
      left->numPathsFromParents.insert(left->numPathsFromParents.end(),
                                       right->numPathsFromParents.begin(),
                                       right->numPathsFromParents.end());
      // Valid paths
      for (uint32_t i = 0; i < right->validPaths.size(); ++i) {
        right->validPaths[i] += bias;
      }
      left->validPaths.insert(left->validPaths.end(), right->validPaths.begin(), right->validPaths.end());
      return *left;
    }

    std::ostream& operator<<(std::ostream& os, const ContextTable& data) {
      // Output the context table current layer data
      os << "this: " << &data << std::endl;
      os << "Layer: " << data.layerIndex_ << std::endl;
      os << "Attribute: ";
      for (uint32_t i = 0; i < data.attr.size(); ++i) {
        os << data.attr[i] << " ";
      }
      os << std::endl;
      os << "Parents:" << std::endl;
      for (uint32_t i = 0; i < data.parents.size(); ++i) {
        os << data.parents[i] << ": " 
           << data.numPathsFromParents[i] << ", " 
           << data.dupNumFromParents[i] << std::endl;
      }
      os << "Valid Paths: ";
      for (uint32_t i = 0; i < data.validPaths.size(); ++i) {
        os << data.validPaths[i] << " ";
      }
      os << std::endl;

      // Output the context table content
      return os;
    }

//=====================================GroupContextTable====================================================
    GroupContextTable::GroupContextTable(CONTEXT_TABLE_PTR table, GroupRANode* ptr) {
      GVector<Expression>::T exprs = ptr->projectExpressions_;  // Projection expressions
      exprs.push_back(ptr->aggregateExpression_);               // Aggregation expression
      DAGTraversal traversor(table, exprs);
      uint32_t visitNumber = 0;
      while (traversor.next(exprs, visitNumber)) {
        VECTOR_ANY projExprValue;
        for (uint32_t i = 0; i < exprs.size() - 1; ++i) {
          projExprValue.push_back(exprs[i].evaluate());
        }
        VAL aggValue = exprs[exprs.size() - 1].evaluate();
        if (!groupMap.count(projExprValue)) {
          groupMap[projExprValue] = AggregatorFactory::create(ptr->aggregateFuncName_);
        }
        int dupNumber = traversor.getDuplicatedNum();
        groupMap[projExprValue]->combine(aggValue, dupNumber);
      }
    }

    ContextTable& GroupContextTable::getContextTable() {
      layerIndex_ = 1;
      uint32_t pathNumber = 0;
      for (GROUP_MAP::iterator it = groupMap.begin(); it != groupMap.end(); ++it) {
        VECTOR_ANY attr = it->first;
        attr.push_back(it->second->reduce());
        CONTEXT_TABLE_PTR row(new ContextTable(CONTEXT_TABLE_NULL_PTR)); // Construct one pointer
        row->addColumns(attr);
        parents.push_back(row);
        ++pathNumber;
      }
      dupNumFromParents.resize(pathNumber, 1);
      validPaths.resize(pathNumber);
      numPathsFromParents.resize(pathNumber);
      for (uint32_t i = 0; i < pathNumber; ++i) {
        validPaths[i] = i;
        numPathsFromParents[i] = i + 1;
      }
      return *this;
    }

    ContextTable& GroupContextTable::operator+=(const ContextTable& other) {
      GROUP_MAP& otherGroupMap = const_cast<ContextTable&>(other).groupMap;
      for (GROUP_MAP::const_iterator it = otherGroupMap.begin(); it != otherGroupMap.end(); ++it) {
        if (!this->groupMap.count(it->first)) {
          this->groupMap[it->first] = it->second;
        }
        else {
          this->groupMap[it->first]->combine(it->second);
        }
      }
      return *this;
    }

    std::ostream& operator<<(std::ostream& os, const GroupContextTable& data) {
      for (GROUP_MAP::const_iterator it = data.groupMap.begin(); it != data.groupMap.end(); ++it) {
        VECTOR_ANY key = it->first;
        AGGREGATOR_PTR val = it->second;
        for (uint32_t i = 0; i < key.size(); ++i) {
          os << key[i] << ",";
        }
        os << val->reduce() << std::endl;
      }
      return os;
    }
  }
}
