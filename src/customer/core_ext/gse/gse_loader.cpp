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
#include "../../core_impl/gse_impl/loader_combiner.hpp"
#include "../../core_impl/gse_impl/loader_mapper.hpp"
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
  gmmnt::GlobalInstances instance("/Users/like/eclipse/workspace/2.0/gdbms/ids_test/gpe.conf");
  topology4::TopologyGraph topology(&instance, datapath);
  topology4::TopologyPrinter topologyprinter(&instance, &topology);
  topologyprinter.PrintMeta();
  topologyprinter.PrintVertexAttributes(true);
  topologyprinter.PrintEdges(
      topology4::EdgeBlockReaderSetting(true, true, true));
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
     * id
     * separator
     * vertex_file
     * edge_file
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
  gse2::GseSingleServerLoader *single_load_worker;
  if (sysConfig.isValidServerWorker(worker_id)) {
    if (argc == 2) {
      /* single server loader with both vertex and edge files */
      /* separator can be char or int value */
      char separator='\t';
      if (config_info[3].length() > 1) {
        separator = (char)(atoi(config_info[3].c_str()));
      } else {
        separator = config_info[3][0];
      }
      std::cout << " Separator : \"" << separator << "\"" << std::endl;
      single_load_worker = new UDIMPL::GSE_UD_Loader(workerConfig, separator);
      std::vector<std::string> inputFiles;
      for (uint32_t i = 4; i < config_info.size(); i++) {
        inputFiles.push_back(config_info[i]);
      }
      single_load_worker->LoadVertexData(inputFiles);
      single_load_worker->LoadEdgeData(inputFiles);
      single_load_worker->commitLoading();
      single_load_worker->singleSrvPartition();
      /* below will printout the whole graph */
      // LoadTopology4("/data/rc4/gstore/0/part");
      exit(0);
    } else {
      worker = new gse2::IdsServerWorker(workerConfig);
      std::cout << *(gse2::IdsServerWorker*)worker << "starts" << std::endl;
    }
  } else if ( sysConfig.isValidClientWorker(worker_id)) {
    std::cout << "invalid parameter!" << std::endl;
    exit(-1);
  } else {
    std::cout << " !!!! invalid worker id (" << worker_id << ")" << std::endl;
    return -1;
  }


  worker->startup();
  worker->run(opts);
  worker->shutdown();
  delete worker;
  return 0;
}



