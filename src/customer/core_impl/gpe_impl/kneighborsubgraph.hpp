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
#include <gse2/dict/enum/enum_mapper.hpp>
#include <core_impl/udt.hpp>
#include "base/global_vector.hpp"
#include "base/global_set.hpp"


namespace UDIMPL {

struct EdgePair{
  VertexLocalId_t src;
  VertexLocalId_t tgt;

  EdgePair(unsigned int s=0, unsigned int t=0): src(s), tgt(t) {}

  friend std::ostream& operator<<(std::ostream& os, const EdgePair& obj){
    os<<obj.src<<" "<<obj.tgt;
    return os;
  }

  friend std::istream& operator>>(std::istream& is, EdgePair& obj){
    // read obj from stream
    is>>obj.src;
    is>>obj.tgt;
    return is;
  }

  friend bool operator==(const EdgePair& left, const EdgePair& right){
    return left.src == right.src && left.tgt == right.tgt;
  }
};

// define a hash function so we can use fast unordered_set to do counting of unique values
struct EdgePairHash : public std::unary_function<EdgePair,std::size_t>{
  std::size_t operator()(EdgePair const& p) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.src);
    boost::hash_combine(seed, p.tgt);
    return seed;
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
  KNeighborSubgraph(unsigned int depth,
                    VertexLocalId_t start_node,
                    bool need_verts,
                    bool need_edges,
                    gutil::JSONWriter* jsonwriter,
                    uint32_t max_vertices = std::numeric_limits<uint32_t>::max())
      : gpelib4::BaseUDF(ProcessingMode_, depth),
        start_node_(start_node),
        need_verts_(need_verts),
        need_edges_(need_edges),
        max_vertices_(max_vertices),
        writer_(jsonwriter) {
    // Nothing to do here.
  }

  ~KNeighborSubgraph() { }

  enum {GV_STARTNODE, GV_VERT_COUNT, GV_NEED_VERTS, GV_NEED_EDGES, GV_EDGELIST, GV_VERTLIST};

  void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
    globalvariables->Register(GV_STARTNODE, new gpelib4::BroadcastVariable<VertexLocalId_t>(start_node_));
    globalvariables->Register(GV_VERT_COUNT,new gpelib4::UnsignedIntSumVariable());
    globalvariables->Register(GV_NEED_VERTS, new gpelib4::BroadcastVariable<bool>(need_verts_));
    globalvariables->Register(GV_NEED_EDGES, new gpelib4::BroadcastVariable<bool>(need_edges_));
    globalvariables->Register(GV_EDGELIST,
                              new SetVariable< EdgePair,EdgePairHash >());
    globalvariables->Register(GV_VERTLIST,
                              new VectorVariable< VertexLocalId_t >(max_vertices_));
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
    valuecontext->GlobalVariable_Reduce<u_int32_t>(GV_VERT_COUNT,1);
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
    context->set_udfedgemapsetting(gpelib4::UDFEdgeMapSetting_RequireTargetVertexValue);
    context->set_udfreducesetting(gpelib4::UDFReduceSetting_RequireVertexValue);
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
      if(srcvid < targetvid){
        context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(srcvid,targetvid));
      }else{
        context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(targetvid,srcvid));
      }
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
    // count and (optionally) store the vertices
    if (singlevalue == 0 ){
      context->GlobalVariable_Reduce<u_int32_t>(GV_VERT_COUNT,1);
      if((context->GlobalVariable_GetValue<bool>(GV_NEED_VERTS) ||
          context->GlobalVariable_GetValue<bool>(GV_NEED_EDGES))) {
        context->GlobalVariable_Reduce<VertexLocalId_t>(GV_VERTLIST,vid);
      }
    }
  }



  /**
     * BeforeIteration is called before each iteration begins.
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
  void BeforeIteration(gpelib4::MasterContext* context) {

  }

  /**
     * BetweenMapAndReduce is called between the map and reduce phases
     * of each iteration.
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
  void BetweenMapAndReduce(gpelib4::MasterContext* context) {

  }

  /**
     * AfterIteration is called after each iteration.
     * It is not required to be defined.
     *
     * @param   context   Can be used to set active flags for vertices.
     */
  void AfterIteration(gpelib4::MasterContext* context) {

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
    edges_ = context->GlobalVariable_GetValue<boost::unordered_set<EdgePair,EdgePairHash> >(GV_EDGELIST);
    vert_count_ = context->GlobalVariable_GetValue<u_int32_t>(GV_VERT_COUNT);
    writer_->WriteStartObject();
    writer_->WriteName("neighborhood_size").WriteUnsignedInt(vert_count_);
    if(context->GlobalVariable_GetValue<bool>(GV_NEED_VERTS)){
      writer_->WriteName("vertices");
      writer_->WriteStartArray();
      for(std::vector<VertexLocalId_t>::iterator it = vids_.begin();
          it != vids_.end();
          ++it){
        writer_->WriteMarkVId(*it);
      }
      writer_->WriteEndArray();
    }
    if(context->GlobalVariable_GetValue<bool>(GV_NEED_EDGES)){
      writer_->WriteName("Edges");
      writer_->WriteStartArray();

      for(boost::unordered_set<EdgePair,EdgePairHash> ::iterator it = edges_.begin();
          it != edges_.end();
          ++it){
        writer_->WriteStartObject();
        writer_->WriteName("src");
        writer_->WriteMarkVId(it->src);
        writer_->WriteName("tgt");
        writer_->WriteMarkVId(it->tgt);
        writer_->WriteEndObject();
      }
      writer_->WriteEndArray();
    }
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
    return "";
  }

  std::vector<VertexLocalId_t>& getVidsToTranslate() {
    return vids_;
  }

  boost::unordered_set<EdgePair,EdgePairHash> & getEdgesToReturn() {
    return edges_;
  }

private:
  // root of k-neighborhood
  VertexLocalId_t start_node_;
  // do you want to collect + print vertex info?
  bool need_verts_;
  // do you need to collect + print edge info?
  bool need_edges_;
  // maximum number of vertices to store
  uint32_t max_vertices_;
  // this writer is a pointer back to the Service's writer object.
  gutil::JSONWriter* writer_;
  // list of found vertices.
  std::vector<VertexLocalId_t> vids_;
  // list of found edges.
  boost::unordered_set<EdgePair,EdgePairHash> edges_;
  // count of vertices found.
  u_int32_t vert_count_;
};
}  // namepsace UDIMPL

#endif /* GPERUN_KNEIGHBORSUBGRAPH_HPP_ */
