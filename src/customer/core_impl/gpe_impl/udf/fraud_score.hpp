// put additional includes that you require here.
#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "../base/global_set.hpp"

namespace lianlian_ns {

  // flag for non-transaction type.
  const size_t N_FLAG = 7;
  const size_t FLAG_MAP[] = {0, 1, 2, 4, 8, 16, 32};
  const size_t ALL_FLAG = 63;

  size_t check_FLAG = 63;

  // weights for each intermediate nodes on hop 1/2/3.
  const float WEIGHT_MAP[][4] = {
    // transaction nodes need no weights, cuz they're not considered in score calculation.
    {0},
    // T_USERID.
    {0, 1000, 500, 100},
    // T_SSN.
    {0, 1000, 500, 100},
    // T_BANKID.
    {0, 1000, 500, 100},
    // T_CELL.
    {0, 1000, 500, 100},
    // T_IMEI.
    {0, 1000, 500, 100},
    // T_IP
    {0, 100, 10, 1},
  };

  // type id corresponding to the graph_config.yaml.
  enum {
    T_TXN = 0,
    T_USERID = 1,
    T_SSN = 2,
    T_BANKID = 3,
    T_CELL = 4,
    T_IMEI = 5,
    T_IP = 6
  };

  // attribute index
  enum {
    A_ISFRAUD = 0
  };

  struct value_t {
    size_t flags;
    boost::unordered_set<VertexLocalId_t> parents;

    value_t(size_t f = 0)
      : flags(f), parents() {}

    value_t(const value_t& other)
      : flags(other.flags), parents(other.parents) {}
  };

  struct message_t {
    size_t flags;
    VertexLocalId_t parent;

    message_t()
      : flags(0), parent(0) {}

    message_t(size_t f, VertexLocalId_t p)
      : flags(f), parent(p) {}

    message_t& operator+=(const message_t& other) {
      return *this;
    }
  };

  struct edge_t {
    VertexLocalId_t src_vid;
    VertexLocalId_t tgt_vid;

    edge_t(VertexLocalId_t sid = 0, VertexLocalId_t tid = 0)
      : src_vid(sid), tgt_vid(tid) {
        if (src_vid > tgt_vid) {
          std::swap(src_vid, tgt_vid);
        }
    }

    bool operator==(const edge_t& rhs) {
      return src_vid == rhs.src_vid && tgt_vid == rhs.tgt_vid;
    }

    friend std::istream& operator>>(std::istream& is, edge_t& e) {
      is >> e.src_vid >> e.tgt_vid;
      return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const edge_t& e) {
      os << e.src_vid << e.tgt_vid;
      return os;
    }

    friend size_t hash_value(const edge_t& e) {
      size_t seed = 0;
      boost::hash_combine(seed, e.src_vid);
      boost::hash_combine(seed, e.tgt_vid);
      return seed;
    }
  };
  
  // UDF definition
  class FraudScoreUDF : public gpelib4::BaseUDF {
    private:
      const VertexLocalId_t source_vid_;
      boost::unordered_set<VertexLocalId_t> vertices_;
      boost::unordered_set<edge_t> edges_;
      bool is_backtracking_;
      float score_;

      gutil::JSONWriter* writer_;

    public:
      enum {GV_FRAUD_TXN, GV_VERTICES, GV_EDGES, GV_SCORE};

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
          edges_(), is_backtracking_(false), score_(0), writer_(writer) {
        printf("ctor: limit = %d, start = %u\n", iteration_limit, source);
      }

      ~FraudScoreUDF() {}

      void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
        globalvariables->Register(GV_FRAUD_TXN, new UDIMPL::SetVariable<VertexLocalId_t>());
        globalvariables->Register(GV_VERTICES, new UDIMPL::SetVariable<VertexLocalId_t>());
        globalvariables->Register(GV_EDGES, new UDIMPL::SetVariable<edge_t>());
        globalvariables->Register(GV_SCORE, new gpelib4::FloatSumVariable(0));
      }

