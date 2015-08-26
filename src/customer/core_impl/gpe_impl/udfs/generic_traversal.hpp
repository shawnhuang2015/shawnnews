/***************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPath
 *
 *  Created on: 7/2/2015
 *  Author: Renchu, Dan Hu
 *  Description: Generic Traversal
****************************************************************/

#ifndef UDFS_GENERIC_TRAVERSAL_HPP_
#define UDFS_GENERIC_TRAVERSAL_HPP_

#include <gpelib4/udfs/singleactiveudfbase.hpp>
#include <gutil/gjsonhelper.hpp>
#include <gutil/gsqlcontainer.hpp>
#include "../base/global_map.hpp"
#include "../base/global_vector.hpp"
#include "../gpath_base/context_table.hpp"
#include "../gpath_base/expression.hpp"
#include "../gpath_base/graphtypeinfo.hpp"
#include "../gpath_base/aggregator.hpp"
#include "../gpath_base/gpathTypeDefines.hpp"
#include "../gpath_base/ragraph.hpp"
#include "../gpath_base/dag_traversal.hpp"
#include "../gpath_base/operations.hpp"

namespace UDIMPL{ 

using namespace GPATH_TOOLS;

class GenericTraversal: public gpelib4::BaseUDF {
public:
  typedef GUnorderedMap<uint32_t, CONTEXT_TABLE_PTR >::T LocalMap;
  //typedef boost::shared_ptr<Operations> OPERATIONS_POINTER;

  //global variables
  enum { GV_TRAVERSAL_META, GV_GRAPH_META_DATA, GV_CONTEXT_TABLE};
  
  static const gpelib4::EngineProcessingMode EngineMode = gpelib4::EngineProcessMode_ActiveVertices;
  static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
  static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined;
  static const int PrintMode_ = gpelib4::Mode_Print_NotDefined;

  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute E_ATTR;
  typedef CONTEXT_TABLE_PTR V_VALUE;
  typedef ContextTableMessage MESSAGE;

  GenericTraversal(GraphMetaData& graphMetaData, Json::Value traverseMeta, 
                   gutil::JSONWriter* jsonwriter): gpelib4::BaseUDF(EngineMode, traverseMeta[ STEPS ].size() + 1), 
                   graphMetaData_(graphMetaData), traverseMeta_(traverseMeta), aggJobs_(), writer_(jsonwriter) { }

  ~GenericTraversal () {
    //Nothing to do here.
  }
  
  void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
    globalvariables->Register(GV_TRAVERSAL_META,
      new gpelib4::BroadcastVariable<Json::Value>(traverseMeta_));
    globalvariables->Register(GV_GRAPH_META_DATA, 
      new gpelib4::BroadcastVariable<GraphMetaData>(graphMetaData_));
    globalvariables->Register(GV_CONTEXT_TABLE,
      new MapVariable<std::string, CONTEXT_TABLE_PTR>());
  }

