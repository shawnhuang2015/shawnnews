#ifndef UDIMPL_GSE_POSTJSON2DELTA_HPP_
#define UDIMPL_GSE_POSTJSON2DELTA_HPP_

#include <post_service/post_json2delta.hpp>

namespace UDIMPL {
  /**
   * Override below when you need to customize the JSON2Delta
   */
  typedef std::tr1::unordered_map<std::string, gutil::RequestIdMaps::vidInfo>::iterator vidmap_itr_t;

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
    */

    bool processOneEdge(Json::Value &EdgeNode) {
      std::cout << "process edge" << std::endl;
      return true;
    }

    bool processOneVertex(Json::Value &VertexNode, bool &newVertex) {
      newVertex = false;
      std::string vid = VertexNode.get("id", "").asString();
      uint32_t vtypeid = (uint32_t) VertexNode.get("typeid", 0).asUInt();
      std::stringstream str;
      str << vtypeid << "_" << vid;
      std::cout << "check str: " << str.str() << std::endl;
      vid = str.str();
      vidmap_itr_t itr = oneReq_maps_->idmaps_.find(vid);

      if(itr != oneReq_maps_->idmaps_.end()) {
        if(itr->second.new_){
          newVertex = true;
          itr->second.new_ = false;
        }

        deltaWriter_.write_flag(
            (uint8_t) topology4::DeltaRecord_Vertex,
            newVertex ? (uint8_t) topology4::DeltaAction_Insert :
            (uint8_t) topology4::DeltaAction_Update);
        deltaWriter_.write(itr->second.vid_);
        std::cout << "write vid: " << itr->second.vid_ << std::endl;
        if (VertexNode.isMember("isFraudulent")) {
          deltaWriter_.write((uint8_t) topology4::DeltaAttributeOperator_Overwrite);
          deltaWriter_.write(VertexNode["fraud"] == "true");
          std::cout << "write attribute: " << (VertexNode["fraud"] == "true") << std::endl;
        } else {
          deltaWriter_.write((uint8_t) topology4::DeltaAttributeOperator_IgnoreIfExisted);
          deltaWriter_.write(false);
        }
        if (VertexNode.isMember("label")) {
          deltaWriter_.write((uint8_t) topology4::DeltaAttributeOperator_Overwrite);
          deltaWriter_.write(VertexNode["label"].asString().c_str());
          std::cout << "write attribute: " << VertexNode["fraud"].asString().c_str() << std::endl;
        } else {
          deltaWriter_.write((uint8_t) topology4::DeltaAttributeOperator_IgnoreIfExisted);
          deltaWriter_.write("");
        }
        //writeVertexAttribute(VertexNode, vtypeid);
        deltaWriter_.write_watermark();
      } else {
        errmsg_ = "error_: missing converted vid for " + vid;
        error_ = true;
      }
      return ! error_;
    }
  };
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