      inline void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context) {
        context->WriteAll(value_t(), false);
        context->Write(source_vid_, value_t(ALL_FLAG));
        is_backtracking_ = false;
      }

      void StartRun(gpelib4::MasterContext* context) {
      }

      void BeforeIteration(gpelib4::MasterContext* context) {}

      inline void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                          const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                          E_ATTR* edgeattr, gpelib4::MultipleValueMapContext<MESSAGE> * context) {
        if (! is_backtracking_) {
          if (context->Iteration() == 1) {
            if (srcvertexattr->type() == T_TXN) {
              context->Write(targetvid, MESSAGE(FLAG_MAP[targetvertexattr->type()], srcvid));
            } else {
              context->Write(targetvid, MESSAGE(FLAG_MAP[srcvertexattr->type()], srcvid));
            }
          } else {
            if (srcvertexattr->type() != T_TXN || ! srcvertexattr->GetBool(A_ISFRAUD, false)) {
              context->Write(targetvid, MESSAGE(srcvertexvalue.flags, srcvid));
            }
          }
        }
      }

      inline void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
          const V_VALUE& singlevalue, gpelib4::MultipleValueMapContext<MESSAGE> * context) {
        if (is_backtracking_) {
          context->GlobalVariable_Reduce(GV_VERTICES, vid);
          std::cout << "ID " << vid << " : ";
          for (boost::unordered_set<VertexLocalId_t>::const_iterator cit = singlevalue.parents.begin();
               cit != singlevalue.parents.end(); ++cit) {
            // add edges & nodes
            context->GlobalVariable_Reduce(GV_VERTICES, *cit);
            context->GlobalVariable_Reduce(GV_EDGES, edge_t(vid, *cit));
            context->SetActiveFlag(*cit);

            std::cout << *cit << " ";
          }
          std::cout << std::endl;
        }
      }

      void BetweenMapAndReduce(gpelib4::MasterContext* context) {}

      inline void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
                         const V_VALUE& vertexvalue,
                         const gutil::Const_Iterator<MESSAGE>& msgvaluebegin,
                         const gutil::Const_Iterator<MESSAGE>& msgvalueend,
                         gpelib4::SingleValueContext<V_VALUE>* context) {
        if (! is_backtracking_) {
          bool is_fraud = vertexattr->GetBool(A_ISFRAUD, false);
          if (vertexattr->type() == T_TXN && is_fraud) {
            // update gv_fraud_vid list.
            context->GlobalVariable_Reduce(GV_FRAUD_TXN, vid);
          }

          // copy the value.
          std::cout << "Parents " << vid << " : ";

          // TODO: copy the value and see if current vertex needs updating.
          // only if flags is updated, we need current vertex to be active in next iteration,
          // otherwise, just update its parents.

          V_VALUE val(vertexvalue);
          size_t temp_flag = val.flags;

          for (gutil::Const_Iterator<MESSAGE> it = msgvaluebegin;
               it != msgvalueend; ++it) {
            /*
<<<<<<< HEAD

            if (temp_flag ^ it->flags && temp_flag != ALL_FLAG) {
              val.parents.insert(it->parent);
              std::cout << " " << it->parent; 
            }
            else {

            }

            
            if (val.flags == it->flags) {
              continue;
            }

=======
*/
            if (val.flags & it->flags) {
              continue;
            }
            val.parents.insert(it->parent);
/*>>>>>>> origin/poc4_lianlian_fraud */
            val.flags |= it->flags;
          }

          std::cout << std::endl;
          if (vertexattr->type() == T_TXN && is_fraud) {
            // adding 1 is actually for the case where start vid is non-transaction,
            // but it doesn't hurt the case with start vid being transaction.
            size_t hop = (context->Iteration() + 1) / 2;
            size_t flag_diff = val.flags & (~ vertexvalue.flags);
            for (size_t i = 0; i < N_FLAG; ++i) {
              if (FLAG_MAP[i] & flag_diff) {
                context->GlobalVariable_Reduce(GV_SCORE, WEIGHT_MAP[i][hop]);
                std::cout << "SCORE : " <<  WEIGHT_MAP[i][hop] << std::endl;
              }
            }
          }
          // TODO: need to write value every time?
          // if flags and parents not updated, no need to write value.
          context->Write(vid, val);
        } else {
            context->SetActiveFlag(vid);
        }
        else {
          context->SetActiveFlag(vid);
          std::cout << "Reduce " << vid << std::endl;
        }
      }

      void AfterIteration(gpelib4::MasterContext* context) {
        if (is_backtracking_ == false && (
            context->Iteration() == 6 ||
            context->GetActiveVertexCount() == 0)) {
          context->SetAllActiveFlag(false);
          // set all fraud vertices active, start backtracking.
          const boost::unordered_set<VertexLocalId_t>& fraud_txn = 
            context->GlobalVariable_GetValue<boost::unordered_set<VertexLocalId_t> >(GV_FRAUD_TXN);
          for (boost::unordered_set<VertexLocalId_t>::const_iterator cit = fraud_txn.begin();
               cit != fraud_txn.end(); ++ cit) {
            context->SetActiveFlag(*cit);
          }
          is_backtracking_ = true;
        }
      }

      void EndRun(gpelib4::BasicContext* context) {
        score_ = context->GlobalVariable_GetValue<float>(GV_SCORE);
        vertices_ = context->GlobalVariable_GetValue<boost::unordered_set<VertexLocalId_t> >(GV_VERTICES);
        edges_ = context->GlobalVariable_GetValue<boost::unordered_set<edge_t> >(GV_EDGES);

        // just make sure the source vertex is returned in json result.
        vertices_.insert(source_vid_);
      }

      inline void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
          V_ATTR* vertexattr, const V_VALUE& singlevalue,
          gpelib4::BasicContext* context) {}

      std::string Output(gpelib4::BasicContext* context) {
        return "";
      }

    public:
      float get_score() const {
        return score_;
      }

      const boost::unordered_set<VertexLocalId_t>& get_vertices() const {
        return vertices_;
      }

      const boost::unordered_set<edge_t>& get_edges() const {
        return edges_;
      }
  };
}
