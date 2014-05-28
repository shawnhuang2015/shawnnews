
#ifndef UDIMPL_GSE_POSTJSON2DELTA_HPP_
#define UDIMPL_GSE_POSTJSON2DELTA_HPP_

#include <post_service/post_json2delta.hpp>
namespace UDIMPL {
class GSE_PostJson2Delta : public gse2::PostJson2Delta {
 public:
  GSE_PostJson2Delta(gse2::IdConverter *idconverter)
      : gse2::PostJson2Delta(idconverter) {
  }

};
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
