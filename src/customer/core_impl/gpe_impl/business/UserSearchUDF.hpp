#ifndef GPERUN_USER_SEARCH_UDF_HPP_
#define GPERUN_USER_SEARCH_UDF_HPP_

#include <gpelib4/udfs/udfinclude.hpp>
#include "base/global_vector.hpp"
#include "base/global_map.hpp"
#include "../../udt.hpp"

namespace UDIMPL {
  #define AnyType 65536
  class UserSearchUDF : public SingleActive_VertexMap_BaseUDF {
  public:
    #define pow2(k) (1llu << k)
    #define EPS (1e-6)
    #define weight_attr "weight"
    #define datetime_attr "date_time"

    inline bool Compare(Operator op, double in_val, double real_val) {
      switch (op) {
        case GT: return in_val < real_val;
        case LT: return in_val > real_val;
        case GE: return in_val <= real_val + EPS;
        case LE: return in_val >= real_val - EPS; 
        case NE: return fabs(in_val - real_val) > EPS;
        case EQ: return fabs(in_val - real_val) <= EPS;
        default: return false;
      }
    }

    struct VertexStatus {
      uint64_t mask;
      std::vector<uint32_t> count; 
      size_t sz;
      VertexStatus(): mask(0), sz(0) {}
      VertexStatus(uint64_t m, uint32_t size)
        : mask(m), sz(size) {}
      void resize() {
          count.resize(sz);
      }
      VertexStatus operator+= (const VertexStatus &vs) {
        this->mask |= vs.mask;
        if (vs.count.size() == sz && this->count.size() == sz) {
          for (size_t k = 0; k < sz; ++k) {
            this->count[k] += vs.count[k];
          }
        }
        return *this;
      }
    };

    typedef VertexStatus V_VALUE;
    typedef VertexStatus MESSAGE;

    // global variable
    enum {GV_USER_LIST};


    UserSearchUDF(std::vector<OntologyCond>& cond,
                  std::vector<uint32_t> pathVertexT)
        : SingleActive_VertexMap_BaseUDF(1), 
          sp_(PathOntoUser), 
          onto_cond_(cond), 
          cond_size_(cond.size()),
          pathVertexT_(pathVertexT) {
      std::cout << "Constructor UserSearchUDF OntologyCond" << std::endl;
      for (size_t k = 0; k < cond.size(); ++k) {
        std::cout << cond[k] << std::endl;
      }

      std::cout << "pathVertexT ";
      for (size_t k = 0; k < pathVertexT_.size(); ++k) {
        std::cout << pathVertexT_[k] << ", ";
      }
      std::cout << std::endl;
    }

    UserSearchUDF(std::vector<ItemBehaviorCond>& cond,
                  std::vector<uint32_t> pathVertexT)
        : SingleActive_VertexMap_BaseUDF(2), 
          sp_(PathItemUser), 
          item_bhr_cond_(cond), 
          cond_size_(cond.size()),
          pathVertexT_(pathVertexT) {
      std::cout << "Constructor UserSearchUDF ItemBehaviorCond" << std::endl;
      for (size_t k = 0; k < cond.size(); ++k) {
        std::cout << cond[k] << std::endl;
      }
      
      std::cout << "pathVertexT ";
      for (size_t k = 0; k < pathVertexT_.size(); ++k) {
        std::cout << pathVertexT_[k] << ", ";
      }
      std::cout << std::endl;
    }

    UserSearchUDF(std::vector<OntoBehaviorCond>& cond,
                  std::vector<uint32_t> pathVertexT)
        : SingleActive_VertexMap_BaseUDF(3), 
          sp_(PathOntoItemUser), 
          onto_bhr_cond_(cond), 
          cond_size_(cond.size()),
          pathVertexT_(pathVertexT) {
      std::cout << "Constructor UserSearchUDF OntoBehaviorCond" << std::endl;
      for (size_t k = 0; k < cond.size(); ++k) {
        std::cout << cond[k] << std::endl;
      }
      
      std::cout << "pathVertexT ";
      for (size_t k = 0; k < pathVertexT_.size(); ++k) {
        std::cout << pathVertexT_[k] << ", ";
      }
      std::cout << std::endl;
    }

