/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * enginejobrunner.hpp: /gperun/src/enginejobrunner.hpp
 *
 *  Created on: May 16, 2013
 *      Author: lichen, nicholas
 ******************************************************************************/

#ifndef GPE_ENGINEJOBRUNNER_HPP_
#define GPE_ENGINEJOBRUNNER_HPP_

#include <gpelib4/enginedriver/enginedriverservice.hpp>
#include <gmmnt/globalmemoryserver.hpp>
#include <post_service/post_listener.hpp>
#include <ext/idconverter.hpp>
#include "gpeconfig.hpp"

namespace gperun {
  class ServiceAPI;

  /**
 * Engine job runner.
 * POC modification: Needed.
 */
  class EngineJobRunner : public gpelib4::EngineDriverService {
  public:
    EngineJobRunner(std::string enginecfgfile, std::string topologypth,
                    unsigned int max_instances,
                    gse2::IdConverter* idconverter,
                    gse2::PostListener* postListener,
                    ServiceAPI* serviceapi)
      : gpelib4::EngineDriverService(enginecfgfile, topologypth,
                                     max_instances,  GPEConfig::udfmode_ != "offline",
                                     true, true),
        idconverter_(idconverter),
        postListener_(postListener),
        serviceapi_(serviceapi) {
      globalinstance_->memserver_->SetServiceMode(GPEConfig::udfmode_ != "offline");
      globalinstance_->memallocator_->Set_memlimitMB(1000000000);
      pulldeltathread_ = NULL;
      Topology_Prepare();
    }

    ~EngineJobRunner(){
      if(pulldeltathread_ != NULL){
        pulldeltathread_->join();
        delete pulldeltathread_;
        pulldeltathread_ = NULL;
      }
    }

    /// prepare topology: like set merge function for attribute.
    /// POC modification: Needed.
    void Topology_Prepare();

    /// request topology pull delta. Following requests will following
    /// with topology with most recent delta.
    void Topology_PullDelta();

    /// subclass provide implementation to translate request to actual udf object
    std::string RunInstance(gpelib4::EngineServiceRequest* instance);

    /**
   * get enumerator mappers.
   */
    gse2::EnumMappers* GetEnumMappers() {
      if (postListener_ == NULL)
        return NULL;
      return postListener_->enumMappers();
    }

    void PrintStatus(std::string tag, std::ostream& stream){
      stream << tag;
      idconverter_->ReportCaching(stream);
    }

  private:
    gse2::IdConverter* idconverter_;
    gse2::PostListener* postListener_;
    /// the pull delta thread
    boost::thread* pulldeltathread_;
    ServiceAPI* serviceapi_;

    /// run one request. POC modification: Needed.
    void Run(gpelib4::EngineServiceRequest* request);
    void RunStandardAPI(gpelib4::EngineServiceRequest* request);
    void PullDeltaThread();
    bool PullDelta();
  };
}  // namespace gperun
#endif /* GPE_ENGINEJOBRUNNER_HPP_ */
