/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: Tango POC
 * loader_mapper.hpp: /src/customer/core_impl/gse_impl/loader_mapper.hpp
 *
 *  Created on: July, 2014
 *      Author: Andrey
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_MT_HPP_
#define SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_MT_HPP_

#include <gse2/loader/gse_single_server_loader_mt.hpp>

namespace UDIMPL {

/**
 * Override below when you need to customize the graph loading.
 * This is the mapper phase: user input line -> vertex/edge record
 */
class GSE_UD_Loader_MT : public gse2::GseSingleServerLoaderMT {
 public:
  GSE_UD_Loader_MT(gse2::WorkerConfig &wrkConfig,
                   uint32_t numParallelLoaders, char separator)
      : gse2::GseSingleServerLoaderMT(wrkConfig, numParallelLoaders,
                                      separator, 1000000) {
  }

  ~GSE_UD_Loader_MT() {
  }

  /**
   * Entry point to load the vertex source
   * @param[in] DataSource: contains data source from command line
   * @return Nothing
   */
  void LoadVertexData(std::vector<std::string> &DataSource) {
    // DataSource[0] contains vertex source file
    char * phone_ptr;
    size_t phone_len;
    fileReader_ = new gutil::FileLineReader(DataSource[0]);
    VLOG(1) << "Opened first input file (should be Vertex file).";
    char * uid_ptr;
    size_t uid_len;
    bool is_existingVertexID = false;
    bool is_existingTangoID = false;

    bool readingUIDOnlyLines = true;  // we start out reading UID file,
    //until we see the marker "END_OF_readingUIDOnlyLines"

    // for UID alias we add "UUU" prefix, so we don't confuse them with phone numbers
    char uid_alias[30];
    uid_alias[0] = 'U';
    uid_alias[1] = 'U';
    uid_alias[2] = 'U';

    vertexWriter_.start_counter();
    lineNumber_ = 0;
    while (fileReader_->MoveNextLine()) {
      ;  // code here
    }  // end of for each line
    fileReader_->CloseFile();
    delete fileReader_;
    vertexWriter_.stop_counter();

    LOG(INFO) << "I'm done loading " << lineNumber_
        << " vertices. Error counts: ";
  }

  /**
   * Process one edge file. Every edgewriter API need uses taskID
   * @param[in] fileName
   * @param[in/out] taskId: threadID
   * @param[in/out] line_count
   */

  void LoadOneEdgeFile(std::string fileName,
                       uint32_t taskId,
                       uint64_t &line_count) {
    gutil::FileLineReader* localFileReader = new gutil::FileLineReader(
        fileName);
    while (localFileReader->MoveNextLine()) {
      line_count++; // code here
    }  // end of for each line
    localFileReader->CloseFile();
    delete localFileReader;
  }

};

}    // namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERMAPPER_MT_HPP_ */
