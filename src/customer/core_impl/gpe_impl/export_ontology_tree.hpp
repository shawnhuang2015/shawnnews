#ifndef GPERUN_EXPORT_ONTOLOGY_TREE_HPP_
#define GPERUN_EXPORT_ONTOLOGY_TREE_HPP_

#include <gpelib4/udfs/udfinclude.hpp>

namespace UDIMPL {
  class ExportOntologyTree : public SingleActiveBaseUDF {
  public:
    typedef uint8_t V_VALUE;
    typedef VertexLocalId_t MESSAGE;

    ExportOntologyTree(unsigned int limit, uint32_t vtype_id, 
        uint32_t etype_id, JSONWriter* jsonwriter)
      : SingleActiveBaseUDF(limit), vtype_id_(vtype_id), 
        etype_id_(etype_id), writer_(jsonwriter) {}

    void Initialize(GlobalSingleValueContext<V_VALUE>* context) {
      context->SetActiveFlagByType(vtype_id_, true);
    }

    void StartRun(MasterContext* context) {
    }

    void EdgeMap( const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                  const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                  E_ATTR* edgeattr, SingleValueMapContext<MESSAGE>* context) {
    }

    void Reduce( const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                 const MESSAGE& accumulator, SingleValueContext<V_VALUE>* context) {
    }

    void AfterIteration(MasterContext* context) {
    }

    void EndRun(BasicContext* context) {
    }

  private:
    uint32_t vtype_id_;
    uint32_t etype_id_;
    JSONWriter* writer_;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_KNEIGHBORSIZE_HPP_ */
