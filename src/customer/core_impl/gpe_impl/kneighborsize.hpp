#ifndef GPERUN_KNEIGHBORSIZE_HPP_
#define GPERUN_KNEIGHBORSIZE_HPP_

#include <gpelib4/udfs/singleactiveudfbase.hpp>

namespace UDIMPL {
  class KNeighborSize : public SingleActiveBaseUDF {
  public:
    typedef uint8_t V_VALUE;
    typedef VertexLocalId_t MESSAGE;

    KNeighborSize(unsigned int depth, VertexLocalId_t start_node, JSONWriter* jsonwriter)
      : SingleActiveBaseUDF(depth), start_node_(start_node), writer_(jsonwriter) {}

    void Initialize(GlobalSingleValueContext<V_VALUE>* valuecontext) {
      valuecontext->Write(start_node_,1,true);
    }

    void StartRun(MasterContext* context) {
      writer_->WriteStartObject();
    }

    void EdgeMap( const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                  const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                  E_ATTR* edgeattr, SingleValueMapContext<MESSAGE>* context) {
      if (targetvertexvalue != 1)
        context->SetActiveFlag(targetvid);
    }

    void Reduce( const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                 const MESSAGE& accumulator, SingleValueContext<V_VALUE>* context) {
      context->Write(vid,1);
    }

    void AfterIteration(MasterContext* context) {
      std::string iterationstr = "iteration" + boost::lexical_cast<std::string>(context->Iteration());
      writer_->WriteName(iterationstr.c_str()).WriteUnsignedInt(context->GetActiveVertexCount());
    }

    void EndRun(BasicContext* context) {
      writer_->WriteEndObject();
    }

  private:
    VertexLocalId_t start_node_;
    JSONWriter* writer_;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_KNEIGHBORSIZE_HPP_ */
