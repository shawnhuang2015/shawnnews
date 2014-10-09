/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0 --> POC
 * loader_mapper.hpp: /src/customer/core_impl/gse_impl/loader_mapper.hpp
 *
 *  Created on: Aug 21, 2014
 *      Author: Like
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

  void GetVertexDefaultAttribute(uint32_t vtype) {
    char buffer[10];
    uint32_t len =0;
    memset(buffer, 0, 10);
    switch (vtype) {
      case 0:
        len = 0;
        break;
      case 1:
      case 3:
        len = 2;
        break;
      case 2:
        len = 2;
        break;
      default:
        GASSERT(false, "invalid vertex type");
        break;
    }
    if (len > 0) {
      vertexWriter_.write(len, buffer);
    }

  }

  /**
   * This is a local function to generate vertex attributes
   * @return Nothing
   */
  void GenVertexAttributeRecord(uint32_t vtype) {
    uint64_t val;
    size_t vatt_len;
    char *input_ptr = 0;
    size_t input_len = 0;
    switch (vtype) {
      case 0:
        break;
      case 1:
      case 3:
        fileReader_->NextUnsignedLong(val, separator_);
        vertexWriter_.write(val);  // int
        fileReader_->NextUnsignedLong(val, separator_);
        vertexWriter_.write((val!=0));  // bool
        break;
      case 2:
        fileReader_->NextString(input_ptr, input_len, separator_);
        vertexWriter_.write(input_ptr, input_len);  // string
        fileReader_->NextUnsignedLong(val, separator_);
        vertexWriter_.write(val);  // int
      default:
        break;
    }
}

  /**
   * Entry point to load the vertex source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
/* v.txt:  userID,vtype,attributes
v0_1,0,
v1_1,1,11,0
v1_2,1,12,0
v1_3,1,13,0
v1_4,1,14,0
v3_1,3,31,0
v3_2,3,32,0
v3_3,3,33,0
v2_1,2,21abc21,21
v2_2,2,22xyz22,22
v2_3,2,23zzz23,0
v0_2,0
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
   * @return edge type
   */
  uint32_t GenTypedEdgeAttributeRecord() {
    /* edgetype, value1, value2, ... */
    char * eval_ptr;
    size_t eval_len;
    uint64_t ulong_etype;
    fileReader_->NextUnsignedLong(ulong_etype, separator_);
    switch (ulong_etype) {
      case 0:
        // no attribute
        break;
      case 1:
      {
        /* attribute is: int, string */
        uint64_t ulong_attr;
        fileReader_->NextUnsignedLong(ulong_attr, separator_);
        edgeWriter_.write(ulong_attr);

        fileReader_->NextString(eval_ptr, eval_len, separator_);
        edgeWriter_.write(eval_ptr, eval_len);
      }
        break;
      case 2:
      {
        fileReader_->NextString(eval_ptr, eval_len, separator_);
        edgeWriter_.write(eval_ptr, eval_len);
      }
        break;
      default:
        GASSERT(false, "wrong edge type");
        break;
    }
    return ulong_etype;
  }

/* e.txt: fromUID,from_vtype,toUID,to_vtype,edge_type,edge_attribute1,2...
1,0,2,0,0
2,0,3,0,0
3,0,1,0,0
v2_1,2,v3_1,3,1,1,e1_2_abc
v1_2,1,v3_2,3,2,e2_1_uvw
v1_2,1,v3_3,3,2,e2_2_uvw
v1_2,1,v0_0,0,2,e0_1_aaa
v1_2,1,v1_0,1,2,e1_1_aaa
v1_2,1,v1_1,1,1,2,e1_2_aa
v1_2,1,v1_2,1,1,3,e1_3_aa
v1_2,1,v0_1,0,0,e0_2_aa
v1_2,1,v0_2,0,0,e0_3_aa
v1_9,1,v3_7,3,0
*/
  void LoadTypedEdges(std::vector<std::string> &DataSource){
    // DataSource[1] contains the edge source file
    fileReader_ = new gutil::FileLineReader(DataSource[1]);
    char vertex_default_attribute[20];
    uint32_t vertex_default_attribute_len;
    char * uid1_ptr;
    size_t uid1_len;
    char * uid2_ptr;
    size_t uid2_len;
    bool is_existingVertexID = false;
    uint64_t vtype1 = 0;
    uint64_t vtype2 = 0;
    edgeWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* fromUID and its vtype */
      fileReader_->NextString(uid1_ptr, uid1_len, separator_);
      fileReader_->NextUnsignedLong(vtype1, separator_);
      /* toUID and its vtype */
      fileReader_->NextString(uid2_ptr, uid2_len, separator_);
      fileReader_->NextUnsignedLong(vtype2, separator_);
      /* get their vids: if new, add default value */
      VERTEXID_T from_vid = upsertNow(uid1_ptr,uid1_len, vtype1, is_existingVertexID);
      if (!is_existingVertexID) {
        GetVertexDefaultAttribute(vtype1);
        vertexWriter_.flush(from_vid);
      }
      VERTEXID_T to_vid = upsertNow(uid2_ptr,uid2_len, vtype2, is_existingVertexID);
      if (!is_existingVertexID) {
        GetVertexDefaultAttribute(vtype2);
        vertexWriter_.flush(to_vid);
      }
      /* process edge data: etype,eatt1,eatt2.... */
      uint32_t edgetype = GenTypedEdgeAttributeRecord();
      edgeWriter_.flush(edgetype, from_vid, to_vid, isDirectedEdge(edgetype));
    }
    fileReader_->CloseFile();
    delete fileReader_;
    edgeWriter_.stop_counter();
  }

  /**
   * Entry point to load the edge source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
  void LoadEdgeData(std::vector<std::string> &DataSource) {
    LoadTypedEdges(DataSource);
  }

};

}    // namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_HPP_ */
