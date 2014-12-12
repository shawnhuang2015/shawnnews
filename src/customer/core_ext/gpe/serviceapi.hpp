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

#include "internalgraphapi.hpp"

namespace gperun{
  /**
   * @brief  ServiceAPI : to provide the interface to access GPE engine functionalities.
   */
  class ServiceAPI{
  public:
    /// run main function for service.
    static void Run(int argc, char** argv, ServiceImplBase* serviceimpl, PostJson2Delta* postdelta_impl = NULL);

    ~ServiceAPI();

    /// convert string vertexid to internal integer id. Return false if failed.
    bool UIdtoVId(EngineServiceRequest& request, std::string strid, VertexLocalId_t& vid);

    /// create a GraphAPI object. Caller need to delete the return pointer.
    GraphAPI* CreateGraphAPI(EngineServiceRequest& request);

    /// get the EnumMappers pointer for enumerator conversion.
    EnumMappers* GetEnumMappers();

    /// generate a worker adaptor based on UDF template UDF and object
    template<typename DERIVEDUDF>
    WorkerAdaptor* GetAdaptor(EngineServiceRequest* request, DERIVEDUDF* udf) {
      if(request->udfstatus_ == NULL && adaptorcache() != NULL)
        return adaptorcache()->GetAdaptor<DERIVEDUDF>(udf, request->querystate_);
      else
        return  new gpelib4::SingleMachineAdaptor<DERIVEDUDF>(
              udf, globalinstance(), topology(), sparsemode(), request->querystate_);
    }

    /// run UDF request.
    std::string RunUDF(EngineServiceRequest* request, WorkerAdaptor* adaptor);

    /// retreieve topology meta information.
    TopologyMeta* GetTopologyMeta();

  private:
    friend class EngineJobRunner;
    ServiceAPI(int, char**, ServiceImplBase*, PostJson2Delta*);
    void RunMain();
    std::string processname_;
    std::vector<std::string> config_info_;
    ServiceImplBase* serviceimpl_;
    PostJson2Delta* postdelta_impl_;
    bool create_postdelta_impl_;
    EngineJobRunner* service_;
    gmmnt::GlobalInstances* globalinstance() const;
    topology4::TopologyGraph* topology() const;
    bool sparsemode() const;
    gpelib4::SingleAdaptorCache* adaptorcache() const;

  };

}  // namespace gperun
#endif /* GPE_SERVICEAPI_HPP_ */