  ALWAYS_INLINE void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE>* context) {
    traverseMeta_ = context->GlobalVariable_GetValue<Json::Value>(GV_TRAVERSAL_META);
    graphMetaData_ = context->GlobalVariable_GetValue<GraphMetaData>(GV_GRAPH_META_DATA);
    
    //std::cout << "Traverse Meta before ragraph" << std::endl;
    //std::cout << traverseMeta_ << std::endl;
    aggJobs_ = boost::shared_ptr<RAGraph>(new RAGraph(traverseMeta_));
    //std::cout << "Traverse Meta after ragraph" << std::endl;
    //std::cout << traverseMeta_ << std::endl;
    // Step number
    stepNumber_ = traverseMeta_[ STEPS ].size() + 1;
    // Set anchor node active
    Json::Value& anchorNode = traverseMeta_[ ANCHOR_NODE ];
    std::string vtypeName = anchorNode[ V_TYPE_CHECK ].asString();
    Json::Value& memberList = anchorNode[ MEMBERSHIP_LIST ];
    uint32_t vtype;
    // Get vertex type
    graphMetaData_.getVertexTypeIndex(vtypeName, vtype);
    if (memberList.size() > 0) {
      // Only activate vertices within membership list
      for (uint32_t k = 0; k < memberList.size(); ++k) {
        context->SetActiveFlag(memberList[k].asInt());
      }
    }
    else {
      // Activate all vertices of this type
      context->SetActiveFlagByType(vtype, true);
      //std::cout << "The type = " << vtype << " is active." << std::endl;
    }
  }

  void StartRun(gpelib4::MasterContext* context) {
    // Turn off source vertex attribute, target vertex attribute and target vertex value.
    context->set_udfedgemapsetting(0);
  }

  void BeforeIteration(gpelib4::MasterContext* context) {
    // Init containers
    memberList_.clear();
    vAddList_.clear();
    vAddListIndex_.clear();
    vAddListType_.clear();
    eAddList_.clear();
    eAddListIndex_.clear();
    eAddListType_.clear();
    vDropList_.clear();
    eDropList_.clear();
    
    Json::Value step;
    
    if (context->Iteration() == 1) {
      // The first iteration, use vertex map to determine which vertices are activated.
      context->set_UDFMapRun(gpelib4::UDFMapRun_VertexMap);
      // Retrieve anchor node information
      step = traverseMeta_[ ANCHOR_NODE ];
    } 
    else {
      // From second iteration, use edge map and filter wihch type edge is activated.
      context->set_UDFMapRun(gpelib4::UDFMapRun_EdgeMap);
      // Retrieve step information
      step = traverseMeta_[ STEPS ][(uint32_t)(context->Iteration() - 2)];
      // etype
      std::string etypeStr = step[ E_TYPE_CHECK ].asString();
      if (etypeStr == _ANY) {
        etype_ = TYPE_ANY;
        context->GetTypeFilterController()->EnableAllEdgeTypes();
      }
      else {
        graphMetaData_.getEdgeTypeIndex(etypeStr, etype_);
        context->GetTypeFilterController()->DisableAllEdgeTypes(); //Disable all edge type
        context->GetTypeFilterController()->EnableEdgeType(etype_); // Enable specific type
      }
      // efilter
      std::string eFilterStr = step[ E_FILTER ].asString();
      eFilter_ = Expression(eFilterStr, aggJobs_->raNodes_[0]->newColumnMeta_);
      //std::cout << "eFilter of iteration " << context->Iteration() << std::endl;
      //std::cout << eFilter_ << std::endl;
      // ename_internal
      enameInternal_ = step[ E_NAME_INTERNAL ].asString();
      // e_add_list
      Json::Value& eAddList = step[ E_ADD_LIST ];
      //std::cout << "EAddList Json" << std::endl;
      //std::cout << step << std::endl;
      //std::cout << eAddList << std::endl;
      for (uint32_t k = 0; k < eAddList.size(); ++k) {
        std::string attr = eAddList[k].asString();
        if (attr == _TYPE) {
          // type
          eAddList_.push_back("type");
          eAddListIndex_.push_back(0);
          eAddListType_.push_back(topology4::Attribute::STRING_TYPE);
        }
        else if (attr == "*") {
          // Add all attributes TODO: _ANY can only be supported at runtime.
          GVector<std::string>::T allAttr;
          graphMetaData_.getAllEdgeAttributes(etypeStr, allAttr);
          for (uint32_t k = 0; k < allAttr.size(); ++k) {
            attr = allAttr[k];
            uint32_t eaIndex, eaType;
            graphMetaData_.getEdgeAttributeIndex(etypeStr, attr, eaIndex);
            graphMetaData_.getEdgeAttributeType(etypeStr, attr, eaType); 
            eAddList_.push_back(attr);
            eAddListIndex_.push_back(eaIndex);
            eAddListType_.push_back(eaType);
          }
        }
        else {
          // Normally add the attribute
          eAddList_.push_back(attr);
          uint32_t eaIndex, eaType;
          graphMetaData_.getEdgeAttributeIndex(etypeStr, attr, eaIndex);
          graphMetaData_.getEdgeAttributeType(etypeStr, attr, eaType); 
          eAddListIndex_.push_back(eaIndex);
          eAddListType_.push_back(eaType);
        }
      }
    
      //std::cout << "Temp eAddList" << "\n";
      for (uint32_t i = 0; i < eAddList_.size(); ++i) {
        //std::cout << eAddList_[i] << " " << eAddListIndex_[i] << " " << eAddListType_[i] << "\n";
      }

      // e_drop_list
      Json::Value& eDropList = step[ E_DROP_LIST ];
      for (uint32_t k = 0; k < eDropList.size(); ++k) {
        eDropList_.push_back(eDropList[k].asString());
      }
    }
    // vtype
    std::string vtypeStr = step[ V_TYPE_CHECK ].asString();
    if (vtypeStr == _ANY) {
      vtype_ = TYPE_ANY;
    } else {
      graphMetaData_.getVertexTypeIndex(vtypeStr, vtype_);
    }
    
    // vname_internal
    vnameInternal_ = step[ V_NAME_INTERNAL ].asString();
    // vfilter
    std::string vFilterStr = step[ V_FILTER ].asString();
    Expression vFilterExpr(vFilterStr, aggJobs_->raNodes_[0]->newColumnMeta_);
    vFilter_ = vFilterExpr;
    //std::cout << "vFilter of iteration " << context->Iteration() << std::endl;
    //std::cout << vFilter_ << std::endl;
    // v_add_list
    Json::Value& vAddList = step[ V_ADD_LIST ];
    for (uint32_t k = 0; k < vAddList.size(); ++k) {
      std::string attr = vAddList[k].asString();
      if (attr == _INTERNAL_ID) {
        // Internal id
        vAddList_.push_back("_internal_id");
        vAddListIndex_.push_back(0);
        vAddListType_.push_back(topology4::Attribute::UINT_TYPE);
      }
      else if (attr == _EXTERNAL_ID) {
        // External id. TODO: need another approach
        vAddList_.push_back("id");
        uint32_t vaIndex, vaType;
        graphMetaData_.getVertexAttributeIndex(vtypeStr, "_external_id", vaIndex);
        graphMetaData_.getVertexAttributeType(vtypeStr, "_external_id", vaType); 
        vAddListIndex_.push_back(vaIndex);
        vAddListType_.push_back(vaType);
      }
      else if (attr == _TYPE) {
        // type
        vAddList_.push_back("type");
        vAddListIndex_.push_back(0);
        vAddListType_.push_back(topology4::Attribute::STRING_TYPE);
      }
      else if (attr == "*") {
        // Add all attributes TODO: _ANY can only be supported at runtime.
        GVector<std::string>::T allAttr;
        graphMetaData_.getAllVertexAttributes(vtypeStr, allAttr);
        for (uint32_t k = 0; k < allAttr.size(); ++k) {
          attr = allAttr[k];
          if (attr == "_external_id") continue; // TODO: Remove this after get rid of _external_id,
          uint32_t vaIndex, vaType;
          graphMetaData_.getVertexAttributeIndex(vtypeStr, attr, vaIndex);
          graphMetaData_.getVertexAttributeType(vtypeStr, attr, vaType); 
          vAddList_.push_back(attr);
          vAddListIndex_.push_back(vaIndex);
          vAddListType_.push_back(vaType);
        }
      }
      else {
        // Normally add the attribute
        vAddList_.push_back(attr);
        uint32_t vaIndex, vaType;
        graphMetaData_.getVertexAttributeIndex(vtypeStr, attr, vaIndex);
        graphMetaData_.getVertexAttributeType(vtypeStr, attr, vaType); 
        vAddListIndex_.push_back(vaIndex);
        vAddListType_.push_back(vaType);
      }
    }
    // v_drop_list
    Json::Value& vDropList = step[ V_DROP_LIST ];
    for (uint32_t k = 0; k < vDropList.size(); ++k) {
      vDropList_.push_back(vDropList[k].asString());
    }
    // membership_list
    Json::Value& memberList = step[ MEMBERSHIP_LIST ];
    for (uint32_t k = 0; k < memberList.size(); ++k) {
      memberList_.insert(memberList[k].asInt());
    }

    //======TODO: Debug===========
    //std::cout << "Iteration Info: " << context->Iteration() << "\n";
    //std::cout << "etype " << etype_ << "\n";
    //std::cout << "efilter " << eFilter_ << "\n";
    //std::cout << "enameInternal " << enameInternal_ << "\n";
    //std::cout << "eDropList " << "\n";
    for (uint32_t i = 0; i < eDropList_.size(); ++i) {
      //std::cout << eDropList_[i] << "\n";
    }
    //std::cout << "eAddList" << "\n";
    for (uint32_t i = 0; i < eAddList_.size(); ++i) {
      //std::cout << eAddList_[i] << " " << eAddListIndex_[i] << " " << eAddListType_[i] << "\n";
    }
    //std::cout << "vtype " << vtype_ << "\n";
    //std::cout << "vfilter " << vFilter_ << "\n";
    //std::cout << "vnameInternal " << vnameInternal_ << "\n";
    //std::cout << "vDropList " << "\n";
    for (uint32_t i = 0; i < vDropList_.size(); ++i) {
      //std::cout << vDropList_[i] << "\n";
    }
    //std::cout << "vAddList" << "\n";
    for (uint32_t i = 0; i < vAddList_.size(); ++i) {
      //std::cout << vAddList_[i] << " " << vAddListIndex_[i] << " " << vAddListType_[i] << "\n";
    }
    //std::cout << "Membership List" << "\n";
    for (GUnorderedSet<uint32_t>::T::iterator it = memberList_.begin(); it != memberList_.end(); ++it) {
      //std::cout << *it << " ";
    }
    //std::cout << endl;
  }

  ALWAYS_INLINE void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
                             const V_VALUE& srcvertexvalue,
                             const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
                             const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
                             gpelib4::SingleValueMapContext<MESSAGE>* context) {
    
    //std::cout << "Edge Map Interation " << context->Iteration() << "\n";
    
    // Collect information according to v_add_list
    VECTOR_ANY attrToAdd;
    addEdgeAttrList(edgeattr, attrToAdd);

    // The context table of this edge
    V_VALUE edgeTable;

    // Empty eFilter, just add columns
    if (eFilter_.emptyExpr_) {
      edgeTable = V_VALUE(new ContextTable(context->Iteration() * 2 - 3, srcvertexvalue));
      edgeTable->addColumns(attrToAdd);
    }
    // Non-empty eFilter, construct by using filter as parameter
    else {
      // Copy eFilter to vector to fit in ContextTable constructor
      GVector<Expression>::T expEFilter;
      expEFilter.push_back(eFilter_);
      edgeTable = V_VALUE(new ContextTable(context->Iteration() * 2 - 3, srcvertexvalue, expEFilter, attrToAdd));
    }

    // Only continue if context table contains at least one row remaining
    if (edgeTable->isValid()) {
      // If context table doesn't contain add list or filter, skip this layer
      if (attrToAdd.size() == 0 && eFilter_.emptyExpr_) {
        edgeTable->setSkipped();
      }
      MESSAGE message(edgeTable);
      context->Write(targetvid, message);
      if (context->Iteration() == 4) {
        //std::cout << "Edge Map Message" << std::endl;
        //std::cout << "target " << targetvid << std::endl;
        //std::cout << "message " << std::endl << message << std::endl;
      }
    }

    // TODO: Debug
    //std::cout << "Edge Map Iteration " << context->Iteration() << std::endl;
    //std::cout << *edgeTable << std::endl;
  } //EdgeMap ends

  ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                               gpelib4::SingleValueMapContext<MESSAGE> * context) {
    //std::cout << "ID = " << vid << " has been set as active" << std::endl;
    context->SetActiveFlag(vid);
  }

  void BetweenMapAndReduce(gpelib4::MasterContext* context) {
    //Nothing to do here.
  }

  ALWAYS_INLINE void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                            const V_VALUE& vertexvalue, const MESSAGE& accumulator,
                            gpelib4::SingleValueContext<V_VALUE>* context) {

    //std::cout << "In reduce with iteration = " << context->Iteration() << std::endl;
    if (context->Iteration() == 4) {
      //std::cout << "Reduce Message" << std::endl;
      //std::cout << "vid " << vid << std::endl;
      //std::cout << "message " << std::endl << accumulator << std::endl;
    }
    // Filter vertices not same type as vtype
    if (vtype_ != TYPE_ANY && vtype_ != vertexattr->type()) {
      //std::cout << vnameInternal_ << " filtered" << std::endl;
      return;
    }
    // Filter vertices not within membership list
    if (memberList_.size() > 0 && memberList_.find(vid) == memberList_.end()) {
      //std::cout << "vid = " << vid <<", returned by no memberList" << std::endl;
      return;
    }
    // Whether this is the last iteration
    bool lastIteration = (context->Iteration() == stepNumber_);
    // Collect information according to v_add_list
    VECTOR_ANY attrToAdd;
    addVertexAttrList(vertexattr, attrToAdd);

    // The context table of this vertex
    V_VALUE vertexTable;
    // For anchor node, construct context table from null pointer as parent
    if (context->Iteration() == 1) {
      vFilter_.update(0, attrToAdd);
      if (vFilter_.pass()) {
        vertexTable = V_VALUE(new ContextTable(CONTEXT_TABLE_NULL_PTR));
        vertexTable->addColumns(attrToAdd);
      }
      else {
        return;
      }
    }
    // For other layer nodes, construct context table from 
    else {
      // Empty vFilter, just add columns
      if (vFilter_.emptyExpr_) {
        vertexTable = V_VALUE(new ContextTable(context->Iteration() * 2 - 2, accumulator.parents_));
        vertexTable->addColumns(attrToAdd);
      }
      // Non-empty vFilter, construct by using filter as parameter
      else {
        // Copy vFilter to vector to fit in ContextTable constructor
        GVector<Expression>::T expVFilter;
        expVFilter.push_back(vFilter_);
        vertexTable = V_VALUE(new ContextTable(context->Iteration() * 2 - 2, accumulator.parents_, expVFilter, attrToAdd));
      }
    }
    
    // TODO: Debug
    //std::cout << "Reduce Iteration " << context->Iteration() << std::endl;
    //std::cout << *vertexTable << std::endl;

    // Only continue if context table contains at least one row remaining
    if (vertexTable->isValid()) {
      // If context table doesn't contain add list or filter, skip this layer
      if (attrToAdd.size() == 0 && vFilter_.emptyExpr_) {
        vertexTable->setSkipped();
      }
      // Write to vertex
      context->Write(vid, vertexTable, !lastIteration);
      // If is last iteration, do aggregation
      if (lastIteration) {
        // TODO: Aggregation
        LocalMap localMap;
        localMap[0] = vertexTable;
        GVector<RANode*>::T& reduceJobs = aggJobs_->getReduceNodes();
        for (uint32_t i = 0; i < reduceJobs.size(); ++i) {
          OPERATIONS_POINTER ptr = Operations::make_operations(reduceJobs[i]);
          GVector<uint32_t>::T& parentIndexVec = reduceJobs[i]->parentsIndex();
          for (size_t j = 0; j < parentIndexVec.size(); ++j) {
            ptr->push(localMap[parentIndexVec[j]]);
          }
          CONTEXT_TABLE_PTR result = ptr->execReduce();
          //std::cout << "Aggregate Result of " << reduceJobs[i]->nodeName_ << std::endl;
          //std::cout << *result << std::endl;
          localMap[reduceJobs[i]->pathNumber_] = result;
          if (reduceJobs[i]->pushGlobalMap_) {    // We need to add a new dummy layer, so construct a new context table
            CONTEXT_TABLE_PTR resultWithDummy = ptr->prepareForGlobalMap(result); // Dummy layer has a bigger layer index
            //std::cout << "Aggregate Result With Dummy" << std::endl;
            //std::cout << *resultWithDummy << std::endl;
            context->GlobalVariable_Reduce<std::pair<std::string, CONTEXT_TABLE_PTR> >
              (GV_CONTEXT_TABLE, std::make_pair(reduceJobs[i]->nodeName_, resultWithDummy));
          }
        }
      }
    }
  } //Reduce Ends.

  ALWAYS_INLINE void AfterIteration(gpelib4::MasterContext* context) { }

  void EndRun(gpelib4::BasicContext* context) {
    // TODO: Write context table of end vertices
    //std::cout << "End Run" << "\n";
    void* ptr = reinterpret_cast<MapVariable<std::string, CONTEXT_TABLE_PTR>* >
      (context->GetGlobalVariable(GV_CONTEXT_TABLE))->GetValuePtr();
    boost::unordered_map<std::string, CONTEXT_TABLE_PTR>* contextMap =
      reinterpret_cast<boost::unordered_map<std::string, CONTEXT_TABLE_PTR>* >(ptr);

    RAGraph endRunAggJobs(traverseMeta_);
    LocalMap localMap;

    writer_->WriteStartObject();
    //write output results in the global map
    VECTOR_STRING& outputNames = endRunAggJobs.getGlobalOutputNodes();
    for (size_t i = 0; i < outputNames.size(); ++i) {
      CONTEXT_TABLE_PTR tablePtr = contextMap->at(outputNames[i]);
      tablePtr->getContextTable();
      //std::cout << *tablePtr << std::endl;
      RANode* raNodePtr = endRunAggJobs.getRANodeByName(outputNames[i]);
      writeContextTable(tablePtr, raNodePtr);
    }
    //frontierNodes
    GVector<RANode*>::T& frontierNodes = endRunAggJobs.getEndRunFrontierNodes();
    for (size_t i = 0; i < frontierNodes.size(); ++i) {
      OPERATIONS_POINTER ptr = Operations::make_operations(frontierNodes[i]);
      VECTOR_STRING& parentsName = frontierNodes[i]->parentsName();
      for (size_t j = 0; j < parentsName.size(); ++j) {
        CONTEXT_TABLE_PTR tablePtr = contextMap->at(parentsName[j]);
        tablePtr->getContextTable();
        ptr->push(tablePtr);
      }
      CONTEXT_TABLE_PTR result = ptr->execEndRun();
      localMap[frontierNodes[i]->pathNumber_] = result;//adding result into local map
      if (frontierNodes[i]->isOutput_) {
        RANode* raNodePtr = endRunAggJobs.getRANodeByName(frontierNodes[i]->nodeName_);
        writeContextTable(result, raNodePtr);
      }
    }
    //EndRun Nodes
    GVector<RANode*>::T& endRunNodes = endRunAggJobs.getEndRunNodes();
    for (size_t i = 0; i < endRunNodes.size();++i) {
      OPERATIONS_POINTER ptr = Operations::make_operations(endRunNodes[i]);
      GVector<uint32_t>::T& parentsIndex = endRunNodes[i]->parentsIndex();
      for (size_t j = 0; j < parentsIndex.size(); ++j) {
        CONTEXT_TABLE_PTR tablePtr = localMap[parentsIndex[j]];
        tablePtr->getContextTable();
        ptr->push(tablePtr);
      }
      CONTEXT_TABLE_PTR result = ptr->execEndRun();
      localMap[endRunNodes[i]->pathNumber_] = result;//adding result into local map
      if (endRunNodes[i]->isOutput_) {
        RANode* raNodePtr = endRunAggJobs.getRANodeByName(endRunNodes[i]->nodeName_);
        writeContextTable(result, raNodePtr);
      }
    }
    writer_->WriteEndObject();
  }

