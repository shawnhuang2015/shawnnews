#ifndef UDIMPL_GSE_POSTJSON2DELTA_HPP_
#define UDIMPL_GSE_POSTJSON2DELTA_HPP_

#include <post_service/post_json2delta.hpp>
//#include <gutil/gjsonhelper.hpp>


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


// using builtin super class version
//    void writeVertexAttribute(Json::Value &VertexNode) {
//    }

// use built-in replace semantic
//    void writeEdgeAttribute(Json::Value &EdgeNode) {
//    }



    // copy of the super class version with one change to help translate ID's.  will propose
    // to change this in the super class version.
    bool processOneVertex(Json::Value &VertexNode, bool &newVertex) {
      std::string nodeIDStr = VertexNode.get("id", "").asString();
      uint32_t vtypeid =  (uint32_t)VertexNode.get("typeid", 0).asUInt();
      std::stringstream str;
      str<<vtypeid<<"_"<<nodeIDStr;
      nodeIDStr = str.str();
      typedef std::tr1::unordered_map<std::string, gutil::RequestIdMaps::vidInfo>::iterator vidmap_itr_t;
      vidmap_itr_t itr = oneReq_maps_->idmaps_.find(nodeIDStr);
      bool deleteflag = VertexNode.get("deleteFlag", "").asBool();
      bool deletePermanent = VertexNode.get("deletePermanent","").asBool();
      bool invalidFlag = VertexNode.get("invalidFlag", "").asBool();


      if(invalidFlag){
        return true;
      }
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
        size_t rollbackpos = deltaWriter_.getCurrentPosition();
        deltaWriter_.write_flag(
              (uint8_t) topology4::DeltaRecord_Vertex,
              deleteflag ?  (uint8_t) topology4::DeltaAction_Delete : (uint8_t) topology4::DeltaAction_Insert);
        deltaWriter_.write(itr->second.vid_);
        size_t attributepos = deltaWriter_.getCurrentPosition();
        if(deleteflag && !deletePermanent){
          deleted_vids_.insert(itr->second.vid_);
        }
        if(!deleteflag)
          writeVertexAttribute(VertexNode, vtypeid);
        deltaWriter_.write_watermark();
        //gutil::CompactWriter::PrintBytes(std::cout, "deltaWriter_", buffer_, deltaWriter_.getCurrentPosition());

      } else {
        errmsg_ = "error_: missing converted vid for " + nodeIDStr;
        error_ = true;
      }
      return !error_;
    }
  };
}   // UDIMPL_GSE_POSTJSON2DELTA_HPP_
#endif
