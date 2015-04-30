/***************************************************************
* sp.hpp
* A simple udf of Shortest Path.
*
* Created by Alan Lee on 4/30/15.
* Copyright (c) 2013 GraphSQL. All rights reserved.
****************************************************************/
/**************INCLUDES*****************************************/
// put additional includes that you require here.
#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>

namespace gperun{
const unsigned long INF = INT_MAX - 1;

// Value type.
struct sp_value_t {
    unsigned long da, da_n;
    unsigned long db, db_n;

    sp_value_t(unsigned long a = INF, unsigned long a_n = 0,
               unsigned long b = INF, unsigned long b_n = 0)
        : da(a), da_n(a_n), db(b), db_n(b_n) {}
};

// Message type with an aggregator.
struct sp_message_t {
    sp_value_t value;

    sp_message_t() : value() {}

    sp_message_t(const sp_value_t& other)
        : value(other.da, other.da_n, other.db, other.db_n) {}

    sp_message_t& operator+=(const sp_message_t& other) {
        if (value.da > other.value.da) {
            value.da = other.value.da;
            value.da_n = other.value.da_n;
        } else if (value.da == other.value.da && value.da < INF) {
            value.da_n += other.value.da_n;
        }

        if (value.db > other.value.db) {
            value.db = other.value.db;
            value.db_n = other.value.db_n;
        } else if (value.db == other.value.db && value.db < INF) {
            value.db_n += other.value.db_n;
        }
        return *this;
    }
};


// enum of attributes index.

class SPUDF : public gpelib4::BaseUDF {
private:
enum {GV_FLAG, GV_SHORTEST_EVEN, GV_COUNT_EVEN, GV_SHORTEST_ODD, GV_COUNT_ODD};

VertexLocalId_t m_vertex_a_;
VertexLocalId_t m_vertex_b_;

public:
// UDF Settings: Computation Modes
static const gpelib4::EngineProcessingMode EngineMode = gpelib4::EngineProcessMode_ActiveVertices; // gpelib4::EngineProcessMode_AllVertices or gpelib4::EngineProcessMode_ActiveVertices
static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue; // set vertices to gpelib4::Mode_SingleValue, gpelib4::Mode_MultipleValue or gpelib4::Mode_SingleMultipleValue
static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally; // options are gpelib4::Mode_Initialize_Globally gpelib4::Mode_Initialize_Individually and gpelib4::Mode_Initialize_FromBucket
static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined. If Not Defined, then multi-value messages are used.
static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined. Adds a hadoop-style combine step that can reduce message loads.
static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined; // gpelib4::Mode_Defined or gpelib4::Mode_NotDefined.
static const int PrintMode_ = gpelib4::Mode_Print_ByVertex; // options are gpelib4::Mode_Print_NodDefined, gpelib4::Mode_Print_ByVertex and gpelib4::Mode_Print_ByEdge. The latter two can be combined with a logical '|'

// These typedefs are required by the engine.
typedef topology4::VertexAttribute V_ATTR;
typedef topology4::EdgeAttribute E_ATTR;
typedef sp_value_t V_VALUE;
typedef sp_message_t MESSAGE;

/**************Class Constructor************/
SPUDF(int iteration_limit, VertexLocalId_t v_a = 0, VertexLocalId_t v_b = 1)
    : gpelib4::BaseUDF(EngineMode, iteration_limit), m_vertex_a_(v_a), m_vertex_b_(v_b) {}

/**************Class Destructor************/
~SPUDF() {}

/**************Initialize Globals************/
void Initialize_GlobalVariables(
gpelib4::GlobalVariables* globalvariables) { 
    globalvariables->Register(GV_FLAG, new gpelib4::BoolVariable(false));
    globalvariables->Register(GV_SHORTEST_EVEN, new gpelib4::LongStateVariable(INF));
    globalvariables->Register(GV_COUNT_EVEN, new gpelib4::LongSumVariable(0));
    globalvariables->Register(GV_SHORTEST_ODD, new gpelib4::LongStateVariable(INF));
    globalvariables->Register(GV_COUNT_ODD, new gpelib4::LongSumVariable(0));
}

/**************
* Assign all vertices the same initial value, meanwhile set them all active.
*
************/
inline void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context) {
    context->WriteAll(sp_value_t(), false);
    context->Write(m_vertex_a_, sp_value_t(0, 1));
    context->Write(m_vertex_b_, sp_value_t(INF, 0, 0, 1));
}

/**
* StartRun is only called before the first iteration begins (before the
* first BeginIteration call).
* It is not required to be defined.
*
* @param context Can be used to set active flags for vertices.
*/
void StartRun(gpelib4::MasterContext* context) {
    if (m_vertex_a_ == m_vertex_b_) {
        reinterpret_cast<gpelib4::BoolVariable*>(context->GetGlobalVariable(GV_FLAG))->Set(true);
        reinterpret_cast<gpelib4::LongStateVariable*>(context->GetGlobalVariable(GV_SHORTEST_EVEN))->Set(0);
        reinterpret_cast<gpelib4::LongSumVariable*>(context->GetGlobalVariable(GV_COUNT_EVEN))->Set(1);
        reinterpret_cast<gpelib4::LongStateVariable*>(context->GetGlobalVariable(GV_SHORTEST_ODD))->Set(0);
        reinterpret_cast<gpelib4::LongSumVariable*>(context->GetGlobalVariable(GV_COUNT_ODD))->Set(1);
    }
}

/**
* BeforeIteration is called before each iteration begins.
* It is not required to be defined.
*
* @param context Can be used to set active flags for vertices.
*/
void BeforeIteration(gpelib4::MasterContext* context) { 
}

/**************
* So EdgeMap always needs an impl, otherwise messages couldn't be passed around?
* Here, it just send a share of rank value of srcvid to targetvid via context->Write().
*
************/
inline void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                    const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                    E_ATTR* edgeattr, gpelib4::SingleValueMapContext<MESSAGE> * context) {
    context->Write(targetvid, MESSAGE(sp_value_t(srcvertexvalue.da + 1, srcvertexvalue.da_n,
                                                 srcvertexvalue.db + 1, srcvertexvalue.db_n)));
}

