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
#include "kstepneighborhoodsubgrap.hpp"

using namespace gperun;

namespace UDIMPL {

  class UDFRunner : public ServiceImplBase{
  public:
    bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request){
      if(request.request_function_== "kneighborsize") {
        return RunUDF_KNeighborSize(serviceapi, request);
      }
      else if(request.request_function == "kstepsubgraph") {
        return RunUDF_KStepNeighborhoodSubgraph(serviceapi, request);
      }
      else {
        return false; /// not a valid request
      }
    }

  private:
    bool RunUDF_KNeighborSize(ServiceAPI& serviceapi, EngineServiceRequest& request){
      // sample to convert vid.
      VertexLocalId_t local_start;
      //if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start))
      if (!serviceapi.UIdtoVId(request, request.request_data_, local_start))
        return false;
      request.outputwriter_->WriteStartObject();
      request.outputwriter_->WriteName("vertex");
      {
        // sample to use GraphAPI
        boost::shared_ptr<GraphAPI> graphapi = serviceapi.CreateGraphAPI(&request);
        VertexAttribute v1;
        graphapi->GetOneVertex(local_start, v1);
        v1.WriteToJson(*request.outputwriter_);
      }
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

    bool RunUDF_KStepNeighborhoodSubgraph(ServiceAPI& serviceapi, EngineServiceRequest& request) {

      uint32_t maxVertices = 50;
      uint32_t depth = 1;
      char buffer [16];

      VertexLocalId_t startVertex = (VertexLocalId_t) -1;
      
      std::string newid;
      std::string newtype;

      if (request.jsoptions_.isMember("id")) {
        newid = request.jsoptions_["id"][0].asString();
      }
      else {
        return false;
      }

      if (request.jsoptions_.isMember("type")) {
        sprintf (buffer, "%d", request.jsoptions_["type"]["typeid"].asInt());
        newtype = std::string(buffer);
      }
      else {
        newtype = "0";
      }

      if (!serviceapi.UIdtoVId(request, newtype+"_"+newid, startVertex)) {
        return false;
      }

      if (request.jsoptions_.isMember("max")) {
        maxVertices = request.jsoptions_["max"][0].asUInt();
      }

      
      if (request.jsoptions_.isMember("depth")) {
        depth = request.jsoptions_["depth"][0].asUInt();
      }

      typedef VISUALIZATION::KStepNeighborhoodSubgraph UDF_t;
      UDF_t udf(startVertex, request.outputwriter_, maxVertices, depth, serviceapi.GetTopologyMeta());

      serviceapi.RunUDF(&request, &udf);

      if (udf.abortmsg_.size() > 0) {
        request.error_ = true;
        request.message_ += udf.abortmsg_;
      } else {
        std::vector<VertexLocalId_t>& vidsToTranslate = udf.getVidsToTranslate();
        for (uint32_t idx = 0; idx < vidsToTranslate.size(); ++idx) {
          request.output_idservice_vids.push_back(vidsToTranslate[idx]);
        }
      }

      return true;
    }

  };
}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
