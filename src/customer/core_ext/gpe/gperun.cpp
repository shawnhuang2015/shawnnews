/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/


#include <gpelib4/ext/idconverter_zmq.hpp>
#include "enginejoblistener.hpp"
#include "enginejobrunner.hpp"
#include "enginejobrunner_zmq.hpp"
#include "gpe_daemon.hpp"
#include "gpeconfig.hpp"



void RunDegreeHistogram(int argc, char** argv) {
  std::string engineConfigFile = std::string(argv[2]);
  gperun::GPEConfig::LoadConfig(engineConfigFile);
  GSQLLogger logger(argv[0], gperun::GPEConfig::log_path_);
  gpelib4::EngineDriver driver(engineConfigFile,
                               gperun::GPEConfig::partitionpath_);
  std::vector<size_t> limits;
  for (int i = 3; i < argc; ++i)
    limits.push_back(boost::lexical_cast<size_t>(argv[i]));
  driver.RunDegreeHistogram(limits);
}

void RunGPEService(int argc, char** argv) {
  std::string engineConfigFile = std::string(argv[1]);
  gperun::GPEConfig::LoadConfig(engineConfigFile);
  GSQLLogger logger(argv[0], gperun::GPEConfig::log_path_);
  gse2::IdConverter idconverter(
      argv[2], argv[3], atoi(argv[4]),
      gperun::GPEConfig::get_request_timeoutsec_);
  gse2::PostListener postListener(&idconverter);
  gperun::EngineJobRunner *runner = new gperun::EngineJobRunner(
      engineConfigFile, gperun::GPEConfig::partitionpath_,
      gperun::GPEConfig::maxjobs_, &idconverter, &postListener);
  runner->Topology_WarmUp(true);
  runner->Topology_PullDelta();
  gperun::GPEDaemon gpe_daemon(
      gperun::GPEConfig::ipaddress_ + ":" + gperun::GPEConfig::port_,
      "/gdbms");
  gpe_daemon.connect(gperun::GPEConfig::zk_connection_, 30000);
  gpe_daemon.StartGPEDaemon();
  gperun::KafkaConnector* connector = new gperun::KafkaConnector(
      &gpe_daemon,
      gperun::GPEConfig::kafka_connection_,
      gperun::GPEConfig::client_prefix_ + ":"
          + gperun::GPEConfig::hostname_,
      gperun::GPEConfig::get_request_queue_,
      gperun::GPEConfig::prefetch_request_queue_,
      gperun::GPEConfig::response_queue_, gperun::GPEConfig::maxjobs_);
  gperun::EngineJobListener* listener = new gperun::EngineJobListener(
      &gpe_daemon, connector, gperun::GPEConfig::hostname_);
  runner->StartDispatch(listener);
  listener->StartListen(runner);
  listener->Join();
  runner->Join();
  delete connector;
  delete listener;
  delete runner;
  if (!gpe_daemon.quit_)
    gpe_daemon.stopDaemon();
}

void RunGPEService_ZMQ(int argc, char** argv) {
  std::string engineConfigFile = std::string(argv[1]);
  gperun::GPEConfig::LoadConfig(engineConfigFile);
  GSQLLogger logger(argv[0], gperun::GPEConfig::log_path_);
  gpelib4::IdConverter_ZMQ idconverter(20 /* timeout*/);

  //gse2::IdConverter idconverter(
  //    argv[2], argv[3], atoi(argv[4]),
  //    gperun::GPEConfig::get_request_timeoutsec_);

  //gse2::PostListener postListener(&idconverter);

  gperun::ZMQEngineJobRunner *runner = new gperun::ZMQEngineJobRunner(
                    engineConfigFile, gperun::GPEConfig::partitionpath_,
                    gperun::GPEConfig::maxjobs_);

  runner->Topology_WarmUp(true);
  runner->setIDConverter(&idconverter);

  string src = gperun::GPEConfig::ipaddress_ + ":" + gperun::GPEConfig::incoming_port_;
  vector<string> dests;
  for(size_t i =0; i<gperun::GPEConfig::rest_incoming_host_strs_.size(); ++i) {
    dests.push_back(gperun::GPEConfig::rest_incoming_host_strs_[i]);
  }
  string publishser = gperun::GPEConfig::ipaddress_ + ":" + gperun::GPEConfig::outgoing_port_to_ids_;

  gpelib4::ZMQConnector *connector = new gpelib4::ZMQConnector(src, dests, publishser);
  runner->setConnector(connector);

  runner->StartDispatch(NULL);

  while(true) {
    usleep(1000);
  }
  
  /*gperun::EngineJobRunner *runner = new gperun::EngineJobRunner(
      engineConfigFile, gperun::GPEConfig::partitionpath_,
      gperun::GPEConfig::maxjobs_, &idconverter, &postListener);
  runner->Topology_WarmUp(true);
  runner->Topology_PullDelta();
  gperun::GPEDaemon gpe_daemon(
      gperun::GPEConfig::ipaddress_ + ":" + gperun::GPEConfig::port_,
      "/gdbms");
  gpe_daemon.connect(gperun::GPEConfig::zk_connection_, 30000);
  gpe_daemon.StartGPEDaemon();
  gperun::KafkaConnector* connector = new gperun::KafkaConnector(
      &gpe_daemon,
      gperun::GPEConfig::kafka_connection_,
      gperun::GPEConfig::client_prefix_ + ":"
          + gperun::GPEConfig::hostname_,
      gperun::GPEConfig::get_request_queue_,
      gperun::GPEConfig::prefetch_request_queue_,
      gperun::GPEConfig::response_queue_, gperun::GPEConfig::maxjobs_);

  gperun::EngineJobListener* listener = new gperun::EngineJobListener(
      &gpe_daemon, connector, gperun::GPEConfig::hostname_);
  runner->StartDispatch(listener);
  listener->StartListen(runner);
  listener->Join();
  runner->Join();
  delete connector;
  delete listener;
  delete runner;
  if (!gpe_daemon.quit_)
    gpe_daemon.stopDaemon();*/
}





// POC modification: Not Likely.
int main(int argc, char** argv) {
#ifdef BUILDVERSION
  std::string versStr = BUILDVERSION;
  std::replace(versStr.begin(), versStr.end(), ',', '\n');
  std::cout << "GPE Server: Version\n" << versStr << std::endl;
#endif

#ifndef REMOVETESTASSERT
  std::cout
      << "\n***Testing Assert Mode. To disable it, add cb=1 when build using scons\n\n";
#endif

  // install failure handler
  google::InstallFailureSignalHandler();  

  if (argc > 3 && std::strcmp(argv[1], "DegreeHistogram") == 0) {
    RunDegreeHistogram(argc, argv);
    return 0;
  }
  if (argc == 5) {
    // start actual service
#ifdef USE_ZMQ
    RunGPEService_ZMQ(argc, argv);
#else
    RunGPEService(argc, argv);
#endif
  } else {
    std::cout
        << "Error arguments.\nUsage:executable <engineconfigfile>"
        << " <sysconfigfile> <graphconfigfile> <workerid>\n"
        << "Or executable DegreeHistogram <engineconfigfile> <degree number limit>\n";
  }
}

