#include "serviceapi.hpp"
#include <gpelib4/enginedriver/enginedriver.hpp>
#include <post_service/post_json2delta.hpp>
#include <topology4/topologygraph.hpp>
#include <gnet/kafka/kafkamessagequeuefactory.hpp>
#include <gnet/dummymessagequeuefactory.hpp>
#include <gutil/glogging.hpp>
#include <gutil/gstring.hpp>
#include "enginejoblistener.hpp"
#include "enginejobrunner.hpp"
#include "gpe_daemon.hpp"
#include "gpeconfig.hpp"
#include "serviceimplbase.hpp"
#include <iostream>

namespace gperun{
  ServiceAPI::ServiceAPI(int argc, char** argv, ServiceImplBase* serviceimpl, gse2::PostJson2Delta* postdelta_impl, MessageQueueFactory* msgqueuefactory){
    serviceimpl_ = serviceimpl;
    create_postdelta_impl_ = false;
    create_msgqueuefactory_ = false;
    postdelta_impl_ = postdelta_impl;
    msgqueuefactory_ = msgqueuefactory;
#ifndef NOPOST
    if(postdelta_impl_ == NULL){
      postdelta_impl_ = new gse2::PostJson2Delta();
      create_postdelta_impl_ = true;
    }
#endif
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

  ServiceAPI::~ServiceAPI(){
    if(create_postdelta_impl_)
      delete postdelta_impl_;
    if(create_msgqueuefactory_)
      delete msgqueuefactory_;
  }

  void ServiceAPI::Run(int argc, char** argv, ServiceImplBase* serviceimpl, gse2::PostJson2Delta* postdelta_impl, MessageQueueFactory* msgqueuefactory){
    ServiceAPI api(argc, argv, serviceimpl, postdelta_impl, msgqueuefactory);
    api.RunMain();
  }

  void ServiceAPI::RunMain(){
    std::string engineConfigFile = config_info_[0];
    gperun::GPEConfig::LoadConfig(engineConfigFile);
    gutil::GSQLLogger logger(processname_.c_str(), gperun::GPEConfig::log_path_);
    topology4::TopologyMeta topologymeta(gperun::GPEConfig::partitionpath_);
    // std::cout << topologymeta << "\n";
    gperun::GPEDaemon gpe_daemon(gperun::GPEConfig::ipaddress_ + ":" + gperun::GPEConfig::port_, "/gdbms");
    if(msgqueuefactory_ == NULL){
#ifndef ComponentTest
      msgqueuefactory_ = new gnet::KAFKAMessageQueueFactory(config_info_[1], &gpe_daemon);
#else
      msgqueuefactory_ = new gnet::DummyMessageQueueFactory();
#endif
      create_msgqueuefactory_ = true;
    }
    gse2::IdConverter idconverter(
          config_info_[0], config_info_[1], atoi(config_info_[2].c_str()),
        gperun::GPEConfig::get_request_timeoutsec_, topologymeta.idresq_pos_, msgqueuefactory_);
    gse2::PostListener* postListener = NULL;
    if(postdelta_impl_ != NULL){
      postdelta_impl_->SetIdConverter(&idconverter);
      postListener = new gse2::PostListener(&idconverter, topologymeta.postq_pos_, msgqueuefactory_,
                                            gperun::GPEConfig::num_post_threads_);
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
      serviceimpl_->Init(*this);
    gpe_daemon.connect(gperun::GPEConfig::zk_connection_, 30000);
    msgqueuefactory_->RegisterWorker();
    gnet::KafkaConnector* connector = new gnet::KafkaConnector(msgqueuefactory_,
                                                               GPEConfig::get_request_queue_,
                                                               GPEConfig::response_queue_, GPEConfig::rest_num_);
    topology4::DeltaRebuilder* rebuilder = NULL;
    if(postdelta_impl_ != NULL){
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
