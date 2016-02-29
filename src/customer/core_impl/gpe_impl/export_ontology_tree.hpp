#ifndef GPERUN_EXPORT_ONTOLOGY_TREE_HPP_
#define GPERUN_EXPORT_ONTOLOGY_TREE_HPP_

#include <gpelib4/udfs/udfinclude.hpp>
#include "base/global_vector.hpp"
#include "base/global_map.hpp"

namespace UDIMPL {
  class ExportOntologyTree : public SingleActiveBaseUDF {
  public:
    typedef uint8_t V_VALUE;
    typedef VertexLocalId_t MESSAGE;

    // global variable
    enum {GV_TREE};

    struct tag_pair_t {
      VertexLocalId_t srcid;
      VertexLocalId_t tgtid;

      tag_pair_t()
        : srcid(0), tgtid(0) {}

      tag_pair_t(VertexLocalId_t sid, VertexLocalId_t tid)
        : srcid(sid), tgtid(tid) {}

      tag_pair_t(const tag_pair_t &other) 
        : srcid(other.srcid), tgtid(other.tgtid) {}

      friend std::istream& operator>>(std::istream& is, tag_pair_t& st) {
        return is;
      }

      friend std::ostream& operator<<(std::ostream& os, const tag_pair_t& st) {
        return os;
      }
    };

    ExportOntologyTree(unsigned int limit, uint32_t vtype_id, 
        uint32_t etype_id, size_t threshold, 
        std::map<VertexLocalId_t, std::vector<VertexLocalId_t> > &tree)
      : SingleActiveBaseUDF(limit), vtype_id_(vtype_id), 
        etype_id_(etype_id), threshold_(threshold), tree_(tree) {}

    void Initialize(GlobalSingleValueContext<V_VALUE>* context) {
      context->SetActiveFlagByType(vtype_id_, true);
    }

    void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      globalvariables->Register(GV_TREE, new VectorVariable<tag_pair_t>());
    }

    void StartRun(MasterContext* context) {
      context->GetTypeFilterController()->DisableAllEdgeTypes();
      context->GetTypeFilterController()->EnableEdgeType(etype_id_);
    }

    void BeforeIteration(MasterContext* context) {
      size_t n_active = context->GetActiveVertexCount();
      if (n_active > threshold_) {
        context->Abort();
      }
    }

    void EdgeMap( const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                  const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                  E_ATTR* edgeattr, SingleValueMapContext<MESSAGE>* context) {
      context->GlobalVariable_Reduce<tag_pair_t>(GV_TREE, tag_pair_t(srcvid, targetvid));
    }

    void Reduce( const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                 const MESSAGE& accumulator, SingleValueContext<V_VALUE>* context) {
    }

    void AfterIteration(MasterContext* context) {
    }

    void EndRun(BasicContext* context) {
      GVector<tag_pair_t>::T &rez = context->GlobalVariable_GetValue<GVector<tag_pair_t>::T>(GV_TREE);
      int size = rez.size();
      for (int i = 0; i < size; ++i) {
        tree_[rez[i].srcid].push_back(rez[i].tgtid);
      }
    }

  private:
    uint32_t vtype_id_;
    uint32_t etype_id_;
    size_t threshold_;
    std::map<VertexLocalId_t, std::vector<VertexLocalId_t> > &tree_;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_KNEIGHBORSIZE_HPP_ */
