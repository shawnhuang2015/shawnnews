/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GSE_2.0
 * loader_impl.hpp: /src/customer/core_impl/gse_impl/loader_combiner.hpp
 *
 *  Created on: May 5, 2014
 *      Author: like
 ******************************************************************************/

#ifndef UDIMPL_GSE_UD_LOADER_COMBINER_HPP_
#define UDIMPL_GSE_UD_LOADER_COMBINER_HPP_

#include <gse2/partition/partitiondatafeeder.hpp>

namespace UDIMPL {

class GSE_UD_LoaderCombiner {
 public:
size_t WriteEdgeRecords2Edgelist(
    VertexLocalId_t srcid,
    std::vector<gse2::PartitionEdgeInfo> &outgoingedges_vec,
    gutil::CompactWriter &edgefile) {
  VertexLocalId_t oldid = 0;
  size_t degree = 0;
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
          ptr == (uint8_t*)outgoingedges_vec[j].prop_ptr_ + outgoingedges_vec[j].prop_length_,
          "Invalid edge input record: srcid " << srcid << " toid " << oldid);
      inputRecordsInOneEdge[udt_key].push_back(udt_val);
    }
    /* Combine all input records of an edge and write out */
    if (j == outgoingedges_vec.size() - 1
        || outgoingedges_vec[j + 1].nid_ != oldid) {
      // last record of this edge
      if (inputRecordsInOneEdge.size() > 0) {
        // attr 1: UDT_PAIR_LIST: numberPairs, <K1V1>, <K2V2>, ...
        edgefile.writeCompressed(inputRecordsInOneEdge.size());
        uint32_t firstLinktime = std::numeric_limits<uint32_t>::max();
        uint32_t lastLinktime = std::numeric_limits<uint32_t>::min();
        uint32_t linkCount = 0;
        uint32_t direction = 0;
        char isPayment = '\0';  // bool
        for (UDT_PAIR_Map_itr_t itr = inputRecordsInOneEdge.begin();
            itr != inputRecordsInOneEdge.end(); ++itr) {
          // each UDT type
          for (uint32_t i = 0; i < itr->second.size(); ++i) {
            if (firstLinktime > itr->second[i])
              firstLinktime = itr->second[i];
            if (lastLinktime < itr->second[i])
              lastLinktime = itr->second[i];
          }
          linkCount = itr->second.size();
          edgefile.writeCompressed(itr->first); // Ki
          edgefile.write((char *) &firstLinktime, sizeof(firstLinktime));
          edgefile.write((char *) &lastLinktime, sizeof(lastLinktime));
          edgefile.write((char *) &linkCount, sizeof(linkCount));
          edgefile.write((char *) &direction, sizeof(direction));
          edgefile.write((char *) &isPayment, sizeof(isPayment));
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
};
};

}    // namespace UDIMPL
#endif    /* UDIMPL_GSE_UD_LOADER_COMBINER_HPP_ */
