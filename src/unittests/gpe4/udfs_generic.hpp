/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * testudf.hpp: /gdbms/src/unittests/gpe/testudf.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPELIB4_TESTUDF_HPP_
#define GPELIB4_TESTUDF_HPP_

#include <gtest/gtest.h>
#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>

namespace unittest_gpe4 {

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
class UDF_UD : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const gpelib4::ValueTypeFlag ValueTypeMode_ =
      gpelib4::Mode_MultipleValue;
  static const gpelib4::UDFDefineFlag InitializeMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ =
      gpelib4::Mode_Defined;
  static const int PrintMode_ = gpelib4::Mode_Print_ByVertex
      | gpelib4::Mode_Print_ByEdge;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute_Writable E_ATTR;
  typedef VertexLocalId_t V_VALUE;
  typedef VertexLocalId_t MESSAGE;

  enum {
    GV_VertexCount,
    GV_EdgeCount,
    GV_MinVid,
    GV_MaxVId,
    GV_NeedTargetValue,
  };

  static bool SortIdAsc(const VertexLocalId_t& v1,
                        const VertexLocalId_t& v2) {
    return v1 < v2;
  }

  static bool SortIdDesc(const VertexLocalId_t& v1,
                         const VertexLocalId_t& v2) {
    return v1 > v2;
  }

  gutil::DataSorter<VertexLocalId_t> asc_sorter_;
  gutil::DataSorter<VertexLocalId_t> desc_sorter_;

  /** Required constructor.
   *
   * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
   * in all-vertex mode.
   *
   * @param   maxiterations   the maximum number of iterations to run
   */
  UDF_UD()
      : gpelib4::BaseUDF(gpelib4::EngineProcessMode_AllVertices, 100),
        asc_sorter_(&UDF_UD::SortIdAsc),
        desc_sorter_(&UDF_UD::SortIdDesc) {
    this->SetVertexValueSorter(&asc_sorter_);
    this->SetMessageValueSorter(&desc_sorter_);
    targetvaluecheck_ = true;
  }

  ~UDF_UD() {
  }

  void Initialize_GlobalVariables(
      gpelib4::GlobalVariables* globalvariables) {
    globalvariables->Register(GV_VertexCount,
                              new gpelib4::UnsignedLongSumVariable());
    globalvariables->Register(GV_EdgeCount,
                              new gpelib4::UnsignedLongSumVariable());
    globalvariables->Register(GV_MinVid,
                              new gpelib4::UnsignedLongMinVariable());
    globalvariables->Register(GV_MaxVId,
                              new gpelib4::UnsignedLongMaxVariable());
    globalvariables->Register(
        GV_NeedTargetValue, new gpelib4::BoolVariable(targetvaluecheck_));
  }

  /** Initialize function to set up the initial values for the entire graph.
   * Run before the first iteration only.
   *
   * @param   valuecontext    Used to write initial values.
   * @param   idservice       Used to translate between user and engine ids.
   */
  void Initialize(
      gpelib4::GlobalMultipleValueContext<V_VALUE>* valuecontext) {
    for (VertexLocalId_t id = 0; id < valuecontext->NumberofVertex();
        ++id) {
      valuecontext->Write(id, id);
    }
  }

