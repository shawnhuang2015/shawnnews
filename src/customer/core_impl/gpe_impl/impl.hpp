/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

#include <gpe/serviceimplbase.hpp>
#include "kneighborsize.hpp"
#include "kstepneighborhoodsubgraph.hpp"
#include "querydispatcher.hpp"

using namespace gperun;

namespace UDIMPL {

class UDFRunner : public ServiceImplBase {
 public:
  bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    if (request.request_function_ == "kneighborsize") {
      return RunUDF_KNeighborSize(serviceapi, request);
      } else if (request.request_function_ == "queryDispatcher") {
#ifdef RUNQUERY
        std::string queryName = request.jsoptions_["query_name"][0].asString();
        QueryDispatcher qdp(queryName,serviceapi, request);
        return qdp.RunQuery();
#else
        return false;
#endif
    }
    
    return false;  /// not a valid request
  }

 private:
  bool RunUDF_KNeighborSize(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    // sample to convert vid.
    VertexLocalId_t local_start;
    // if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start))
    if (!serviceapi.UIdtoVId(request, request.request_data_, local_start))
      return false;
    request.outputwriter_->WriteStartObject();
    request.outputwriter_->WriteName("vertex");
    {
      // sample to use GraphAPI
      boost::shared_ptr<GraphAPI> graphapi =
          serviceapi.CreateGraphAPI(&request);
      VertexAttribute v1;
      graphapi->GetOneVertex(local_start, v1);
      v1.WriteToJson(*request.outputwriter_);
    }
    request.outputwriter_->WriteName("neighborsize");
    // sample to run one UDF
    typedef KNeighborSize UDF_t;
    UDF_t udf(3, local_start, request.outputwriter_);
    serviceapi.RunUDF(&request, &udf);
    if (udf.abortmsg_.size() > 0) {
      request.error_ = true;
      request.message_ += udf.abortmsg_;
      return true;
    }
    request.outputwriter_->WriteEndObject();
    request.output_idservice_vids.push_back(local_start);
    return true;
  }
};
}  // namespace UDIMPL
#endif  // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
