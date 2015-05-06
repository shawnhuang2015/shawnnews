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
#include "udf/fr.hpp"
#include "udf/yeepay.hpp"

using namespace gperun;

namespace UDIMPL {

  class UDFRunner : public ServiceImplBase{
  public:
    bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request){
      if(request.request_function_== "kneighborsize")
        return RunUDF_KNeighborSize(serviceapi, request);
      else if (request.request_function_ == "rec") {
        return RunUDF_Rec(serviceapi, request);
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

    bool RunUDF_Rec(ServiceAPI& serviceapi, EngineServiceRequest& request){
      // sample to convert vid.
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start)) {
        return false;
      }
      // sample to run one UDF
      typedef gperun::UDF UDF_t;
      UDF_t udf(5, local_start, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);
      if(udf.abortmsg_.size() > 0){
        request.error_ = true;
        request.message_ += udf.abortmsg_;
        return true;
      }
      request.output_idservice_vids.push_back(local_start);
      for (std::vector<VertexLocalId_t>::const_iterator cit = udf.m_vids.begin();
           cit != udf.m_vids.end(); ++cit) {
        request.output_idservice_vids.push_back(*cit);
      }
      return true;
    }

    bool RunUDF_CheckIP(ServiceAPI& serviceapi, EngineServiceRequest& request) {
      return true;
    }
  };
}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