  bool targetvaluecheck_;

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
  void EdgeMap(
      const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
      const gutil::Const_Iterator<V_VALUE>& srcvertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& srcvertexvalueend,
      const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
      const gutil::Const_Iterator<V_VALUE>& targetvertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& targetvertexvalueend,
      E_ATTR* edgeattr,
      gpelib4::MultipleValueMapContext<MESSAGE>* context) {
    context->GlobalVariable_Reduce<uint64_t>(GV_EdgeCount, 1);  // add edge count
    context->SetPhase(context->Iteration());
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexAttribute(targetvid, targetvertexattr);
    // std::cout << srcvid << "," << targetvid << ","
    // << edgeattr->id_m2_ << "\n";
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);
    AssertVertexValue(srcvid, srcvertexvaluebegin, srcvertexvalueend,
                      context);
    if (context->GlobalVariable_GetValue<bool>(GV_NeedTargetValue))
      AssertVertexValue(targetvid, targetvertexvaluebegin,
                        targetvertexvalueend, context);
    context->Write(targetvid, srcvid);
    context->Write(srcvid, targetvid);  // Intended to add duplicates
    if (context->Iteration() == 1)  // add 1/2 edge value
      if ((srcvid + targetvid) % 2 == 0) {
        std::string str = "two";
        edgeattr->WriteEdgeData(const_cast<char *>(str.c_str()),
                                str.size() + 1);
      }
    // add 1/3 edge value, may override iteration 1 edge writable value
    if (context->Iteration() == 2)
      if ((srcvid + targetvid) % 3 == 0) {
        std::string str = "three";
        edgeattr->WriteEdgeData(const_cast<char *>(str.c_str()),
                                str.size() + 1);
      }
    if (context->Iteration() == 3) {  // check value
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
    ASSERT_EQ(vertexattr->outgoing_degree() >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetUInt32(1, 0), vid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  /// it should contains itself, its neighbors
  void AssertVertexValue(
      const VertexLocalId_t& vid,
      const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
      gpelib4::BasicContext* context) {
    size_t size = vertexvalueend - vertexvaluebegin;
    if (false) {
      std::cout << "\n" << vid;
      for (gutil::Const_Iterator<V_VALUE> iter = vertexvaluebegin;
          iter != vertexvalueend; ++iter) {
        std::cout << " ";
        std::cout << *iter;
      }
      std::cout << "\n";
    }
    if (context->Iteration() == 1) {
      // GASSERT(vertexvalueend - 1 == vertexvaluebegin, vid);
      ASSERT_EQ(vertexvalueend - 1, vertexvaluebegin);
      ASSERT_EQ(*vertexvaluebegin, vid);
      return;
    }
    unsigned int count = 1;
    GASSERT(
        std::find(vertexvaluebegin, vertexvalueend, vid) != vertexvalueend,
        vid);
    ASSERT_NE(std::find(vertexvaluebegin, vertexvalueend, vid),
              vertexvalueend);
    if (vid > 0) {
      count++;
      ASSERT_NE(std::find(vertexvaluebegin, vertexvalueend, vid >> 1),
                vertexvalueend);
      if ((vid << 1) < context->NumberofVertex()) {
        count++;
        ASSERT_NE(std::find(vertexvaluebegin, vertexvalueend, vid << 1),
                  vertexvalueend);
      }
    }
    if (((vid << 1) + 1) < context->NumberofVertex()) {
      count++;
      ASSERT_NE(
          std::find(vertexvaluebegin, vertexvalueend, (vid << 1) + 1),
          vertexvalueend);
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
                 const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
                 const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
                 gpelib4::MultipleValueMapContext<MESSAGE>* context) {
    context->GlobalVariable_Reduce<uint64_t>(GV_VertexCount, 1);  // add vertex count
    context->GlobalVariable_Reduce<uint64_t>(GV_MinVid, vid);  // calc min
    context->GlobalVariable_Reduce<uint64_t>(GV_MaxVId, vid);  // calc max
    AssertVertexAttribute(vid, vertexattr);
    AssertVertexValue(vid, vertexvaluebegin, vertexvalueend, context);
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
                     const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
                     const gutil::Const_Iterator<MESSAGE>& msgvalueend,
                     gpelib4::LocalContext<MESSAGE>* localcontext) {
    MESSAGE currentmsg = (MESSAGE) -1;
    for (gutil::Const_Iterator<MESSAGE> iter = msgvaluebegin;
        iter != msgvalueend; ++iter) {
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
              const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
              const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
              const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
              const gutil::Const_Iterator<MESSAGE>& msgvalueend,
              gpelib4::MultipleValueContext<V_VALUE>* context) {
    /*if (vid <= 2) {
     std::cout << "\nReduce " << vid << ":";
     for (gutil::Const_Iterator<MESSAGE> iter = msgvaluebegin;
     iter != msgvalueend; ++iter) {
     std::cout << " ";
     std::cout << *iter;
     }
     std::cout << "\n";
     }*/
    AssertVertexAttribute(vid, vertexattr);
    AssertVertexValue(vid, vertexvaluebegin, vertexvalueend, context);
    MESSAGE currentmsg = (MESSAGE) -1;
    for (gutil::Const_Iterator<MESSAGE> iter = msgvaluebegin;
        iter != msgvalueend; ++iter) {
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
    if (context->Iteration() > 1) {
      context->GlobalVariable_Reduce<uint64_t>(GV_VertexCount, -1);  // reduce vertex count
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_VertexCount),
                context->NumberofVertex());
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_MinVid), 0u);
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_MaxVId),
                context->NumberofVertex() - 1);
    }
    ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_EdgeCount),
              context->NumberofEdge());
    ASSERT_EQ(context->GetPhase(), context->Iteration());
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
  void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr,
             const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
             const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
             gpelib4::BasicContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    AssertVertexValue(vid, vertexvaluebegin, vertexvalueend, context);
    ostream.WriteUnsignedInt(vid);
    for (gutil::Const_Iterator<V_VALUE> iter = vertexvaluebegin;
        iter != vertexvalueend; ++iter) {
      ostream << " ";
      ostream.WriteUnsignedInt(*iter);
    }
    ostream << "\n";
  }

  ALWAYS_INLINE
  void WriteEdgeValue(gutil::GOutputStream& ostream,
                      gpelib4::BasicContext* context,
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
    ostream.WriteUnsignedInt(srcvid);
    ostream.put(',');
    ostream.WriteUnsignedInt(targetvid);
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
  void StartRun(gpelib4::MasterContext* context) {
  }

  /**
   * BeforeIteration is called before each iteration begins.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BeforeIteration(gpelib4::MasterContext* context) {
    if (context->Iteration() == 1) {
      context->set_UDFMapRun(gpelib4::UDFMapRun_EdgeMap);
    } else {
      context->set_UDFMapRun(
          gpelib4::UDFMapRun_EdgeMap | gpelib4::UDFMapRun_VertexMap);
    }
    reinterpret_cast<gpelib4::UnsignedLongSumVariable*>(context
        ->GetGlobalVariable(GV_VertexCount))->Set(0);
    reinterpret_cast<gpelib4::UnsignedLongSumVariable*>(context
        ->GetGlobalVariable(GV_EdgeCount))->Set(0);
  }

  /**
   * BetweenMapAndReduce is called between the map and reduce phases
   * of each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void BetweenMapAndReduce(gpelib4::MasterContext* context) {
    if (context->Iteration() > 1) {
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_VertexCount),
                context->NumberofVertex());
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_MinVid), 0u);
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_MaxVId),
                context->NumberofVertex() - 1);
    }
    ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_EdgeCount),
              context->NumberofEdge());
    if (context->Iteration() == 3)
      context->Stop();
  }

  /**
   * AfterIteration is called after each iteration.
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void AfterIteration(gpelib4::MasterContext* context) {
    // merge will happen in next iteration map, so only set merge mode in AfterIteration
    if (context->Iteration() == 1) {
      context->set_MultipleValueMergeMode(
          gbucket::MultipleValueMergeMode_Append);
    } else {
      context->set_MultipleValueMergeMode(
          gbucket::MultipleValueMergeMode_Overwrite);
    }
    if (context->Iteration() == 3)
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_VertexCount),
                context->NumberofVertex());
    else
      ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_VertexCount),
                0u);
    ASSERT_EQ(context->GlobalVariable_GetValue<uint64_t>(GV_EdgeCount),
              context->NumberofEdge());
  }

  /**
   * EndRun is called after the final iteration ends (after the
   * last AfterIteration call).
   * It is not required to be defined.
   *
   * @param   context     Can be used to set active flags for vertices.
   */
  void EndRun(gpelib4::BasicContext* context) {
  }

  std::string Output(gpelib4::BasicContext* context) {
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
    if (other.value_ > 0)
      value_ = other.value_;
    return *this;
  }
};

