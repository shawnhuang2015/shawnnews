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
//#include "udf/fr.hpp"
//#include "udf/yeepay.hpp"

using namespace gperun;

namespace UDIMPL {

  class UDFRunner : public ServiceImplBase{
  public:
    bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request){
      if(request.request_function_== "kneighborsize") {
        return RunUDF_KNeighborSize(serviceapi, request);
      } else if (request.request_function_ == "check_ip") {
        return RunUDF_CheckIP(serviceapi, request);
      }
      return false; /// not a valid request
    }

  private:
    bool RunUDF_KNeighborSize(ServiceAPI& serviceapi, EngineServiceRequest& request){
      // sample to convert vid.
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start))
        return false;
      request.outputwriter_->WriteStartObject();
      request.outputwriter_->WriteName("vertex");
      // sample to use GraphAPI
      GraphAPI* graphapi = serviceapi.CreateGraphAPI(&request);
      graphapi->GetOneVertex(local_start)->WriteToJson(*request.outputwriter_);
      delete graphapi;
      request.outputwriter_->WriteName("neighborsize");
      // sample to run one UDF
      typedef KNeighborSize UDF_t;
      UDF_t udf(3, local_start, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);
      if(udf.abortmsg_.size() > 0){
        request.error_ = true;
        request.message_ += udf.abortmsg_;
        return true;
      }
      request.outputwriter_->WriteEndObject();
      request.output_idservice_vids.push_back(local_start);
      return true;
    }


    bool RunUDF_CheckIP(ServiceAPI& serviceapi, EngineServiceRequest& request) {
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start)) {
        return false;
      }

      typedef yeepay_ns::YeepaySubGraphExtractUDF UDF_t;

      size_t depth = request.jsoptions_.isMember("depth") ? 
        strtoul(request.jsoptions_["depth"][0].asString().c_str(), NULL, 10) : 1;
      size_t start_time = request.jsoptions_.isMember("starttime") ? 
        strtoul(request.jsoptions_["starttime"][0].asString().c_str(), NULL, 10) : 0;
      size_t end_time = request.jsoptions_.isMember("endtime") ? 
        strtoul(request.jsoptions_["endtime"][0].asString().c_str(), NULL, 10) : yeepay_ns::INF;

      UDF_t udf(depth * 2, local_start, depth * 2, start_time, end_time, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);
      if(udf.abortmsg_.size() > 0){
        request.error_ = true;
        request.message_ += udf.abortmsg_;
        return true;
      }

      request.output_idservice_vids.push_back(local_start);
      for (boost::unordered_set<VertexLocalId_t>::const_iterator cit = udf.vids.begin();
           cit != udf.vids.end(); ++cit) {
        request.output_idservice_vids.push_back(*cit);
      }
      return true;
    }
  };
}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
