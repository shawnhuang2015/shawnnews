/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * udfdelta.hpp: /gdbms/src/unittests/gpe/udfdelta.hpp
 *
 *  Created on: Oct 9, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_UDFDELTA_HPP_
#define GPE_UDFDELTA_HPP_

#include <topologydelta/topologydeltaretriever.hpp>

namespace unittest {

#include <udfinclude.hpp>

class TestTopologyDeltaRetriever : public gpelib::TopologyDeltaRetriever {
 public:
  typedef gpelib::TopologyDeltaRetriever::NodeRecord NodeRecord;
  typedef gpelib::TopologyDeltaRetriever::EdgeRecord EdgeRecord;

  explicit TestTopologyDeltaRetriever(VertexLocalId_t num_vertices)
      : num_vertices_(num_vertices) {
    buf_ = new char[1024];
  }

  ~TestTopologyDeltaRetriever() {
    delete[] buf_;
  }

  void PullDelta() {
    gpelib::TopologyDeltaRetriever::PullDelta();
    size_t index = dataversion_ / 2;
    if (dataversion_ % 2 == 0) {
      {  // modify vid = index + 1 to bad
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = (index + 1) * 2;
        noderecord->newNode_ = false;
        noderecord->record_num_ = dataversion_;
        noderecord->attributes_ = FetchVertexProp(index + 2);
        noderecords_.push_back(noderecord);
      }
      {  // incorrect record to reinsert an existing one
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = index * 2;
        noderecord->newNode_ = true;
        noderecord->record_num_ = dataversion_;
        noderecord->attributes_ = FetchVertexProp(index);
        noderecords_.push_back(noderecord);
      }
      {  // incorrect record to update an non-existing one
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = 399999999;
        noderecord->newNode_ = false;
        noderecord->record_num_ = dataversion_;
        noderecord->attributes_ = FetchVertexProp(0);
        noderecords_.push_back(noderecord);
      }
      {  // new vertex inside hole, good attribute. vid = num_vertices_ + index * 2
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = index * 2 + 1;
        noderecord->newNode_ = true;
        std::stringstream ss1;
        ss1 << "id_" << (num_vertices_ + index * 2) * 2;
        noderecord->userID_ = ss1.str();
        noderecord->record_num_ = dataversion_;
        noderecord->attributes_ = FetchVertexProp(
            num_vertices_ + 2 * index);
        noderecords_.push_back(noderecord);
      }
      {  // new vertex outside hole, no attribute. vid = num_vertices_ + index * 2 + 1
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = 2 * num_vertices_ + index;
        noderecord->newNode_ = true;
        std::stringstream ss1;
        ss1 << "id_" << (num_vertices_ + 2 * index + 1) * 2;
        noderecord->userID_ = ss1.str();
        noderecord->record_num_ = dataversion_;
        // noderecord->attributes_ = FetchVertexProp(
        //    num_vertices_ + 2 * index + 1);
        noderecords_.push_back(noderecord);
      }
      {  // make existing edge from (index + 1) *2 to (index + 1) bad
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = (index + 1) * 4;
        edgerecord->ToID_ = (index + 1) * 2;
        edgerecord->record_num_ = dataversion_;
        edgerecord->attributes_ = FetchEdgeProp(0, 0);
        edgerecords_.push_back(edgerecord);
      }
      {  // new edges from num_vertices_ + index * 2 to (index + 1). good attribute
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = index * 2 + 1;
        edgerecord->ToID_ = (index + 1) * 2;
        edgerecord->record_num_ = dataversion_;
        edgerecord->attributes_ = FetchEdgeProp(num_vertices_ + index * 2,
                                                index + 1);
        edgerecords_.push_back(edgerecord);
      }
      {  // new edges from num_vertices_ + index * 2 + 1 to (index + 1). bad attribute
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = 2 * num_vertices_ + index;
        edgerecord->ToID_ = (index + 1) * 2;
        edgerecord->record_num_ = dataversion_;
        edgerecord->attributes_ = NULL;  // FetchEdgeProp(0, 0);
        edgerecords_.push_back(edgerecord);
      }
      {  // invalid edge
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = 9999;
        edgerecord->ToID_ = 0;
        edgerecord->record_num_ = dataversion_;
        edgerecord->attributes_ = NULL;  // FetchEdgeProp(0, 0);
        edgerecords_.push_back(edgerecord);
      }
    } else {
      {  // modify vid = index + 1 to good
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = (index + 1) * 2;
        noderecord->newNode_ = false;
        noderecord->record_num_ = dataversion_;
        noderecord->attributes_ = FetchVertexProp(index + 1);
        noderecords_.push_back(noderecord);
      }
      {  // make up attribute. vid = num_vertices_ + index * 2 + 1
        NodeRecord* noderecord = new NodeRecord();
        noderecord->ID_ = 2 * num_vertices_ + index;
        noderecord->newNode_ = false;
        noderecord->record_num_ = dataversion_;
        noderecord->attributes_ = FetchVertexProp(
            num_vertices_ + 2 * index + 1);
        noderecords_.push_back(noderecord);
      }
      {  // make existing edge from (index + 1) *2 to (index + 1) good
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = (index + 1) * 4;
        edgerecord->ToID_ = (index + 1) * 2;
        edgerecord->attributes_ = FetchEdgeProp((index + 1) * 2,
                                                index + 1);
        edgerecord->record_num_ = dataversion_;
        edgerecords_.push_back(edgerecord);
      }
      {  // make edge from num_vertices_ + index * 2 + 1 to (index + 1). good attribute
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = 2 * num_vertices_ + index;
        edgerecord->ToID_ = (index + 1) * 2;
        edgerecord->record_num_ = dataversion_;
        edgerecord->attributes_ = FetchEdgeProp(
            num_vertices_ + index * 2 + 1, index + 1);
        edgerecords_.push_back(edgerecord);
      }
      {  // add edge from num_vertices_ + index * 2 + 1 to num_vertices_ + 2 * index. good attribute
        EdgeRecord* edgerecord = new EdgeRecord();
        edgerecord->FromID_ = index * 2 + 1;
        edgerecord->ToID_ = 2 * num_vertices_ + index;
        edgerecord->record_num_ = dataversion_;
        edgerecord->attributes_ = FetchEdgeProp(
            num_vertices_ + index * 2 + 1, num_vertices_ + 2 * index);
        edgerecords_.push_back(edgerecord);
      }
    }
    dataversion_++;
    std::cout << *this;
  }

