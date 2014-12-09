/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

#include "kneighborsize.hpp"
#include "../udt.hpp"
#include <core_ext/gpe/serviceapi.hpp>
#include <core_ext/gpe/serviceimplbase.hpp>

namespace UDIMPL {

  class GPE_UD_Impl : public gperun::ServiceImplBase{
  public:
    /// one time initialization for service.
    void Init(gperun::ServiceAPI& serviceapi, YAML::Node& configuration_node) {
       // std::cout << *serviceapi.GetTopologyMeta() << "\n";
    }

    /// run one query request.
    bool RunQuery(gperun::ServiceAPI& serviceapi, gpelib4::EngineServiceRequest& request){
      if(request.request_function_== "kneighborsize"){
        RunUDF_KNeighborSize(serviceapi, request);
        return true;
      }
      return false;
    }

  private:

    void RunUDF_KNeighborSize(gperun::ServiceAPI& serviceapi, gpelib4::EngineServiceRequest& request){
      typedef KNeighborSize UDF_t;
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start)){
        return;
      }
      request.outputwriter_->WriteStartObject();
      gapi4::GraphAPI* graphapi = serviceapi.CreateGraphAPI(request);
      VertexAttribute* vattr = graphapi->GetOneVertex(local_start);
      request.outputwriter_->WriteName("vertex");
      vattr->WriteToJson(*request.outputwriter_);
      delete graphapi;
      request.outputwriter_->WriteName("neighborsize");
      UDF_t udf(3, local_start, request.outputwriter_);
      serviceapi.RunUDF(&request, serviceapi.GetAdaptor(&request, &udf));
      request.output_idservice_vids.push_back(local_start);
      request.outputwriter_->WriteEndObject();
    }

  };
}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
