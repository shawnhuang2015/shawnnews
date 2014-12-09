#include "serviceapi.hpp"
#include <gpelib4/enginedriver/enginedriver.hpp>
#include <post_service/post_json2delta.hpp>
#include <topology4/topologygraph.hpp>
#include <gutil/glogging.hpp>
#include <gutil/gstring.hpp>
#include "enginejoblistener.hpp"
#include "enginejobrunner.hpp"
#include "gpe_daemon.hpp"
#include "gpeconfig.hpp"
#include "serviceimplbase.hpp"
#include <iostream>

namespace gperun{
  ServiceAPI::ServiceAPI(int argc, char** argv, ServiceImplBase* serviceimpl, gse2::PostJson2Delta* postdelta_impl){
    serviceimpl_ = serviceimpl;
    postdelta_impl_ = postdelta_impl;
    service_ = NULL;
#ifdef BUILDVERSION
    std::string versStr = BUILDVERSION;
    std::replace(versStr.begin(), versStr.end(), ',', '\n');
    std::cout << "GPE Server: Version\n" << versStr << std::endl;
#endif

#ifndef REMOVETESTASSERT
    std::cout
        << "\n***Testing Assert Mode. To disable it, add cb=1 when build using scons\n\n";
#endif

    if(gutil::GEnvVariable::Use_JEMalloc_){
      std::cout
          << "\n***************************  Using JeMalloc ***************************\n\n";
    } else if(gutil::GEnvVariable::Use_TCMalloc_){
      std::cout
          << "\n***************************  Using TcMalloc ***************************\n\n";
    }

    // install failure handler
    google::InstallFailureSignalHandler();
    if (argc == 2) {
      processname_ =  argv[0];
      config_info_ = gutil::tokenize_file(std::string(argv[1]));
    } else {
      std::cout
          << "Error arguments."
          << "\nUsage:executable <gpe_config_property_file>"
          << "\n         (which has <sysconfigfile> <graphconfigfile> <workerid>)\n";
      exit(0);
    }
  }

  void ServiceAPI::RunMain(){
    std::string engineConfigFile = config_info_[0];
    YAML::Node configuration_root = gperun::GPEConfig::LoadConfig(engineConfigFile);
    gutil::GSQLLogger logger(processname_.c_str(), gperun::GPEConfig::log_path_);
    topology4::TopologyMeta topologymeta(gperun::GPEConfig::partitionpath_);
    std::cout << topologymeta << "\n";
    gse2::IdConverter idconverter(
          config_info_[0], config_info_[1], atoi(config_info_[2].c_str()),
        gperun::GPEConfig::get_request_timeoutsec_, topologymeta.idresq_pos_);
    gse2::PostListener* postListener = NULL;
    if(postdelta_impl_ == NULL)
      gperun::GPEConfig::enabledelta_ = false;
    if(gperun::GPEConfig::enabledelta_){
      postdelta_impl_->SetIdConverter(&idconverter);
      postListener = new gse2::PostListener(&idconverter, topologymeta.postq_pos_, gperun::GPEConfig::num_post_threads_);
      postListener->setPostProcessor(postdelta_impl_);
    }
    gperun::EngineJobRunner *runner = new gperun::EngineJobRunner(
                                        engineConfigFile, gperun::GPEConfig::partitionpath_,
                                        gperun::GPEConfig::maxjobs_, &idconverter,
                                        postListener, this);
    service_ = runner;
    runner->Topology_WarmUp(true);
    if(runner->topology()->GetDeltaRecords() != NULL){
      runner->topology()->GetDeltaRecords()->SetSetting(gperun::GPEConfig::accuratedegree_);
    }
    runner->Topology_PullDelta();
    if(serviceimpl_ != NULL)
      serviceimpl_->Init(*this, configuration_root);
    gperun::GPEDaemon gpe_daemon(
          gperun::GPEConfig::ipaddress_ + ":" + gperun::GPEConfig::port_,
          "/gdbms");
    gpe_daemon.connect(gperun::GPEConfig::zk_connection_, 30000);
    gpe_daemon.StartGPEDaemon();
    gnet::KafkaConnector* connector = NULL;
    {
      gnet::KafkaConnectorConfig connectorCfg;
      connectorCfg.rest_num_ = gperun::GPEConfig::rest_num_;
      gnet::KafkaConfig writerCfg;
      writerCfg.topicName_ = gperun::GPEConfig::response_queue_;
      writerCfg.connectionStr_ = gperun::GPEConfig::kafka_connection_;
      connectorCfg.writer_topic_cfg_ = writerCfg;

      gnet::KafkaConfig get_request_Cfg;
      get_request_Cfg.topicName_ = gperun::GPEConfig::get_request_queue_;
      get_request_Cfg.connectionStr_ = gperun::GPEConfig::kafka_connection_;

      connectorCfg.readers_cfg_.push_back(get_request_Cfg);

      if(gperun::GPEConfig::prefetch_request_queue_.size()>0) {
        gnet::KafkaConfig prefetch_request_Cfg;
        prefetch_request_Cfg.topicName_ = gperun::GPEConfig::prefetch_request_queue_;
        prefetch_request_Cfg.connectionStr_ = gperun::GPEConfig::kafka_connection_;
        connectorCfg.readers_cfg_.push_back(prefetch_request_Cfg);
      }
      connector = new gnet::KafkaConnector(&gpe_daemon, connectorCfg,
                                           gperun::GPEConfig::client_prefix_ + ":" + gperun::GPEConfig::hostname_);
    }
    topology4::DeltaRebuilder* rebuilder = NULL;
    if(gperun::GPEConfig::enabledelta_){
      rebuilder = new topology4::DeltaRebuilder(runner->globalinstance(), runner->topology(), runner);
      rebuilder->rebuildsetting_ = gperun::GPEConfig::rebuildsetting_;
    }
    gperun::EngineJobListener* listener = new gperun::EngineJobListener(
                                            &gpe_daemon, connector, gperun::GPEConfig::hostname_, rebuilder);
    runner->StartDispatch(listener);
    listener->StartListen(runner);
    listener->Join();
    runner->Join();
    delete connector;
    delete listener;
    if(rebuilder != NULL)
      delete rebuilder;
    delete runner;
    if(postListener != NULL)
      delete postListener;
    if (!gpe_daemon.quit_)
      gpe_daemon.stopDaemon();
  }

