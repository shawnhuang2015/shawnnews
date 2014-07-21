/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GAP
 *
 *  Created on: 6/18/2014
 *  Author: Adam + Nicholas
 *  Description: Lists all of the shortest paths between S and T.
 ******************************************************************************/

#ifndef GPERUN_LISTSHORTESTPATHS_HPP_
#define GPERUN_LISTSHORTESTPATHS_HPP_

#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>
#include <gpelib4/enginebase/variable.hpp>
#include "core_impl/udt.hpp"
#include "core_ext/gpe/util.hpp"
#include "core_ext/gpe/registry.hpp"
#include "gse/gse2/dict/enum/enum_mapper.hpp"
#include "core_ext/gpe/global_vector.hpp"



namespace UDIMPL {


  /**
   *
   * This UDF prints out all the edges on all the shortest paths between the
   * source and target vertices. It does this: starting from the source, the
   * computation expands outward with each node storing its incoming edges
   * until the target is found. When the target is found, all the other vertices
   * are deactivated and the target backtracks to the source node. In the end,
   * all the edges will be stored on the source node so that they can be easily
   * printed out.
   */
  class ListShortestPaths : public gpelib4::BaseUDF {
   public:

    // These consts are required by the engine.
    //Mode_SingleValue or Mode_MultipleValue or Mix Value
    static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_MultipleValue;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_NotDefined;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_NotDefined;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag PrintMode_ = gpelib4::Mode_NotDefined;
    static const gpelib4::EngineProcessingMode ProcessingMode_ = gpelib4::EngineProcessMode_ActiveVertices;

    // These typedefs are required by the engine.
    typedef topology4::VertexAttribute V_ATTR;
    typedef topology4::EdgeAttribute E_ATTR;
    typedef PathMember V_VALUE;
    typedef PathMember MESSAGE;

    /** Required constructor.
     *
     * We pass EngineProcessMode_ActiveVertices to BaseUDF to specify that we run
     * in active-vertex mode.
     *
     * @param   maxiterations   the maximum number of iterations to run
     */
    ListShortestPaths(unsigned int maxiteration,
                      VertexLocalId_t sourcevid,
                      VertexLocalId_t targetvid,
                      unsigned int weight_index) : gpelib4::BaseUDF(ProcessingMode_, maxiteration) {
      // TODO: Add constructor code here.
    }

    ~ListShortestPaths() {
      // TODO: Add destructor code here
    }

    /** Initialize function to set up the initial values for the entire graph.
     * Run before the first iteration only.
     *
     * @param   valuecontext  Used to write initial values.
     * @param   idservice     Used to translate between user and engine ids.
     */
    void Initialize(gpelib4::GlobalMultipleValueContext<V_VALUE>* valuecontext) {
      // TODO: Add code to initialize vertex values here.
    }

    /**
     * The edge map function is called during the map phase of each iteration.
     * It is called on all the edges of any active vertices.
     *
     * @param   srcvid          The id of the source.
     * @param   srcvertexattr       The attributes of the source.
     * @param   srcvertexvaluebegin   The beginning iterator for the source values.
     * @param   srcvertexvalueend     The ending iterator for the source values.
     * @param   targetvid         The id of the target.
     * @param   targetvertexattr    The attributes of the target.
     * @param   targetvertexvaluebegin  The beginning iterator for the target values.
     * @param   targetvertexvalueend  The ending iterator for the target values.
     * @param   edgeattr        The attributes of the edge.
     * @param   context         Used to write out values to vertices.
     */
    ALWAYS_INLINE void EdgeMap( const VertexLocalId_t& srcvid,
                                V_ATTR* srcvertexattr,
                                const gutil::Const_Iterator<V_VALUE>& srcvertexvaluebegin,
                                const gutil::Const_Iterator<V_VALUE>& srcvertexvalueend,
                                const VertexLocalId_t& targetvid,
                                V_ATTR* targetvertexattr,
                                const gutil::Const_Iterator<V_VALUE>& targetvertexvaluebegin,
                                const gutil::Const_Iterator<V_VALUE>& targetvertexvalueend,
                                E_ATTR* edgeattr,
                                gpelib4::MultipleValueMapContext<MESSAGE>* context) {
      //TODO: Add code to map function here
    }


    /**
     * The reduce function is called after the map phase of each iteration. It is
     * called on every vertex that either received messages or was signaled to be
     * active.
     *
     * @param   vid         The id of this vertex.
     * @param   vertexattr      The attributes of this vertex.
     * @param   vertexvaluebegin  The beginning iterator for the vertex values.
     * @param   vertexvalueend    The ending iterator for the vertex values.
     * @param   msgvaluebegin     The beginning iterator for the received messages.
     * @param   msgvalueend     The ending iterator for the received messages.
     * @param   context       Used to write out values for this vertex.
     */
    ALWAYS_INLINE void Reduce( const VertexLocalId_t& vid,
                               V_ATTR* vertexattr,
                               const gutil::Const_Iterator<V_VALUE>& vertexvaluebegin,
                               const gutil::Const_Iterator<V_VALUE>& vertexvalueend,
                               const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
                               const gutil::Const_Iterator<MESSAGE>& msgvalueend,
                               gpelib4::MultipleValueContext<V_VALUE>* context) {
      //TODO: Add code to reduce function here
    }


  /////////////////////// optional section for define user defined actions for engine. ///////////////////////
    /**
     * StartRun is only called before the first iteration begins (before the
     * first BeginIteration call).
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
    void StartRun(gpelib4::MasterContext* context) {
      //TODO: Add start run action here
      context->set_MultipleValueMergeMode(gbucket::MultipleValueMergeMode_ClearReplace);

      // You can modify the following default UDF settings:
      //context->set_udfedgemapsetting(  gpelib4::UDFEdgeMapSetting_RequireSourceVertexAttribute
      //                              | gpelib4::UDFEdgeMapSetting_RequireTargetVertexAttribute
      //                              | gpelib4::UDFEdgeMapSetting_RequireTargetVertexValue);
      //context->set_udfreducesetting(   gpelib4::UDFReduceSetting_RequireVertexAttribute
      //                              | gpelib4::UDFReduceSetting_RequireVertexValue);
      //context->set_udfprintsetting(0);


    }

    /**
     * BeforeIteration is called before each iteration begins.
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
    void BeforeIteration(gpelib4::MasterContext* context) {
      //TODO: Add iteration begin action here
    }

    /**
     * BetweenMapAndReduce is called between the map and reduce phases
     * of each iteration.
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
    void BetweenMapAndReduce(gpelib4::MasterContext* context) {
      //TODO: Add action between map and reduce here
    }

    /**
     * AfterIteration is called after each iteration.
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
    void AfterIteration(gpelib4::MasterContext* context) {
      //TODO: Add iteration end action here
    }

    /**
     * EndRun is called after the final iteration ends (after the
     * last AfterIteration call).
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
    void EndRun(gpelib4::BasicContext* context) {
      //TODO: Add end run action here
    }

    /////////////////////// optional section for output final results from vertex value buckets. ///////////////////////
    /**
     * Output is a general output method which can be used to do custom
     * outputting.
     * It is not required to be defined.
     *
     * @param   buckettable   Stores the values from the computation.
     * @param   idservice     Translates between user and engine ids.
     */
    std::string Output(gpelib4::BasicContext* context) {
      //TODO: Add code to output result here
      return "";
    }

   private:
    //TODO: define any required private members
  };
}

#endif /* GPERUN_LISTSHORTESTPATHS_HPP_ */
