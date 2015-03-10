#include <iostream>
//int main()
//{
//    std::cout << "Hello Graph" << std::endl;
//}
//

#include <gpelib4/udfs/singleactiveudfbase.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>
  struct PageRankMessage{
    PageRankMessage(){value_ = 0;}
    PageRankMessage(double value){value_ = value;}
    double value_;
    PageRankMessage& operator+=(const PageRankMessage& other) {
      value_ += other.value_;
      return *this;
    }
  };
  class PageRank : public UDIMPL::SingleBatchBaseUDF{
  public:
    typedef double V_VALUE;
    typedef PageRankMessage MESSAGE;
    PageRank(unsigned int depth)
      : SingleBatchBaseUDF(depth) {}
    void Initialize(GlobalSingleValueContext<V_VALUE>* valuecontext) {
      valuecontext->WriteAll(1);
    }
    void EdgeMap( const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                  const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                  E_ATTR* edgeattr, SingleValueMapContext<MESSAGE>* context) {
      context->Write(targetvid, srcvertexvalue / srcvertexattr->outgoing_degree());
    }
    void Reduce( const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                 const MESSAGE& accumulator, SingleValueContext<V_VALUE>* context) {
      context->Write(vid, 0.15 + 0.85 * accumulator.value_);
    }
    void Write(gutil::GOutputStream& ostream, const VertexLocalId_t& vid,
               V_ATTR* vertexattr, const V_VALUE& vertexvalue,
               gpelib4::BasicContext* context) {
      ostream.WriteUnsignedInt(vid);
      ostream.put(',');
      ostream.WriteUnsignedFloat(vertexvalue);
      ostream.put('\n');
    }
  };
   
  int main(int argc, char** argv) {
    std::string cfgfile = "";
    std::string partitionpath = argv[1];
    std::string output = argv[2];
    gpelib4::EngineDriver driver(cfgfile, partitionpath);
    PageRank udf(10);
    driver.RunSingleVersion(&udf, output);
  }
