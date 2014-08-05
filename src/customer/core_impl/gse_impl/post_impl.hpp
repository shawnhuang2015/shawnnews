#ifndef UDIMPL_GSE_POSTJSON2DELTA_HPP_
#define UDIMPL_GSE_POSTJSON2DELTA_HPP_

#include <post_service/post_json2delta.hpp>
#include "../udt.hpp"

namespace UDIMPL {
  /**
 * Override below when you need to customize the JSON2Delta
 */
  class UD_PostJson2Delta : public gse2::PostJson2Delta {
  public:
    UD_PostJson2Delta(gse2::IdConverter *idconverter)
      : gse2::PostJson2Delta(idconverter) {
    }

    PostJson2Delta* Clone() {
      return new UD_PostJson2Delta(idconverter_);
    }

    bool s2_parseJSON(std::string &requestStr,
                      Json::Value &vertex_list_node,
                      Json::Value &edge_list_node) {
      gutil::GTimer timer;
      Json::Value requestRoot;
      Json::Reader jsonReader;
      if (!jsonReader.parse(requestStr, requestRoot)) {
        error_ = true;
        errmsg_ = "parse input failed " + requestStr;
        return false;
      }
      timer.Stop("s2_parseJSON " + requestid_,  Verbose_EngineHigh);
      Json::Value json_updateEvent = requestRoot["updateEvent"];
      vertex_list_node = requestRoot["nodeList"];
      edge_list_node = requestRoot["edgeList"];

      if (!json_updateEvent.isNull()) {
        if (json_updateEvent.asString() == "edge")
          edge_list_node = requestRoot["updateTarget"]["edgeList"];
        else
          vertex_list_node = requestRoot["updateTarget"]["nodeList"];
      }
      return true;
    }

    std::string s99_genReturn() {
      Json::Value response_root;
      if (error_)
      {
        response_root["isSuccess"] = "false";
        response_root["errorCode"] = "ids01";
        response_root["errorMessage"] = errmsg_;
        response_root["context"]["nodeFailCount"] = getNumNewVertices()+ getNumUpdVertices();
        response_root["context"]["edgeFailCount"] = getNumUpdEdges();
      }
      else {
        response_root["isSuccess"] = "true";
        response_root["errorCode"] = "0";
        response_root["errorMessage"] = "";
        response_root["context"]["nodeFailCount"] = 0;
        response_root["context"]["edgeFailCount"] = 0;
      }
      response_root["context"]["nodeSubmitCount"] = getNumNewVertices() + getNumUpdVertices();;
      response_root["context"]["edgeSubmitCount"] = getNumUpdEdges();
      response_root["engine_time"] = getExecuteTime();
      return response_root.toStyledString();
    }

    /**
   * Given a vertex JSON node, write out its attributes
   * @param[in] VertexNode
   */
    void writeVertexAttribute(Json::Value &VertexNode) {
    }

    void writeEdgeAttribute(Json::Value &EdgeNode) {
    }

    bool processOneEdge(Json::Value &EdgeNode) {
      return !error_;
    }

    bool processOneVertex(Json::Value &VertexNode, bool &newVertex) {
      return !error_;
    }

  };
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
