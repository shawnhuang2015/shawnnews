/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * jobs_udf.hpp: /gdbms/src/unittests/gpe/jobs_udf.hpp
 *
 *  Created on: Sep 24, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_JOBS_UDF_HPP_
#define GPE_JOBS_UDF_HPP_

#include <baseudf/baseudf.hpp>
#include <gutil/glogging.hpp>
#include <enginebase/defaultvertexattribute.hpp>
#include <enginebase/defaultedgeattribute.hpp>
#include <enginejob/enginejobcomposite.hpp>
#include <gtest/gtest.h>

namespace unittest {

#include <udfinclude.hpp>

struct UDF_JobMessage {
  int value_;

  UDF_JobMessage()
      : value_(0) {
  }

  explicit UDF_JobMessage(int value)
      : value_(value) {
  }

  UDF_JobMessage& operator+=(const UDF_JobMessage& other) {
    value_ = other.value_;
    return *this;
  }
};

template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_Job1 : public gpelib::BaseUDF {
 public:
  // These consts are required by the engine.
  static const int ValueTypeMode_ = Mode_SingleValue;
  static const int InitializeMode_ = Mode_Initialize_Individually;
  static const int JobInitializeMode_ = Mode_Initialize_FromBucket;
  static const int AggregateReduceMode_ = Mode_Defined;
  static const int CombineReduceMode_ = Mode_NotDefined;
  static const int VertexMapMode_ = Mode_Defined;
  static const int PrintMode_ = Mode_Defined;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_AllVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef int V_VALUE;
  typedef UDF_JobMessage MESSAGE;
  typedef VertexLocalId_t INIT_V_VALUE;

  explicit UDF_Job1(size_t maxiteration)
      : BaseUDF(ProcessingMode_, maxiteration) {
  }

  ~UDF_Job1() {
  }

  ALWAYS_INLINE void Initialize(SingleValueContext* context,
                                const VertexLocalId_t vid,
                                V_ATTR* vertexattr, IdService* idservice) {
    context->template Write<V_VALUE>(vid, vid);
  }

  ALWAYS_INLINE void Initialize(
      SingleValueContext* context, const VertexLocalId_t& vid,
      const INIT_V_VALUE& oldsinglevalue,
      Const_Iterator<INIT_V_VALUE>& oldvalue_beginiter,
      Const_Iterator<INIT_V_VALUE>& oldvalue_enditer) {
    context->template Write<V_VALUE>(vid, *oldvalue_beginiter);
  }

  ALWAYS_INLINE void EdgeMap(const VertexLocalId_t& srcvid,
                             V_ATTR* srcvertexattr,
                             const V_VALUE& srcvertexvalue,
                             const VertexLocalId_t& targetvid,
                             V_ATTR* targetvertexattr,
                             const V_VALUE& targetvertexvalue,
                             E_ATTR* edgeattr,
                             SingleValueMapContext* context) {
    char c = 'a' + this->iteration() - 1;
    edgeattr->AppendEdgeData(&c, 1);
  }

  ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid,
                               V_ATTR* vertexattr,
                               const V_VALUE& singlevalue,
                               SingleValueMapContext* context) {
    context->template Write < MESSAGE > (vid, MESSAGE(singlevalue + 1));
  }

  ALWAYS_INLINE void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                            const V_VALUE& vertexvalue,
                            MESSAGE& accumulator,
                            SingleValueContext* context) {
    context->template Write<V_VALUE>(vid, accumulator.value_);
  }

  ALWAYS_INLINE void Write(GOutputStream& ostream,
                           const VertexLocalId_t& vid, V_ATTR* vertexattr,
                           const V_VALUE& vertexvalue,
                           IdService* idservice) {
    ostream << vid << " " << vertexvalue << "\n";
  }

 private:
};

