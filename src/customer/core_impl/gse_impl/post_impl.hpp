#ifndef UDIMPL_GSE_POSTJSON2DELTA_HPP_
#define UDIMPL_GSE_POSTJSON2DELTA_HPP_

#include <post_service/post_json2delta.hpp>

namespace UDIMPL {
  /**
   * Override below when you need to customize the JSON2Delta
   */
  class UD_PostJson2Delta : public gse2::PostJson2Delta {
  public:
    UD_PostJson2Delta() : gse2::PostJson2Delta() {
    }

    PostJson2Delta* Clone() {
      return new UD_PostJson2Delta();
    }

    /**
     * For both of these functions, they are optional but recommended if you have complex attribute
     * sets to manage.

    void writeVertexAttribute(Json::Value &VertexNode) {
    }

    void writeEdgeAttribute(Json::Value &EdgeNode) {
    }

    bool processOneEdge(Json::Value &EdgeNode) {
    }

    bool processOneVertex(Json::Value &VertexNode, bool &newVertex) {
    }

    */

  };
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
