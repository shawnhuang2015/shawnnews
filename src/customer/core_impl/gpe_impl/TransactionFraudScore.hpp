#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>
#include <gpelib4/udfs/singleactiveudfbase.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>
#include "base/global_set.hpp"
#include "base/global_vector.hpp"
#include "core_impl/udt.hpp"

namespace gperun{
    const int FLAG_USER_ID = 1;
    const int FLAG_SSN     = 2;
    const int FLAG_BANK_ID = 4;
    const int FLAG_CELL    = 8;
    const int FLAG_IMEI    = 16;
    const int FLAG_IP      = 32;
    const int TOTAL_FLAGS  = 6;
    const int MAX_ITERATION = 6;
    
    struct FlagMessage {
        int flags;

        FlagMessage(int x = 0): flags(x) {
        }

        bool cover(const FlagMessage & message) const {
            return (flags & message.flags) == message.flags;
        }

        FlagMessage& operator+= (const FlagMessage & message) {
            flags |= message.flags;
            return *this;
        }
    };

  struct Vertex {
    uint64_t vid;
    uint32_t type;
    bool isFraud;
    Vertex(uint64_t v = 0): vid(v), type(0), isFraud(false) {}
    Vertex(uint64_t v, uint32_t t, bool fraud): vid(v), type(t), isFraud(fraud) {}
    friend std::ostream& operator<<(std::ostream& os, const Vertex& obj) {
      os << obj.vid << " " ;
      os << obj.type <<  " ";
      os << obj.isFraud << " ";
      return os;
    }
    friend std::istream& operator>>(std::istream& is, Vertex& obj) {
      is >> obj.vid;
      is >> obj.type;
      is >> obj.isFraud;
      return is;
    }
    friend bool operator==(const Vertex& left, const Vertex& right) {
      return left.vid == right.vid;
    }
  };

  struct VertexHash : public std::unary_function<Vertex,std::size_t> {
    std::size_t operator()(Vertex const& p) const {
      std::size_t seed = 0;
      boost::hash_combine(seed, p.vid);
      return seed;
    }
  };

  struct TypeDistance {
    uint32_t type;
    uint32_t distance;

    TypeDistance(uint32_t t =0, uint32_t d = 0): type(t), distance(d) {
    }
    friend std::ostream& operator<<(std::ostream& os, const TypeDistance& obj) {
      os << obj.type << " ";
      os << obj.distance << " ";
      return os;
    }
    friend std::istream& operator>>(std::istream& is, TypeDistance& obj) {
      is >> obj.type;
      is >> obj.distance;
      return is;
    }

    friend bool operator==(const TypeDistance& left, const TypeDistance& right) {
      return (left.type == right.type);
    }
  };

  struct TypeDistanceHash : public std::unary_function<TypeDistance,std::size_t> {
    std::size_t operator()(TypeDistance const& p) const {
      std::size_t seed = 0;
      boost::hash_combine(seed, p.type);
      return seed;
    }
  };


  struct EdgePair{
    VertexLocalId_t src;
    VertexLocalId_t tgt;
  
    EdgePair(unsigned int s=0, unsigned int t=0): src(s), tgt(t) {}
  
