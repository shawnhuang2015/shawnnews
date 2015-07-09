/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GAP
 *
 *  Created on: 07/09/2015
 *  Author: Shawn Huang
 *  Description: Computes a k-neighborhood subgraph, with attributes
 ******************************************************************************/

#ifndef GPERUN_KETPNEIGHBORHOODSUBGRAPH_HPP_
#define GPERUN_KETPNEIGHBORHOODSUBGRAPH_HPP_

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
#include <gutil/jsonwriter.hpp>
#include <topology4/metafiles.hpp>

namespace VISUALIZATION {

  struct EdgePair {
    VertexLocalId_t src;
    VertexLocalId_t tgt;
    uint32_t type;
    uint32_t srctype;
    uint32_t tgttype;
    std::string attr;

    EdgePair(unsigned int s = 0, unsigned int t = 0, uint32_t vt = 0,
      std::string a = "", uint32_t svt = 0, uint32_t tvt = 0)
    : src(s), tgt(t), type(vt), attr(a), srctype(svt), tgttype(tvt) { }

    friend std::ostream& operator<<(std::ostream& os, const EdgePair& obj) {
      os << obj.src << " " << obj.tgt;
      return os;
    }

    friend std::istream& operator>>(std::istream& is, EdgePair& obj) {
      // read obj from stream
      is >> obj.src;
      is >> obj.tgt;
      return is;
    }

    friend bool operator==(const EdgePair& left, const EdgePair& right) {
      return left.src == right.src && left.tgt == right.tgt;
    }
  };

  // define a hash function so we can use fast unordered_set to do counting of unique values
  struct EdgePairHash : public std::unary_function<EdgePair, std::size_t> {
    std::size_t operator()(EdgePair const& p) const {
      std::size_t seed = 0;
      boost::hash_combine(seed, p.src);
      boost::hash_combine(seed, p.tgt);
      return seed;
    }
  };

  struct VertexWithAttributes {
    VertexLocalId_t vid;
    uint32_t vtype;
    std::string attr;

    VertexWithAttributes() : vid(-1), vtype(0), attr() { }
    VertexWithAttributes(VertexLocalId_t v, uint32_t t, std::string a) : vid(v), vtype(t), attr(a) { }

    friend std::ostream& operator<<(std::ostream& os, const VertexWithAttributes& obj) {
      return os;
    }

    friend std::istream& operator>>(std::istream& is, VertexWithAttributes& obj) {
      return is;
    }
  };

  class KStepNeighborhoodSubgraph : public gpelib4::BaseUDF {
  public:
    // These consts are required by the engine.
    // Mode_SingleValue or Mode_MultipleValue or Mix Value
    static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
    // Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
    // Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
    // Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
    // Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined;
    // Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag PrintMode_ = gpelib4::Mode_NotDefined;
    static const gpelib4::EngineProcessingMode ProcessingMode_ = gpelib4::EngineProcessMode_ActiveVertices;

    // These typedefs are required by the engine.
    typedef topology4::VertexAttribute V_ATTR;
    typedef topology4::EdgeAttribute E_ATTR;
    typedef uint8_t V_VALUE;
    typedef VertexLocalId_t MESSAGE;

    // unsigned int depth = 2;

    /** Required constructor.
       *
       * We pass EngineProcessMode_AllVertices to BaseUDF to specify that we run
       * in all-vertex mode.
       *
       * @param   maxiterations   the maximum number of iterations to run
       */
    KStepNeighborhoodSubgraph(VertexLocalId_t start_node,
                           gutil::JSONWriter* jsonwriter,
                           uint32_t max_vertices = std::numeric_limits<uint32_t>::max(),
                           uint32_t depth = 1,
                           topology4::TopologyMeta* topologyMeta = NULL)
        : gpelib4::BaseUDF(ProcessingMode_, depth),
          start_node_(start_node),
          max_vertices_(max_vertices),
          writer_(jsonwriter),
          topologyMeta_(topologyMeta) {
      // Nothing to do here.
    }

    ~KStepNeighborhoodSubgraph() {}

    enum {GV_STARTNODE, GV_VERT_COUNT, GV_EDGELIST, GV_VERTLIST};