/**************
* VertexMap does nothing but check if current vertex is a sink, add its value to SINK_RANK if so.
*
************/
inline void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
                      const V_VALUE& singlevalue, gpelib4::SingleValueMapContext<MESSAGE> * context) {
//    fprintf(stderr, "@VM, vid: %u, (%lu, %lu, %lu, %lu)\n",
//            vid, singlevalue.da, singlevalue.da_n, singlevalue.db, singlevalue.db_n);
}

/**
* BetweenMapAndReduce is called between the map and reduce phases
* of each iteration.
* It is not required to be defined.
*
* @param context Can be used to set active flags for vertices.
*/
void BetweenMapAndReduce(gpelib4::MasterContext* context) { 
}

/**************
*
* What do we use it for?
* CombineReduce Function (depends on settings)
*
*
************/

/**************
* Calculate new_rank for current vertex, also check and set CHANGED if necessary.
*
************/
inline void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr, 
                   const V_VALUE& vertexvalue,
                   const MESSAGE& accumulator,
                   gpelib4::SingleValueContext<V_VALUE>* context) {
    V_VALUE new_value(vertexvalue.da, vertexvalue.da_n, vertexvalue.db, vertexvalue.db_n);
    bool changed = false;

    if (accumulator.value.da < vertexvalue.da) {
        changed = true;
        new_value.da = accumulator.value.da;
        new_value.da_n = accumulator.value.da_n;
    } else if (accumulator.value.da == vertexvalue.da && accumulator.value.da < INF) {
        changed = true;
        new_value.da_n = accumulator.value.da_n + vertexvalue.da_n;
    }

    if (accumulator.value.db < vertexvalue.db) {
        changed = true;
        new_value.db = accumulator.value.db;
        new_value.db_n = accumulator.value.db_n;
    } else if (accumulator.value.db == vertexvalue.db && accumulator.value.db < INF) {
        changed = true;
        new_value.db_n = accumulator.value.db_n + vertexvalue.db_n;
    }

    if (changed && new_value.da < INF && new_value.db < INF) {
        reinterpret_cast<gpelib4::BoolVariable*>(context->GetGlobalVariable(GV_FLAG))->Set(true);
        if (new_value.da <= new_value.db) {
            unsigned long shortest = new_value.da + new_value.db;
            unsigned long count = new_value.da_n * new_value.db_n;

            if (shortest % 2) {
                unsigned long gv_shortest = context->GlobalVariable_GetValue<unsigned long>(GV_SHORTEST_ODD);
                unsigned long gv_count = context->GlobalVariable_GetValue<unsigned long>(GV_COUNT_ODD);

                if (shortest < gv_shortest) {
                    gv_shortest = shortest;
                    gv_count = count;
                } else if (shortest == gv_shortest) {
                    gv_count += count;
                }

                reinterpret_cast<gpelib4::LongStateVariable*>(context->GetGlobalVariable(GV_SHORTEST_ODD))->Set(gv_shortest);
                reinterpret_cast<gpelib4::LongSumVariable*>(context->GetGlobalVariable(GV_COUNT_ODD))->Set(gv_count);
            } else {
                unsigned long gv_shortest = context->GlobalVariable_GetValue<unsigned long>(GV_SHORTEST_EVEN);
                unsigned long gv_count = context->GlobalVariable_GetValue<unsigned long>(GV_COUNT_EVEN);

                if (shortest < gv_shortest) {
                    gv_shortest = shortest;
                    gv_count = count;
                } else if (shortest == gv_shortest) {
                    gv_count += count;
                }

                reinterpret_cast<gpelib4::LongStateVariable*>(context->GetGlobalVariable(GV_SHORTEST_EVEN))->Set(gv_shortest);
                reinterpret_cast<gpelib4::LongSumVariable*>(context->GetGlobalVariable(GV_COUNT_EVEN))->Set(gv_count);
            }
        }
    }

    if (changed) {
        context->Write(vid, new_value);
    }
}

