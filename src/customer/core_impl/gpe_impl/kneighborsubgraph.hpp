/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GAP
 *
 *  Created on: 7/17/2014
 *  Author: Adam
 *  Description: Computes a k-neighborhood subgraph, with attributes
 ******************************************************************************/

#ifndef GPERUN_KNEIGHBORSUBGRAPH_HPP_
#define GPERUN_KNEIGHBORSUBGRAPH_HPP_

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

struct EdgePair{
    VertexLocalId_t src;
    VertexLocalId_t tgt;

    EdgePair(unsigned int f=0, unsigned int s=0): src(f), tgt(s) {}

    friend std::ostream& operator<<(std::ostream& os, const EdgePair& obj)
    {
      os<<obj.src<<" "<<obj.tgt;
      return os;
    }

    friend std::istream& operator>>(std::istream& is, EdgePair& obj)
    {
      // read obj from stream
      is>>obj.src;
      is>>obj.tgt;
      return is;
    }
  };

  class KNeighborSubgraph : public gpelib4::BaseUDF {
   public:

    // These consts are required by the engine.
    //Mode_SingleValue or Mode_MultipleValue or Mix Value
    static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
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
    typedef uint8_t V_VALUE;
    typedef VertexLocalId_t MESSAGE;

    /** Required constructor.
     *
     * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
     * in all-vertex mode.
     *
     * @param   maxiterations   the maximum number of iterations to run
     */
    KNeighborSubgraph(unsigned int maxiterations, VertexLocalId_t start_node, bool need_edges, gutil::JSONWriter* jsonwriter) :
      gpelib4::BaseUDF(ProcessingMode_, maxiterations),
      start_node_(start_node),
      need_edges_(need_edges),
      writer_(jsonwriter)
    {
      // TODO: Add constructor code here.
    }

    ~KNeighborSubgraph() {
      // TODO: Add destructor code here
    }

    enum {GV_STARTNODE, GV_NEED_EDGES, GV_EDGELIST, GV_VERTLIST};
    void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      globalvariables->Register(GV_STARTNODE, new gpelib4::BroadcastVariable<VertexLocalId_t>(start_node_));
      globalvariables->Register(GV_NEED_EDGES, new gpelib4::BroadcastVariable<bool>(need_edges_));
      globalvariables->Register(GV_EDGELIST,
                                new VectorVariable< EdgePair >());
      globalvariables->Register(GV_VERTLIST,
                                new VectorVariable< VertexLocalId_t >());
    }


    /** Initialize function to set up the initial values for the entire graph.
     * Run before the first iteration only.
     *
     * @param   valuecontext  Used to write initial values.
     * @param   idservice     Used to translate between user and engine ids.
     */
    void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE>* valuecontext) {
      VertexLocalId_t start_node = valuecontext->GlobalVariable_GetValue<VertexLocalId_t>(GV_STARTNODE);
      std::cout<<"initializing "<<start_node;
      valuecontext->GlobalVariable_Reduce<VertexLocalId_t>(GV_VERTLIST,
                                                        start_node);
      valuecontext->WriteAll(0,false);
      valuecontext->Write(start_node,1,true);
    }

    /**
     * StartRun is only called before the first iteration begins (before the
     * first BeginIteration call).
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
    void StartRun(gpelib4::MasterContext* context) {
      // append so that we get all edges incident on this vertex.
      // You can modify the following default UDF settings:
      context->set_udfedgemapsetting(gpelib4::UDFEdgeMapSetting_RequireTargetVertexValue);
      context->set_udfreducesetting(gpelib4::UDFReduceSetting_RequireVertexValue);
      //context->set_udfprintsetting(0);

      //context->set_print_decimal_places(5);
      //context->set_udfprintsetting(gpelib4::UDFPrintSetting_RequireVertexAttribute);
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
                                const V_VALUE& srcvertexvalue,
                                const VertexLocalId_t& targetvid,
                                V_ATTR* targetvertexattr,
                                const V_VALUE& targetvertexvalue,
                                E_ATTR* edgeattr,
                                gpelib4::SingleValueMapContext<MESSAGE>* context) {
      // Activate any neighbors that haven't been active before.
      if (targetvertexvalue != 1) {
        context->SetActiveFlag(targetvid);
      }

      // Store our own edges.
      if(context->GlobalVariable_GetValue<bool>(GV_NEED_EDGES)){
        context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST,
                                                 EdgePair(srcvid,targetvid));
      }
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
                               const V_VALUE& singlevalue,
                               const MESSAGE& accumulator,
                               gpelib4::SingleValueContext<V_VALUE>* context) {


      //bool check = true;
      context->Write(vid,1);

      // if we are just activated, then we need to be active in edge map so that we can
      // store edges and activate neighbors.
      if (singlevalue == 0) {
        context->GlobalVariable_Reduce<VertexLocalId_t>(GV_VERTLIST,
                                                        vid);
        context->SetActiveFlag(vid);
      }
    }

    /**
     * The write function is called on all vertices after the computation finishes.
     *
     * @param   ostream       The output stream object.
     * @param   vid         The id of the vertex we are printing.
     * @param   vertexattr      The attributes of the vertex we are printing.
     * @param   vertexvaluebegin  The beginning iterator for the vertex values.
     * @param   vertexvalueend    The ending iterator for the vertex values.
     * @param   idservice       Used to translate between engine and user ids.
     */
