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
#include <gse2/loader/canonnical_loader_config.hpp>
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


void LoadTopology4(std::string datapath, std::string graphStore) {
  //generate  a dummy gpe config file ".dump_config.yaml"
  std::string dumpConfigFile = ".dump_config.yaml";
  ofstream dumpConfig;
  dumpConfig.open (dumpConfigFile.c_str());
  dumpConfig<< "INSTANCE:\n";
  dumpConfig<< "    cache_store_path: /data/rc4/tmp/cache_store\n";
  dumpConfig<< "    engine_setting:\n";
  dumpConfig<< "        bucket_split_perpartition: 1\n";
  dumpConfig<< "        num_running_threads: 0\n";
  dumpConfig<< "    executable: poc_gpe_server\n";
  dumpConfig<< "    get_request_timeoutsec: 10\n";
  dumpConfig<< "    graph_partition_snapshot:\n";
  dumpConfig<< "        snapshot_path: " <<graphStore<<"\n";
  dumpConfig<< "    ids_id: 1\n";
  dumpConfig<< "    ip: 127.0.0.1\n";
  dumpConfig<< "    machine_config:\n";
  dumpConfig<< "        disks:\n";
  dumpConfig<< "        -   compress_method: None\n";
  dumpConfig<< "            num_load_threads: 1\n";
  dumpConfig<< "            num_save_threads: 1\n";
  dumpConfig<< "            path: /data/rc4/tmp/gdisks\n";
  dumpConfig<< "            type: SSD\n";
  dumpConfig<< "        memory_limit: 32768\n";
  dumpConfig<< "        num_memory_server_threads: 2\n";
  dumpConfig<< "    name: GPE_Server_1\n";
  dumpConfig<< "    num_max_running_instances: 6\n";
  dumpConfig<< "    num_post_threads: 3\n";
  dumpConfig<< "    port: 7001\n";
  dumpConfig<< "    prefetch_request_limit: 12\n";
  dumpConfig<< "    prefetch_request_timeoutsec: 5\n";
  dumpConfig<< "    queue_client_name_prefix: gpe\n";
  dumpConfig.close();
  gmmnt::GlobalInstances instance(dumpConfigFile);
  topology4::TopologyGraph topology(&instance, datapath); 
  topology4::TopologyPrinter topologyprinter(&instance, &topology); 
  topologyprinter.PrintMeta(); 
  topologyprinter.PrintVertexAttributes(true); 
  topologyprinter.PrintEdges( 
      topology4::EdgeBlockReaderSetting(true, true, true)); 
  remove(dumpConfigFile.c_str());

} 

/**
 * The Driver take two arguments:
 *
 * arg1: the intermediate yaml file
 * arg2: the graph schema yaml file
 *
 */
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
  //init loader config
  //gse2::Canonnical_Loader_Config loaderConfig("/home/vagrant/neo4j.yaml");
  std::string tmpYaml(argv[1]);
  gse2::Canonnical_Loader_Config loaderConfig(tmpYaml);

  gse2::SysConfig sysConfig(argv[3]); 
  std::string graphStore = sysConfig.graphs_.store_root_;
  sysConfig.printout(); 
  gutil::GSQLLogger logger(argv[0], sysConfig.GLOG_DIR_ + "/gse_loader"); 

  std::string graphConfigPath(argv[2]);
  //gse2::GraphConfig graphConfig(/vagrant/repo2/product/bin/output/neo4j.yaml"); 
  gse2::GraphConfig graphConfig(graphConfigPath); 
  graphConfig.printout(); 
  uint32_t worker_id = 1; 
  gse2::WorkerConfig workerConfig(sysConfig, graphConfig, worker_id);
  if (sysConfig.isValidServerWorker(worker_id)) { 
    bool dumpGraph = loaderConfig.Dump;
    char separator = ','; 
    gse2::GSE_CANONNICAL_LOADER single_load_worker(workerConfig, loaderConfig, separator); 
    std::vector<std::string> inputFiles; 
    single_load_worker.LoadVertexData(inputFiles); 
    single_load_worker.LoadEdgeData(inputFiles); 
    single_load_worker.commitLoading(); 
    single_load_worker.singleSrvPartition(); 
    if (dumpGraph) { 
      LoadTopology4(workerConfig.partitionRootDir_,graphStore); 
    } 
  }  
  return 0; 
}
