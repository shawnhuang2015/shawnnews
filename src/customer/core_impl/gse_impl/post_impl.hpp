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

    bool s2_parseJSON(std::string &requestStr,
                      Json::Value &vertex_list_node,
                      Json::Value &edge_list_node) {
      // gutil::GTimer timer;
      Json::Value requestRoot;
      Json::Reader jsonReader;
      if (!jsonReader.parse(requestStr, requestRoot)) {
        error_ = true;
        errmsg_ = "parse input failed " + requestStr;
        return false;
      }
      // timer.Stop("s2_parseJSON " + requestid_,  Verbose_EngineHigh);
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
   * For both of these functions, they are optional but recommended if you have complex attribute
   * sets to manage.
   */
    void writeVertexAttribute(Json::Value &VertexNode) {
    }

    void writeEdgeAttribute(Json::Value &EdgeNode) {
    }

    bool processOneEdge(Json::Value &EdgeNode) {
      return processIntWeightedEdge(EdgeNode);
    }

    bool processIntWeightedEdge(Json::Value &EdgeNode){
      std::string fromIDStr = EdgeNode.get("startNode", "").asString();
      std::string toIDStr = EdgeNode.get("endNode", "").asString();
      Json::Value wt = EdgeNode["weight"];
      if(wt.isNull()){
        errmsg_ = "Error_: weight required for all edge updates";
        error_ = true;
        return !error_;
      }
      uint32_t weight = wt.asUInt();
      VertexLocalId_t fromVid = getVidfromUid(fromIDStr);
      VertexLocalId_t toVid = getVidfromUid(toIDStr);
      /* write from=> to */
      deltaWriter_.write_flag((uint8_t) topology4::DeltaRecord_Edge,
                              (uint8_t) topology4::DeltaAction_Insert);
      deltaWriter_.write(fromVid);
      deltaWriter_.write(toVid);
      // 0 is hard-coding edge type ID.
      deltaWriter_.write((uint32_t) 0);
      // I could use writeEdgeAttribute from the super class, but it's just one attribute
      // so why waste time on the function call?
      deltaWriter_.write((uint8_t) topology4::DeltaAttributeOperator_Overwrite);
      deltaWriter_.write(weight);

      deltaWriter_.write_watermark();
      return !error_;

    }

    bool processOneVertex(Json::Value &VertexNode, bool &newVertex) {
      return processOneVertexNoAttribs(VertexNode,newVertex);
    }

    bool processOneVertexNoAttribs(Json::Value &VertexNode, bool &newVertex){
      newVertex = false;
      std::string nodeIDStr = VertexNode.get("id", "").asString();
      VertexLocalId_t vid = getVidfromUid(nodeIDStr, newVertex);
      if(vid != (VertexLocalId_t)-1){
        deltaWriter_.write_flag(
              (uint8_t) topology4::DeltaRecord_Vertex,
                (uint8_t) topology4::DeltaAction_Update);
        deltaWriter_.write(vid);
        // no attributes in this situation.
        //writeVertexAttribute(VertexNode);
        deltaWriter_.write_watermark();
      } else {
        errmsg_ = "error_: missing converted vid for " + nodeIDStr;
        error_ = true;
      }
      return !error_;
    }


  };
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
