/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved. 
 * Purpose:  generated data loader 
 * Generated on: 05/11/2014 
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
#include <core_impl/gse_impl/canonnical_loader_mapper.hpp> 
#include <core_impl/gse_impl/loader_mapper_mt.hpp> 
#include <core_impl/gse_impl/loader_mapper_distributed.hpp> 
#include <gse2/partition/gse_single_server_repartition.hpp> 
#include <gse2/loader/canonnical_loader_mapper.hpp> 
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


void LoadTopology4(std::string datapath) { 
  gmmnt::GlobalInstances instance("/data/rc4/config/gpe/gpe1.conf");
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
  if (!gsql_fd_checker.passed_) { 
    exit(-1); 
  } 
  gse2::Canonnical_Loader_Config loaderConfig("/home/vagrant/neo4j.yaml");

  gse2::SysConfig sysConfig("/data/rc4/config/gse_loader/gse_loader1.conf"); 
  sysConfig.printout(); 
  GSQLLogger logger(argv[0], sysConfig.GLOG_DIR_ + "/gse_loader"); 
  gse2::GraphConfig graphConfig("/vagrant/repo2/product/bin/output/neo4j.yaml"); 
  graphConfig.printout(); 
  uint32_t worker_id = 1; 
  gse2::WorkerConfig workerConfig(sysConfig, graphConfig, worker_id);
  if (sysConfig.isValidServerWorker(worker_id)) { 
    bool dumpGraph = true; 
    char separator = ','; 
    gse2::GSE_CANONNICAL_LOADER single_load_worker(workerConfig, separator); 
    std::vector<std::string> inputFiles; 
    single_load_worker.LoadVertexData(inputFiles); 
    single_load_worker.LoadEdgeData(inputFiles); 
    single_load_worker.commitLoading(); 
    single_load_worker.singleSrvPartition(); 
    if (dumpGraph) { 
      LoadTopology4(workerConfig.partitionRootDir_); 
    } 
  } 
  return 0; 
}
