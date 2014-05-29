#ifndef UDIMPL_GSE_POSTJSON2DELTA_HPP_
#define UDIMPL_GSE_POSTJSON2DELTA_HPP_

#include <post_service/post_json2delta.hpp>
#include "../udt.hpp"

namespace UDIMPL {
/**
 * Override below when you need to customize the JSON2Delta
 */
class UD_PostJson2Delta : public gse2::PostJson2Delta {
  typedef std::tr1::unordered_map<std::string, gse2::IdConverter::vidInfo>::iterator vidmap_itr_t;
 public:
  UD_PostJson2Delta(gse2::IdConverter *idconverter)
      : gse2::PostJson2Delta(idconverter) {
  }

  /**
   * Given a vertex JSON node, write out its attributes
   * @param[in] VertexNode
   */
   void writeVertexAttribute(Json::Value &VertexNode) {
  }

  void writeEdgeAttribute(Json::Value &EdgeNode) {
#if 0
    UDT::OccurInfo_t occurInfo;
    occurInfo.firstLinktime = 0;
    occurInfo.lastLinktime = 0;
    occurInfo.linkCount = 0;
    occurInfo.direction = 0;
    occurInfo.isPayment = false;
    uint8_t attributeoperator =
        ((uint8_t) topology4::DeltaAttributeOperator_Add);
    deltaWriter_.write(attributeoperator);
    uint32_t typeID = enumMappers_.encode(2,  // "transactions",
        EdgeNode.get("type", "").asString());
    std::string occurTimeStr = EdgeNode.get("occurTime", "").asString();
    if (occurTimeStr.size() > 0) {
      occurInfo.lastLinktime = occurInfo.firstLinktime =
          seconds_from_epoch(occurTimeStr);
    }
    if (occurInfo.lastLinktime == 0) {
      deltaWriter_.write((uint32_t)0);  // 0 pair of UDT
    } else {
      deltaWriter_.write((uint32_t)1);  // 1 pair of UDT
      occurInfo.linkCount = 1;
    }

    deltaWriter_.write(typeID);  // type 0
    deltaWriter_.write(sizeof(occurInfo), (char *) &occurInfo);  // UDT

    // attr 2: intensity REAL -- 4 bytes
    deltaWriter_.write(attributeoperator);
    float intensity = EdgeNode.get("intensity", 0.0).asFloat();
    deltaWriter_.write(intensity);
    // attr 3: isBridge bool -- 1 byte
    deltaWriter_.write(attributeoperator);
    bool isBridge = EdgeNode.get("isBridge", false).asBool();
    deltaWriter_.write(isBridge);
#endif
  }


  bool processOneEdge(Json::Value &EdgeNode) {
#if 0
    std::string fromIDStr = EdgeNode.get("startNode", "").asString();
    std::string toIDStr = EdgeNode.get("endNode", "").asString();
    VertexLocalId_t fromVid = getVidfromUid(fromIDStr);
    VertexLocalId_t toVid = getVidfromUid(toIDStr);
    /* write from=> to */
    uint8_t firstbyte = (((uint8_t) topology4::DeltaRecord_Edge)
        << topology4::DeltaRecordTypeBits)
        + (((uint8_t) topology4::DeltaAction_Insert));
    deltaWriter_.write_flag((uint8_t) topology4::DeltaRecord_Edge,
                            (uint8_t) topology4::DeltaAction_Insert);
    deltaWriter_.write(fromVid);
    deltaWriter_.write(toVid);
    deltaWriter_.write((uint32_t) 0);
    writeEdgeAttribute(EdgeNode);
    deltaWriter_.write_watermark();
#endif
    return !error_;
  }

  bool processOneVertex(Json::Value &VertexNode, bool &newVertex) {
#if 0
    uint8_t firstbyte = 0;
    newVertex = false;
    std::string nodeIDStr = VertexNode.get("id", "").asString();
    vidmap_itr_t itr = oneReq_maps_->idmaps_.find(nodeIDStr);
    if (itr != oneReq_maps_->idmaps_.end()) {  // should always succeed
      if (itr->second.new_) {
        newVertex = true;
        /* after match this, set flag new_ to false:
         * this serves 2 corner cases
         * 1) nodelist has duplicate id ...
         * 2) a new node exists in edge but not in node list
         *
         */
        itr->second.new_ = false;
      }

      deltaWriter_.write_flag(
          (uint8_t) topology4::DeltaRecord_Vertex,
          newVertex ?
              (uint8_t) topology4::DeltaAction_Insert :
              (uint8_t) topology4::DeltaAction_Update);
      deltaWriter_.write(itr->second.vid_);
      writeVertexAttribute(VertexNode);
      deltaWriter_.write_watermark();
    } else {
      errmsg_ = "error_: missing converted vid for " + nodeIDStr;
      error_ = true;
    }
#endif
    return !error_;
  }

  bool s2_parseJSON(std::string &requestStr, Json::Value &vertex_list_node,
                    Json::Value &edge_list_node) {

    Json::Value requestRoot;
    Json::Reader jsonReader;
    if (!jsonReader.parse(requestStr, requestRoot))
      return false;
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

  std::string s5_genReturn() {
    Json::Value response_root;
    response_root["debug"] = "";
    if (error_)
      response_root["message"] = errmsg_;
    else
      response_root["message"] = responsemsg_.str();
    response_root["error"] = error_;
    response_root["engine_time"] = getExecuteTime();
    delete oneReq_maps_;
    return response_root.toStyledString();
  }

  std::string process(std::string requestid, std::string requestStr) {
    Lock_t lock(mutex_);
    Json::Value vertex_list_node;
    Json::Value edge_list_node;
    if (s0_init(requestid)) {
      if (s1_processNewSegs()) {
        if (s2_parseJSON(requestStr, vertex_list_node, edge_list_node)) {
          if (s3_processVertices(vertex_list_node)) {
            if (validateNodes()) {
              s4_processEdges(edge_list_node);
            }
          }
        }
      }
    }
    return s5_genReturn();
  }

 private:
  /* Once processed all vertices in nodelist, the idmap for this request
   * shouldn't contain any new node. Otherwise, we have the case of new node
   * exists in edgelist only
   */
  bool validateNodes() {
    for (vidmap_itr_t itr = oneReq_maps_->idmaps_.begin();
        itr != oneReq_maps_->idmaps_.end(); itr++) {
      if (itr->second.new_) {
        errmsg_ = itr->first + " ";
        error_ = true;
      }
    }
    if (error_) {
      errmsg_ = "error: new nodes " + errmsg_
          + +"don't exist in nodelist but exist in edgelist.";
    }
    return !error_;
  }
}
;
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