/**
 * 1. Test D Topology: vertex attributes, edge attribute, id map
 * 2. Test single value save / load
 * 3. Test Active mode
 * 4. Test Edge Writable
 */
class UDF_D : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const gpelib4::ValueTypeFlag ValueTypeMode_ =
      gpelib4::Mode_SingleValue;
  static const gpelib4::UDFDefineFlag InitializeMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ =
      gpelib4::Mode_NotDefined;
  static const int PrintMode_ = gpelib4::Mode_Print_ByVertex
      | gpelib4::Mode_Print_ByEdge;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute_Writable E_ATTR;
  typedef int V_VALUE;
  typedef UDF_DMessage MESSAGE;

  UDF_D()
      : BaseUDF(gpelib4::EngineProcessMode_ActiveVertices, 100) {
  }

  ~UDF_D() {
  }

  void Initialize(
      gpelib4::GlobalSingleValueContext<V_VALUE>* valuecontext) {
    valuecontext->WriteAll(0, false);
    valuecontext->Write(0, 1);
  }

  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
               gpelib4::SingleValueMapContext<MESSAGE>* context) {
    // std::cout << "Map " << srcvid << "," << targetvid << ","
    //          << srcvertexvalue + 1 << "\n";
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexAttribute(targetvid, targetvertexattr);
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);
    int iteration = context->Iteration();
    // GASSERT(srcvertexvalue == iteration,
    //        srcvid << "," << srcvertexvalue << "," << iteration);
    ASSERT_EQ(srcvertexvalue, iteration);
    context->Write(targetvid, MESSAGE(srcvertexvalue + 1));
    char zero = '0';
    edgeattr->WriteEdgeData(&zero, 1);
  }

  /// verifyed the attribute is correct
  void AssertVertexAttribute(const VertexLocalId_t& vid,
                             V_ATTR* vertexattr) {
    int iid = vid;
    ASSERT_EQ(vertexattr->outgoing_degree() >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetUInt32(1, 0), vid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& vertexvalue, MESSAGE& accumulator,
              gpelib4::SingleValueContext<V_VALUE>* context) {
    // std::cout << "Reduce " << vid << "," << accumulator.value_ << "\n";
    AssertVertexAttribute(vid, vertexattr);
    context->Write(vid, accumulator.value_);
  }

  ALWAYS_INLINE
  void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& vertexvalue,
             gpelib4::BasicContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    unsigned int one = 1;
    ASSERT_LT(vid, one << (vertexvalue - 1));
    if (vid > 0)
      ASSERT_GE(vid, one << (vertexvalue - 2));
    ostream << vid << "," << vertexvalue << "\n";
  }

  ALWAYS_INLINE
  void WriteEdgeValue(gutil::GOutputStream& ostream,
                      gpelib4::BasicContext* context,
                      const VertexLocalId_t& srcvid,
                      const VertexLocalId_t& targetvid, E_ATTR* edgeattr) {
    VertexLocalId_t id_m2 = (VertexLocalId_t) (edgeattr->GetDouble(0, 0.0)
        + 0.0001);
    ASSERT_EQ(id_m2, srcvid + targetvid);
    if (edgeattr->GetDataPtr() == NULL)
      return;
    ASSERT_EQ(*(edgeattr->GetDataPtr()), '0');
    ASSERT_EQ(edgeattr->GetDataLength(), 1ul);
    ostream << srcvid << "," << targetvid << "\n";
  }

  void StartRun(gpelib4::MasterContext* context) {
  }

  void BeforeIteration(gpelib4::MasterContext* context) {
  }

  void BetweenMapAndReduce(gpelib4::MasterContext* context) {
  }

  void AfterIteration(gpelib4::MasterContext* context) {
  }

  void EndRun(gpelib4::BasicContext* context) {
  }

  std::string Output(gpelib4::BasicContext* context) {
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
class UDF_Abort : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const gpelib4::ValueTypeFlag ValueTypeMode_ =
      gpelib4::Mode_SingleValue;
  static const gpelib4::UDFDefineFlag InitializeMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag PrintMode_ = gpelib4::Mode_Defined;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute E_ATTR;
  typedef int V_VALUE;
  typedef UDF_DMessage MESSAGE;

  explicit UDF_Abort(AbortLocation abortlocation = AbortLocation_StartRun)
      : BaseUDF(gpelib4::EngineProcessMode_AllVertices, 100000000),
        abortlocation_(abortlocation) {
  }

  ~UDF_Abort() {
  }

  void Initialize_GlobalVariables(
      gpelib4::GlobalVariables* globalvariables) {
    globalvariables->Register(
        0, new gpelib4::UnsignedIntStateVariable(abortlocation_));
  }

  void Initialize(
      gpelib4::GlobalSingleValueContext<V_VALUE>* valuecontext) {
    AbortLocation abortlocation = (AbortLocation) valuecontext
        ->GlobalVariable_GetValue<uint32_t>(0);
    if (abortlocation == AbortLocation_Initialize)
      valuecontext->Stop();
    ASSERT_TRUE(abortlocation != AbortLocation_StartRun);
  }

  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
               gpelib4::SingleValueMapContext<MESSAGE>* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    ASSERT_TRUE(
        abortlocation != AbortLocation_StartRun
            && abortlocation != AbortLocation_Initialize
            && abortlocation != AbortLocation_BeforeIteration);
    if (abortlocation == AbortLocation_Map && !context->IsAborted()) {
      context->Stop();
      // std::cout << "\n\n\n\n\n Map Abort on " << srcvid << "\n\n\n\n\n\n";
    }
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& vertexvalue, MESSAGE& accumulator,
              gpelib4::SingleValueContext<V_VALUE>* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    ASSERT_TRUE(
        abortlocation != AbortLocation_StartRun
            && abortlocation != AbortLocation_Initialize
            && abortlocation != AbortLocation_BeforeIteration
            && abortlocation != AbortLocation_Map
            && abortlocation != AbortLocation_BetweenMapAndReduce);
    if (abortlocation == AbortLocation_Reduce && !context->IsAborted()) {
      context->Stop();
      // std::cout << "\n\n\n\n\n Reduce Abort on " << vid << "\n\n\n\n\n\n";
    }
  }

  ALWAYS_INLINE
  void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& vertexvalue,
             gpelib4::BasicContext* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    ASSERT_TRUE(
        abortlocation != AbortLocation_StartRun
            && abortlocation != AbortLocation_Initialize);
  }

  void StartRun(gpelib4::MasterContext* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    if (abortlocation == AbortLocation_StartRun)
      context->Stop();
  }

  void BeforeIteration(gpelib4::MasterContext* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    if (abortlocation == AbortLocation_BeforeIteration)
      context->Stop();
    ASSERT_TRUE(
        abortlocation != AbortLocation_StartRun
            && abortlocation != AbortLocation_Initialize);
  }

  void BetweenMapAndReduce(gpelib4::MasterContext* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    if (abortlocation == AbortLocation_BetweenMapAndReduce)
      context->Stop();
    ASSERT_TRUE(
        abortlocation != AbortLocation_StartRun
            && abortlocation != AbortLocation_Initialize
            && abortlocation != AbortLocation_BeforeIteration
            && abortlocation != AbortLocation_Map);
  }

  void AfterIteration(gpelib4::MasterContext* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    if (abortlocation == AbortLocation_AfterIteration)
      context->Stop();
    ASSERT_TRUE(
        abortlocation == AbortLocation_BeforeIteration
            || abortlocation == AbortLocation_Map
            || abortlocation == AbortLocation_BetweenMapAndReduce
            || abortlocation == AbortLocation_Reduce
            || abortlocation == AbortLocation_AfterIteration);
  }

  void EndRun(gpelib4::BasicContext* context) {
    AbortLocation abortlocation = (AbortLocation) context
        ->GlobalVariable_GetValue<uint32_t>(0);
    ASSERT_TRUE(abortlocation != AbortLocation_StartRun);
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
class UDF_MixValue : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const int ValueTypeMode_ = gpelib4::Mode_SingleMultipleValue;
  static const int InitializeMode_ = gpelib4::Mode_Initialize_Individually;
  static const int AggregateReduceMode_ = gpelib4::Mode_NotDefined;
  static const int CombineReduceMode_ = gpelib4::Mode_Defined;
  static const int VertexMapMode_ = gpelib4::Mode_Defined;
  static const int PrintMode_ = gpelib4::Mode_Defined;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute_Writable E_ATTR;
  typedef UDF_MixValueData V_VALUE;
  typedef UDF_MixValueData MESSAGE;

  static bool SortIdAsc(const VertexLocalId_t& v1,
                        const VertexLocalId_t& v2) {
    return v1 < v2;
  }

  gutil::DataSorter<VertexLocalId_t> asc_sorter_;

  UDF_MixValue()
      : BaseUDF(gpelib4::EngineProcessMode_AllVertices, 2),
        asc_sorter_(&UDF_MixValue::SortIdAsc) {
    this->SetMessageValueSorter(&asc_sorter_);
    this->SetVertexValueSorter(&asc_sorter_);
  }

  void StartRun(gpelib4::MasterContext* context) {
    context->set_MultipleValueMergeMode(
        gbucket::MultipleValueMergeMode_Append);
  }

  ~UDF_MixValue() {
  }

  ALWAYS_INLINE
  void CombineReduce(const VertexLocalId_t& vid,
                     const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
                     const gutil::Const_Iterator<MESSAGE>& msgvalueend,
                     gpelib4::LocalContext<MESSAGE>* localcontext) {
    MESSAGE currentmsg;
    for (gutil::Const_Iterator<MESSAGE> iter = msgvaluebegin;
        iter != msgvalueend; ++iter) {
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
    ASSERT_EQ(vertexattr->outgoing_degree() >= 0, true);
    ASSERT_EQ(vertexattr->GetBool(0, false), vid % 2 == 0);
    ASSERT_EQ(vertexattr->GetUInt32(1, 0), vid);
    ASSERT_EQ(atoi(vertexattr->GetString(2).c_str()), iid);
  }

  void AssertVertexValues(
      const VertexLocalId_t& vid, const V_VALUE& singlevalue,
      const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
      gpelib4::BasicContext* context) {
    if (context->Iteration() == 1) {
      ASSERT_EQ(vid, singlevalue.id_);
      gutil::Const_Iterator<V_VALUE> iter = vertexvaluebegin;
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
    } else if (context->Iteration() == 2) {
      ASSERT_EQ(vid + 1, singlevalue.id_);
    }
  }

  ALWAYS_INLINE
  void Initialize(gpelib4::MixValueContext<V_VALUE>* context,
                  VertexLocalId_t vid, V_ATTR* vertexattr) {
    if (vid % 2 == 0) {
      context->Write(vid, UDF_MixValueData(vid));
    } else {
      context->Write(vid, UDF_MixValueData(vid));
    }
    if (vid >= 2)
      context->WriteToMultiple(vid, UDF_MixValueData(vid - 2));
    if (vid >= 1)
      context->WriteToMultiple(vid, UDF_MixValueData(vid - 1));
    AssertVertexAttribute(vid, vertexattr);
  }

  ALWAYS_INLINE
  void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& singlevalue,
             const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
             const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
             gpelib4::BasicContext* context) {
    AssertVertexAttribute(vid, vertexattr);
    ostream << vid;
    ostream << ", ";
    ostream << singlevalue.id_;
    ostream << ", ";
    for (gutil::Const_Iterator<V_VALUE> iter = vertexvaluebegin;
        iter < vertexvalueend; ++iter) {
      ostream << iter->id_;
      ostream << ", ";
    }
    ostream << "\n";
  }

  ALWAYS_INLINE
  void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                 const V_VALUE& singlevalue,
                 const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
                 const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
                 gpelib4::MultipleValueMapContext<MESSAGE>* context) {
    AssertVertexValues(vid, singlevalue, vertexvaluebegin, vertexvalueend,
                       context);
    AssertVertexAttribute(vid, vertexattr);
    context->Write(vid, MESSAGE(vid + 1));
  }

  ALWAYS_INLINE
  void EdgeMap(
      const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
      const V_VALUE& srcvertexvalue,
      const gutil::Const_Iterator<V_VALUE>& srcvertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& srcvertexvalueend,
      const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
      const V_VALUE& targetvertexvalue,
      const gutil::Const_Iterator<V_VALUE>& targetvertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& targetvertexvalueend,
      E_ATTR* edgeattr,
      gpelib4::MultipleValueMapContext<MESSAGE>* context) {
    AssertVertexValues(srcvid, srcvertexvalue, srcvertexvaluebegin,
                       srcvertexvalueend, context);
    AssertVertexAttribute(srcvid, srcvertexattr);
    AssertVertexValues(targetvid, targetvertexvalue,
                       targetvertexvaluebegin, targetvertexvalueend,
                       context);
    AssertVertexAttribute(targetvid, targetvertexattr);
    context->Write(targetvid, MESSAGE(srcvid + targetvid));
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& singlevalue,
              const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
              const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
              const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
              const gutil::Const_Iterator<MESSAGE>& msgvalueend,
              gpelib4::MixValueContext<V_VALUE>* context) {
    AssertVertexValues(vid, singlevalue, vertexvaluebegin, vertexvalueend,
                       context);
    AssertVertexAttribute(vid, vertexattr);
    context->Write(vid, V_VALUE(singlevalue.id_ + 1));
    for (gutil::Const_Iterator<MESSAGE> iter = msgvaluebegin;
        iter != msgvalueend; ++iter) {
      context->WriteToMultiple(vid, *iter);
    }
  }

 private:
};

