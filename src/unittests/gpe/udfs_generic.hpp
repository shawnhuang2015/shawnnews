/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * testudf.hpp: /gdbms/src/unittests/gpe/testudf.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_TESTUDF_HPP_
#define GPE_TESTUDF_HPP_

#include <baseudf/baseudf.hpp>
#include <gtest/gtest.h>

namespace unittest {

#include <udfinclude.hpp>

/**
 * 1. Test UD Topology: vertex attributes, edge attribute, id map
 * 2. Test multiple value save / load
 * 3. Test Append / Replace mode
 * 4. Test VertexMap and set_UDFMapRun
 * 5. Test Edge Writable
 * 6. Test GlobalVariables
 *
 *
 *
 * \tparam  VID_t       The user-supplied ID type.
 * \tparam  V_ATTR_t    The vertex attribute type.
 * \tparam  E_ATTR_t    The edge attribute type.
 */
template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_UD : public gpelib::BaseUDF {
 public:
  // These consts are required by the engine.
  static const ValueTypeFlag ValueTypeMode_ = Mode_MultipleValue;
  static const UDFDefineFlag InitializeMode_ = Mode_Defined;
  static const UDFDefineFlag AggregateReduceMode_ = Mode_NotDefined;
  static const UDFDefineFlag CombineReduceMode_ = Mode_Defined;
  static const UDFDefineFlag VertexMapMode_ = Mode_Defined;
  static const int PrintMode_ = Mode_Print_ByVertex | Mode_Print_ByEdge;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_AllVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef UDF_UD UDF_t;
  typedef VertexLocalId_t V_VALUE;
  typedef VertexLocalId_t MESSAGE;

  enum {
    GV_VertexCount,
    GV_EdgeCount,
    GV_MinVid,
    GV_MaxVId
  };

  static bool SortIdAsc(const VertexLocalId_t& v1,
                        const VertexLocalId_t& v2) {
    return v1 < v2;
  }

  static bool SortIdDesc(const VertexLocalId_t& v1,
                         const VertexLocalId_t& v2) {
    return v1 > v2;
  }

  DataSorter<VertexLocalId_t> asc_sorter_;
  DataSorter<VertexLocalId_t> desc_sorter_;

  /** Required constructor.
   *
   * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
   * in all-vertex mode.
   *
   * @param   maxiterations   the maximum number of iterations to run
   */
  UDF_UD()
      : BaseUDF(ProcessingMode_, 10000000),
        asc_sorter_(&UDF_UD::SortIdAsc),
        desc_sorter_(&UDF_UD::SortIdDesc) {
  }

  ~UDF_UD() {
  }

  void Initialize_GlobalVariables(
      gpelib::GlobalVariables* globalvariables) {
    globalvariables->Register(GV_VertexCount,
                              new UnsignedLongSumVariable());
    globalvariables->Register(GV_EdgeCount, new UnsignedLongSumVariable());
    globalvariables->Register(GV_MinVid, new UnsignedLongMinVariable());
    globalvariables->Register(GV_MaxVId, new UnsignedLongMaxVariable());
  }

  /** Initialize function to set up the initial values for the entire graph.
   * Run before the first iteration only.
   *
   * @param   valuecontext    Used to write initial values.
   * @param   idservice       Used to translate between user and engine ids.
   */
  void Initialize(MultipleValueContext* valuecontext,
                  IdService* idservice) {
    for (VertexLocalId_t id = 0; id < this->numofvertex(); ++id) {
      if (id == 0 || id == this->numofvertex() - 1)
        ASSERT_EQ(idservice->MapEngineID < VID > (id * 2), id);
      valuecontext->Write(id, id);
    }
  }

  /**
   * The edge map function is called during the map phase of each iteration.
   * It is called on all the edges of any active vertices.
   *
   * @param   srcvid                  The id of the source.
   * @param   srcvertexattr           The attributes of the source.
   * @param   srcvertexvaluebegin     The beginning iterator for the source values.
   * @param   srcvertexvalueend       The ending iterator for the source values.
   * @param   targetvid               The id of the target.
   * @param   targetvertexattr        The attributes of the target.
   * @param   targetvertexvaluebegin  The beginning iterator for the target values.
   * @param   targetvertexvalueend    The ending iterator for the target values.
   * @param   edgeattr                The attributes of the edge.
   * @param   context                 Used to write out values to vertices.
   */
  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const Const_Iterator<V_VALUE>& srcvertexvaluebegin,
               const Const_Iterator<V_VALUE>& srcvertexvalueend,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const Const_Iterator<V_VALUE>& targetvertexvaluebegin,
               const Const_Iterator<V_VALUE>& targetvertexvalueend,
               E_ATTR* edgeattr, MultipleValueMapContext* context) {
    context->GlobalVariable_Reduce < uint64_t > (GV_EdgeCount, 1);  // add edge count
    context->SetPhase(this->iteration());
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexAttribute(targetvid, targetvertexattr);
    // std::cout << srcvid << "," << targetvid << ","
    // << edgeattr->id_m2_ << "\n";
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);