template<typename VID_t, typename V_ATTR_t, typename E_ATTR_t>
class UDF_Job2 : public gpelib::BaseUDF {
 public:
  // These consts are required by the engine.
  static const int ValueTypeMode_ = Mode_MultipleValue;
  static const int InitializeMode_ = Mode_Initialize_NotDefined;
  static const int JobInitializeMode_ = Mode_Initialize_FromBucket;
  static const int AggregateReduceMode_ = Mode_NotDefined;
  static const int CombineReduceMode_ = Mode_NotDefined;
  static const int VertexMapMode_ = Mode_Defined;
  static const int PrintMode_ = Mode_Print_ByVertex;
  static const EngineProcessingMode ProcessingMode_ =
      EngineProcessMode_AllVertices;

  // These typedefs are required by the engine.
  typedef VID_t VID;
  typedef V_ATTR_t V_ATTR;
  typedef E_ATTR_t E_ATTR;
  typedef UDF_Job2 UDF_t;
  typedef VertexLocalId_t V_VALUE;
  typedef VertexLocalId_t MESSAGE;
  typedef int INIT_V_VALUE;

  explicit UDF_Job2(size_t maxiteration)
      : BaseUDF(ProcessingMode_, maxiteration) {
  }

  ~UDF_Job2() {
  }

  ALWAYS_INLINE void Initialize(
      MultipleValueContext* context, const VertexLocalId_t& vid,
      const INIT_V_VALUE& oldvalue,
      Const_Iterator<INIT_V_VALUE>& oldvalue_beginiter,
      Const_Iterator<INIT_V_VALUE>& oldvalue_enditer) {
    V_VALUE v = oldvalue;
    context->template Write<V_VALUE>(vid, v);
  }

  ALWAYS_INLINE void EdgeMap(
      const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,
      const Const_Iterator<V_VALUE>& srcvertexvaluebegin,
      const Const_Iterator<V_VALUE>& srcvertexvalueend,
      const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,
      const Const_Iterator<V_VALUE>& targetvertexvaluebegin,
      const Const_Iterator<V_VALUE>& targetvertexvalueend,
      E_ATTR* edgeattr, MultipleValueMapContext* context) {
    ASSERT_EQ(edgeattr->GetDataLength(), 2ul);
    ASSERT_EQ(*edgeattr->GetDataPtr(), 'a');
    ASSERT_EQ(*(edgeattr->GetDataPtr() + 1), 'b');
  }

  ALWAYS_INLINE void VertexMap(
      const VertexLocalId_t& vid, V_ATTR* vertexattr,
      const Const_Iterator<V_VALUE>& vertexvaluebegin,
      const Const_Iterator<V_VALUE>& vertexvalueend,
      MultipleValueMapContext* context) {
    ASSERT_EQ(vertexvalueend.ptr() - vertexvaluebegin.ptr(), 1);
    context->Write(vid, *vertexvaluebegin + 1);
  }

  ALWAYS_INLINE void Reduce(
      const VertexLocalId_t& vid, V_ATTR* vertexattr,
      const Const_Iterator<V_VALUE>& vertexvaluebegin,
      const Const_Iterator<V_VALUE>& vertexvalueend,
      const Const_Iterator<MESSAGE>& msgvaluebegin,
      const Const_Iterator<MESSAGE>& msgvalueend,
      MultipleValueContext* context) {
    for (Const_Iterator<MESSAGE> iter = msgvaluebegin; iter != msgvalueend;
        ++iter) {
      context->Write(vid, *iter);
    }
  }

  ALWAYS_INLINE void Write(GOutputStream& ostream,
                           const VertexLocalId_t& vid, V_ATTR* vertexattr,
                           const Const_Iterator<V_VALUE>& vertexvaluebegin,
                           const Const_Iterator<V_VALUE>& vertexvalueend,
                           IdService* idservice) {
    ASSERT_EQ(vertexvalueend.ptr() - vertexvaluebegin.ptr(), 1);
    ASSERT_EQ(vid + 20, *vertexvaluebegin);
    ostream << vid << " " << *vertexvaluebegin << "\n";
  }

 private:
};

