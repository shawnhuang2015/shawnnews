/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMERS_GPE_UTIL_H_
#define SRC_CUSTOMERS_GPE_UTIL_H_

#include <topology4/topologygraph.hpp>
#include <ext/idconverter.hpp>
#include <json/json.h>

namespace gperun {

  class Util{
  public:
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

    static std::string MarkVId(VertexLocalId_t id) {
      return gse2::IdConverter::MarkVId(id);
    }

  };

}     // namespace gperun
#endif  // SRC_CUSTOMERS_GPE_UTIL_H_