    AssertVertexValue(srcvid, srcvertexvaluebegin, srcvertexvalueend);
    AssertVertexValue(targetvid, targetvertexvaluebegin,
                      targetvertexvalueend);

    context->Write(targetvid, srcvid);
    context->Write(srcvid, targetvid);  // Intended to add duplicates

    if (this->iteration() == 1)  // add 1/2 edge value
      if ((srcvid + targetvid) % 2 == 0) {
        std::string str = "two";
        edgeattr->WriteEdgeData(const_cast<char *>(str.c_str()),
                                str.size() + 1);
      }
    // add 1/3 edge value, may override iteration 1 edge writable value
    if (this->iteration() == 2)
      if ((srcvid + targetvid) % 3 == 0) {
        std::string str = "three";
        edgeattr->WriteEdgeData(const_cast<char *>(str.c_str()),
                                str.size() + 1);
      }
    if (this->iteration() == 3) {  // check value
      if ((srcvid + targetvid) % 3 == 0) {
        ASSERT_EQ(std::string(edgeattr->GetDataPtr()), "three");
        ASSERT_EQ(edgeattr->GetDataLength(), 6u);
      } else if ((srcvid + targetvid) % 2 == 0) {
        ASSERT_EQ(std::string(edgeattr->GetDataPtr()), "two");
        ASSERT_EQ(edgeattr->GetDataLength(), 4u);
      }
    }
  }

  /// verifyed the attribute is correct
  void AssertVertexAttribute(const VertexLocalId_t& vid,
                             V_ATTR* vertexattr) {
    int iid = vid;
    ASSERT_EQ(vertexattr->outgoing_degree_ >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetInt(1, 0), iid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  /// it should contains itself, its neighbors
  void AssertVertexValue(const VertexLocalId_t& vid,
                         const Const_Iterator<V_VALUE>& vertexvaluebegin,
                         const Const_Iterator<V_VALUE>& vertexvalueend) {
    if (this->iteration() == 1) {
      ASSERT_EQ(vertexvalueend.ptr() - 1, vertexvaluebegin.ptr());
      ASSERT_EQ(*vertexvaluebegin, vid);
      return;
    }
    unsigned int count = 1;
    ASSERT_NE(std::find(vertexvaluebegin.ptr(), vertexvalueend.ptr(), vid),
              vertexvalueend.ptr());
    if (vid > 0) {
      count++;
      ASSERT_NE(
          std::find(vertexvaluebegin.ptr(), vertexvalueend.ptr(),
                    vid >> 1),
          vertexvalueend.ptr());
      if ((vid << 1) < this->numofvertex()) {
        count++;
        ASSERT_NE(
            std::find(vertexvaluebegin.ptr(), vertexvalueend.ptr(),
                      vid << 1),
            vertexvalueend.ptr());
      }
    }
    if (((vid << 1) + 1) < this->numofvertex()) {
      count++;
      ASSERT_NE(
          std::find(vertexvaluebegin.ptr(), vertexvalueend.ptr(),
                    (vid << 1) + 1),
          vertexvalueend.ptr());
    }
    ASSERT_EQ(vertexvalueend - count, vertexvaluebegin);
  }

  /**
   * The vertex map function is called during the map phase of each iteration.
   * It is called on all the active vertices.
   *
   * @param   vid                 The id of this vertex.
   * @param   vertexattr          The attributes of this vertex.
   * @param   vertexvaluebegin    The beginning iterator for the values of this vertex.
   * @param   vertexvalueend      The ending iterator for the values of this vertex.
   * @param   context             Used to write out values to vertices.
   */
  ALWAYS_INLINE
  void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                 const Const_Iterator<V_VALUE>& vertexvaluebegin,
                 const Const_Iterator<V_VALUE>& vertexvalueend,
                 MultipleValueMapContext* context) {
    context->GlobalVariable_Reduce < uint64_t > (GV_VertexCount, 1);  // add vertex count
    context->GlobalVariable_Reduce < uint64_t > (GV_MinVid, vid);  // calc min
    context->GlobalVariable_Reduce < uint64_t > (GV_MaxVId, vid);  // calc max
    AssertVertexAttribute(vid, vertexattr);
    AssertVertexValue(vid, vertexvaluebegin, vertexvalueend);
    context->Write(vid, vid);
  }

  /**
   * The combine reduce function is called on lists of messages received
   * by vertices. It is very similar to the MapReduce / Hadoop combiner,
   * and similarly you should not assume that it is always called.
   *
   * @param   vid             The id we are combining for.
   * @param   msgvaluebegin   The beginning iterator for the messages.
   * @param   msgvalueend     The ending iterator for the messages.
   * @param   localcontext    Used to write out values for this vertex.
   */
  ALWAYS_INLINE
  void CombineReduce(const VertexLocalId_t& vid,
                     const Const_Iterator<MESSAGE>& msgvaluebegin,
                     const Const_Iterator<MESSAGE>& msgvalueend,
                     LocalContext* localcontext) {
    std::sort(msgvaluebegin.ptr(), msgvalueend.ptr());
    MESSAGE currentmsg = (MESSAGE) -1;
    for (Const_Iterator<MESSAGE> iter = msgvaluebegin; iter != msgvalueend;
        ++iter) {
      if (currentmsg == *iter) {
        continue;
      } else {
        if (currentmsg != (VertexLocalId_t) -1)
          localcontext->WriteValue(currentmsg);
        currentmsg = *iter;
      }
    }
    if (currentmsg != (VertexLocalId_t) -1)
      localcontext->WriteValue(currentmsg);
  }

  /**
   * The reduce function is called after the map phase of each iteration. It is
   * called on every vertex that either received messages or was signaled to be
   * active.
   *
   * @param   vid                 The id of this vertex.
   * @param   vertexattr          The attributes of this vertex.
   * @param   vertexvaluebegin    The beginning iterator for the vertex values.
   * @param   vertexvalueend      The ending iterator for the vertex values.
   * @param   msgvaluebegin       The beginning iterator for the received messages.
   * @param   msgvalueend         The ending iterator for the received messages.
   * @param   context             Used to write out values for this vertex.
   */
  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const Const_Iterator<V_VALUE>& vertexvaluebegin,
              const Const_Iterator<V_VALUE>& vertexvalueend,
              const Const_Iterator<MESSAGE>& msgvaluebegin,
              const Const_Iterator<MESSAGE>& msgvalueend,
              MultipleValueContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    AssertVertexValue(vid, vertexvaluebegin, vertexvalueend);
    std::sort(msgvaluebegin.ptr(), msgvalueend.ptr());
    MESSAGE currentmsg = (MESSAGE) -1;
    for (Const_Iterator<MESSAGE> iter = msgvaluebegin; iter != msgvalueend;
        ++iter) {
      if (currentmsg == *iter) {
        continue;
      } else {
        if (currentmsg != (VertexLocalId_t) -1)
          context->Write(vid, currentmsg);
        currentmsg = *iter;
      }
    }
    if (currentmsg != (VertexLocalId_t) -1)
      context->Write(vid, currentmsg);
    if (this->iteration() > 1) {
      context->GlobalVariable_Reduce < uint64_t > (GV_VertexCount, -1);  // reduce vertex count
      ASSERT_EQ(
          context->GlobalVariable_GetValue < uint64_t > (GV_VertexCount),
          this->numofvertex());
      ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_MinVid),
                0u);
      ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_MaxVId),
                this->numofvertex() - 1);
    }
    ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_EdgeCount),
              this->numofedge());
    ASSERT_EQ(context->GetPhase(), this->iteration());
  }

  /**
   * The write function is called on all vertices after the computation finishes.
   *
   * @param   ostream             The output stream object.
   * @param   vid                 The id of the vertex we are printing.
   * @param   vertexattr          The attributes of the vertex we are printing.
   * @param   vertexvaluebegin    The beginning iterator for the vertex values.
   * @param   vertexvalueend      The ending iterator for the vertex values.
   * @param   idservice           Used to translate between engine and user ids.
   */
  ALWAYS_INLINE
  void Write(gpelib::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr,
             const Const_Iterator<V_VALUE>& vertexvaluebegin,
             const Const_Iterator<V_VALUE>& vertexvalueend,
             IdService* idservice) {
    AssertVertexAttribute(vid, vertexattr);
    AssertVertexValue(vid, vertexvaluebegin, vertexvalueend);
    std::stringstream ss;
    ss << vid * 2;
    ASSERT_EQ(ss.str(), idservice->MapVID < VID > (vid));
  }

  ALWAYS_INLINE
  void WriteEdgeValue(gpelib::GOutputStream& ostream, IdService* idservice,
                      const VertexLocalId_t& srcvid,
                      const VertexLocalId_t& targetvid, E_ATTR* edgeattr) {
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);
    if (edgeattr->GetDataPtr() == NULL)
      return;
    {  // check value
      if ((srcvid + targetvid) % 3 == 0) {
        ASSERT_EQ(std::string(edgeattr->GetDataPtr()), "three");
        ASSERT_EQ(edgeattr->GetDataLength(), 6u);
      } else if ((srcvid + targetvid) % 2 == 0) {
        ASSERT_EQ(std::string(edgeattr->GetDataPtr()), "two");
        ASSERT_EQ(edgeattr->GetDataLength(), 4u);
      }
    }
    idservice->WriteMapVID < VID > (srcvid, ostream);
    ostream.put(',');
    idservice->WriteMapVID < VID > (targetvid, ostream);
    ostream.put(',');
    ostream.WriteUnsignedFloat(edgeattr->GetDouble(0, 0.0));
    ostream.put(',');
    ostream.write(edgeattr->GetDataPtr(), edgeattr->GetDataLength() - 1);
    ostream.put('\n');
  }

  /**
   * StartRun is only called before the first iteration begins (before the
   * first BeginIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void StartRun(BaseContext* context) {
  }

  /**
   * BeforeIteration is called before each iteration begins.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BeforeIteration(BaseContext* context) {
    if (this->iteration() == 1) {
      this->SetValueProcessingMethod(ValueProcessingMethod_Append);
      this->set_UDFMapRun(UDFMapRun_EdgeMap);
      SetVertexValueSorter(&asc_sorter_);
      SetMessageValueSorter(&asc_sorter_);
    } else {
      this->SetValueProcessingMethod(ValueProcessingMethod_Replace);
      this->set_UDFMapRun(UDFMapRun_EdgeMap | UDFMapRun_VertexMap);
      SetVertexValueSorter(&desc_sorter_);
      SetMessageValueSorter(&desc_sorter_);
    }
    reinterpret_cast<UnsignedLongSumVariable*>(context->GetGlobalVariable(
        GV_VertexCount))->Set(0);
    reinterpret_cast<UnsignedLongSumVariable*>(context->GetGlobalVariable(
        GV_EdgeCount))->Set(0);
  }

  /**
   * BetweenMapAndReduce is called between the map and reduce phases
   * of each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BetweenMapAndReduce(BaseContext* context) {
    if (this->iteration() > 1) {
      ASSERT_EQ(
          context->GlobalVariable_GetValue < uint64_t > (GV_VertexCount),
          this->numofvertex());
      ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_MinVid),
                0u);
      ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_MaxVId),
                this->numofvertex() - 1);
    }
    ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_EdgeCount),
              this->numofedge());
    if (this->iteration() == 3)
      context->Stop();
  }

  /**
   * AfterIteration is called after each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void AfterIteration(BaseContext* context) {
    ASSERT_EQ(
        context->GlobalVariable_GetValue < uint64_t > (GV_VertexCount),
        0u);
    ASSERT_EQ(context->GlobalVariable_GetValue < uint64_t > (GV_EdgeCount),
              this->numofedge());
  }

  /**
   * EndRun is called after the final iteration ends (after the
   * last AfterIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void EndRun(BaseContext* context) {
  }

  /**
   * Output is a general output method which can be used to do custom
   * outputting.
   * It is not required to be defined.
   *
   * @param   buckettable     Stores the values from the computation.
   * @param   idservice       Translates between user and engine ids.
   */
  std::string Output(gbucket::BucketTable* buckettable,
                     IdService* idservice) {
    return "";
  }

 private:
};

