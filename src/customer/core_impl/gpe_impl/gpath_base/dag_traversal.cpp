#include "context_table.hpp"
#include "dag_traversal.hpp"
#include "column_meta.hpp"
#include "expression.hpp"
namespace UDIMPL {
  namespace GPATH_TOOLS {
    DAGTraversal::DAGTraversal(CONTEXT_TABLE_PTR ptr, GVector<Expression>::T& expressions): minLayerIndex_(std::numeric_limits<uint32_t>::max()) {
      if (ptr->validPaths.empty()) {
        return;
      }
      //create a sNode for the current contextTable
      GVector<uint32_t>::T traversalPaths(ptr->validPaths.size(), 0);//create the validPaths for traversal
      for (uint32_t i = 0; i < ptr->validPaths.size(); ++i) {
         traversalPaths[i] = i;
      }
      StackNode sNode(ptr, traversalPaths);//in the first layer, all the validPaths are needed to be traversed
      for (uint32_t i = 0; i < expressions.size(); ++i) {//update the expression with the current layer of the contextTable
        if (expressions[i].minLayerIndex_ < minLayerIndex_) {
          minLayerIndex_ = expressions[i].minLayerIndex_;
        }
      }
      stack_.push(sNode);//put sNode into stack;
    }

    DAGTraversal::DAGTraversal(CONTEXT_TABLE_PTR ptr): minLayerIndex_(0) {
      //create a sNode for the current contextTable
      GVector<uint32_t>::T traversalPaths(ptr->validPaths.size(), 0);//create the validPaths for traversal
      for (uint32_t i = 0; i < ptr->validPaths.size(); ++i) {
         traversalPaths[i] = i;
      }
      StackNode sNode(ptr, traversalPaths);//in the first layer, all the validPaths are needed to be traversed
      stack_.push(sNode);//put sNode into stack;
    //  lastVisitedNode_ = sNode;//set the last visited stackNode as current contextTable node
    }

    void DAGTraversal::pushValidParentsIntoStack(StackNode& sNode, std::stack<StackNode>& st) {
      if (sNode.traversalPaths.empty()) {
        return;
      }
      CONTEXT_TABLE_PTR ptr = sNode.ptr;//get a copy of the contextTable pointer for the sNode

      //only one possible pointer, all traversalPaths belong to this pointer
      if (ptr->parents.size() == 1) {
        //first traslate the sNode.traversalPaths index into validPaths Index
        GVector<uint32_t>::T traversalPaths(sNode.traversalPaths.size(), 0);
        for (uint32_t i = 0; i < sNode.traversalPaths.size(); ++i) {
          traversalPaths[i] = ptr->validPaths[sNode.traversalPaths[i]];
        }
        st.push(StackNode(ptr->parents[0], traversalPaths, sNode.dupNum * ptr->dupNumFromParents[0]));//sNode.dupNum store the current dupNum obtained so far, ptr->dupNumFromParents[0] stores the dupNum from sNode to StackNode(ptr->parents[0] 
        return;
      }

      //if parents.size() is not 1, search to find it out
      uint32_t parentIndex = 0;
      uint32_t offSet = 0;
      //create a vector for all parents node of stackNode
      GVector<StackNode>::T vst;
      for (uint32_t i = 0; i < ptr->parents.size(); i++) {
        vst.push_back(StackNode(ptr->parents[i]));//create a sNode for each parent, but some of them are not pushed into stack
      }

      //sNode.traversalPaths provide the index of validPaths which are needed to be traversed
      for (uint32_t i = 0; i < sNode.traversalPaths.size(); ++i) {
        uint32_t validPathIndex = sNode.traversalPaths[i];//traversalPaths[i] provide the Index of validPaths for current layer need to be traversed
        while (ptr->validPaths[validPathIndex] >= ptr->numPathsFromParents[parentIndex]) {//find the approiate parentIndex for current validPath index
          offSet = ptr->numPathsFromParents[parentIndex];//update the offset with the current size
          parentIndex++;
        }
        vst[parentIndex].traversalPaths.push_back(ptr->validPaths[validPathIndex] - offSet);//put the local index into
      }//for

      for (int i = (int)vst.size() - 1; i >=0; --i) {
        if (!vst[i].traversalPaths.empty()) {//if this parent node has non-empty valid path, push it into the stack
          vst[i].dupNum = ptr->dupNumFromParents[i] * sNode.dupNum;//multiply by the duplicated number traversed so far, as a result, the stackNode has the total duplicated number from its node to the last node;
          st.push(vst[i]);
        }
      }//for
    }//pushValidParentsIntoStack

