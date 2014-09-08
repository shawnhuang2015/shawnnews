/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0
 * ids_main.cpp: /gse_2.0/src/graphsql/server/gse2/ids/ids_main.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: like
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>
#include <iostream>
#include <lib/stddefines.h>
#include <gse2/config/sys_config.hpp>
#include <gse2/ids/impl/ids_client_worker.hpp>
#include <gse2/ids/impl/ids_server_worker.hpp>
#include <core_impl/gse_impl/loader_combiner.hpp>
#include <core_impl/gse_impl/loader_mapper.hpp>
#include <core_impl/gse_impl/loader_mapper_mt.hpp>
#include <core_impl/gse_impl/loader_mapper_distributed.hpp>
#include <gse2/partition/gse_single_server_repartition.hpp>
#include <gutil/glogging.hpp>
#include <gutil/gstring.hpp>


#include <gutil/filelinereader.hpp>
#include "topology4/topologyprinter.hpp"
#include "topology4/deltarebuilder.hpp"
#include "gpelib4/engine/master.hpp"
#include "gpelib4/engine/worker.hpp"
#include "gpelib4/engine/singlemachineadaptor.hpp"
#include "gpelib4/enginedriver/enginedriverservice.hpp"
#include "gpelib4/enginedriver/singleadaptorcache.hpp"
std::string enginecfgfile_ = "";
std::string topologycfgfile_ = "";
std::string outputpath_ = "/tmp/gpe";


void LoadTopology4(std::string datapath) {
  gmmnt::GlobalInstances instance("/data/rc4/config/gpe/gpe1.conf");
  topology4::TopologyGraph topology(&instance, datapath);
  topology4::TopologyPrinter topologyprinter(&instance, &topology);
  topologyprinter.PrintMeta();
  topologyprinter.PrintVertexAttributes(true);
  topologyprinter.PrintEdges(
      topology4::EdgeBlockReaderSetting(true, true, true));
}

void gse_loading_help(std::string config_file) {
  std::cout << "Please check " << config_file << std::endl;
  std::cout << "The expected input in gse_loader1.propery: \n"
    << "gse_loader1.config\n"
    << "graph_config\n"
    << "server_id (1)\n"
    << "separator (, 09, ...)\n"
    << "[MT]  (MT: parallel edge loading using multiple threads)\n"
    << "[10]  (if above is MT: number of parallel threads)\n"
    << "vertex_file or vertex_dir\n"
    << "edge_file or edge_dir\n"
    << "DUMP (put this keyword enables loaded topology displayed on screen)\n" << std::endl;
  exit(-1);
}

static char __getSeparator(std::string separatorStr) {
  char separator='\t';
  if (separatorStr.length() > 1) {
    separator = (char)(atoi(separatorStr.c_str()));
  } else {
    separator = separatorStr[0];
  }
  std::cout << " Separator : \"" << separator << "\"" << std::endl;
  return separator;
}

int main(int argc, char ** argv) {
#ifdef BUILDVERSION
  std::string versStr = BUILDVERSION;
  std::replace(versStr.begin(), versStr.end(), ',', '\n');
  std::cout << "GraphSQL 2.0 Graph Loader: \n --- Version --- \n" << versStr << std::endl;
#endif
  Gsql_Fd_Checker gsql_fd_checker(100000);
  std::vector<std::string> opts;
  if (!gsql_fd_checker.passed_)
    exit(-1);
  if (argc < 2) {
    printf("USAGE: %s <loading_worker_config_info>\n ", argv[0]);
    exit(1);
    /*
     * sys_config
     * graph_config
     * server_id (1)
     * separator (, 09, ...)
     * [MT]  (MT: parallel edge loading using multiple threads)
     * vertex_file or vertex_dir
     * edge_file or edge_dir
     * ...
     */
  }
  std::vector<std::string> config_info = gutil::tokenize_file(std::string(argv[1]));
  gse2::SysConfig sysConfig(config_info[0]);
  sysConfig.printout();
  GSQLLogger logger(argv[0], sysConfig.GLOG_DIR_ + "/gse_loader");
  gse2::GraphConfig graphConfig(config_info[1]);
  graphConfig.printout();
  uint32_t worker_id = atoi(config_info[2].c_str());
  gse2::WorkerConfig workerConfig(sysConfig, graphConfig, worker_id);
  gse2::IdsWorker *worker = 0;
  if (sysConfig.isValidServerWorker(worker_id)) {
    if (argc == 2) {
      // init dumpGraph flag to false
      bool dumpGraph = false;
      /* single server loader with both vertex and edge files */
      /* separator can be char or int value */
      if (config_info.size() < 6)
        gse_loading_help(std::string(argv[1]));
      char separator = __getSeparator(config_info[3]);
      if (config_info[4] == "MT") {
        UDIMPL::GSE_UD_Loader_MT single_load_worker_mt(workerConfig,
                                                       atoi(config_info[5].c_str()),
                                                       separator);
        std::vector<std::string> inputFiles;
        for (uint32_t i = 6; i < config_info.size(); i++) {
          // check if dump is enabled
          if (config_info[i] == "DUMP") {
            dumpGraph = true;
          } else {
            inputFiles.push_back(config_info[i]);
          }
        }
        std::cout << " DUMP Graph: " << std::boolalpha << dumpGraph << std::endl;
        single_load_worker_mt.LoadVertexData(inputFiles);
        single_load_worker_mt.LoadEdgeData(inputFiles);
        single_load_worker_mt.commitLoading();
        single_load_worker_mt.singleSrvPartition();
      } else {
        UDIMPL::GSE_UD_Loader single_load_worker(workerConfig, separator);
        std::vector<std::string> inputFiles;
        for (uint32_t i = 4; i < config_info.size(); i++) {
          // check if dump is enabled
          if (config_info[i] == "DUMP") {
            dumpGraph = true;
          } else {
            inputFiles.push_back(config_info[i]);
          }
        }

        std::cout << " DUMP Graph: " << std::boolalpha << dumpGraph << std::endl;
        single_load_worker.LoadVertexData(inputFiles);
        single_load_worker.LoadEdgeData(inputFiles);
        single_load_worker.commitLoading();
        single_load_worker.singleSrvPartition();
      }
      /* below will printout the whole graph if DUMP is appear after 4th line 
       * in property file
       */
      if (dumpGraph) {
        LoadTopology4(workerConfig.partitionRootDir_);
      }
      exit(0);
    } else {
      std::cout << " !!!! invalid input" << std::endl;
      return -1;
    }
  } else if ( sysConfig.isValidClientWorker(worker_id)) {
    bool dumpGraph = false;
    /* single server loader with both vertex and edge files */
    /* separator can be char or int value */
    if (config_info.size() < 6)
      gse_loading_help(std::string(argv[1]));
    char separator = __getSeparator(config_info[3]);
    UDIMPL::GSE_UD_Loader_Dist parallel_load_worker(workerConfig,
                                                    separator);
    std::vector<std::string> inputFiles;
    for (uint32_t i = 4; i < config_info.size(); i++) {
      // check if dump is enabled
      if (config_info[i] == "DUMP") {
        dumpGraph = true;
      } else {
        inputFiles.push_back(config_info[i]);
      }
    }
    std::cout << " DUMP Graph: " << std::boolalpha << dumpGraph << std::endl;
    parallel_load_worker.run(inputFiles);
  } else {
    std::cout << " !!!! invalid worker id (" << worker_id << ")" << std::endl;
    return -1;
  }
  return 0;
}



