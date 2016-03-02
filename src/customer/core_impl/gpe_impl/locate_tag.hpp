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

      tag_t() : id(0), name() {}

      tag_t(VertexLocalId_t id, std::string name)
        : id(id), name(name) {}

      tag_t(const tag_t &other)
        : id(other.id), name(other.name) {}

      friend std::istream& operator>>(std::istream& is, tag_t& st) {
        return is;
      }

      friend std::ostream& operator<<(std::ostream& os, const tag_t& st) {
        return os;
      }
    };

    // global variable
    enum {GV_TAG};

    LocateTagUDF(unsigned int limit, uint32_t vtype_id, std::vector<tag_t> &out)
      : SingleActive_VertexMap_BaseUDF(limit), vtype_id_(vtype_id), out_(out) {}

    void Initialize(GlobalSingleValueContext<V_VALUE>* context) {
      context->SetActiveFlagByType(vtype_id_, true);
    }

    void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      globalvariables->Register(GV_TAG, new VectorVariable<tag_t>());
    }

    void StartRun(MasterContext* context) {
    }

    void BeforeIteration(MasterContext* context) {
    }

    ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,const V_VALUE& vertexvalue,
                               gpelib4::SingleValueMapContext<MESSAGE> * context) {
      context->GlobalVariable_Reduce<tag_t>(GV_TAG, 
          tag_t(vid, vertexattr->GetString("name")));
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
      GVector<tag_t>::T &rez = context->GlobalVariable_GetValue<GVector<tag_t>::T>(GV_TAG);
      int size = rez.size();
      for (int i = 0; i < size; ++i) {
        out_.push_back(rez[i]);
      }
    }

  private:
    uint32_t vtype_id_;
    std::vector<tag_t> &out_;
  };
}  // namepsace UDIMPL

#endif
