/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * enginejoblistener.hpp: /gdbms/src/customers/gpe_base/service/enginejoblistener.hpp
 *
 *  Created on: May 16, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_ENGINEJOBLISTENER_HPP_
#define GPE_ENGINEJOBLISTENER_HPP_

#include <gutil/gsystem.hpp>
#include <topology4/deltarebuilder.hpp>
#include <gpelib4/enginebase/servicebase.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include  "gpeconfig.hpp"
#include  "kafkaconnector.hpp"

namespace gperun {

/**
 * engine job listener with delta post messages support.
 * POC modification: Not Likely.
 */
class EngineJobListener : public gpelib4::JobListener {
 public:
  typedef boost::mutex Mutex_t;
  typedef boost::mutex::scoped_lock Lock_t;

  EngineJobListener(GPEDaemon* daemon, KafkaConnector* connector,
                    std::string hostname, topology4::DeltaRebuilder* deltarebuilder = NULL)
      : connector_(connector),
        daemon_(daemon),
        hostname_(hostname),
        deltarebuilder_(deltarebuilder){
    if(deltarebuilder_ != NULL)
     deltarebuilder_->StartRebuildThread();
  }

  ~EngineJobListener() {
  }

  void ReadRequest(std::string& requestid, std::string& request);

  void SetResponse(std::string& requestid, char* response, size_t response_size);

 private:
  /// Lock just for buffer_
  Mutex_t mutex_;
  /// Kafka connector
  KafkaConnector* connector_;
  /// GPE daemon pointer
  GPEDaemon* daemon_;
  /// the request ready queue
  std::vector<boost::tuple<std::string, std::string, std::string> > ready_queue_;
  /// host name
  std::string hostname_;
  topology4::DeltaRebuilder* deltarebuilder_;

  bool HandleOffLineRequest(std::string& requestid, std::string& request);
};

}  // namespace gperun

#endif /* GPE_ENGINEJOBLISTENER_HPP_ */
