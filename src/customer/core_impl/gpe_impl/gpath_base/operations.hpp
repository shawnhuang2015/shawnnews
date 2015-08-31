/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: Generic path query, operator base class, used for unary, binary Operations on the contextTable
 *
 *      Author: Dan Hu
 *      Date: Jul 31th 2015
 ******************************************************************************/
#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>
#include <iostream>
#include <utility>
#include <string>
#include "boost/variant.hpp"
#include "aggregator.hpp"
#include "expression.hpp"
#include "gpathTypeDefines.hpp"
#include "boost/functional/hash.hpp"
#include "context_table.hpp"
#include "ragraph.hpp"
#include "dag_traversal.hpp"

#ifndef MODULE_OPERATIONS_HPP_
#define MODULE_OPERATIONS_HPP_


namespace UDIMPL {
  namespace GPATH_TOOLS {
    class Operations;

    typedef boost::shared_ptr<Operations> OPERATIONS_POINTER;
    
    class Operations {
    public:
      RANode* raNode;
      GVector<CONTEXT_TABLE_PTR>::T data; //used to store input contextTables
      Operations(): data() {}
      Operations(RANode* ra): raNode(ra) {}
      virtual ~Operations() {}
      void push(CONTEXT_TABLE_PTR ct) {
        data.push_back(ct);
      }
      static OPERATIONS_POINTER make_operations(RANode*);
 
      virtual CONTEXT_TABLE_PTR execReduce() = 0;
      virtual CONTEXT_TABLE_PTR execEndRun() = 0;
      // Add dummy layer if necessary before inserting into global map
      virtual CONTEXT_TABLE_PTR prepareForGlobalMap(CONTEXT_TABLE_PTR ptr) = 0;
    };

    class DoSelect: public Operations {
    public:
      DoSelect(RANode* ra): Operations(ra){}
      CONTEXT_TABLE_PTR execReduce() {
        GVector<Expression>::T expr;
        expr.push_back(reinterpret_cast<SelectRANode*>(raNode)->filter_);

        CONTEXT_TABLE_PTR table(new ContextTable(*data[0])); //get the contextTable

        GVector<uint32_t>::T validPaths;  // temporary valid path container
        uint32_t pos = 0;                 // index within valid path
        uint32_t visitNumber;             // number of paths
        DAGTraversal traversor(table, expr);
        while (traversor.next(expr, visitNumber)) {
          if (expr[0].pass()) {
            validPaths.insert(validPaths.end(), 
                              table->validPaths.begin() + pos, 
                              table->validPaths.begin() + pos + visitNumber);
          }
          pos += visitNumber;
        }
        table->validPaths = validPaths;
        return table;
      }
      
      CONTEXT_TABLE_PTR execEndRun() {
        return execReduce();
      }

      // For selection, add a dummy layer
      CONTEXT_TABLE_PTR prepareForGlobalMap(CONTEXT_TABLE_PTR ptr) {
        return CONTEXT_TABLE_PTR(new ContextTable(ptr->layerIndex_ + 1, ptr));
      }
    }; //end DoSelect

    class DoProject: public Operations {
    public:
      DoProject(RANode* ra): Operations(ra){}
      CONTEXT_TABLE_PTR execReduce() {
        ProjectRANode* ptr = reinterpret_cast<ProjectRANode*>(raNode);
        GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T rows;
        
        GVector<Expression>::T exprs = ptr->projectExpressions_;  // Expressions
        // TODO: Remove
        //std::cout << "Project Expressions" << std::endl;
        for (uint32_t i = 0; i < exprs.size(); ++i) {
          //std::cout << exprs[i] << std::endl;
        }
        DAGTraversal traversor(data[0], exprs);                   // Traversor
        uint32_t visitNumber = 0;
        while (traversor.next(exprs, visitNumber)) {              // Traverse a new row
          VECTOR_ANY attr;
          for (uint32_t i = 0; i < exprs.size(); ++i) {           // Construct a row vector
            attr.push_back(exprs[i].evaluate());
          }
          CONTEXT_TABLE_PTR row(new ContextTable(CONTEXT_TABLE_NULL_PTR)); // Construct one pointer
          row->addColumns(attr);                                  // Add columns
          rows[row] = traversor.getDuplicatedNum();               // Set path number and dup number
        }

        CONTEXT_TABLE_PTR table(new ContextTable(1, rows));       // Construct the context table
        return table;
      }

      CONTEXT_TABLE_PTR execEndRun() {
        return execReduce();
      }

      // For projection, don't need a new layer
      CONTEXT_TABLE_PTR prepareForGlobalMap(CONTEXT_TABLE_PTR ptr) {
        return CONTEXT_TABLE_PTR(new ContextTable(ptr->layerIndex_ + 1, ptr));
      }
    }; //end DoProject

    class DoGroup: public Operations {
    public:
      DoGroup(RANode* ra): Operations(ra){}
      CONTEXT_TABLE_PTR execReduce() {
        GroupRANode* ptr = reinterpret_cast<GroupRANode*>(raNode);
        CONTEXT_TABLE_PTR groupResult(new GroupContextTable(data[0], ptr)); 
        return groupResult;
      }
      
      CONTEXT_TABLE_PTR execEndRun() {
        return execReduce();
      }

      // For group, don't need a new layer
      CONTEXT_TABLE_PTR prepareForGlobalMap(CONTEXT_TABLE_PTR ptr) {
        return ptr;
      }
    }; //end Do Group

    class DoUnion: public Operations {
    public:
      DoUnion(RANode* ra): Operations(ra){}
      CONTEXT_TABLE_PTR execReduce() {
        UnionRANode* ptr = reinterpret_cast<UnionRANode*>(raNode);
        GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T rows;
        rows[data[0]] = 1;
        rows[data[1]] = 1;
        
        CONTEXT_TABLE_PTR table(
          new ContextTable(std::max(data[0]->layerIndex_, data[1]->layerIndex_) + 1, rows)
        );

        return table;
      }

      CONTEXT_TABLE_PTR execEndRun() {
        return execReduce();
      }

      // For union, don't need a new layer
      CONTEXT_TABLE_PTR prepareForGlobalMap(CONTEXT_TABLE_PTR ptr) {
        return CONTEXT_TABLE_PTR(new ContextTable(ptr->layerIndex_ + 1, ptr));
      }
    };
    
    class DoNothing: public Operations {
    public:
      DoNothing(RANode* ra): Operations(ra){}
      CONTEXT_TABLE_PTR execReduce() {
        return data[0];
      }

      CONTEXT_TABLE_PTR execEndRun() {
        return execReduce();
      }

      // For doing nothing, don't need a new layer
      CONTEXT_TABLE_PTR prepareForGlobalMap(CONTEXT_TABLE_PTR ptr) {
        return CONTEXT_TABLE_PTR(new ContextTable(ptr->layerIndex_ + 1, ptr));
      }
    };

    OPERATIONS_POINTER Operations::make_operations(RANode* ra) {
      if (ra->raType_ == NOTHING) {
        return OPERATIONS_POINTER(new DoNothing(ra));
      } else if (ra->raType_ == SELECT) {
        return OPERATIONS_POINTER(new DoSelect(ra));
      } else if (ra->raType_ == PROJECT) {
        return OPERATIONS_POINTER(new DoProject(ra));
      } else if (ra->raType_ == GROUP) {
        return OPERATIONS_POINTER(new DoGroup(ra));
      } else if (ra->raType_ == UNION) {
        return OPERATIONS_POINTER(new DoUnion(ra));
      }
      return OPERATIONS_POINTER();
    }
  }//GPATH_TOOLS
}//UDIMPL
#endif