    bool DAGTraversal::next(GVector<Expression>::T& expressions, uint32_t& visitedNum) {
      if (stack_.empty()) {
        return false;
      }
      
      while (stack_.top().ptr->layerIndex_ > minLayerIndex_) {
        StackNode curNode = stack_.top();
        stack_.pop();
        //update expressions
        for (uint32_t i = 0; i < expressions.size(); ++i) {//update the expression with the current layer of the contextTable
          expressions[i].update(curNode.ptr->layerIndex_, curNode.ptr->attr);//update expression using the current layer attr
        }
        pushValidParentsIntoStack(curNode, stack_);//push all its valid parent nodes into the stack
      }
      lastVisitedNode_ = stack_.top();
      stack_.pop();//pop out from stack_, so that next will work on the next top node in the stack_
      //update expressions for the last traversal layer
      for (uint32_t i = 0; i < expressions.size(); ++i) {//update the expression with the current layer of the contextTable
        expressions[i].update(lastVisitedNode_.ptr->layerIndex_, lastVisitedNode_.ptr->attr);//update expression using the current layer attr
      }
      visitedNum = lastVisitedNode_.traversalPaths.size();//traversalPaths.size() is the number of valid paths traversed in this next() run.
      return true;
    }//next

    bool DAGTraversal::next(ColumnMeta& cMeta, VECTOR_ANY& attr) {
      //ATTENTION: attr must have the size of cMeta.column
      if (stack_.empty()) {
        return false;
      }
      
      while (stack_.top().ptr != CONTEXT_TABLE_NULL_PTR) {
        StackNode curNode = stack_.top();
        stack_.pop();
        //update expressions
        uint32_t idx = cMeta.getLayerBias(curNode.ptr->layerIndex_);//obtain the insertion start index for ptr->attr in attr
        uint32_t endx = idx + curNode.ptr->attr.size();//the end index of ptr->attr in attr vector
        for (uint32_t i = idx; i < endx; ++i) {
          attr[i] = curNode.ptr->attr[i - idx];//update the attr
        }
        pushValidParentsIntoStack(curNode, stack_);//push all its valid parent nodes into the stack
      }
      lastVisitedNode_ = stack_.top();//store the sNode with NULL pointer as last visited node, its dupNum is total duplicate number along this path
      stack_.pop();//pop out from stack_, so that next will work on the next top node in the stack_
      return true;
    }//next

    uint32_t DAGTraversal::getDuplicatedNum() {//get duplicated number for the current next run
      if (lastVisitedNode_.ptr == CONTEXT_TABLE_NULL_PTR) {//if the lastVisitedNode is the NULL pointer layer, the dupNum is the total duplicate number
        return lastVisitedNode_.dupNum;
      }
      //if the lastVisitedNode is not the NULL pointer layer, build a new stack, repeat traversal until the entire subgraph is visited
      std::stack<StackNode> st;
      st.push(lastVisitedNode_);
      uint32_t totalDupNum = 0;//used to store all duplicated number in the subgraph
      while (!st.empty()) {
        StackNode curNode = st.top();
        st.pop();
        if (curNode.ptr != CONTEXT_TABLE_NULL_PTR) {
          pushValidParentsIntoStack(curNode, st);//push all its valid parent nodes into the new stack, so that the entire subgraph is visited
        } else {
          totalDupNum += curNode.dupNum;
        }
      }
      return totalDupNum;
    }//getDuplicatedNum

  }//GPATH_TOOLS
}//UDIMPL