/**
* AfterIteration is called after each iteration.
* It is not required to be defined.
*
* @param context Can be used to set active flags for vertices.
*/
void AfterIteration(gpelib4::MasterContext* context) { 
    bool flag = context->GlobalVariable_GetValue<bool>(GV_FLAG);
    if (flag) {
        context->Stop();
    }
}

/**
* EndRun is called after the final iteration ends (after the
* last AfterIteration call).
* It is not required to be defined.
*
* @param context Can be used to set active flags for vertices.
*/
void EndRun(gpelib4::BasicContext* context) {
    unsigned long shortest_odd = context->GlobalVariable_GetValue<unsigned long>(GV_SHORTEST_ODD);
    unsigned long count_odd = context->GlobalVariable_GetValue<unsigned long>(GV_COUNT_ODD);
    unsigned long shortest = context->GlobalVariable_GetValue<unsigned long>(GV_SHORTEST_EVEN);
    unsigned long count = context->GlobalVariable_GetValue<unsigned long>(GV_COUNT_EVEN);

    if (shortest_odd < shortest) {
        shortest = shortest_odd;
        count = count_odd;
    }

    fprintf(stderr, "%u <-> %u, shortest: %lu, count: %lu\n",
            m_vertex_a_,
            m_vertex_b_,
            shortest,
            count);
            
}

/**************
* Output some info of each vertex.
*
************/
inline void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
                  V_ATTR* vertexattr, const V_VALUE& singlevalue,
                  gpelib4::BasicContext* context) {
    ostream << "vid: " << vid << ", "
            << "da: " << singlevalue.da << ", "
            << "da_count: " << singlevalue.da_n << ", "
            << "db: " << singlevalue.db << ", "
            << "db_count: " << singlevalue.db_n << std::endl;
}

std::string Output(gpelib4::BasicContext* context) {
    return "";
}

};
}