/**
 * test dense case performance
 1. Performance comparison for EdgeMap
 2. On my MacPro
 3. No multiple threads.
 4. 512 active nodes.
 5. Each node has 512 edges. So total 256K edges.
 6. Vertex / Edge has 8 attributes

 Performance results:
 A. Minimal(Go through edges + Source Vertex Value): 14.833 ms
 B. A + load Target Vertex Value: 16.119ms
 C. B + Parse Source Vertex Attribute:  96.342 ms
 D. C + Parse Target Vertex Attribute: 185.177 ms
 E. D + Parse Edge Attribute: 260.716 ms
 *
 */
class UDF_DenseD : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const gpelib4::ValueTypeFlag ValueTypeMode_ =
      gpelib4::Mode_SingleValue;
  static const gpelib4::UDFDefineFlag InitializeMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ =
      gpelib4::Mode_NotDefined;
  static const int PrintMode_ = gpelib4::Mode_Print_ByVertex;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute_Writable E_ATTR;
  typedef int V_VALUE;
  typedef int MESSAGE;

  UDF_DenseD()
      : BaseUDF(gpelib4::EngineProcessMode_ActiveVertices, 2) {
  }

  ~UDF_DenseD() {
  }

  void Initialize(
      gpelib4::GlobalSingleValueContext<V_VALUE>* valuecontext) {
    valuecontext->Write(0, 1);
  }

  ALWAYS_INLINE
  void ReadAttribute(topology4::Attribute* attribute) {
    attribute->GetBool(0, false);
    attribute->GetUInt32(1, 0);
    attribute->GetDouble(2, 0);
    attribute->GetString(3);
    attribute->GetBool(4, false);
    attribute->GetUInt32(5, 0);
    attribute->GetDouble(6, 0);
    attribute->GetString(7);
  }

  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
               gpelib4::SingleValueMapContext<MESSAGE>* context) {
    ReadAttribute(srcvertexattr);
    ReadAttribute(targetvertexattr);
    ReadAttribute(edgeattr);
    context->Write(targetvid, srcvertexvalue + 1);
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& vertexvalue, MESSAGE& accumulator,
              gpelib4::SingleValueContext<V_VALUE>* context) {
    ReadAttribute(vertexattr);
    context->Write(vid, accumulator);
  }

  ALWAYS_INLINE
  void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr, const V_VALUE& vertexvalue,
             gpelib4::BasicContext* context) {
    ReadAttribute(vertexattr);
    ostream.WriteUnsignedInt(vid);
    ostream << " ";
    ostream.WriteUnsignedInt(vertexvalue);
    ostream << "\n";
  }

  void StartRun(gpelib4::MasterContext* context) {
    // context->set_udfedgemapsetting(0);
    // UDFEdgeMapSetting_RequireSourceVertexAttribute = 1,
    // UDFEdgeMapSetting_RequireTargetVertexAttribute = 2,
    // UDFEdgeMapSetting_RequireTargetVertexValue = 4,
    // context->set_udfreducesetting(0);
    // context->set_udfprintsetting(0);
  }

  void BeforeIteration(gpelib4::MasterContext* context) {
  }

  void BetweenMapAndReduce(gpelib4::MasterContext* context) {
  }

  void AfterIteration(gpelib4::MasterContext* context) {
  }

  void EndRun(gpelib4::BasicContext* context) {
  }

  std::string Output(gpelib4::BasicContext* context) {
    return "";
  }

 private:
};

