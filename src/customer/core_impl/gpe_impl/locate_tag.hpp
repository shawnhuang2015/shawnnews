#ifndef GPERUN_LOCATE_TAG_HPP_
#define GPERUN_LOCATE_TAG_HPP_

#include <gpelib4/udfs/udfinclude.hpp>
#include "base/global_vector.hpp"
#include "base/global_map.hpp"

namespace UDIMPL {
  class LocateTagUDF : public SingleActive_VertexMap_BaseUDF {
  public:
    typedef uint8_t V_VALUE;
    typedef float MESSAGE;

    struct tag_t {
      VertexLocalId_t id;
      std::string name;
      uint32_t vtype_id;
      float weight;

      tag_t() : id(0), name(), vtype_id(0), weight(0) {}

      tag_t(VertexLocalId_t id, std::string name, uint32_t tid, float w)
        : id(id), name(name), vtype_id(tid), weight(w) {}

      tag_t(const tag_t &other)
        : id(other.id), name(other.name), vtype_id(other.vtype_id), weight(other.weight) {}

      friend std::istream& operator>>(std::istream& is, tag_t& st) {
        return is;
      }

      friend std::ostream& operator<<(std::ostream& os, const tag_t& st) {
        return os;
      }
    };

    // global variable
    enum {GV_TAG};

    LocateTagUDF(unsigned int limit, uint32_t vtype_id, 
        std::set<std::string> &tags)
      : SingleActive_VertexMap_BaseUDF(limit), vtype_id_(vtype_id), tags_(tags) {}

    void Initialize(GlobalSingleValueContext<V_VALUE>* context) {
      context->SetActiveFlagByType(vtype_id_, true);
    }

    void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      globalvariables->Register(GV_TAG, new VectorVariable<tag_t>());
    }

    void StartRun(MasterContext* context) {
//      context->GetTypeFilterController()->DisableAllEdgeTypes();
//      for (std::set<uint32_t>::iterator it = etype_id_.begin();
//          it != etype_id_.end(); ++it) {
//        context->GetTypeFilterController()->EnableEdgeType(*it);
//      }
    }

    void BeforeIteration(MasterContext* context) {
    }

    ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,const V_VALUE& vertexvalue,
                               gpelib4::SingleValueMapContext<MESSAGE> * context) {
      std::string name = vertexattr->GetString("name");
      if (tags_.find(name) != tags_.end()) {
      }
    }

    void EdgeMap( const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                  const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                  E_ATTR* edgeattr, SingleValueMapContext<MESSAGE>* context) {
//      double weight = edgeattr->GetDouble("weight", 1.0);
//      context->Write(targetvid, weight);
    }

    void Reduce( const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                 const MESSAGE& accumulator, SingleValueContext<V_VALUE>* context) {
//      context->GlobalVariable_Reduce<tag_t>(GV_TAG, 
//          tag_t(vid, vertexattr->GetString("name"), vertexattr->type(), accumulator));
    }

    void AfterIteration(MasterContext* context) {
    }

    void EndRun(BasicContext* context) {
//      GVector<tag_t>::T &rez = context->GlobalVariable_GetValue<GVector<tag_t>::T>(GV_TAG);
//      int size = rez.size();
//      for (int i = 0; i < size; ++i) {
//        tags_.push_back(rez[i]);
//      }
    }

  private:
    uint32_t vtype_id_;
    const std::set<std::string> &tags_;
  };
}  // namepsace UDIMPL

#endif