    void Initialize(GlobalSingleValueContext<V_VALUE>* context) {
      std::cout << "UDF Initialize" << std::endl;
      //For specified path, active the start nodes.
      //It still can not handle the case - the same vertex
      //appear several times with different filters
      if (sp_ == PathOntoUser) {
        for (size_t k = 0; k < onto_cond_.size(); ++k) {
          std::vector<uint64_t> startIds = onto_cond_[k].startId;
          for (size_t i = 0; i < startIds.size(); ++i) {
            context->Write(startIds[i], V_VALUE(pow2(k), cond_size_));
          }
        }
      } else if (sp_ == PathItemUser) {
        for (size_t k = 0; k < item_bhr_cond_.size(); ++k) {
          V_VALUE value(pow2(k), cond_size_);
          value.resize();
          value.count[k] = 1;
          std::vector<uint64_t> startIds = item_bhr_cond_[k].startId;
          for (size_t i = 0; i < startIds.size(); ++i) {
            context->Write(startIds[i], value);
          }
        }
      } else if (sp_ == PathOntoItemUser) {
        for (size_t k = 0; k < onto_bhr_cond_.size(); ++k) {
          V_VALUE value(pow2(k), cond_size_);
          value.resize();
          value.count[k] = 1;
          std::vector<uint64_t> startIds = onto_bhr_cond_[k].startId;
          for (size_t i = 0; i < startIds.size(); ++i) {
            context->Write(startIds[i], value);
          }
        }
      }
    }

    void Initialize_GlobalVariables(gpelib4::GlobalVariables* globalvariables) {
      std::cout << "UDF Initialize_GlobalVariables" << std::endl;
      globalvariables->Register(GV_USER_LIST, new SetVariable<VertexLocalId_t>());
    }

    void StartRun(MasterContext* context) {
      std::cout << "UDF StartRun" << std::endl;
      //context->GetTypeFilterController()->DisableAllEdgeTypes();
      //context->GetTypeFilterController()->EnableEdgeType(etype_id_);
    }

    ALWAYS_INLINE void EdgeMap( const VertexLocalId_t& srcvid, V_ATTR* srcvertexattr, const V_VALUE& srcvertexvalue,
                  const VertexLocalId_t& targetvid, V_ATTR* targetvertexattr, const V_VALUE& targetvertexvalue,
                  E_ATTR* edgeattr, SingleValueMapContext<MESSAGE>* context) {
        std::cout << "UDF EdgeMap" << std::endl;
        size_t iter = context->Iteration();
        if ((srcvertexattr->type() != pathVertexT_[iter - 1] && pathVertexT_[iter - 1] != AnyType)
            || targetvertexattr->type() != pathVertexT_[iter]) {
          return;
        }
        if (sp_ == PathOntoUser) {
          uint64_t mask = srcvertexvalue.mask;
          uint64_t nmask = 0;
          size_t sz = srcvertexvalue.sz;
          double real_wgt = edgeattr->GetDouble(weight_attr, 0.0);
          for (size_t k = 0; k < sz; ++k) {
            if ((pow2(k) & mask) != 0) {
              double in_wgt = onto_cond_[k].weight; 
              Operator op = onto_cond_[k].op;
              if (Compare(op, in_wgt, real_wgt)) {
                nmask |= pow2(k);
              }
            }
          }
          if (nmask != 0) {
            std::cout << "SendMsg nmask = " << nmask << ", sz = " << sz << std::endl; 
            MESSAGE mesg(nmask, sz);
            context->Write(targetvid, mesg);
          }
        } else if (sp_ == PathItemUser) {
          context->Write(targetvid, srcvertexvalue);
        } else if (sp_ == PathOntoItemUser) {
          context->Write(targetvid, srcvertexvalue);
        }
    }

    ALWAYS_INLINE void VertexMap(const VertexLocalId_t& vid, V_ATTR* vertexattr,const V_VALUE& vertexvalue,
                               gpelib4::SingleValueMapContext<MESSAGE> * context) {
        std::cout << "UDF VertexMap" << std::endl;
        if (sp_ == PathOntoUser) {
        } else if (sp_ == PathItemUser) {
        } else if (sp_ == PathOntoItemUser) {
        }
    }
    