/**
 * test type filter
 */
class UDF_DenseD_TypeFilter : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const gpelib4::ValueTypeFlag ValueTypeMode_ =
      gpelib4::Mode_SingleValue;
  static const gpelib4::UDFDefineFlag InitializeMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ =
      gpelib4::Mode_NotDefined;
  static const int PrintMode_ = gpelib4::Mode_Print_NotDefined;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute E_ATTR;
  typedef int V_VALUE;
  typedef int MESSAGE;

  UDF_DenseD_TypeFilter()
      : BaseUDF(gpelib4::EngineProcessMode_AllVertices, 4) {
  }

  ~UDF_DenseD_TypeFilter() {
  }

  void Initialize_GlobalVariables(
      gpelib4::GlobalVariables* globalvariables) {
    globalvariables->Register(0,
                              new gpelib4::UnsignedLongSumVariable());
  }

  ALWAYS_INLINE
  void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
               const V_VALUE& srcvertexvalue,
               const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
               const V_VALUE& targetvertexvalue, E_ATTR* edgeattr,
               gpelib4::SingleValueMapContext<MESSAGE>* context) {
    context->GlobalVariable_Reduce<uint64_t>(0, 1);
    if(context->Iteration() == 2)
      ASSERT_EQ(srcvid >= targetvid, true);
    else if(context->Iteration() == 3)
      ASSERT_EQ(srcvid < targetvid, true);
    else if(context->Iteration() == 4)
      ASSERT_EQ(true, false); // not possible
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const V_VALUE& vertexvalue, MESSAGE& accumulator,
              gpelib4::SingleValueContext<V_VALUE>* context) {
  }

  void StartRun(gpelib4::MasterContext* context) {
  }

  void BeforeIteration(gpelib4::MasterContext* context) {
    // 1st iteration type 0 and 1
    // 2nd iteration type 0
    // 3st iteration type 1
    // 4th iteration no type
    if(context->Iteration() > 1){
      std::vector<uint32_t> types;
      if(context->Iteration() == 2)
        types.push_back(0);
      else if(context->Iteration() == 3)
        types.push_back(1);
      context->GetTypeFilterController()->SetEnabledEdgeTypes(types);
    }
    reinterpret_cast<gpelib4::UnsignedLongSumVariable*>(context
            ->GetGlobalVariable(0))->Set(0);
  }

  void BetweenMapAndReduce(gpelib4::MasterContext* context) {
  }

  void AfterIteration(gpelib4::MasterContext* context) {
      std::cout << "Iteration " << context->Iteration() << " go through "
                << context->GlobalVariable_GetValue<uint64_t>(0) << ".\n";
  }

  void EndRun(gpelib4::BasicContext* context) {
  }

  std::string Output(gpelib4::BasicContext* context) {
    return "";
  }

 private:
};