//    ALWAYS_INLINE void Write( gpelib::GOutputStream& ostream,
//                              const VertexLocalId_t& vid,
//                              V_ATTR* vertexattr,
//                              const V_VALUE& singlevalue,
//                              gpelib4::BasicContext* context) {

//      if(context->GlobalVariable_GetValue<bool>(GV_NEED_EDGES)){
//        for (Const_Iterator<V_VALUE> iter = vertexvaluebegin; iter != vertexvalueend; ++iter) {
//          context->GlobalVariable_Reduce<EdgePair>(GV_VERTLIST,
//                                                   EdgePair(vid,*iter));
//        }
//      }else{
//        context->GlobalVariable_Reduce<VertexLocalId_t>(GV_VERTLIST,
//                                                   vid);
//      }
//    }

  /////////////////////// optional section for define user defined actions for engine. ///////////////////////


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
      vids_ = context->GlobalVariable_GetValue<std::vector<VertexLocalId_t> >(GV_VERTLIST);
//      edges_ = context->GlobalVariable_GetValue<std::vector<EdgePair> >(GV_EDGELIST);
//      writer_->WriteStartObject();
//      writer_->WriteName("vertices");
//      writer_->WriteStartArray();
//      for(std::vector<VertexLocalId_t>::iterator it = vids_.begin();
//          it != vids_.end();
//          ++it){
//        writer_->WriteMarkVId(*it);
//      }
//      writer_->WriteEndArray();

//      writer_->WriteName("Edges");
//      writer_->WriteStartArray();

//      for(std::vector<EdgePair>::iterator it = edges_.begin();
//          it != edges_.end();
//          ++it){
//        writer_->WriteStartObject();
//        writer_->WriteName("src");
//        writer_->WriteMarkVId(it->src);
//        writer_->WriteName("tgt");
//        writer_->WriteMarkVId(it->tgt);
//        writer_->WriteEndObject();
//      }


//      writer_->WriteEndArray();
//      writer_->WriteEndObject();

      writer_->WriteStartObject();
      writer_->WriteName("neighborhood_size").WriteUnsignedInt(vids_.size());
      writer_->WriteEndObject();
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

    std::vector<VertexLocalId_t>& getVidsToTranslate(){
      return vids_;
    }

    std::vector<EdgePair>& getEdgesToReturn(){
      return edges_;
    }

   private:
    VertexLocalId_t start_node_;
    bool need_edges_;
    gutil::JSONWriter* writer_;
    std::vector<VertexLocalId_t> vids_;
    std::vector<EdgePair> edges_;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_KNEIGHBORSUBGRAPH_HPP_ */
