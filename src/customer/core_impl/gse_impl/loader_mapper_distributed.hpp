/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0 --> POC
 * loader_mapper_distributed.hpp: /src/customer/core_impl/gse_impl/loader_mapper_distributed.hpp
 *
 *  Created on: Sept 5, 2014
 *      Author: xxxx
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_DISTIBUTED_HPP_
#define SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_DISTIBUTED_HPP_

#include <gse2/loader/gse_parallel_loader.hpp>

namespace UDIMPL {

/**
 * Override below when you need to customize the graph loading.
 * This is the mapper phase: user input line -> vertex/edge record
 */
class GSE_UD_Loader_Dist : public gse2::GseParallelLoader {
 public:
  GSE_UD_Loader_Dist(gse2::WorkerConfig &wrkConfig, char separator)
      : gse2::GseParallelLoader(wrkConfig, separator) {
  }

  ~GSE_UD_Loader_Dist() {
  }


  /**
   * This is a local function to generate vertex attributes
   * @return Nothing
   */
  void GenVertexAttributeRecord() {
#if 0
// example code:
   /* 1st -> is a enumerator
      2nd -> is int
      3rd -> is bool
   */
    uint64_t val;
    size_t vatt_len;
    char *input_ptr = 0;
    size_t input_len = 0;
    /**** first ***/
    fileReader_->NextString(input_ptr, input_len, separator_);
    uint32_t encoded_id = enumMappers_.encode( 1, // "id_type",
        std::string(input_ptr, input_len));
    vertexWriter_.write(encoded_id);  // attribute 1: id_type
    /**** 2nd ***/
      fileReader_->NextUnsignedLong(val, separator_);
      vertexWriter_.write(val);  // int
    /**** 3nd ***/
      fileReader_->NextUnsignedLong(val, separator_);
      vertexWriter_.write((val==0));  // bool
#endif
}

  /**
   * Entry point to load the vertex source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
  void LoadVertexData(std::vector<std::string> &DataSource) {
    // DataSource[0] contains the vertex source file
#if 0
    fileReader_ = new gutil::FileLineReader(DataSource[0]);
    char * uid_ptr;
    size_t uid_len;
    bool is_existingVertexID = false;
    vertexWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* 0#UserID,0,9,0,8,0,0,0,0
       * note UserID above will be an integer for vertex typeID
       */
      fileReader_->NextString(uid_ptr, uid_len, separator_);
      GenVertexAttributeRecord();
      vertexWriter_.flush(0, uid_ptr, uid_len);
    }
    fileReader_->CloseFile();
    delete fileReader_;
    vertexWriter_.stop_counter();
#endif
  }

  /**
   * Standard function to generate edge attributes
   * @return Nothing
   */
  void GenEdgeAttributeRecord() {
#if 0
    // this is the alipay example. this data is also combined in loader_combiner.hpp
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

  void LoadWeightedIntEdges(std::vector<std::string> &DataSource){
    // DataSource[1] contains the edge source file
    fileReader_ = new gutil::FileLineReader(DataSource[1]);
    char * uid1_ptr;
    size_t uid1_len;
    char * uid2_ptr;
    size_t uid2_len;
    uint64_t ulong_attr;
    bool is_existingVertexID = false;
    edgeWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* 1,2,6
       * 2,1,5
       */
      fileReader_->NextString(uid1_ptr, uid1_len, separator_);
      fileReader_->NextString(uid2_ptr, uid2_len, separator_);
      // write the weight
      fileReader_->NextUnsignedLong(ulong_attr,separator_);
      // edge attribute
      edgeWriter_.write(ulong_attr);
      edgeWriter_.flush(0, 0, 0, uid1_ptr, uid1_len, uid2_ptr, uid2_len);
    }
    fileReader_->CloseFile();
    delete fileReader_;
    edgeWriter_.stop_counter();
  }

  void LoadPlainEdges(std::vector<std::string> &DataSource){
    // DataSource[1] contains the edge source file
    fileReader_ = new gutil::FileLineReader(DataSource[1]);
    char * uid1_ptr;
    size_t uid1_len;
    char * uid2_ptr;
    size_t uid2_len;
    edgeWriter_.start_counter();
    while (fileReader_->MoveNextLine()) {
      /* 1,2
       * 2,1
       */
      fileReader_->NextString(uid1_ptr, uid1_len, separator_);
      fileReader_->NextString(uid2_ptr, uid2_len, separator_);
      edgeWriter_.flush(0, 0, 0, uid1_ptr, uid1_len, uid2_ptr, uid2_len);
    }
    fileReader_->CloseFile();
    delete fileReader_;
    edgeWriter_.stop_counter();
  }

  void LoadPlainEdgesWithDefaultVertex(std::vector<std::string> &DataSource){
      // DataSource[1] contains the edge source file
    fileReader_ = new gutil::FileLineReader(DataSource[1]);
    char * uid1_ptr;
    char * uid2_ptr;
    size_t uid1_len;
    size_t uid2_len;
    uint64_t ulong_attr;
    bool is_existingVertexID = false;

    /* In case we want to give some default value of a new vertex */
    // MODIFICATION NEEDED HERE
    // this writes all 0's to the vertex attribute space.
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
      /* write out the edge */
      edgeWriter_.flush(0, 0, 0,
                        uid1_ptr, uid1_len, defaultV_attr, default_verextAttr_length,
                        uid2_ptr, uid2_len, defaultV_attr, default_verextAttr_length);
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
    LoadPlainEdges(DataSource);
  }

};

}    // namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_DISTIBUTED_HPP_ */
