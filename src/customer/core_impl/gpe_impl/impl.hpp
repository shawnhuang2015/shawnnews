/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

#include <gutil/gsqlcontainer.hpp>
#include <topology4/topologygraph.hpp>
#include <gpelib4/enginebase/baseudf.hpp>
#include <gpelib4/enginedriver/enginedriverservice.hpp>
#include <ext/idconverter.hpp>
#include <json/json.h>

namespace UDIMPL {

class GPE_UD_Impl{
public:
    typedef GSQLMap_t<std::string, std::string> Maps_t;

    /// customized implementation for topology: like set merge function for attribute.
    static void Topology_Prepare(topology4::TopologyGraph* topology){
        /*topology4::TopologyMeta* topologymeta = topology->GetTopologyMeta();
        topologymeta->GetEdgeType(0).attributes_.attributelist_[0].merge_func_ =
            boost::bind(&UDT::OccurInfo_t::Add, _1, _2);*/
    }

    /// customized implementation to run query UDF
    static bool RunQuery(gpelib4::EngineDriverService* service,
                         gse2::IdConverter::RequestIdMaps* maps,
                         std::vector<std::string>& request_argv,
                         std::map<std::string,std::string>& options,
                         std::vector<VertexLocalId_t>& idservice_vids,
                         Json::Value& response_root,
                         Maps_t& customizedsetttings){
        return false;
    }

    /// customized implementation to load user defined setting variables.
    static void LoadCustimzedSetting(YAML::Node& root, Maps_t& customizedsetttings){
    }

    /// standard function to convert user vertex id to internal vertex id.
    static bool UIdtoVId(topology4::TopologyGraph* topology,
                         gse2::IdConverter::RequestIdMaps* maps,
                         Json::Value& root, std::string strid,
                         VertexLocalId_t& vid) {
        std::tr1::unordered_map<std::string, gse2::IdConverter::vidInfo>::iterator it =
                maps->idmaps_.find(strid);
        if (it == maps->idmaps_.end()) {
            root["message"] = "error_: incorrect vid " + strid;
            root["error"] = true;
            return false;
        }
        vid = it->second.vid_;
        if (!topology->IsValidVertexId(vid)) {
            root["message"] = "error_: incorrect vid " + strid;
            root["error"] = true;
            return false;
        }
        return true;
    }
};

}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
