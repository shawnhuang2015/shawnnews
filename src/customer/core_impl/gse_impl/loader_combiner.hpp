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

#include <gse2/partition/gse_loader_combiner_base.hpp>
#include <core_impl/udt.hpp>

namespace UDIMPL {

/**
 * This is the combiner phase:  vertex/edge records --> graph vertex/edge.
 *
 * gse_loader_combiner_base provides the general implementation of vertex
 * and edge record combiners. If you don't need to combine the records, then
 * no need to override either of them.
 *
 */
class GSE_UD_LoaderCombiner : public gse2::GSE_LoaderCombiner_Base {
public:
#if 0
  /**
   * Combine multiple vertex attribute records of a vertex.
   * Note: WriteVertexRecords2OneVertex will be invoked whenever
   *  a typed vertex has attributes. It a typed vertex doesn't define
   *  any attribute, GSE will skip calling this function.
   *
   * @param[in] vtype: the vertex type
   * @param[in] vid: the vertex ID
   * @param[in] vectex_vec: vector of attributes of vid
   * @param[in] vertexfile: vertex file writer (CompactWriter)
   */
  void WriteVertexRecords2OneVertex(
      VTYPEID_T vtype, VertexLocalId_t vid,
      std::vector<gse2::PartitionVertexInfo> &vectex_vec,
      gutil::CompactWriter &vertexfile) {
    // write your own code
  }

  /**
   * Combine and write all edges of a vertex
   * @param[in] srcid
   * @param[in] outgoingedges_vec
   * @param[in] edgefile
   * @return number of edges of this vertex
   */
  virtual size_t WriteEdgeRecords2Edgelist(
      VertexLocalId_t srcid,
      std::vector<gse2::PartitionEdgeInfo> &outgoingedges_vec,
      gutil::CompactWriter &edgefile) {
    // write your own code
    return 123; // degree
  }
#endif
};

}    // namespace UDIMPL
#endif    /* SRC_CUSTOMER_COREIMPL_GSEIMPL_LOADERCOMBINER_HPP_ */
