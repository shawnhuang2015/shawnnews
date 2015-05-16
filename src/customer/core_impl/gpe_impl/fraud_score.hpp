// put additional includes that you require here.
#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "../base/global_vector.hpp"

namespace lianlian_ns {

  enum {F_USERID = 1,
        F_SSN = 2,
        F_BANKID = 4,
        F_CELL = 8,
        F_IMEI = 16,
        F_IP = 32,
        F_ALL = 63
      };

  struct value_t {
    size_t flags;
    std::vector<VertexLocalId_t> parents;

    value_t(size_t f = 0)
      : flags(f), parents() {}
  };

  struct message_t {
    size_t flags;
    VertexLocalId_t parent;

    message_t()
      : flags(0), parent(0) {}

    message_t(size_t f, VertexLocalId_t p)
      : flags(f), parent(p) {}
  };

  typedef std::pair<VertexLocalId_t, VertexLocalId_t> edge_t;

  // UDF definition
  class FraudScoreUDF : public gpelib4::BaseUDF {
    private:
      const VertexLocalId_t source_vid_;
      boost::unordered_set<VertexLocalId_t> vertices_;
      boost::unordered_set<edge_t> edges_;
      bool is_backtracking_;
      gutil::JSONWriter* writer_;

    public:
      // YeepaySubGraphExtractUDF Settings: Computation Modes
      static const gpelib4::EngineProcessingMode EngineMode = gpelib4::EngineProcessMode_ActiveVertices; // gpelib4::EngineProcessMode_AllVertices or gpelib4::EngineProcessMode_ActiveVertices
      static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue; // set vertices to gpelib4::Mode_SingleValue, gpelib4::Mode_MultipleValue or gpelib4::Mode_SingleMultipleValue
      static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally; // options are gpelib4::Mode_Initialize_Globally gpelib4::Mode_Initialize_Individually and gpelib4::Mode_Initialize_FromBucket
      static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_NotDefined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined. If Not Defined, then multi-value messages are used.
      static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined. Adds a hadoop-style combine step that can reduce message loads.
      static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined.
      static const int PrintMode_ = gpelib4::Mode_Print_ByVertex; // options are gpelib4::Mode_Print_NodDefined, gpelib4::Mode_Print_ByVertex and gpelib4::Mode_Print_ByEdge. The latter two can be combined with a logical '|'

      typedef topology4::VertexAttribute V_ATTR;
      typedef topology4::EdgeAttribute E_ATTR;
      typedef value_t V_VALUE;
      typedef message_t MESSAGE;

      FraudScoreUDF(int iteration_limit, VertexLocalId_t source, gutil::JSONWriter* writer = NULL)
        : gpelib4::BaseUDF(EngineMode, iteration_limit), source_vid_(source), vertices_(),
          edges_(), is_backtracking_(false), writer_(writer) {
      }

      ~FraudScoreUDF() {}

      void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      }