// Note: you do not need this if you use your own struct for the message
// and define operator+= appropriately for it. In that case, change the
// typedef below and remove this struct.
struct UDF_DMessage {
  int value_;

  UDF_DMessage()
      : value_(0) {
  }

  explicit UDF_DMessage(int value)
      : value_(value) {
  }

  UDF_DMessage& operator+=(const UDF_DMessage& other) {
    value_ = other.value_;
    return *this;
  }
};

/**
 * 1. Test D Topology: vertex attributes, edge attribute, id map
 * 2. Test single value save / load
 * 3. Test Active mode
 * 4. Test Edge Writable
 *
 * \tparam  VID_t       The user-supplied ID type.
 * \tparam  V_ATTR_t    The vertex attribute type.
 * \tparam  E_ATTR_t    The edge attribute type.
 */
template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_D : public gpelib::BaseUDF {
 public:
  // These consts are required by the engine.
  static const ValueTypeFlag ValueTypeMode_ = Mode_SingleValue;
  static const UDFDefineFlag InitializeMode_ = Mode_Defined;
  static const UDFDefineFlag AggregateReduceMode_ = Mode_Defined;
  static const UDFDefineFlag CombineReduceMode_ = Mode_NotDefined;
  static const UDFDefineFlag VertexMapMode_ = Mode_NotDefined;
  static const int PrintMode_ = Mode_Print_ByVertex | Mode_Print_ByEdge;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_ActiveVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef UDF_D UDF_t;
  typedef int V_VALUE;
  typedef UDF_DMessage MESSAGE;

  /** Required constructor.
   *
   * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
   * in all-vertex mode.
   *
   * @param   maxiterations   the maximum number of iterations to run
   */
  UDF_D()
      : BaseUDF(ProcessingMode_, 100000000) {
  }

  ~UDF_D() {
  }

  /** Initialize function to set up the initial values for the entire graph.
   * Run before the first iteration only.
   *
   * @param   valuecontext    Used to write inital values.
   * @param   idservice       Used to translate between user and engine ids.
   */
  void Initialize(SingleValueContext* valuecontext, IdService* idservice) {
    for (VertexLocalId_t id = 0; id < this->numofvertex(); ++id) {
      if (id == 0 || id == this->numofvertex() - 1) {
        std::stringstream ss;
        ss << "id_" << id * 2;
        VertexLocalId_t iid = idservice->MapEngineID(ss.str().c_str());
        ASSERT_EQ(iid, id);
      }
    }
    valuecontext->WriteAll(0, false);
    valuecontext->Write < V_VALUE > (0, 1);
  }

  /**
   * The edge map function is called during the map phase of each iteration.
   * It is called on all the edges of any active vertices.
   *
   * @param   srcvid              The id of the source.
   * @param   srcvertexattr       The attributes of the source.
   * @param   srcvertexvalue      The value of the source.
   * @param   targetvid           The id of the target.
   * @param   targetvertexattr    The attributes of the target.
   * @param   targetvertexvalue   The value of the target.
   * @param   edgeattr            The attributes of the edge.
   * @param   context             Used to write out values to vertices.
   */
  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
               SingleValueMapContext* context) {
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexAttribute(targetvid, targetvertexattr);
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);
    int iteration = this->iteration();
    ASSERT_EQ(srcvertexvalue, iteration);
    if (edgeattr->is_outgoing())
      context->template Write < MESSAGE
          > (targetvid, MESSAGE(srcvertexvalue + 1));
    else
      ASSERT_EQ(targetvertexvalue, iteration - 1);
    char zero = '0';
    edgeattr->WriteEdgeData(&zero, 1);
  }

  /// verifyed the attribute is correct
  void AssertVertexAttribute(const VertexLocalId_t& vid,
                             V_ATTR* vertexattr) {
    int iid = vid;
    ASSERT_EQ(vertexattr->outgoing_degree_ >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetInt(1, 0), iid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  /**
   * The reduce function is called after the map phase of each iteration. It is
   * called on every vertex that either received messages or was signaled to be
   * active.
   *
   * @param   vid             The id of this vertex.
   * @param   vertexattr      The attributes of this vertex.
   * @param   vertexvalue     The value of this vertex.
   * @param   accumulator     The accumulation of all messages sent to here.
   * @param   context         Used to write out values for this vertex.
   */
  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& vertexvalue, MESSAGE& accumulator,
              SingleValueContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    context->template Write < V_VALUE > (vid, accumulator.value_);
  }

  /**
   * The write function is called on all vertices after the computation finishes.
   *
   * @param   ostream         The output stream object.
   * @param   vid             The id of the vertex we are printing.
   * @param   vertexattr      The attributes of the vertex we are printing.
   * @param   vertexvalue     The value of the vertex we are printing.
   * @param   idservice       Used to translate between engine and user ids.
   */
  ALWAYS_INLINE
  void Write(GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& vertexvalue,
             IdService* idservice) {
    AssertVertexAttribute(vid, vertexattr);
    std::stringstream ss;
    ss << "id_" << vid * 2;
    ASSERT_EQ(ss.str(), idservice->MapVID < VID > (vid));
    unsigned int one = 1;
    ASSERT_LT(vid, one << (vertexvalue - 1));
    if (vid > 0)
      ASSERT_GE(vid, one << (vertexvalue - 2));
  }

  ALWAYS_INLINE
  void WriteEdgeValue(gpelib::GOutputStream& ostream, IdService* idservice,
                      const VertexLocalId_t& srcvid,
                      const VertexLocalId_t& targetvid, E_ATTR* edgeattr) {
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);
    if (edgeattr->GetDataPtr() == NULL)
      return;
    ASSERT_EQ(*(edgeattr->GetDataPtr()), '0');
    ASSERT_EQ(edgeattr->GetDataLength(), 1ul);
  }

  /**
   * StartRun is only called before the first iteration begins (before the
   * first BeginIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void StartRun(BaseContext* context) {
  }

  /**
   * BeforeIteration is called before each iteration begins.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BeforeIteration(BaseContext* context) {
  }

  /**
   * BetweenMapAndReduce is called between the map and reduce phases
   * of each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BetweenMapAndReduce(BaseContext* context) {
  }

  /**
   * AfterIteration is called after each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void AfterIteration(BaseContext* context) {
  }

  /**
   * EndRun is called after the final iteration ends (after the
   * last AfterIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void EndRun(BaseContext* context) {
  }

  /**
   * Output is a general output method which can be used to do custom
   * outputting.
   * It is not required to be defined.
   *
   * @param   buckettable     Stores the values from the computation.
   * @param   idservice       Translates between user and engine ids.
   */
  std::string Output(gbucket::BucketTable* buckettable,
                     IdService* idservice) {
    return "";
  }

 private:
};