class UnitTestComposite : public gpelib::EngineJobComposite {
 public:
  UnitTestComposite(gmmnt::GlobalInstances* globalinstance,
                    gpelib::GPEGraphRegistry* graphregistry,
                    gpelib::EngineSetting* enginesetting,
                    std::string partitionpath, std::string outputfile)
      : gpelib::EngineJobComposite(globalinstance, graphregistry,
                                   enginesetting),
        partitionpath_(partitionpath),
        outputfile_(outputfile) {
  }

  void Run() {
    std::string graph1id = "graph1";
    std::string graph2id = "graph2";
    graphregistry_->RegisterGraph(
        (new gpelib::GPEGraph(graph1id))->SetTopologyPath(partitionpath_));
    graphregistry_->RegisterGraph(new gpelib::GPEGraph(graph2id));
    typedef gpelib::VertexAttribute V_ATTR;
    typedef gpelib::EdgeAttribute_Writable E_ATTR;
    typedef unittest::UDF_Job1<size_t, V_ATTR, E_ATTR> UDF_Job1_t;
    typedef unittest::UDF_Job2<size_t, V_ATTR, E_ATTR> UDF_Job2_t;
    gpelib::EngineJob<UDF_Job1_t> job1(globalinstance_, graphregistry_,
                                       enginesetting_, graph1id, graph2id);
    gpelib::EngineJob<UDF_Job2_t> job2(globalinstance_, graphregistry_,
                                       enginesetting_, graph2id, graph1id);
    size_t iteration = 1;
    size_t max_iteration = 4;
    while (true) {
      UDF_Job1_t udf_1(2);
      job1.SetUDF(&udf_1);
      if (iteration == 1)
        job1.InitUDFByInitGraph();
      else
        job1.InitUDFByJobGraph();
      job1.Run();
      // use job1.GetOutputGraph to adjust if needed.

      UDF_Job2_t udf_2(3);
      job2.SetUDF(&udf_2);
      job2.InitUDFByJobGraph();
      job2.Run();
      job2.GetOutputGraph()->SetEdgeValueStore(
          boost::shared_ptr<gpelib::EdgeWritableValueStore>());
      if (iteration++ == max_iteration) {
        job2.Print(outputfile_);
        break;
      }
    }
  }

  /*void Run_JobNoReuse() {
   std::string graph1id = "graph3";
   std::string graph2id = "graph4";
   graphregistry_->RegisterGraph(
   (new gpelib::GPEGraph(graph1id))->SetTopologyPath(partitionpath_));
   graphregistry_->RegisterGraph(new gpelib::GPEGraph(graph2id));
   typedef gpelib::VertexAttribute V_ATTR;
   typedef gpelib::EdgeAttribute E_ATTR;
   typedef unittest::UDF_Job1<size_t, V_ATTR, E_ATTR> UDF_Job1_t;
   typedef unittest::UDF_Job2<size_t, V_ATTR, E_ATTR> UDF_Job2_t;

   size_t iteration = 1;
   size_t max_iteration = 4;
   while (true) {
   UDF_Job1_t udf_1(2);
   UDF_Job2_t udf_2(3);
   gpelib::EngineJob<UDF_Job1_t> job1(globalinstance_, graphregistry_,
   enginesetting_, graph1id,
   graph2id);
   gpelib::EngineJob<UDF_Job2_t> job2(globalinstance_, graphregistry_,
   enginesetting_, graph2id,
   graph1id);
   job1.SetUDF(&udf_1);
   job2.SetUDF(&udf_2);
   if (iteration == 1)
   job1.InitUDFByMode<UDF_Job1_t::InitializeMode_>();
   else
   job1.InitUDF();
   job1.Run();
   job2.InitUDF();
   job2.Run();
   if (iteration++ == max_iteration) {
   job2.Print(outputfile_);
   break;
   }
   }
   }*/

 private:
  std::string partitionpath_;
  std::string outputfile_;
};

}  // namespace unittest

#endif /* GPE_JOBS_UDF_HPP_ */