    friend std::ostream& operator<<(std::ostream& os, const EdgePair& obj){
      os<<obj.src<<" "<<obj.tgt << " ";
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

 
/**TransactionFraudScore
   *
   * Computes the fraud score for the specify transaction 
   *
   *
   */
  class TransactionFraudScore : public gpelib4::BaseUDF {
  private: 
    uint32_t score_;

    boost::unordered_set<Vertex,VertexHash> vertices_;
    boost::unordered_set<EdgePair,EdgePairHash> edges_;
    VertexLocalId_t TRANSACTION_ID; 
    enum {GV_DISTANCE, GV_TRANSACTION_ID, GV_VERTLIST, GV_EDGELIST}; 
    enum {TYPE_TRANSACTION, TYPE_USERID, TYPE_SSN, TYPE_BANKID, TYPE_CELL, TYPE_IMEI, TYPE_IP};
  public:
    // UDF Settings: Computation Modes
    static const gpelib4::EngineProcessingMode EngineMode_ = gpelib4::EngineProcessMode_ActiveVertices;
    static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
    static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
    static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
    static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
    static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined;
    static const int PrintMode_ = gpelib4::Mode_NotDefined;
    
    // These typedefs are required by the engine.
    typedef topology4::VertexAttribute V_ATTR;
    typedef topology4::EdgeAttribute_Writable E_ATTR;
    typedef FlagMessage V_VALUE;
    typedef FlagMessage MESSAGE;
 
    /**************Class Constructor************/
    TransactionFraudScore(int iteration_limit, uint32_t transaction_id, JSONWriter* writer): 
	gpelib4::BaseUDF(EngineMode_, iteration_limit * 2), TRANSACTION_ID(transaction_id), writer_(writer) {}
     
    /**************Class Destructor************/
    ~TransactionFraudScore() {}
     
    /**************Initialize Globals************/
    void Initialize_GlobalVariables(
      gpelib4::GlobalVariables* globalvariables) { 
        globalvariables->Register(GV_TRANSACTION_ID, new gpelib4::UnsignedIntStateVariable(TRANSACTION_ID));
        globalvariables->Register(GV_VERTLIST, new UDIMPL::SetVariable<Vertex, VertexHash>());
        globalvariables->Register(GV_EDGELIST, new UDIMPL::SetVariable<EdgePair, EdgePairHash>());
        globalvariables->Register(GV_DISTANCE, new UDIMPL::SetVariable<TypeDistance, TypeDistanceHash>());   
    }
 
     ALWAYS_INLINE void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context)  {   
        TRANSACTION_ID = context->GlobalVariable_GetValue<VertexLocalId_t>(GV_TRANSACTION_ID);
        context->WriteAll(V_VALUE(), false);         
        context->Write(TRANSACTION_ID, V_VALUE(FLAG_USER_ID | FLAG_SSN | FLAG_BANK_ID | FLAG_CELL | FLAG_IMEI | FLAG_IP), true);         
    }
     
    /*
    ALWAYS_INLINE void Initialize(gpelib4::SingleValueContext<V_VALUE> * context, VertexLocalId_t vid, V_ATTR* vertexattr)  {   
 		if (vertexattr->type() == TYPE_TRANSACTION && vid == TRANSACTION_ID) {
			context->Write(vid, V_VALUE(FLAG_USER_ID | FLAG_SSN | FLAG_BANK_ID | FLAG_CELL | FLAG_IMEI | FLAG_IP), true);         
		} else {
			context->Write(vid, V_VALUE(), false);
		}
    } */

    void StartRun(gpelib4::MasterContext* context) { }
 
    void BeforeIteration(gpelib4::MasterContext* context) { 
    }
 
    ALWAYS_INLINE void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,const V_VALUE& srcvertexvalue, 
                           const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,const V_VALUE& targetvertexvalue,
                           E_ATTR* edgeattr,gpelib4::SingleValueMapContext<MESSAGE> * context){
        if(srcvertexattr ->type() == TYPE_TRANSACTION) {
          //first iteration, send out flag for entity
          if (context->Iteration() == 1) {
                MESSAGE mesg(1 << (targetvertexattr->type() - 1));
                context->Write(targetvid, mesg);
                context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(srcvid, targetvid));
          }
          else {
            //only if src is not fraudulent
            if (!srcvertexattr->GetBool(0, false)) {
                context->Write(targetvid, srcvertexvalue);
                context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(srcvid, targetvid));
            }
          }
        }
        else { // activate all other targets
          if (context->Iteration() == 1) {
            MESSAGE mesg(1 << (srcvertexattr->type() - 1));
            context->Write(targetvid, mesg);
            context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(srcvid, targetvid));
          } else {
            context->Write(targetvid, srcvertexvalue);
            context->GlobalVariable_Reduce<EdgePair>(GV_EDGELIST, EdgePair(srcvid, targetvid));
          }
        }
    }

    ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                             const V_VALUE& singlevalue,
                             gpelib4::SingleValueMapContext<MESSAGE> * context){
      context->GlobalVariable_Reduce<Vertex>(GV_VERTLIST, Vertex(vid));
    }
     
    void BetweenMapAndReduce(gpelib4::MasterContext* context) { }
 
    ALWAYS_INLINE void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                              const V_VALUE& vertexvalue, MESSAGE& accumulator,
                              gpelib4::SingleValueContext<V_VALUE>* context){
        context->GlobalVariable_Reduce<Vertex>(GV_VERTLIST, Vertex(vid));
        boost::unordered_set<TypeDistance,TypeDistanceHash> typedistances = context->GlobalVariable_GetValue<boost::unordered_set<TypeDistance,TypeDistanceHash> >(GV_DISTANCE);

        if ( !vertexvalue.cover(accumulator) ) {
            V_VALUE value = vertexvalue;
            value += accumulator;
            //if fraud transaction
            if (vertexattr ->type() == TYPE_TRANSACTION && vertexattr->GetBool(0, false)) {
                int flags = value.flags;
                // it will be fine adding 1 for both transaction or non-transaction queries.
                uint32_t dist = (context->Iteration() + 1) / 2;
                for (int i = 0; i < TOTAL_FLAGS; ++i) {
                  if(typedistances.find(TypeDistance(i)) == typedistances.end()) {
                    if ((flags & (1 << i)) > 0) {
                      context->GlobalVariable_Reduce<TypeDistance>(GV_DISTANCE, TypeDistance(i, dist));
                    }
                  }
                }
            }
            context->Write(vid, value);
        }
    }
 
    void AfterIteration(gpelib4::MasterContext* context) { 
    //  std::cout << "oh, my god AfterIter " << context->GetActiveVertexCount() <<  "\n";
    }
     
    void EndRun(gpelib4::BasicContext* context) {
            boost::unordered_set<TypeDistance,TypeDistanceHash> typedistances = context->GlobalVariable_GetValue<boost::unordered_set<TypeDistance,TypeDistanceHash> >(GV_DISTANCE);
          boost::unordered_set<TypeDistance,TypeDistanceHash>::iterator it;
          float score = 0;
          for(it = typedistances.begin(); it != typedistances.end(); it++) {
              uint32_t distance = it ->distance;
              uint32_t type = it ->type;
              switch(type) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4: 
                  			switch (distance) {
                  				case 1: score += 1000; break;
                  				case 2: score += 500;  break;
                  				case 3: score += 100;  break;
                  			}
                        break;
                case 5:
                  			switch (distance) {
                  				case 1: score += 100; break;
                  				case 2: score += 10;  break;
                  				case 3: score += 1;  break;
                  			}
                        break;

              }
          }
          boost::unordered_set<EdgePair,EdgePairHash> edges = context->GlobalVariable_GetValue<boost::unordered_set<EdgePair,EdgePairHash> >(GV_EDGELIST);
          boost::unordered_set<Vertex,VertexHash> vertices = context->GlobalVariable_GetValue<boost::unordered_set<Vertex,VertexHash> >(GV_VERTLIST);

          score_ = score;
          edges_ = edges;
          vertices_ = vertices;
    }
     
    // If PrintMode_ = gpelib4::Mode_Print_ByVertex  or if PrintMode_ = gpelib4::Mode_Print_ByVertex|gpelib4::Mode_Print_ByEdge
    ALWAYS_INLINE void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
                             V_ATTR* vertexattr, const V_VALUE& singlevalue,
                             gpelib4::BasicContext* context){
    }
    /** 
    * Output
    */
    std::string Output(gpelib4::BasicContext* context) {
        return "";
    }

    uint32_t getScore() {
      return score_;
    }

    boost::unordered_set<Vertex,VertexHash> getVertices() {
      return vertices_;
    }

    boost::unordered_set<EdgePair,EdgePairHash> getEdges() {
      return edges_;
    }
     
    JSONWriter* writer_;

    
  };
}
// 
//int main(int argc, char** argv) {
//   std::string cfgfile = "";
//   std::string partitionpath = argv[1];
//   std::string output = argv[2];
//   std::string transactionIdStr = argv[3];
//   uint32_t transactionId = atoi(transactionIdStr.c_str());
//   gpelib4::EngineDriver driver(cfgfile, partitionpath);
//   gperun::TransactionFraudScore udf(gperun::MAX_ITERATION, transactionId);
//   driver.RunSingleVersion(&udf, output);
//   return 0;
//}
//
//
