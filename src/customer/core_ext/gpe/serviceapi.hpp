/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * serviceapi.hpp: /gdbms/src/customers/gpe/serviceapi.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_SERVICEAPI_HPP_
#define GPE_SERVICEAPI_HPP_

#include <gapi4/graphapi.hpp>
#include <gpelib4/engine/singlemachineadaptor.hpp>
#include <gpelib4/enginedriver/singleadaptorcache.hpp>
#include <gpelib4/enginedriver/engineservicerequest.hpp>
#include <gse2/dict/enum/enum_mapper.hpp>

namespace topology4{
  class TopologyGraph;
}

namespace gse2{
  class PostJson2Delta;
}

namespace gperun{
  class ServiceImplBase;
  class EngineJobRunner;

  /**
   * @brief  ServiceAPI : to provide the interface to access GPE engine functionalities.
   */
  class ServiceAPI{
  public:
    ServiceAPI(int argc, char** argv, ServiceImplBase* serviceimpl, gse2::PostJson2Delta* postdelta_impl);

    /// run main function for service.
    void RunMain();

    /// convert string vertexid to internal integer id. Return false if failed.
    bool UIdtoVId(gpelib4::EngineServiceRequest& request, std::string strid, VertexLocalId_t& vid);

    /// create a GraphAPI object. Caller need to delete the return pointer.
    gapi4::GraphAPI* CreateGraphAPI(gpelib4::EngineServiceRequest& request);

    /// get the EnumMappers pointer for enumerator conversion.
    gse2::EnumMappers* GetEnumMappers();

    /// generate a worker adaptor based on UDF template UDF and object
    template<typename DERIVEDUDF>
    gpelib4::WorkerAdaptor* GetAdaptor(gpelib4::EngineServiceRequest* request, DERIVEDUDF* udf) {
      if(request->udfstatus_ == NULL && adaptorcache() != NULL)
        return adaptorcache()->GetAdaptor<DERIVEDUDF>(udf, request->querystate_);
      else
        return  new gpelib4::SingleMachineAdaptor<DERIVEDUDF>(
              udf, globalinstance(), topology(), sparsemode(), request->querystate_);
    }

    /// run UDF request.
    std::string RunUDF(gpelib4::EngineServiceRequest* request, gpelib4::WorkerAdaptor* adaptor);

    /// retreieve topology meta information.
    topology4::TopologyMeta* GetTopologyMeta();

  private:
    friend class EngineJobRunner;

    std::string processname_;
    std::vector<std::string> config_info_;
    ServiceImplBase* serviceimpl_;
    gse2::PostJson2Delta* postdelta_impl_;
    EngineJobRunner* service_;

    gmmnt::GlobalInstances* globalinstance() const;
    topology4::TopologyGraph* topology() const;
    bool sparsemode() const;
    gpelib4::SingleAdaptorCache* adaptorcache() const;

  };

}  // namespace gperun
#endif /* GPE_SERVICEAPI_HPP_ */