      inline void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context) {
        context->WriteAll(value_t(), false);
        context->Write(source_vid_, value_t(F_ALL));
        is_backtracking_ = false;
      }

      void StartRun(gpelib4::MasterContext* context) {
      }

      void BeforeIteration(gpelib4::MasterContext* context) {}

      inline void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                          const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                          E_ATTR* edgeattr, gpelib4::SingleValueMapContext<MESSAGE> * context) {
      }

      inline void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
          const V_VALUE& singlevalue, gpelib4::SingleValueMapContext<MESSAGE> * context) {
      }

      void BetweenMapAndReduce(gpelib4::MasterContext* context) {}

      inline void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
                         const V_VALUE& vertexvalue,
                         const MESSAGE& accumulator,
                         gpelib4::SingleValueContext<V_VALUE>* context) {
        context->Write(vid, accumulator.value);
      }

      void AfterIteration(gpelib4::MasterContext* context) {
      }

      void EndRun(gpelib4::BasicContext* context) {
        if (writer_ == NULL) {
          std::cerr << "m_writer_ is NULL" << std::endl;
          return;
        }
        writer_->WriteStartObject();

        // fill vertices info into json.
        writer_->WriteName("vertices");
        writer_->WriteStartArray();

        const std::vector<path_t>& path = context->GlobalVariable_GetValue<std::vector<path_t> >(GV_PATHS);
        session_map_t group_by_session(HASHSIZE_FACTOR * path.size());
        for (std::vector<path_t>::const_iterator cit = path.begin(); cit != path.end(); ++cit) {
          std::pair<session_map_t::iterator, bool> ret = group_by_session.insert(
            std::make_pair(cit->session_vid, 1));
          session_map_t::iterator it = ret.first;
          it->second.insert(cit->ip_vid);
          vids.insert(cit->session_vid);

          std::pair<boost::unordered_set<VertexLocalId_t>::iterator, bool> ret1 = vids.insert(cit->ip_vid);
          if (ret1.second) {
            writer_->WriteStartObject();
            writer_->WriteName("id");
            writer_->WriteMarkVId(cit->ip_vid);
            writer_->WriteName("type");
            writer_->WriteUnsignedInt<unsigned>(0);
            writer_->WriteName("attr");
            writer_->WriteStartObject();
            writer_->WriteEndObject();
            writer_->WriteEndObject();
          }
        }

        // end filling vertices.
        writer_->WriteEndArray();

        size_t vid_count = vids.size();
        // just a rough estimate of the bucket size.
        vertex_map_t group_by_vertex(HASHSIZE_FACTOR * vid_count * (vid_count - 1) / 2);
        for (session_map_t::const_iterator cit = group_by_session.begin();
             cit != group_by_session.end(); ++cit) {
          two_way_connect(cit->first, cit->second, group_by_vertex);
        }

        // fill edges info into json.
        writer_->WriteName("edges");
        writer_->WriteStartArray();

        for (vertex_map_t::const_iterator cit = group_by_vertex.begin();
             cit != group_by_vertex.end(); ++cit) {
          writer_->WriteStartObject();

          writer_->WriteName("type");
          writer_->WriteUnsignedInt<unsigned>(0);

          writer_->WriteName("src");
          writer_->WriteStartObject();
          writer_->WriteName("id");
          writer_->WriteMarkVId(cit->first.first);
          writer_->WriteName("type");
          writer_->WriteUnsignedInt<unsigned>(0);
          writer_->WriteEndObject();

          writer_->WriteName("tgt");
          writer_->WriteStartObject();
          writer_->WriteName("id");
          writer_->WriteMarkVId(cit->first.second);
          writer_->WriteName("type");
          writer_->WriteUnsignedInt<unsigned>(0);
          writer_->WriteEndObject();

          writer_->WriteName("attr");
          writer_->WriteStartObject();
          writer_->WriteName("sids");
          writer_->WriteStartArray();
          for (boost::unordered_set<VertexLocalId_t>::const_iterator cit1 = cit->second.begin();
               cit1 != cit->second.end(); ++cit1) {
            writer_->WriteMarkVId(*cit1);
          }
          writer_->WriteEndArray();
          writer_->WriteEndObject();

          writer_->WriteEndObject();
        }

        // end filling edges.
        writer_->WriteEndArray();

        // end writing json.
        writer_->WriteEndObject();
      }

      inline void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
          V_ATTR* vertexattr, const V_VALUE& singlevalue,
          gpelib4::BasicContext* context) {}

      std::string Output(gpelib4::BasicContext* context) {
        return "";
      }

    private:
      void two_way_connect(VertexLocalId_t session_id, 
                           const boost::unordered_set<VertexLocalId_t>& vertices,
                           vertex_map_t& group_by_vertex) {
        for (boost::unordered_set<VertexLocalId_t>::const_iterator cit = vertices.begin();
             cit != vertices.end(); ++cit) {
          boost::unordered_set<VertexLocalId_t>::const_iterator cit_a = cit;
          boost::unordered_set<VertexLocalId_t>::const_iterator cit_b = boost::next(cit_a);
          for (; cit_b != vertices.end(); ++cit_b) {
            std::pair<vertex_map_t::iterator, bool> ret = group_by_vertex.insert(
              std::make_pair(std::make_pair(*cit_a, *cit_b), 1));
            vertex_map_t::iterator it = ret.first;
            it->second.insert(session_id);
          }
        }
      }
  };
}
