/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0
 * loader_impl.hpp: /src/customer/core_impl/gse_impl/loader_mapper.hpp
 *
 *  Created on: May 5, 2014
 *      Author: like
 ******************************************************************************/

#ifndef UDIMPL_GSE_UD_LOADER_MAPPER_HPP_
#define UDIMPL_GSE_UD_LOADER_MAPPER_HPP_

#include <gse2/loader/gse_single_server_loader.hpp>

namespace UDIMPL {
class GSE_UD_Loader : public gse2::GseSingleServerLoader {
 public:
  GSE_UD_Loader(gse2::WorkerConfig &wrkConfig, char separator)
      : gse2::GseSingleServerLoader(wrkConfig, separator) {
  }

  ~GSE_UD_Loader() {
  }

  /**
   *
   * @param attr_ptr
   * @param attr_len
   * @param uid1_ptr
   * @param uid1_len
   *  - type: "INT"
   name: "id_type"
   nullable: false
   fixedsize: 0    # for base data type, fixedsize doesn't matter.
   enumerator: 1   # this is enumerator ID, from 1 to $attribute_enumerators
   - type: "BOOL"
   name: "isRisk"
   nullable: false
   fixedsize: 0
   - type: "INT"
   name: "numOfRisks"
   nullable: false
   - type: "BOOL"
   name: "isReject"
   nullable: false
   - type: "INT"
   name: "rejectCount"
   nullable: false
   - type: "BOOL"
   name: "isBlack"
   nullable: false
   - type: "BOOL"
   name: "isWhite"
   nullable: false
   - type: "BOOL"
   name: "isKeyNode"
   nullable: false
   - type: "BOOL"
   name: "chargeStatus"
   nullable: false
   *
   */

  /**
   * This is a local function to generate vertex attributes
   * @return Nothing
   */
  void localGenVertexAttribute() {
    uint64_t val;
    size_t vatt_len;
    char *input_ptr = 0;
    size_t input_len = 0;
    fileReader_->NextString(input_ptr, input_len, separator_);
    uint32_t encoded_id = enumMappers_.encode( 1, // "id_type",
        std::string(input_ptr, input_len));
    vertexWriter_.write(encoded_id);  // attribute 1: id_type
    /* 4,0,9,0,8,0,0,0,0  :  4 is converted type ID */
    /* 1,2,3,4,5,6,7,8,9 : position */
    for (uint32_t i = 2; i <= 9; i++) {
      fileReader_->NextUnsignedLong(val, separator_);
      if (i == 3 || i == 5) {
        vertexWriter_.write(val);  // attribute 3 and 5 are integer.
      } else {
        vertexWriter_.write(val != 0);
      }
    }
  }

  /**
   * Entry point to load the vertex source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
  void LoadVertexData(std::vector<std::string> &DataSource) {
    // DataSource[0] contains the vertex source file
    fileReader_ = new gutil::FileLineReader(DataSource[0]);
    char * uid_ptr;
    size_t uid_len;
    static int bufferlen = 100;
    char attr_ptr[bufferlen];
    size_t attr_len;
    bool is_existingVertexID = false;
    vertexWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* 0#UserID,0,9,0,8,0,0,0,0
       * note UserID above will be an integer for vertex typeID
       */
      fileReader_->NextString(uid_ptr, uid_len, separator_);
      VERTEXID_T vid = upsertNow(uid_ptr, uid_len, 0,
                                 is_existingVertexID);
      if (!is_existingVertexID) {
        localGenVertexAttribute();
        vertexWriter_.flush(vid);
      }
    }
    fileReader_->CloseFile();
    delete fileReader_;
    vertexWriter_.stop_counter();
  }

  /**
   * Standard function to generate edge attributes
   * @return Nothing
   */
  void GenEdgeAttributeRecord() {
    char * etype_ptr;
    size_t etype_len;
    /* TA,400756276684 */
    fileReader_->NextString(etype_ptr, etype_len, separator_);
    uint32_t compid = enumMappers_.encode(2, // "transactions",
        std::string(etype_ptr, etype_len));
    edgeWriter_.write(compid);  // UDT->KEY

    uint64_t ulong_attr;
    fileReader_->NextUnsignedLong(ulong_attr, separator_);
    edgeWriter_.write(ulong_attr);  // UDT->VAL
  }

  /**
   * Entry point to load the edge source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
  void LoadEdgeData(std::vector<std::string> &DataSource) {
    // DataSource[1] contains the edge source file
    fileReader_ = new gutil::FileLineReader(DataSource[1]);
    char * uid1_ptr;
    char * uid2_ptr;
    size_t uid1_len;
    size_t uid2_len;
    uint64_t ulong_attr;
    bool is_existingVertexID = false;

    /* In case we want to give some default value of a new vertex */
    static int default_verextAttr_length = 10;
    char defaultV_attr[default_verextAttr_length];
    memset(defaultV_attr, 0, default_verextAttr_length);

    /* 0: print start */
    edgeWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* 6UserID,600UserID,TB,131332265729
       */
      fileReader_->NextString(uid1_ptr, uid1_len, separator_);
      fileReader_->NextString(uid2_ptr, uid2_len, separator_);
      GenEdgeAttributeRecord();
      /* If fromID is a new vertex, add it to vertex table w/o attribute */
      VERTEXID_T from_vid = upsertNow(uid1_ptr, uid1_len, 0, is_existingVertexID);
      if (!is_existingVertexID) {
        vertexWriter_.flush(from_vid, defaultV_attr, 0);
      }
      /* If toID is a new vertex, add it to vertex table w/o attribute */
      VERTEXID_T to_vid = upsertNow(uid2_ptr, uid2_len, 0, is_existingVertexID);
      if (!is_existingVertexID) {
        vertexWriter_.flush(to_vid, defaultV_attr, 0);
      }
      /* write out the edge */
      edgeWriter_.flush(0, from_vid, to_vid, isDirectedEdge(0));
    }
    fileReader_->CloseFile();
    delete fileReader_;

    edgeWriter_.stop_counter();
  }

};

}    // namespace UDIMPL
#endif    /* UDIMPL_GSE_UD_LOADER_MAPPER_HPP_ */
