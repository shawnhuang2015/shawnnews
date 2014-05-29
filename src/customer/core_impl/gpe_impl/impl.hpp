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
#include "../udt.hpp"
#include "../../core_ext/gpe/registry.hpp"
#include "../../core_ext/gpe/util.hpp"
#include "../../core_ext/gpe/unionfind.hpp"
#include "../../core_ext/gpe/global_vector.hpp"

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

    /// customized implementation to register UDF
    static void registerAll() {
      // Add your UDF register func here
      // gperun::registerLinkRecommendation2Step(Registries::pureTopologyRegistry);
      // gperun::registerQueryGraph(Registries::pureTopologyRegistry);
    }

  };

}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