/**
 * 1. Test bitset / bucket value in distributed mode. Every time Map / Reduce writes to other segments.
 */
class UDF_JumpSegments : public gpelib4::BaseUDF {
 public:
  // These consts are required by the engine.
  static const gpelib4::ValueTypeFlag ValueTypeMode_ =
      gpelib4::Mode_MultipleValue;
  static const gpelib4::UDFDefineFlag InitializeMode_ =
      gpelib4::Mode_Defined;
  static const gpelib4::UDFDefineFlag AggregateReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag CombineReduceMode_ =
      gpelib4::Mode_NotDefined;
  static const gpelib4::UDFDefineFlag VertexMapMode_ =
      gpelib4::Mode_Defined;
  static const int PrintMode_ = gpelib4::Mode_Print_ByVertex;

  // These typedefs are required by the engine.
  typedef topology4::VertexAttribute V_ATTR;
  typedef topology4::EdgeAttribute E_ATTR;
  typedef VertexLocalId_t V_VALUE;
  typedef VertexLocalId_t MESSAGE;

  UDF_JumpSegments()
      : gpelib4::BaseUDF(gpelib4::EngineProcessMode_ActiveVertices, 20),
        idjump_(0) {
  }

  VertexLocalId_t idjump_;

  void Initialize(
      gpelib4::GlobalMultipleValueContext<V_VALUE>* valuecontext) {
    idjump_ = valuecontext->NumberofVertex() / 7 + 1;
    valuecontext->Write(0, 0);
  }