enum AbortLocation {
  AbortLocation_StartRun = 0,
  AbortLocation_Initialize = 1,
  AbortLocation_BeforeIteration = 2,
  AbortLocation_Map = 3,
  AbortLocation_BetweenMapAndReduce = 4,
  AbortLocation_Reduce = 5,
  AbortLocation_AfterIteration = 6,
};

/**
 * 1. Test Abort
 *
 * \tparam  VID_t       The user-supplied ID type.
 * \tparam  V_ATTR_t    The vertex attribute type.
 * \tparam  E_ATTR_t    The edge attribute type.
 */
template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_Abort : public BaseUDF {
 public:
  // These consts are required by the engine.
  static const ValueTypeFlag ValueTypeMode_ = Mode_SingleValue;
  static const UDFDefineFlag InitializeMode_ = Mode_Defined;
  static const UDFDefineFlag AggregateReduceMode_ = Mode_Defined;
  static const UDFDefineFlag CombineReduceMode_ = Mode_NotDefined;
  static const UDFDefineFlag VertexMapMode_ = Mode_NotDefined;
  static const UDFDefineFlag PrintMode_ = Mode_Defined;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_AllVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef UDF_Abort UDF_t;
  typedef int V_VALUE;
  typedef UDF_DMessage MESSAGE;

  /** Required constructor.
   *
   * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
   * in all-vertex mode.
   *
   * @param   maxiterations   the maximum number of iterations to run
   */
  explicit UDF_Abort(AbortLocation abortlocation)
      : BaseUDF(ProcessingMode_, 100000000),
        abortlocation_(abortlocation) {
  }

  ~UDF_Abort() {
  }

  /** Initialize function to set up the initial values for the entire graph.
   * Run before the first iteration only.
   *
   * @param   valuecontext    Used to write inital values.
   * @param   idservice       Used to translate between user and engine ids.
   */
  void Initialize(SingleValueContext* valuecontext, IdService* idservice) {
    if (abortlocation_ == AbortLocation_Initialize)
      valuecontext->Stop();
    ASSERT_TRUE(abortlocation_ != AbortLocation_StartRun);
  }

  /**
   * The edge map function is called during the map phase of each iteration.
   * It is called on all the edges of any active vertices.
   *
   * @param   srcvid              The id of the source.
   * @param   srcvertexattr       The attributes of the source.
   * @param   srcvertexvalue      The value of the source.
   * @param   targetvid           The id of the target.
   * @param   targetvertexattr    The attributes of the target.
   * @param   targetvertexvalue   The value of the target.
   * @param   edgeattr            The attributes of the edge.
   * @param   context             Used to write out values to vertices.
   */
  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
               SingleValueMapContext* context) {
    ASSERT_TRUE(
        abortlocation_ != AbortLocation_StartRun
            && abortlocation_ != AbortLocation_Initialize
            && abortlocation_ != AbortLocation_BeforeIteration);
    if (abortlocation_ == AbortLocation_Map && !context->IsAborted()) {
      context->Stop();
      // std::cout << "\n\n\n\n\n Map Abort on " << srcvid << "\n\n\n\n\n\n";
    }
  }

  /**
   * The reduce function is called after the map phase of each iteration. It is
   * called on every vertex that either received messages or was signaled to be
   * active.
   *
   * @param   vid             The id of this vertex.
   * @param   vertexattr      The attributes of this vertex.
   * @param   vertexvalue     The value of this vertex.
   * @param   accumulator     The accumulation of all messages sent to here.
   * @param   context         Used to write out values for this vertex.
   */
  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& vertexvalue, MESSAGE& accumulator,
              SingleValueContext* context) {
    ASSERT_TRUE(
        abortlocation_ != AbortLocation_StartRun
            && abortlocation_ != AbortLocation_Initialize
            && abortlocation_ != AbortLocation_BeforeIteration
            && abortlocation_ != AbortLocation_Map
            && abortlocation_ != AbortLocation_BetweenMapAndReduce);
    if (abortlocation_ == AbortLocation_Reduce && !context->IsAborted()) {
      context->Stop();
      // std::cout << "\n\n\n\n\n Reduce Abort on " << vid << "\n\n\n\n\n\n";
    }
  }

  /**
   * The write function is called on all vertices after the computation finishes.
   *
   * @param   ostream         The output stream object.
   * @param   vid             The id of the vertex we are printing.
   * @param   vertexattr      The attributes of the vertex we are printing.
   * @param   vertexvalue     The value of the vertex we are printing.
   * @param   idservice       Used to translate between engine and user ids.
   */
  ALWAYS_INLINE
  void Write(GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& vertexvalue,
             IdService* idservice) {
    ASSERT_TRUE(
        abortlocation_ != AbortLocation_StartRun
            && abortlocation_ != AbortLocation_Initialize);
  }

  /**
   * StartRun is only called before the first iteration begins (before the
   * first BeginIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void StartRun(BaseContext* context) {
    if (abortlocation_ == AbortLocation_StartRun)
      context->Stop();
  }

  /**
   * BeforeIteration is called before each iteration begins.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BeforeIteration(BaseContext* context) {
    if (abortlocation_ == AbortLocation_BeforeIteration)
      context->Stop();
    ASSERT_TRUE(
        abortlocation_ != AbortLocation_StartRun
            && abortlocation_ != AbortLocation_Initialize);
  }

  /**
   * BetweenMapAndReduce is called between the map and reduce phases
   * of each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BetweenMapAndReduce(BaseContext* context) {
    if (abortlocation_ == AbortLocation_BetweenMapAndReduce)
      context->Stop();
    ASSERT_TRUE(
        abortlocation_ != AbortLocation_StartRun
            && abortlocation_ != AbortLocation_Initialize
            && abortlocation_ != AbortLocation_BeforeIteration
            && abortlocation_ != AbortLocation_Map);
  }

  /**
   * AfterIteration is called after each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void AfterIteration(BaseContext* context) {
    if (abortlocation_ == AbortLocation_AfterIteration)
      context->Stop();
    ASSERT_TRUE(abortlocation_ == AbortLocation_AfterIteration);
  }

  /**
   * EndRun is called after the final iteration ends (after the
   * last AfterIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void EndRun(BaseContext* context) {
    ASSERT_TRUE(abortlocation_ != AbortLocation_StartRun);
  }

  /**
   * Output is a general output method which can be used to do custom
   * outputting.
   * It is not required to be defined.
   *
   * @param   buckettable     Stores the values from the computation.
   * @param   idservice       Translates between user and engine ids.
   */
  std::string Output(gbucket::BucketTable* buckettable,
                     IdService* idservice) {
    return "";
  }

 private:
  AbortLocation abortlocation_;
};