    ALWAYS_INLINE void Reduce( const VertexLocalId_t& vid, V_ATTR* vertexattr, const V_VALUE& singlevalue,
                 const MESSAGE& accumulator, SingleValueContext<V_VALUE>* context) {
        std::cout << "UDF Reduce" << std::endl;
        size_t iter = context->Iteration();
        if (vertexattr->type() != pathVertexT_[iter]) return;
        if (sp_ == PathOntoUser) {
          uint64_t mask = accumulator.mask;
          size_t sz = accumulator.sz;
          if (pow2(sz) - 1 == mask) {
            std::cout << "Reduce mask = " << mask << ", sz = " << sz << ", pow2 = " << pow2(sz) - 1 << std::endl;
            context->GlobalVariable_Reduce<VertexLocalId_t>(GV_USER_LIST, vid);
          }
        } else if (sp_ == PathItemUser) {
          if (iter == 1) { //behavior vertex - check the datetime filter  
            V_VALUE val = accumulator; 
            uint64_t datetime = vertexattr->GetUInt(datetime_attr, 0);
            uint64_t mask = accumulator.mask;
            size_t sz = accumulator.sz;
            for (size_t k = 0; k < sz; ++k) {
              if ((mask & pow2(k)) != 0) {
                uint64_t timeStart = item_bhr_cond_[k].timeStart;
                uint64_t timeEnd = item_bhr_cond_[k].timeEnd;
                if (timeStart > datetime || datetime > timeEnd) {
                  val.count[k] = 0;
                  val.mask ^= pow2(k);
                }
              }
            }
            context->Write(vid, val);
          } else if (iter < 2) {
            context->Write(vid, accumulator);
          } else { //user vertex - check the count filter
            uint64_t mask = accumulator.mask;
            size_t sz = accumulator.sz;
            std::cout << "Reduce mask = " << mask << ", sz = " << sz << std::endl;
            for (size_t k = 0; k < sz; ++k) {
              bool good = false;
              if ((mask & pow2(k)) != 0) {
                uint64_t times = item_bhr_cond_[k].times;
                Operator op = item_bhr_cond_[k].op;
                if (Compare(op, times, accumulator.count[k])) {
                  good = true;
                }
                std::cout << "Reduce op = " << op << ", times = " << times << ", count = " << accumulator.count[k] << std::endl;
              }
              if (!good) return;
            }
            context->GlobalVariable_Reduce<VertexLocalId_t>(GV_USER_LIST, vid);
          }
        } else if (sp_ == PathOntoItemUser) {
          if (iter == 2) { //behavior vertex - check the datetime filter 
            V_VALUE val = accumulator; 
            uint64_t datetime = vertexattr->GetUInt(datetime_attr, 0);
            uint64_t mask = accumulator.mask;
            size_t sz = accumulator.sz;
            for (size_t k = 0; k < sz; ++k) {
              if ((mask & pow2(k)) != 0) {
                uint64_t timeStart = onto_bhr_cond_[k].timeStart;
                uint64_t timeEnd = onto_bhr_cond_[k].timeEnd;
                if (timeStart > datetime || datetime > timeEnd) {
                  val.count[k] = 0;
                  val.mask ^= pow2(k);
                }
              }
            }
            context->Write(vid, val);
          } else if (iter < 3) {
            context->Write(vid, accumulator);
          } else { // user vertex - check the count filter
            uint64_t mask = accumulator.mask;
            size_t sz = accumulator.sz;
            std::cout << "Reduce mask = " << mask << ", sz = " << sz << std::endl;
            for (size_t k = 0; k < sz; ++k) {
              bool good = false;
              if ((mask & pow2(k)) != 0) {
                uint64_t times = onto_bhr_cond_[k].times;
                Operator op = onto_bhr_cond_[k].op;
                if (Compare(op, times, accumulator.count[k])) {
                  good = true;
                }
                std::cout << "Reduce op = " << op << ", times = " << times << ", count = " << accumulator.count[k] << std::endl;
              }
              if (!good) return;
            }
            context->GlobalVariable_Reduce<VertexLocalId_t>(GV_USER_LIST, vid);
          }
        }
    }

    void AfterIteration(MasterContext* context) {
        std::cout << "UDF AfterIteration" << std::endl;
    }

    void EndRun(BasicContext* context) {
      std::cout << "UDF EndRun" << std::endl;
      userIds = context->GlobalVariable_GetValue<boost::unordered_set<VertexLocalId_t> >(GV_USER_LIST);
    }

    void getUserSet(std::set<VertexLocalId_t> &userSet) {
      std::cout << "UDF getUserSet" << std::endl;
      for (boost::unordered_set<VertexLocalId_t>::iterator it = userIds.begin();
              it != userIds.end(); ++it) {
        userSet.insert(*it);
      }
    }

  private:
    SearchPath sp_;
    std::vector<OntologyCond> onto_cond_;
    std::vector<ItemBehaviorCond> item_bhr_cond_;
    std::vector<OntoBehaviorCond> onto_bhr_cond_;
    uint32_t cond_size_;
    std::vector<uint32_t> pathVertexT_;
    std::vector<VertexLocalId_t> translate_ids_;
    boost::unordered_set<VertexLocalId_t> userIds;
  };
}  // namepsace UDIMPL

#endif /* GPERUN_USER_SEARCH_UDF_HPP_ */