    void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      globalvariables->Register(GV_STARTNODE, new gpelib4::BroadcastVariable<VertexLocalId_t>(start_node_));
      globalvariables->Register(GV_VERT_COUNT, new gpelib4::UnsignedIntSumVariable());
      globalvariables->Register(GV_EDGELIST, new UDIMPL::SetVariable< EdgePair, EdgePairHash >());
      globalvariables->Register(GV_VERTLIST, new UDIMPL::VectorVariable< VertexWithAttributes >(max_vertices_));
    }


    /** Initialize function to set up the initial values for the entire graph.
       * Run before the first iteration only.
       *
       * @param   valuecontext  Used to write initial values.
       * @param   idservice     Used to translate between user and engine ids.
       */
    void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE>* valuecontext) {
      VertexLocalId_t start_node = valuecontext->GlobalVariable_GetValue<VertexLocalId_t>(GV_STARTNODE);
      // std::cout << "initializing " << start_node;
      // valuecontext->GlobalVariable_Reduce<VertexWithAttributes>(GV_VERTLIST,
      //                                                          startVertex);
      valuecontext->GlobalVariable_Reduce<u_int32_t>(GV_VERT_COUNT, 1);
      valuecontext->WriteAll(0, false);
      valuecontext->Write(start_node, 1, true);
    }

    /**
       * StartRun is only called before the first iteration begins (before the
       * first BeginIteration call).
       * It is not required to be defined.
       *
       * @param   context   Can be used to set active flags for vertices.
       */
    void StartRun(gpelib4::MasterContext* context) {
      // context->set_udfedgemapsetting(gpelib4::UDFEdgeMapSetting_RequireTargetVertexValue);
      // context->set_udfreducesetting(gpelib4::UDFReduceSetting_RequireVertexValue);
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
    ALWAYS_INLINE void EdgeMap(const VertexLocalId_t& srcvid,
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

      std::ostringstream stream;
      stream << "\"" << *edgeattr << "\"";
      std::string attr =  stream.str();

      // Store our own edges.
      if (srcvid < targetvid) {
        context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(srcvid, targetvid, edgeattr->type(), attr, srcvertexattr->type(), targetvertexattr->type()));
      } else {
        context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(targetvid, srcvid, edgeattr->type(), attr, targetvertexattr->type(), srcvertexattr->type()));
      }
    }

    ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& value, gpelib4::SingleValueMapContext<MESSAGE>* context) {
      if (context->Iteration() == 1) {
        // gutil::JSONWriter writer;
        // vertexattr->WriteToJson(writer);
        // VertexWithAttributes v(vid, vertexattr->type(), writer.str());

        std::ostringstream stream;
        stream << "\"" << *vertexattr << "\"";
        std::string attr =  stream.str();

        VertexWithAttributes v(vid, vertexattr->type(), attr);
        context->GlobalVariable_Reduce<VertexWithAttributes>(GV_VERTLIST, v);
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
    ALWAYS_INLINE void Reduce(const VertexLocalId_t& vid,
                              V_ATTR* vertexattr,
                              const V_VALUE& singlevalue,
                              const MESSAGE& accumulator,
                              gpelib4::SingleValueContext<V_VALUE>* context) {
      context->Write(vid, 1);

      // if we are activated for the first time, then we need to be active in the 
      // next edge map so that we can count and store the vertices and edges.
      if (singlevalue == 0) {
        context->GlobalVariable_Reduce<u_int32_t>(GV_VERT_COUNT, 1);

        // gutil::JSONWriter vwriter;
        // vertexattr->WriteToJson(vwriter);
        // VertexWithAttributes v(vid, vertexattr->type(), vwriter.str());

        std::ostringstream stream;
        stream << "\"" << *vertexattr << "\"";
        std::string attr =  stream.str();

        VertexWithAttributes v(vid, vertexattr->type(), attr);

        context->GlobalVariable_Reduce<VertexWithAttributes>(GV_VERTLIST, v);
      }
    }



    /**
       * BeforeIteration is called before each iteration begins.
       * It is not required to be defined.
       *
       * @param   context   Can be used to set active flags for vertices.
       */
    void BeforeIteration(gpelib4::MasterContext* context) {}

    /**
       * BetweenMapAndReduce is called between the map and reduce phases
       * of each iteration.
       * It is not required to be defined.
       *
       * @param   context   Can be used to set active flags for vertices.
       */
    void BetweenMapAndReduce(gpelib4::MasterContext* context) {}

    /**
       * AfterIteration is called after each iteration.
       * It is not required to be defined.
       *
       * @param   context   Can be used to set active flags for vertices.
       */
    void AfterIteration(gpelib4::MasterContext* context) {}

    /**
       * EndRun is called after the final iteration ends (after the
       * last AfterIteration call).
       * It is not required to be defined.
       *
       * @param   context   Can be used to set active flags for vertices.
       */
    void EndRun(gpelib4::BasicContext* context) {
      vids_ = context->GlobalVariable_GetValue<std::vector<VertexWithAttributes> >(GV_VERTLIST);
      edges_ = context->GlobalVariable_GetValue<boost::unordered_set<EdgePair, EdgePairHash> >(GV_EDGELIST);

      std::vector<topology4::VertexTypeMeta> vertextypemeta_ = topologyMeta_->vertextypemeta_;
      std::vector<topology4::EdgeTypeMeta> edgetypemeta_ = topologyMeta_->edgetypemeta_;

      vert_count_ = context->GlobalVariable_GetValue<u_int32_t>(GV_VERT_COUNT);
      writer_->WriteStartObject();
      writer_->WriteName("neighborhood_size").WriteUnsignedInt(vert_count_);

      boost::unordered_set<VertexLocalId_t> vertices;

      writer_->WriteName("vertices");
      writer_->WriteStartArray();

      for (std::vector<VertexWithAttributes>::iterator it = vids_.begin(); it != vids_.end(); ++it) {
        writer_->WriteStartObject()
                .WriteName("id").WriteMarkVId(it->vid)
                //.WriteName("type").WriteUnsignedInt(it->vtype)
                .WriteName("type").WriteString(vertextypemeta_[it->vtype].typename_)
                .WriteName("attr").WriteRaw(it->attr.c_str(), it->attr.size())
                .WriteEndObject();
        vertices.insert(it->vid);
        vidsToTranslate_.push_back(it->vid);
      }
      writer_->WriteEndArray();

      writer_->WriteName("edges");
      writer_->WriteStartArray();

      for (boost::unordered_set<EdgePair, EdgePairHash>::iterator it = edges_.begin(); it != edges_.end(); ++it) {
        if (vertices.find(it->src) == vertices.end() || vertices.find(it->tgt) == vertices.end()) {
          continue;
        }
        writer_->WriteStartObject()
                .WriteName("src").WriteStartObject()
                  .WriteName("id").WriteMarkVId(it->src)
                  //.WriteName("type").WriteUnsignedInt(it->srctype)
                  .WriteName("type").WriteString(vertextypemeta_[it->srctype].typename_)
                  .WriteEndObject()
                .WriteName("tgt").WriteStartObject()
                  .WriteName("id").WriteMarkVId(it->tgt)
                  //.WriteName("type").WriteUnsignedInt(it->tgttype)
                  .WriteName("type").WriteString(vertextypemeta_[it->tgttype].typename_)
                  .WriteEndObject()
                //.WriteName("type").WriteUnsignedInt(it->type)
                  .WriteName("type").WriteString(edgetypemeta_[it->type].typename_)
                .WriteName("attr").WriteRaw(it->attr.c_str(), it->attr.size())
                .WriteEndObject();
      }
      writer_->WriteEndArray();

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
      return vidsToTranslate_;
    }

    boost::unordered_set<EdgePair, EdgePairHash>& getEdgesToReturn() {
      return edges_;
    }

  private:
    // root of k-neighborhood
    VertexLocalId_t start_node_;
    // maximum number of vertices to store
    uint32_t max_vertices_;
    // this writer is a pointer back to the Service's writer object.
    gutil::JSONWriter* writer_;
    // list of found vertices.
    std::vector<VertexWithAttributes> vids_;
    std::vector<VertexLocalId_t> vidsToTranslate_;
    // list of found edges.
    boost::unordered_set<EdgePair, EdgePairHash> edges_;
    // count of vertices found.
    u_int32_t vert_count_;
    // TopologyMeta
    topology4::TopologyMeta* topologyMeta_;
  };
}  // namespace VISUALIZATION

#endif /* GPERUN_KETPNEIGHBORHOODSUBGRAPH_HPP_ */
