/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * serviceimplbase.hpp: /gdbms/src/customers/gpe/serviceimplbase.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_SERVICEIMPLBASE_HPP_
#define GPE_SERVICEIMPLBASE_HPP_

#include <gpelib4/enginedriver/engineservicerequest.hpp>
#include <vector>

namespace gperun{
  class ServiceAPI;

  class ServiceImplBase{
  public:
    virtual ~ServiceImplBase() {}
    /// one time initialization for service.
    virtual void Init(ServiceAPI& serviceapi, YAML::Node& configuration_node) {}
    /// run one query request.
    virtual bool RunQuery(ServiceAPI& serviceapi, gpelib4::EngineServiceRequest& request){ return false;}

  };
}  // namespace gperun
#endif /* GPE_SERVICEIMPLBASE_HPP_ */