  gvw::GVAttributes* FetchVertexProp(VertexLocalId_t id) {
    char* prop_ptr = buf_;
    prop_ptr[0] = gpelib::Attribute::BOOL_TYPE;
    prop_ptr[1] = id % 2 == 0 ? 1 : 0;
    prop_ptr[2] = gpelib::Attribute::INT_TYPE;
    int iid = id;
    memcpy(prop_ptr + 3, reinterpret_cast<char *>(&iid), sizeof(iid));
    prop_ptr[7] = gpelib::Attribute::STRING_TYPE;
    std::stringstream ss;
    ss << id;
    std::string sid = ss.str();
    unsigned int length = sid.size();
    memcpy(prop_ptr + 8, reinterpret_cast<char *>(&length),
           sizeof(length));
    memcpy(prop_ptr + 12, sid.c_str(), length);
    return new gvw::GVAttributes(3, prop_ptr);
  }

  gvw::GVAttributes* FetchEdgeProp(VertexLocalId_t id1,
                                   VertexLocalId_t id2) {
    char* prop_ptr = buf_;
    prop_ptr[0] = gpelib::Attribute::REAL_TYPE;
    double d = id1 + id2;
    memcpy(prop_ptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
    return new gvw::GVAttributes(1, prop_ptr);
  }

 private:
  char* buf_;
  VertexLocalId_t num_vertices_;
};

template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_Delta : public gpelib::BaseUDF {
 public:
  // These consts are required by the engine.
  static const ValueTypeFlag ValueTypeMode_ = Mode_MultipleValue;
  static const UDFDefineFlag InitializeMode_ = Mode_Defined;
  static const UDFDefineFlag AggregateReduceMode_ = Mode_NotDefined;
  static const UDFDefineFlag CombineReduceMode_ = Mode_NotDefined;
  static const UDFDefineFlag VertexMapMode_ = Mode_Defined;
  static const int PrintMode_ = Mode_Defined;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_ActiveVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef VertexLocalId_t V_VALUE;
  typedef VertexLocalId_t MESSAGE;

  size_t dataversion_;
  VertexLocalId_t orig_totalvertex_;
  size_t orig_totaledge_;

  UDF_Delta(size_t dataversion, VertexLocalId_t orig_totalvertex,
            size_t orig_totaledge)
      : BaseUDF(ProcessingMode_, 1),
        dataversion_(dataversion),
        orig_totalvertex_(orig_totalvertex),
        orig_totaledge_(orig_totaledge) {
    this->SetValueProcessingMethod(ValueProcessingMethod_Append);
  }

  void Initialize_GlobalVariables(
      gpelib::GlobalVariables* globalvariables) {
    globalvariables->Register(0, new UnsignedLongSumVariable());
  }

  void Initialize(MultipleValueContext* valuecontext,
                  IdService* idservice) {
    ASSERT_EQ(this->numofvertex(),
              orig_totalvertex_ + (dataversion_ + 1) / 2 * 2);
    for (VertexLocalId_t id = 0; id < this->numofvertex(); ++id) {
      valuecontext->Write(id, id);
      if (id >= orig_totalvertex_ - 1) {
        std::stringstream ss;
        ss << "id_" << id * 2;
        VertexLocalId_t iid = idservice->MapEngineID(ss.str().c_str());
        ASSERT_EQ(iid, id);
      }
    }
  }

  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const Const_Iterator<V_VALUE>& srcvertexvaluebegin,
               const Const_Iterator<V_VALUE>& srcvertexvalueend,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const Const_Iterator<V_VALUE>& targetvertexvaluebegin,
               const Const_Iterator<V_VALUE>& targetvertexvalueend,
               E_ATTR* edgeattr, MultipleValueMapContext* context) {
    context->GlobalVariable_Reduce < uint64_t > (0, 1);  // add edge count
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexAttribute(targetvid, targetvertexattr);
    ASSERT_EQ(
        srcvertexvalueend.ptr() - srcvertexvaluebegin.ptr() == 1
            && *srcvertexvaluebegin == srcvid,
        true);
    ASSERT_EQ(
        targetvertexvalueend.ptr() - targetvertexvaluebegin.ptr() == 1
            && *targetvertexvaluebegin == targetvid,
        true);
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0,
                                                                   10000)
        + 0.0001);
    if (dataversion_ % 2 == 1 && id_m2 != srcvid + targetvid)
      GCOUT(Verbose_UDFLow) << "\n" << srcvid << ", " << targetvid << ", "
                            << id_m2 << "\n";
    else
      ASSERT_EQ(id_m2, srcvid + targetvid);
  }

  /// verifyed the attribute is correct
  void AssertVertexAttribute(const VertexLocalId_t& vid,
                             V_ATTR* vertexattr) {
    int iid = vid;
    if (dataversion_ % 2 == 1) {
      size_t index = dataversion_ / 2;
      if (vid == orig_totalvertex_ + 2 * index
          || vid == orig_totalvertex_ + 2 * index + 1 || vid == index + 1)
        return;
    }
    ASSERT_EQ(vertexattr->outgoing_degree_ >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetInt(1, 0), iid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  ALWAYS_INLINE
  void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                 const Const_Iterator<V_VALUE>& vertexvaluebegin,
                 const Const_Iterator<V_VALUE>& vertexvalueend,
                 MultipleValueMapContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    ASSERT_EQ(
        vertexvalueend.ptr() - vertexvaluebegin.ptr() == 1
            && *vertexvaluebegin == vid,
        true);
    context->Write(vid, vid + 1);
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const Const_Iterator<V_VALUE>& vertexvaluebegin,
              const Const_Iterator<V_VALUE>& vertexvalueend,
              const Const_Iterator<MESSAGE>& msgvaluebegin,
              const Const_Iterator<MESSAGE>& msgvalueend,
              MultipleValueContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    ASSERT_EQ(
        vertexvalueend.ptr() - vertexvaluebegin.ptr() == 1
            && *vertexvaluebegin == vid,
        true);
    ASSERT_EQ(
        msgvalueend.ptr() - msgvaluebegin.ptr() == 1
            && *msgvaluebegin == vid + 1,
        true);
    context->Write(vid, *msgvaluebegin);
  }

  void BetweenMapAndReduce(BaseContext* context) {
    ASSERT_EQ(
        context->GlobalVariable_GetValue < uint64_t > (0),
        orig_totaledge_ + (dataversion_ + 1) / 2 * 6
            - (dataversion_ % 2) * 2);
  }

  ALWAYS_INLINE
  void Write(gpelib::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr,
             const Const_Iterator<V_VALUE>& vertexvaluebegin,
             const Const_Iterator<V_VALUE>& vertexvalueend,
             IdService* idservice) {
    AssertVertexAttribute(vid, vertexattr);
    ASSERT_EQ(
        vertexvalueend.ptr() - vertexvaluebegin.ptr() == 2
            && *vertexvaluebegin == vid
            && *(vertexvaluebegin + 1) == vid + 1,
        true);
    if (vid <= dataversion_ + 10 || vid >= orig_totalvertex_ - 10) {
      std::stringstream ss;
      ss << "id_" << vid * 2;
      ASSERT_EQ(ss.str(), idservice->MapVID < VID > (vid));
      ostream << vid << " " << ss.str() << " "
          << vertexattr->outgoing_degree_ << "\n";
    }
  }
};

}  // namespace unittest

#endif /* GPE_UDFDELTA_HPP_ */
