
/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: contextTable traversal module
 *
 *      Author: Dan Hu
 *      Date: Aug 18th 2015
 ******************************************************************************/

#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>
#include "boost/variant.hpp"
#include "gpathTypeDefines.hpp"
#include <limits>

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_DAG_TRAVERSAL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_DAG_TRAVERSAL_HPP_
namespace UDIMPL {
  namespace GPATH_TOOLS {

  class ContextTable;
  class Expression;
  class ColumnMeta;

  struct StackNode {
    CONTEXT_TABLE_PTR ptr;
    GVector<uint32_t>::T traversalPaths;
    uint32_t dupNum;//duplicate number of pathes traversed so far, it is accumulated dupNum of all previous traversed node;

    StackNode(): ptr(CONTEXT_TABLE_NULL_PTR), traversalPaths(), dupNum(1) {}
    StackNode(CONTEXT_TABLE_PTR p)
      : ptr(p), traversalPaths(), dupNum(1) {}
    StackNode(CONTEXT_TABLE_PTR p, GVector<uint32_t>::T& vi)
      : ptr(p), traversalPaths(vi), dupNum(1) {}
    StackNode(CONTEXT_TABLE_PTR p, GVector<uint32_t>::T& vi, uint32_t num)
      : ptr(p), traversalPaths(vi), dupNum(num) {}
  };

  class DAGTraversal {
  public:
    DAGTraversal(): stack_(), minLayerIndex_(std::numeric_limits<uint32_t>::max()) {}
    //build the DAGTraversal from the ContextTable pointer, using validPaths
    DAGTraversal(CONTEXT_TABLE_PTR ct, GVector<Expression>::T& expressions);
    //build the DAGTraversal from the ContextTable pointer, this is used for traversaling the entire graph
    DAGTraversal(CONTEXT_TABLE_PTR ct);
    //provide the traversal for one row each call, it will update the expressions through the traversal but it won't recalcuate the expressions which already calculated in the previous call.
    bool next(GVector<Expression>::T& expressions, uint32_t& visitedNum);
    //provide the traversal for one row each call, it will update the attributes into a vector through the traversal but it won't recalcuate the expressions which already calculated in the previous call.
    bool next(ColumnMeta& cMeta, VECTOR_ANY& attr);
    //This method provide the duplicate num for the previous traversal
    uint32_t getDuplicatedNum();
  //private:
    static void pushValidParentsIntoStack(StackNode& sNode, std::stack<StackNode>& st);
    void pushValidParentsIntoStack(StackNode& sNode) {
      pushValidParentsIntoStack(sNode, this->stack_);//if no stack is provided, use its own stack
    }
    std::stack<StackNode> stack_;
    uint32_t minLayerIndex_;
    StackNode lastVisitedNode_; // record the dumplicated Num obtained from the traversal
  };//DAGTraversal

  }//GPATH_TOOLS
}//UDIMPL
#endif
