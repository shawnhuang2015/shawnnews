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
                         std::string& message, bool& error, std::string strid,
                         VertexLocalId_t& vid, topology4::QueryState& querystate) {
      std::tr1::unordered_map<std::string, gse2::IdConverter::vidInfo>::iterator it =
          maps->idmaps_.find(strid);
      if (it == maps->idmaps_.end()) {
        message = "error_: incorrect vid " + strid;
        error = true;
        return false;
      }
      vid = it->second.vid_;
      if (!topology->IsValidVertexId(vid, querystate)) {
        message = "error_: incorrect vid " + strid;
        error = true;
        return false;
      }
      return true;
    }

    /**
     * @brief MarkVId: obsolete. Using JSONWriter to WriteMarkVId
     * @param id
     * @return
     */
    static std::string MarkVId(VertexLocalId_t id) __attribute__ ((deprecated)) {
    }

  };

}     // namespace gperun
#endif  // SRC_CUSTOMERS_GPE_UTIL_H_
