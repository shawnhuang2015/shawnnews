/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * enginejobrunner.hpp: /gperun/src/enginejobrunner.hpp
 *
 *  Created on: May 16, 2014
 *      Author: Zuoning
 * -- this is originally for axciom
 ******************************************************************************/

#ifndef GPE_ZMQ_ENGINEJOBRUNNER_HPP_
#define GPE_ZMQ_ENGINEJOBRUNNER_HPP_

#include <gpelib4/enginedriver/enginedriverservice_zmq.hpp>
#include <gpelib4/ext/idconverter_zmq.hpp>
#include <gpelib4/enginedriver/singleadaptorcache.hpp>
#include <gapi4/graphapi.hpp>
#include <post_service/post_listener.hpp>
#include "../../core_impl/gpe_impl/impl.hpp"

using namespace std;

namespace gperun {

class ZMQEngineJobRunner : public gpelib4::ZMQEngineDriverService {
public:
    ZMQEngineJobRunner(const string &enginecfgfile, 
                       const string &topologypth,
                       unsigned int max_instances)
                       : gpelib4::ZMQEngineDriverService(enginecfgfile, 
                                                         topologypth,
                                                         max_instances, 
                                                         true, 
                                                         true, 
                                                         true),
                       postListener_(NULL) {
        InitLookupMaps();
    }

    /// request topology pull delta. Following requests will following
    /// with topology with most recent delta.
    // XXX: commented out first
    /*void Topology_PullDelta(std::stringstream* debugstr = NULL) {
        if (postListener_ == NULL)
          return;
        uint64_t deltasize = 0;
        char* deltadata = postListener_->getAllDelta(deltasize);
        if (deltadata == NULL || deltasize == 0)
          return;
        topology_->GetDeltaRecords()->ReadDeltas(
            reinterpret_cast<uint8_t*>(deltadata), deltasize);
        delete[] deltadata;
        topology_->HandleNewDelta();
    }*/

    void InitLookupMaps(); 

    string RunInstance(gpelib4::ZMQEngineServiceRequest* instance);

    unsigned int Run(gpelib4::ZMQEngineServiceRequest* request,
                     Json::Value& root,
                     gpelib4::IdConverter_ZMQ::RequestIdMaps* maps);

    /// POC modification: Not Likely.
    void ShowOneVertexInfo(gpelib4::ZMQEngineServiceRequest* request,
                           Json::Value& root,
                           VertexLocalId_t vid, 
                           std::vector<VertexLocalId_t>& idservice_vids);



public:
    //static boost::unordered_map<std::string, uint32_t> owner_id_map_;
    gse2::PostListener* postListener_;

};

//boost::unordered_map<std::string, uint32_t> ZMQEngineJobRunner::owner_id_map_;

}  // namespace gperun

#endif /* GPE_ZMQ_ENGINEJOBRUNNER_HPP */
