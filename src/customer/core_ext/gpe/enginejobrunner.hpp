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
#include <post_service/post_listener.hpp>
#include <ext/idconverter.hpp>

namespace gperun {

/**
 * Engine job runner.
 * POC modification: Needed.
 */
class EngineJobRunner : public gpelib4::EngineDriverService {
 public:
  EngineJobRunner(std::string enginecfgfile, std::string topologypth,
                  unsigned int max_instances,
                  gse2::IdConverter* idconverter,
                  gse2::PostListener* postListener)
      : gpelib4::EngineDriverService(enginecfgfile, topologypth,
                                     max_instances, true, true, true,
                                     NULL),
        idconverter_(idconverter),
        postListener_(postListener) {
    globalinstance_->memserver_->SetServiceMode(true);
    globalinstance_->memallocator_->Set_memlimitMB(1000000000);
    Topology_Prepare();
  }

  /// prepare topology: like set merge function for attribute.
  /// POC modification: Needed.
  void Topology_Prepare();

  /// request topology pull delta. Following requests will following
  /// with topology with most recent delta.
  void Topology_PullDelta(std::stringstream* debugstr = NULL);

  /// subclass provide implementation to translate request to actual udf object
  std::string RunInstance(EngineServiceRequest* instance);

  /**
   * get enumerator mappers.
   */
  gse2::EnumMappers* GetEnumMappers() {
    if (postListener_ == NULL)
      return NULL;
    return postListener_->enumMappers();
  }

 private:
  gse2::IdConverter* idconverter_;
  gse2::PostListener* postListener_;

  /// run one request. POC modification: Needed.
  unsigned int Run(EngineServiceRequest* request, Json::Value& root,
                   gse2::IdConverter::RequestIdMaps* maps);

  /// POC modification: Not Likely.
  void ShowOneVertexInfo(EngineServiceRequest* request, Json::Value& root,
                         VertexLocalId_t vid, std::vector<VertexLocalId_t>& idservice_vids);
};
}  // namespace gperun
#endif /* GPE_ENGINEJOBRUNNER_HPP_ */
