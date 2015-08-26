/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: Generic path query, contextTable module
 *
 *      Author: Renchu
 *      Date: Aug 7th 2015
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_CONTEXT_TABLE_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_CONTEXT_TABLE_HPP_

#include <gutil/gsqlcontainer.hpp>
#include <gpe/serviceimplbase.hpp>
#include "boost/variant.hpp"
#include "gpathTypeDefines.hpp"
#include "boost/functional/hash.hpp"
#include "aggregator.hpp"

namespace UDIMPL {
  namespace GPATH_TOOLS {
    // Generic comparator
    template <typename ContainerType>
    struct container_hash {
      std::size_t operator()(ContainerType const& c) const {
        return boost::hash_range(c.begin(), c.end());
      }
    };
    // Group Map
    typedef GUnorderedMap<VECTOR_ANY, AGGREGATOR_PTR, container_hash<VECTOR_ANY> >::T GROUP_MAP;

    class ContextTable;
    class GroupRANode;

    typedef boost::shared_ptr<ContextTable> CONTEXT_TABLE_PTR;
    static const CONTEXT_TABLE_PTR CONTEXT_TABLE_NULL_PTR = CONTEXT_TABLE_PTR((ContextTable*)NULL);
    
    // Message send between vertices
    class ContextTableMessage {
    public:
      GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t >::T parents_;

      ContextTableMessage();
      ContextTableMessage(GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents);
      ContextTableMessage(const ContextTableMessage& cpy);

      ContextTableMessage& operator+=(const ContextTableMessage& other);
      // construct a one-row message
      explicit ContextTableMessage(CONTEXT_TABLE_PTR ptr);
      
      friend std::ostream& operator<<(std::ostream& os, const ContextTableMessage& data); 
    };

    class Expression; //forward decalration
    class StackNode;

    // ContextTable
    class ContextTable {
    public:
      uint32_t layerIndex_;
      // Attributes added at this vertice / edge
      VECTOR_ANY attr;
      // ContextTable pointer pointing to previous traversal steps
      GVector<CONTEXT_TABLE_PTR>::T parents;
      // Duplication number of paths from each parent vertex
      GVector<uint32_t>::T dupNumFromParents;
      // Number of paths from each parent vertex, accumulated
      GVector<uint32_t>::T numPathsFromParents;
      // Indices of valid paths in accumulated parent paths
      GVector<uint32_t>::T validPaths;
      //Whether this contextTable is going to be skipped or not, used to save contextTable in the reduce but skip in the edge Map
      bool isSkipped;
      // Group Map used for groupby
      GROUP_MAP groupMap;

      ContextTable();

      virtual ~ContextTable() { }

      // Construct a context table for anchor node
      explicit ContextTable(const CONTEXT_TABLE_PTR ptrNull);
      // bulid a contextTable from contextTable pointer, used in edgeMap for create a node of edge from source vertex contextTable
      void addContextTablePointer(const CONTEXT_TABLE_PTR& ptr) {
        if (!ptr->isSkipped) {//if the contextTable is not skipped, push it into the parents 
          this->parents.push_back(ptr);
          this->dupNumFromParents.push_back(1);
          this->numPathsFromParents.push_back(ptr->validPaths.size());
          for (uint32_t i = 0; i < ptr->validPaths.size(); ++i) {
            this->validPaths.push_back(i);
          }
        } else {//if the contextTable is skipped contextTable, then build the current one using ptr->parents;
          this->parents = ptr->parents;
          this->dupNumFromParents = ptr->dupNumFromParents;
          this->numPathsFromParents = ptr->numPathsFromParents;
          this->validPaths = ptr->validPaths;
        }
      }
      // Construct a context table from vertex node without filter
      ContextTable(uint32_t layerIndex, const CONTEXT_TABLE_PTR ptr);
      // Construct a context table from vertex node with filter
      ContextTable(uint32_t layerIndex, const CONTEXT_TABLE_PTR ptr, GVector<Expression>::T& filter, VECTOR_ANY& attr);
      // Construct a context table from another contextTable
      ContextTable(const ContextTable& ct);
      // Construct a context table from aggregated parent pointers
      ContextTable(uint32_t layerIndex, const GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents);
      // Construct a context table from aggregated parent pointers and using filters
      ContextTable(uint32_t layerIndex, const GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents, GVector<Expression>::T& filter, VECTOR_ANY& attr);
      // Whether the vertex / edge is still valid after this iteration (context table not empty)
      bool isValid();
      // Whether the vertex / edge is still valid after this iteration (context table not empty)
      void setSkipped();
      // Add columns
      void addColumns(VECTOR_ANY& cols);
      void reset(uint32_t layerIndex, const GUnorderedMap<CONTEXT_TABLE_PTR, uint32_t>::T& parents);
      // TODO: Remove columns by indices
      // void removeColumns(GVector<uint32_t>::T& indices);
      void applyFilter(GVector<Expression>::T& filter, StackNode& sNode);

      // Combine context table method
      virtual ContextTable& operator+=(const ContextTable& other);
      // Get contexttable
      virtual ContextTable& getContextTable();

      friend std::ostream& operator<<(std::ostream& os, const ContextTable& data);
    };

    class GroupContextTable: public ContextTable {
    public:
      // Construct a group map from a normal context table, and GroupRANode pointer
      GroupContextTable(CONTEXT_TABLE_PTR table, GroupRANode* ptr);
      ContextTable& getContextTable();
      ContextTable& operator+=(const ContextTable& other);
      friend std::ostream& operator<<(std::ostream& os, const GroupContextTable& data);
    };
  }
}

#endif  //SRC_CUSTOMER_COREIMPL_GPEIMPL_GPATH_BASE_CONTEXT_TABLE_HPP_