private:
  // Write context table, with RANode
  void writeContextTable(CONTEXT_TABLE_PTR table, RANode* raNode) {
    ColumnMeta& columnMeta = raNode->newColumnMeta_;
    
    //write the name of contextTable
    std::string& tableName = raNode->nodeName_;
    writer_->WriteName(tableName.c_str());
    writer_->WriteStartObject();

    //write column names
    writer_->WriteName("columns");
    writer_->WriteStartArray();
    for (uint32_t k = 0; k < columnMeta.indexToInternalName.size(); ++k) {
      writer_->WriteString(columnMeta.indexToInternalName[k]);
    }
    writer_->WriteEndArray();
    
    //write rows of contextTable
    writer_->WriteName("data");
    writer_->WriteStartArray();
    DAGTraversal traversor(table);
    VECTOR_ANY row(columnMeta.colNumber);   // Have an initial size as the whole context table
    while (traversor.next(columnMeta, row)) {
      uint32_t dupNumber = traversor.getDuplicatedNum();
      for (uint32_t i = 0; i < dupNumber; ++i) {
        writer_->WriteStartArray();
        for (uint32_t lp = 0; lp < row.size(); ++lp) {
          switch (row[lp].which()) {
            case INT:
              writer_->WriteInt(boost::get<int>(row[lp]));
              break;
            case DOUBLE:
              writer_->WriteFloat(boost::get<double>(row[lp]));
              break;
            case STRING:
              writer_->WriteString(boost::get<std::string>(row[lp]));
              break;
            case BOOL:
              writer_->WriteBool(boost::get<bool>(row[lp]));
              break;
          }
          //TODO: handle id translation
        }
        writer_->WriteEndArray();
      }
    }
    writer_->WriteEndArray();
    writer_->WriteEndObject();
  }

  GraphMetaData graphMetaData_;
  Json::Value traverseMeta_;
  boost::shared_ptr<RAGraph> aggJobs_; 
  gutil::JSONWriter* writer_;
  // Traversal step number
  uint32_t stepNumber_;
  // Step related variables
  uint32_t etype_;
  Expression eFilter_;
  std::string enameInternal_;
  GVector<std::string>::T eDropList_;
  GVector<uint32_t>::T eDropListIndex_;
  GVector<uint32_t>::T eDropListType_;
  GVector<std::string>::T eAddList_; 
  GVector<uint32_t>::T eAddListIndex_;
  GVector<uint32_t>::T eAddListType_;
  uint32_t vtype_;
  std::string vnameInternal_;
  GUnorderedSet<uint32_t>::T memberList_;
  Expression vFilter_;
  GVector<std::string>::T vDropList_;
  GVector<uint32_t>::T vDropListIndex_;
  GVector<uint32_t>::T vDropListType_;
  GVector<std::string>::T vAddList_;
  GVector<uint32_t>::T vAddListIndex_;
  GVector<uint32_t>::T vAddListType_;

  void addVertexAttrList(V_ATTR* vAttr, VECTOR_ANY& valToAdd) {
    for (uint32_t i = 0; i < vAddList_.size(); ++i) {
      std::string& attr = vAddList_[i];
      if (attr == _EXTERNAL_ID) {
        // External id TODO: Change to system internal method
        valToAdd.push_back(getAttr(vAttr, vAddListIndex_[i], vAddListType_[i]));
      }
      else if (attr == _TYPE) {
        // type
        // Vertex Type index, e.g., company: 0, member: 1
        uint32_t vType = vAttr->type();
        // Vertex TypeName, e.g., 0: company, 1: member
        std::string vTypeName;
        graphMetaData_.getVertexTypeName(vType, vTypeName);
        valToAdd.push_back(vTypeName);
      }
      else {
        // Normal attribute
        valToAdd.push_back(getAttr(vAttr, vAddListIndex_[i], vAddListType_[i]));
      }
    }
  }

  void addEdgeAttrList(E_ATTR* eAttr, VECTOR_ANY& valToAdd) {
    for (uint32_t i = 0; i < eAddList_.size(); ++i) {
      std::string& attr = eAddList_[i];
      if (attr == _TYPE) {
        // type
        // Edge Type index, e.g., member-work-company: 0, member-skill: 1
        uint32_t eType = eAttr->type();
        // Edge TypeName, e.g., 0: member-work-company, 1: member-skill
        std::string eTypeName;
        graphMetaData_.getEdgeTypeName(eType, eTypeName);
        valToAdd.push_back(eTypeName);
      }
      else {
        // Normal attribute
        valToAdd.push_back(getAttr(eAttr, eAddListIndex_[i], eAddListType_[i]));
      }
    }
  }

  VAL getAttr( topology4::Attribute* attr, uint32_t index, uint32_t type) {
    switch (type) {
      case topology4::Attribute::BOOL_TYPE:
        return attr->GetBool(index, false);
      case topology4::Attribute::UINT_TYPE:
        return (int)attr->GetUInt32(index, 0);
      case topology4::Attribute::REAL_TYPE:
        return attr->GetDouble(index, 0);
      case topology4::Attribute::STRING_TYPE:
        return attr->GetString(index);
      // TODO: more other types need to be handled.
    }
    return "";
  }
  /*
  void deleteAttrList(GVector<std::string>::T toDeleteList, ContextTable& contextTable) {
    for (uint32_t k = 0; k < toDeleteList.size(); ++k) {
      if (!contextTable.deleteColumn(toDeleteList[k])) {// if the delete fails, sending out error
        GCERR << "Invalid Vertex Attribute: " << toDeleteList[k] << "\n";
        // TODO: Abort and return error message
      }
    }
  }*/
}; //GenericTraversal

} //UDIMPL namespace end
#endif // UDFS_GENERIC_TRAVERSAL_HPP_


