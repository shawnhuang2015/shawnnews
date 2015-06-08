/***************************************************************
*  fraud_score.hpp
*    User defined function that computes score and retrun all subgraph for all
*    fraud nodes
*
*    Created by Alan Lee on 5/18/15.
*
*    Modifed by Shawn Huang on 5/20/15.
*
*    Copyright (c) 2013 GraphSQL. All rights reserved.
****************************************************************/

// put additional includes that you require here.
#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "../base/global_set.hpp"

namespace lianlian_score_ns {

  // flag for non-transaction type.
  const uint16_t N_FLAG = 7;
  const uint16_t FLAG_MAP[] = {0, 1, 2, 4, 8, 16, 32};
  const uint16_t ALL_FLAG = 63;

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
    uint16_t flags;

    value_t(uint16_t f = 0)
      : flags(f) {
      }

    value_t(const value_t& other)
      : flags(other.flags) {}
  };

  struct message_t {
    uint16_t flags;

    message_t()
      : flags(0) {}

    message_t(uint16_t f)
      : flags(f) {}

    message_t& operator+=(const message_t& other) {
      flags |= other.flags;
      return *this;
    }
  };

  // UDF definition
  class FraudScoreUDF : public gpelib4::BaseUDF {
    private:
      const VertexLocalId_t source_vid_;
      size_t score_;

      gutil::JSONWriter* writer_;

    public:
      enum {GV_SCORE};

    public:
      // YeepaySubGraphExtractUDF Settings: Computation Modes
      static const gpelib4::EngineProcessingMode EngineMode = gpelib4::EngineProcessMode_ActiveVertices; // gpelib4::EngineProcessMode_AllVertices or gpelib4::EngineProcessMode_ActiveVertices
      static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue; // set vertices to gpelib4::Mode_SingleValue, gpelib4::Mode_MultipleValue or gpelib4::Mode_SingleMultipleValue
      static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally; // options are gpelib4::Mode_Initialize_Globally gpelib4::Mode_Initialize_Individually and gpelib4::Mode_Initialize_FromBucket
      static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined. If Not Defined, then multi-value messages are used.
      static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined. Adds a hadoop-style combine step that can reduce message loads.
      static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_NotDefined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined.
      static const int PrintMode_ = gpelib4::Mode_Print_ByVertex; // options are gpelib4::Mode_Print_NodDefined, gpelib4::Mode_Print_ByVertex and gpelib4::Mode_Print_ByEdge. The latter two can be combined with a logical '|'

      typedef topology4::VertexAttribute V_ATTR;
      typedef topology4::EdgeAttribute E_ATTR;
      typedef value_t V_VALUE;
      typedef message_t MESSAGE;

      FraudScoreUDF(int iteration_limit, VertexLocalId_t source, gutil::JSONWriter* writer = NULL)
        : gpelib4::BaseUDF(EngineMode, iteration_limit), source_vid_(source),
          score_(0), writer_(writer) {
      }

      ~FraudScoreUDF() {}

      void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
        globalvariables->Register(GV_SCORE, new gpelib4::FloatSumVariable(0));
      }

      inline void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context) {
        context->WriteAll(value_t(), false);
        context->Write(source_vid_, value_t(ALL_FLAG));
      }

      void StartRun(gpelib4::MasterContext* context) {
        context->set_udfedgemapsetting(gpelib4::UDFEdgeMapSetting_RequireSourceVertexAttribute);
      }

      void BeforeIteration(gpelib4::MasterContext* context) {

      }

      inline void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                          const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                          E_ATTR* edgeattr, gpelib4::SingleValueMapContext<MESSAGE> * context) {
        if (context->Iteration() == 1) {
          if (srcvertexattr->type() == T_TXN) {
//            context->Write(targetvid, MESSAGE(FLAG_MAP[targetvertexattr->type()]));
            context->Write(targetvid, MESSAGE(64));  // magic number.
          } else {
            context->Write(targetvid, MESSAGE(FLAG_MAP[srcvertexattr->type()]));
          }
        } else {
          if (srcvertexattr->type() == T_TXN && srcvertexattr->GetBool(A_ISFRAUD, false)) {
            // It is Black Transaction. And Stop.
          }
          else {
            context->Write(targetvid, MESSAGE(srcvertexvalue.flags));
          }
        }
      }

//      inline void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
//          const V_VALUE& singlevalue, gpelib4::SingleValueMapContext<MESSAGE> * context) {
//      }

      void BetweenMapAndReduce(gpelib4::MasterContext* context) {

      }

      inline void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
                         const V_VALUE& vertexvalue,
                         const MESSAGE& accumulator,
                         gpelib4::SingleValueContext<V_VALUE>* context) {
        uint16_t accumulator_flags = accumulator.flags;
        if (accumulator_flags >= 64) {  // magic number.
          accumulator_flags = (accumulator_flags - 64) | FLAG_MAP[vertexattr->type()];
        }

        V_VALUE val(vertexvalue);
        val.flags |= accumulator_flags;
        uint16_t flag_diff = ~vertexvalue.flags & val.flags;
        bool is_fraud = vertexattr->GetBool(A_ISFRAUD, false);

        if (vertexattr->type() == T_TXN && is_fraud) {
          uint16_t hop = (context->Iteration() + 1) / 2;
          for (uint16_t i = 1; i < N_FLAG; ++i) {
            if (FLAG_MAP[i] & flag_diff) {
              context->GlobalVariable_Reduce(GV_SCORE, WEIGHT_MAP[i][hop]);
            }
          }
        }

        if (flag_diff) {
          context->Write(vid, val);
        }
      }

      void AfterIteration(gpelib4::MasterContext* context) {
      }

      void EndRun(gpelib4::BasicContext* context) {
        score_ = (size_t)context->GlobalVariable_GetValue<float>(GV_SCORE);
      }

      inline void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
          V_ATTR* vertexattr, const V_VALUE& singlevalue,
          gpelib4::BasicContext* context) {}

      std::string Output(gpelib4::BasicContext* context) {
        return "";
      }

    public:
      size_t get_score() const {
        return score_;
      }
  };
}