  bool ServiceAPI::UIdtoVId(gpelib4::EngineServiceRequest& request,
                            std::string strid, VertexLocalId_t& vid) {
    std::tr1::unordered_map<std::string, gutil::RequestIdMaps::vidInfo>::iterator it =
        request.requestid_maps_->idmaps_.find(strid);
    if (it == request.requestid_maps_->idmaps_.end()) {
      request.message_ = "error_: incorrect vid " + strid;
      request.error_ = true;
      return false;
    }
    vid = it->second.vid_;
    if (!topology()->IsValidVertexId(vid, *request.querystate_)) {
      request.message_ = "error_: incorrect vid " + strid;
      request.error_ = true;
      return false;
    }
    return true;
  }

  gapi4::GraphAPI* ServiceAPI::CreateGraphAPI(gpelib4::EngineServiceRequest& request){
    return new gapi4::GraphAPI(topology(), request.querystate_);
  }

  gse2::EnumMappers* ServiceAPI::GetEnumMappers(){
    return  service_->GetEnumMappers();
  }

  gmmnt::GlobalInstances* ServiceAPI::globalinstance() const {return service_->globalinstance();}
  topology4::TopologyGraph* ServiceAPI::topology() const {return service_->topology();}
  bool ServiceAPI::sparsemode() const { return service_->sparsemode();}
  gpelib4::SingleAdaptorCache* ServiceAPI::adaptorcache() const { return service_->adaptorcache();}
  topology4::TopologyMeta* ServiceAPI::GetTopologyMeta() {return topology()->GetTopologyMeta();}

  std::string ServiceAPI::RunUDF(gpelib4::EngineServiceRequest* request, gpelib4::WorkerAdaptor* adaptor) {
    gutil::GTimer timer;
    GPROFILER(request->requestid_) << "GPE|Run|Start_RunUDF|" << "\n";
    std::string result = service_->RunSingleAdaptor(adaptor, request);
    GPROFILER(request->requestid_) << "GPE|Run|Finish_RunUDF|" << "\n";
    GVLOG(Verbose_EngineHigh) << "ExecuteRequest," << request->requestid_
                              << "|" << timer.GetTotalMilliSeconds()
                              << "ms|" << result << "|";
    return result;
  }

}