  ALWAYS_INLINE
  void EdgeMap(
      const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
      const gutil::Const_Iterator<V_VALUE>& srcvertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& srcvertexvalueend,
      const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
      const gutil::Const_Iterator<V_VALUE>& targetvertexvaluebegin,
      const gutil::Const_Iterator<V_VALUE>& targetvertexvalueend,
      E_ATTR* edgeattr,
      gpelib4::MultipleValueMapContext<MESSAGE>* context) {
    // do nothing
  }

  ALWAYS_INLINE
  void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                 const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
                 const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
                 gpelib4::MultipleValueMapContext<MESSAGE>* context) {
    ASSERT_EQ(vertexvalueend, vertexvaluebegin + 1);
    ASSERT_EQ(
        *vertexvaluebegin,
        (idjump_ * (context->Iteration() - 1) * 2)
            % context->NumberofVertex());
    VertexLocalId_t targetid = (vid + idjump_) % context->NumberofVertex();
    context->Write(targetid, targetid);
    std::cout << "Map " << targetid << "\n";
  }

  ALWAYS_INLINE
  void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
              const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
              const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
              const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
              const gutil::Const_Iterator<MESSAGE>& msgvalueend,
              gpelib4::MultipleValueContext<V_VALUE>* context) {
    ASSERT_EQ(msgvalueend, msgvaluebegin + 1);
    ASSERT_EQ(
        *msgvaluebegin,
        (idjump_ * (context->Iteration() * 2 - 1))
            % context->NumberofVertex());
    VertexLocalId_t targetid = (vid + idjump_) % context->NumberofVertex();
    context->Write(targetid, targetid);
    std::cout << "Reduce " << targetid << "\n";
  }

  ALWAYS_INLINE
  void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
             V_ATTR* vertexattr,
             const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
             const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
             gpelib4::BasicContext* context) {
    if (vertexvaluebegin == vertexvalueend)
      return;
    ostream.WriteUnsignedInt(vid);
    for (gutil::Const_Iterator<V_VALUE> iter = vertexvaluebegin;
        iter != vertexvalueend; ++iter) {
      ostream << " ";
      ostream.WriteUnsignedInt(*iter);
    }
    ostream << "\n";
  }

  void StartRun(gpelib4::MasterContext* context) {
    context->set_UDFMapRun(gpelib4::UDFMapRun_VertexMap);
    context->set_MultipleValueMergeMode(
        gbucket::MultipleValueMergeMode_Overwrite);
  }
};
}  // namespace unittest_gpe4

#endif /* GPELIB4_TESTUDF_HPP_ */