struct UDF_MixValueData {
  VertexLocalId_t id_;

  UDF_MixValueData()
      : id_((VertexLocalId_t) -1) {
  }

  explicit UDF_MixValueData(VertexLocalId_t id)
      : id_(id) {
  }

  UDF_MixValueData& operator+=(const UDF_MixValueData& other) {
    if (id_ == (VertexLocalId_t) -1 || id_ > other.id_)
      id_ = other.id_;
    return *this;
  }

  bool operator<(const UDF_MixValueData& second) const {
    return id_ < second.id_;
  }
};

/**
 * 1. Test Abort
 *
 * \tparam  VID_t       The user-supplied ID type.
 * \tparam  V_ATTR_t    The vertex attribute type.
 * \tparam  E_ATTR_t    The edge attribute type.
 */
template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_MixValue : public BaseUDF {
 public:
  // These consts are required by the engine.
  static const int ValueTypeMode_ = Mode_SingleMultipleValue;
  static const int InitializeMode_ = Mode_Initialize_Individually;
  static const int AggregateReduceMode_ = Mode_NotDefined;
  static const int CombineReduceMode_ = Mode_Defined;
  static const int VertexMapMode_ = Mode_Defined;
  static const int PrintMode_ = Mode_Defined;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_AllVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef UDF_MixValue UDF_t;
  typedef UDF_MixValueData V_VALUE;
  typedef UDF_MixValueData MESSAGE;

  /** Required constructor.
   *
   * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
   * in all-vertex mode.
   *
   * @param   maxiterations   the maximum number of iterations to run
   */
  UDF_MixValue()
      : BaseUDF(ProcessingMode_, 2) {
    this->SetValueProcessingMethod(ValueProcessingMethod_Append);
  }

  ~UDF_MixValue() {
  }

  ALWAYS_INLINE
  void CombineReduce(const VertexLocalId_t& vid,
                     const Const_Iterator<MESSAGE>& msgvaluebegin,
                     const Const_Iterator<MESSAGE>& msgvalueend,
                     LocalContext* localcontext) {
    std::sort(msgvaluebegin.ptr(), msgvalueend.ptr());
    MESSAGE currentmsg;
    for (Const_Iterator<MESSAGE> iter = msgvaluebegin; iter != msgvalueend;
        ++iter) {
      if (currentmsg.id_ == iter->id_) {
        continue;
      } else {
        if (currentmsg.id_ != (VertexLocalId_t) -1)
          localcontext->WriteValue(currentmsg);
        currentmsg = *iter;
      }
    }
    if (currentmsg.id_ != (VertexLocalId_t) -1)
      localcontext->WriteValue(currentmsg);
  }

  /// verifyed the attribute is correct
  void AssertVertexAttribute(const VertexLocalId_t& vid,
                             V_ATTR* vertexattr) {
    int iid = vid;
    ASSERT_EQ(vertexattr->outgoing_degree_ >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetInt(1, 0), iid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  void AssertVertexValues(const VertexLocalId_t& vid,
                          const V_VALUE& singlevalue,
                          const Const_Iterator<V_VALUE>& vertexvaluebegin,
                          const Const_Iterator<V_VALUE>& vertexvalueend) {
    if (this->iteration() == 1) {
      ASSERT_EQ(vid, singlevalue.id_);
      Const_Iterator<V_VALUE> iter = vertexvaluebegin;
      if (vid >= 2) {
        GASSERT(iter != vertexvalueend, "");
        ASSERT_EQ(iter->id_, vid - 2);
        ++iter;
      }
      if (vid >= 1) {
        GASSERT(iter != vertexvalueend, "");
        ASSERT_EQ(iter->id_, vid - 1);
        ++iter;
      }
      ASSERT_EQ(iter, vertexvalueend);
    } else if (this->iteration() == 2) {
      ASSERT_EQ(vid + 1, singlevalue.id_);
    }
  }

  ALWAYS_INLINE
  void Initialize(MixValueContext* context, VertexLocalId_t vid,
                  V_ATTR* vertexattr, IdService* idservice) {
    if (vid % 2 == 0) {
      context->WriteToSingle(vid, UDF_MixValueData(vid));
    } else {
      context->Write(vid, UDF_MixValueData(vid));
    }
    if (vid >= 2)
      context->WriteToMultiple(vid, vid - 2);
    if (vid >= 1)
      context->WriteToMultiple(vid, vid - 1);
    AssertVertexAttribute(vid, vertexattr);
  }

  ALWAYS_INLINE
  void Write(gpelib::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& singlevalue,
             const Const_Iterator<V_VALUE>& vertexvaluebegin,
             const Const_Iterator<V_VALUE>& vertexvalueend,
             IdService* idservice) {
    AssertVertexAttribute(vid, vertexattr);
    std::stringstream ss;
    ss << vid * 2;
    ASSERT_EQ(ss.str(), idservice->MapVID < VID > (vid));
    idservice->WriteMapVID < VID > (vid, ostream);
    ostream << ", ";
    ostream << singlevalue.id_;
    ostream << ", ";
    for (Const_Iterator<V_VALUE> iter = vertexvaluebegin;
        iter < vertexvalueend; ++iter) {
      ostream << iter->id_;
      ostream << ", ";
    }
    ostream << "\n";
  }

  ALWAYS_INLINE
  void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                 const V_VALUE& singlevalue,
                 const Const_Iterator<V_VALUE>& vertexvaluebegin,
                 const Const_Iterator<V_VALUE>& vertexvalueend,
                 MultipleValueMapContext* context) {
    AssertVertexValues(vid, singlevalue, vertexvaluebegin, vertexvalueend);
    AssertVertexAttribute(vid, vertexattr);
    context->Write(vid, MESSAGE(vid + 1));
  }

  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const Const_Iterator<V_VALUE>& srcvertexvaluebegin,
               const Const_Iterator<V_VALUE>& srcvertexvalueend,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue,
               const Const_Iterator<V_VALUE>& targetvertexvaluebegin,
               const Const_Iterator<V_VALUE>& targetvertexvalueend,
               E_ATTR* edgeattr, MultipleValueMapContext* context) {
    AssertVertexValues(srcvid, srcvertexvalue, srcvertexvaluebegin,
                       srcvertexvalueend);
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexValues(targetvid, targetvertexvalue,
                       targetvertexvaluebegin, targetvertexvalueend);
    AssertVertexAttribute(targetvid, targetvertexattr);
    context->Write(targetvid, MESSAGE(srcvid + targetvid));
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& singlevalue,
              const Const_Iterator<V_VALUE>& vertexvaluebegin,
              const Const_Iterator<V_VALUE>& vertexvalueend,
              const Const_Iterator<MESSAGE>& msgvaluebegin,
              const Const_Iterator<MESSAGE>& msgvalueend,
              MixValueContext* context) {
    AssertVertexValues(vid, singlevalue, vertexvaluebegin, vertexvalueend);
    AssertVertexAttribute(vid, vertexattr);
    context->WriteToSingle(vid, V_VALUE(singlevalue.id_ + 1));
    for (Const_Iterator<MESSAGE> iter = msgvaluebegin; iter != msgvalueend;
        ++iter) {
      context->WriteToMultiple(vid, *iter);
    }
  }

 private:
};

}  // namespace unittest

#endif /* GPE_TESTUDF_HPP_ */
