#include <topology4/edgeattribute.hpp>
#include <topology4/vertexattribute.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginebase/context.hpp>
#include <gpelib4/udfs/singleactiveudfbase.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>

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

 
/**TransactionFraudScore
   *
   * Computes the fraud score for the specify transaction 
   *
   *
   */
  class TransactionFraudScore : public gpelib4::BaseUDF {
  private: 
  // define constants here, make sure to initialize them in the constructor
	uint32_t TRANSACTION_ID; 
//  const int INF = 1000000;
    enum {GV_DISTANCE, GV_TRANSACTION_ID}; 
    enum {TYPE_TRANSACTION, TYPE_USERID, TYPE_SSN, TYPE_BANKID, TYPE_CELL, TYPE_IMEI, TYPE_IP};
  public:
    // UDF Settings: Computation Modes
    static const gpelib4::EngineProcessingMode EngineMode_ = gpelib4::EngineProcessMode_ActiveVertices;
    //static const gpelib4::EngineProcessingMode EngineMode_ = gpelib4::EngineProcessMode_AllVertices;
    static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
    //static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
    static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Individually;
    static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
    static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
    static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_NotDefined;
    static const int PrintMode_ = gpelib4::Mode_Print_ByVertex;
    
    // These typedefs are required by the engine.
    typedef topology4::VertexAttribute V_ATTR;
    typedef topology4::EdgeAttribute_Writable E_ATTR;
    typedef FlagMessage V_VALUE;
    typedef FlagMessage MESSAGE;
 
    /**************Class Constructor************/
    TransactionFraudScore(int iteration_limit, uint32_t transaction_id, JSONWriter* writer): gpelib4::BaseUDF(EngineMode_, iteration_limit * 2), TRANSACTION_ID(transaction_id), writer_(writer) {}
     
    /**************Class Destructor************/
    ~TransactionFraudScore() {}
     
    /**************Initialize Globals************/
    void Initialize_GlobalVariables(
      gpelib4::GlobalVariables* globalvariables) { 
        globalvariables->Register(GV_DISTANCE, new gpelib4::UnsignedIntStateVariable(0));
        globalvariables->Register(GV_TRANSACTION_ID, new gpelib4::UnsignedIntStateVariable(TRANSACTION_ID));
    }
 
    //ALWAYS_INLINE void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE> * context)  {   
    //    TRANSACTION_ID = context->GlobalVariable_GetValue<uint32_t>(GV_TRANSACTION_ID);
    //    context->WriteAll(V_VALUE(), false);         
    //    context->Write(TRANSACTION_ID, V_VALUE(FLAG_USER_ID | FLAG_SSN | FLAG_BANK_ID | FLAG_CELL | FLAG_IMEI | FLAG_IP), true);         
    //}

    ALWAYS_INLINE void Initialize(gpelib4::SingleValueContext<V_VALUE> * context, VertexLocalId_t vid, V_ATTR* vertexattr)  {   
		if (vertexattr->type() == TYPE_TRANSACTION && vid == TRANSACTION_ID) {
			context->Write(vid, V_VALUE(FLAG_USER_ID | FLAG_SSN | FLAG_BANK_ID | FLAG_CELL | FLAG_IMEI | FLAG_IP), true);         
		} else {
			context->Write(vid, V_VALUE(), false);
		}
    }

    void StartRun(gpelib4::MasterContext* context) { }
 
    void BeforeIteration(gpelib4::MasterContext* context) { 
    }
 
    ALWAYS_INLINE void EdgeMap(const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr,const V_VALUE& srcvertexvalue, 
                           const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr,const V_VALUE& targetvertexvalue,
                           E_ATTR* edgeattr,gpelib4::SingleValueMapContext<MESSAGE> * context){
        if (!srcvertexattr->GetBool(0, false)) {
            if (context->Iteration() == 1) {
                MESSAGE mesg(1 << (targetvertexattr->type() - 1));
                context->Write(targetvid, mesg);
            } else {
                context->Write(targetvid, srcvertexvalue);
            }
        }
    }
     
    void BetweenMapAndReduce(gpelib4::MasterContext* context) { }
 
    ALWAYS_INLINE void Reduce(const VertexLocalId_t& vid, V_ATTR* vertexattr,
                              const V_VALUE& vertexvalue, MESSAGE& accumulator,
                              gpelib4::SingleValueContext<V_VALUE>* context){
        if ( !vertexvalue.cover(accumulator) ) {
            V_VALUE value = vertexvalue;
            value += accumulator;
            if (vertexattr->GetBool(0, false)) {
                int flags = value.flags;
                uint32_t dist = context->Iteration() / 2;
                uint32_t res  = context->GlobalVariable_GetValue<uint32_t>(GV_DISTANCE);
                for (int i = 0; i < TOTAL_FLAGS; ++i) {
                    if ((flags & (1 << i)) > 0) {
                        if (((res >> (i * 2)) & 3) == 0) {
                            res |= dist << (i * 2);
                        }
                    }
                }
                reinterpret_cast<gpelib4::UnsignedIntStateVariable*>(context->GetGlobalVariable(GV_DISTANCE))->Set(res);
            }
            context->Write(vid, value);
        }
    }
 
    void AfterIteration(gpelib4::MasterContext* context) { 
    //  std::cout << "oh, my god AfterIter " << context->GetActiveVertexCount() <<  "\n";
    }
     
    void EndRun(gpelib4::BasicContext* context) {
		uint32_t dist = context->GlobalVariable_GetValue<uint32_t>(GV_DISTANCE);
		int depth = 0;
		int score = 0;
		for (int i = 0; i + 1 < MAX_ITERATION; ++i) {
			depth = (dist >> (i * 2)) & 3;
			switch (depth) {
				case 1: score += 1000; break;
				case 2: score += 500;  break;
				case 3: score += 100;  break;
			}
		}
		// for IP
		depth = (dist >> (MAX_ITERATION * 2 - 2)) & 3;
		switch (depth) {
			case 1: score += 100; break;
			case 2: score += 10;  break;
			case 3: score += 1;   break;
		}
		for (int i = 0; i < 6; ++i) {
			std::cout << (dist & 3) << std::endl;
			dist >>= 2;
		}
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
