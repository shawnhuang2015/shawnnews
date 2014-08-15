/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0 --> POC
 * loader_mapper.hpp: /src/customer/core_impl/gse_impl/loader_mapper.hpp
 *
 *  Created on: May 5, 2014
 *      Author: xxxx
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_HPP_
#define SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_HPP_

#include <gse2/loader/gse_single_server_loader.hpp>

namespace UDIMPL {

/**
 * Override below when you need to customize the graph loading.
 * This is the mapper phase: user input line -> vertex/edge record
 */
class GSE_UD_Loader : public gse2::GseSingleServerLoader {
 public:
  GSE_UD_Loader(gse2::WorkerConfig &wrkConfig, char separator)
      : gse2::GseSingleServerLoader(wrkConfig, separator) {
  }

  ~GSE_UD_Loader() {
  }


  /**
   * This is a local function to generate vertex attributes
   * @return Nothing
   */
  void GenVertexAttributeRecord(uint32_t vtype) {
/* example data: uid,vtype,att1,att2
    v0_1,0,
    v1_1,1,11,0
    v1_2,1,12,0
    v1_3,1,13,0
    v1_4,1,14,0
    v3_1,3,31,0
    v3_2,3,32,0
    v3_3,3,33,0
    v2_1,2,21abc21,1
    v2_2,2,22xyz22,1
    v2_3,2,23zzz23,0
    v0_2,0
*/
    uint64_t val;
    size_t vatt_len;
    char *input_ptr = 0;
    size_t input_len = 0;
    switch (vtype) {
      case 0:
        break;
      case 1:
      case 3:
        break;
        fileReader_->NextUnsignedLong(val, separator_);
        vertexWriter_.write(val);  // int
        fileReader_->NextUnsignedLong(val, separator_);
        vertexWriter_.write((val==0));  // bool
      case 2:
        fileReader_->NextString(input_ptr, input_len, separator_);
        vertexWriter_.write(input_ptr, input_len);  // string
        fileReader_->NextUnsignedLong(val, separator_);
        vertexWriter_.write((val==0));  // bool
      default:
        break;
    }
}

  /**
   * Entry point to load the vertex source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
  void LoadVertexData(std::vector<std::string> &DataSource) {
    fileReader_ = new gutil::FileLineReader(DataSource[0]);
    char * uid_ptr;
    size_t uid_len;
    uint64_t vtype;
    bool is_existingVertexID = false;
    vertexWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      fileReader_->NextString(uid_ptr, uid_len, separator_);
      fileReader_->NextUnsignedLong(vtype, separator_);
      VERTEXID_T vid = upsertNow(uid_ptr, uid_len, vtype,
                                 is_existingVertexID);
      if (!is_existingVertexID) {
        GenVertexAttributeRecord(vtype);
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
#if 0
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
#endif
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
    size_t uid1_len;
    char * uid2_ptr;
    size_t uid2_len;
    bool is_existingVertexID = false;
    edgeWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* 1,2
       * 2,1
       */
      fileReader_->NextString(uid1_ptr, uid1_len, separator_);
      fileReader_->NextString(uid2_ptr, uid2_len, separator_);
      uid1_ptr[uid1_len] = '\0';
      uid2_ptr[uid2_len] = '\0';
      VERTEXID_T from_vid = upsertNow(uid1_ptr, 0, is_existingVertexID);
      VERTEXID_T to_vid = upsertNow(uid2_ptr, 0, is_existingVertexID);
      edgeWriter_.flush(0, from_vid, to_vid, isDirectedEdge(0), 0, 0);
    }
    fileReader_->CloseFile();
    delete fileReader_;
    edgeWriter_.stop_counter();

#if 0
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
#endif
  }

};

}    // namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_HPP_ */
