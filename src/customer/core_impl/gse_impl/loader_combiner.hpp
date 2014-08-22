/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0 --> POC
 * loader_combiner.hpp: /src/customer/core_impl/gse_impl/loader_combiner.hpp
 *
 *  Created on: May 5, 2014
 *      Author: xxxx
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERCOMBINER_HPP_
#define SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERCOMBINER_HPP_

#include <gse2/partition/partitiondatafeeder.hpp>
#include "../udt.hpp"
namespace UDIMPL {

/**
 * Override below when you need to customize the graph loading.
 * This is the combiner phase:  vertex/edge records --> graph vertex/edge
 */
class GSE_UD_LoaderCombiner {
 public:
  /**
   * Combine and write all edges of a vertex
   * @param[in] srcid
   * @param[in] outgoingedges_vec
   * @param[in] edgefile
   * @return number of edges of this vertex
   */
  size_t WriteEdgeRecords2Edgelist(
      VertexLocalId_t srcid,
      std::vector<gse2::PartitionEdgeInfo> &outgoingedges_vec,
      gutil::CompactWriter &edgefile) {
    /* this is the combiner w/o any edge attribute */
    VertexLocalId_t oldid = 0;
    size_t degree = 0;
    for (uint32_t j = 0; j < outgoingedges_vec.size(); j++) {
      if (outgoingedges_vec[j].nid_ != oldid || j == 0) {
        // first record of a different toID
        degree++;
        edgefile.writeCompressed(outgoingedges_vec[j].nid_ - oldid);
        oldid = outgoingedges_vec[j].nid_;
      }
    }
    return degree;
#if 0
    /* this is a complicated example from Alipay */
    typedef std::map<uint32_t, std::vector<uint32_t> > UDT_PAIR_Map_t;
    typedef UDT_PAIR_Map_t::iterator UDT_PAIR_Map_itr_t;

    UDT_PAIR_Map_t inputRecordsInOneEdge;
    for (uint32_t j = 0; j < outgoingedges_vec.size(); j++) {
      if (outgoingedges_vec[j].nid_ != oldid || j == 0) {
        // first record of a different toID
        degree++;
        edgefile.writeCompressed(outgoingedges_vec[j].nid_ - oldid);
        oldid = outgoingedges_vec[j].nid_;
        inputRecordsInOneEdge.clear();
      }
      /* accumulate attributes of the edge */
      if (outgoingedges_vec[j].prop_ptr_ != NULL
          && outgoingedges_vec[j].prop_length_ > 0) {
        uint8_t *ptr = (uint8_t*) outgoingedges_vec[j].prop_ptr_;
        uint32_t udt_key = gutil::CompactWriter::readCompressed(ptr);
        uint32_t udt_val = gutil::CompactWriter::readCompressed(ptr);
        GASSERT(
            ptr
            == (uint8_t*) outgoingedges_vec[j].prop_ptr_
            + outgoingedges_vec[j].prop_length_,
            "Invalid edge input record: srcid " << srcid << " toid "
            << oldid);
        inputRecordsInOneEdge[udt_key].push_back(udt_val);
      }
      UDIMPL::OccurInfo_t occurInfo;
      /* Combine all input records of an edge and write out */
      if (j == outgoingedges_vec.size() - 1
          || outgoingedges_vec[j + 1].nid_ != oldid) {
        // last record of this edge
        if (inputRecordsInOneEdge.size() > 0) {
          // attr 1: UDT_PAIR_LIST: numberPairs, <K1V1>, <K2V2>, ...
          edgefile.writeCompressed(inputRecordsInOneEdge.size());
          occurInfo.firstLinktime = std::numeric_limits < uint32_t
          > ::max();
          occurInfo.lastLinktime = std::numeric_limits < uint32_t
          > ::min();
          occurInfo.linkCount = 0;
          occurInfo.direction = 0;
          occurInfo.isPayment = false;
          for (UDT_PAIR_Map_itr_t itr = inputRecordsInOneEdge.begin();
              itr != inputRecordsInOneEdge.end(); ++itr) {
            // each UDT type
            for (uint32_t i = 0; i < itr->second.size(); ++i) {
              if (occurInfo.firstLinktime > itr->second[i])
              occurInfo.firstLinktime = itr->second[i];
              if (occurInfo.lastLinktime < itr->second[i])
              occurInfo.lastLinktime = itr->second[i];
            }
            occurInfo.linkCount = itr->second.size();
            edgefile.writeCompressed(itr->first);  // Ki
            edgefile.write((char *) &occurInfo, sizeof(occurInfo));
          }
          // attr 2: intensity REAL -- 4 bytes
          float intensity = 0;
          edgefile.write((char *) &intensity, sizeof(intensity));
          // attr 3: isBridge bool -- 1 byte
          const char isBridge = '\1';
          edgefile.write((char *) &isBridge, sizeof(isBridge));
        }
      }
    }
    return degree;
#endif
  }
}
;

}    // namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERCOMBINER_HPP_ */
