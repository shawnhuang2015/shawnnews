/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * enginejoblistener.cpp: /gdbms/src/customers/gpe_base/service/enginejoblistener.cpp
 *
 *  Created on: May 6, 2014
 *      Author: lichen
 ******************************************************************************/

#include "enginejoblistener.hpp"

namespace gperun {

void EngineJobListener::ReadRequest(std::string& requestid,
                                    std::string& request) {
  // POC modification: Not Likely.
  while (true) {
    // process requests fit on current delta
    if (!ready_queue_.empty()) {
      boost::tuple<std::string, std::string, std::string>& requestPair =
          ready_queue_.back();
      requestid = requestPair.get<0>();
      request = requestPair.get<1>();
      ready_queue_.pop_back();
      GVLOG(Verbose_UDFHigh) << "running " << requestid << ", " << request;
      return;
    }
    // wait until service not doing anything
    while (jobrunner()->GetExecutingAndQueuedJobCount() > 0) {
      usleep(1000);  // sleep one ms.
    }
    if (daemon_->quit_ && connector_->ReaderIsNull()) {
      // quite and no more requests. tell listener to stop
      requestid = "";
      request = "";
      return;
    }
    if (daemon_->rebuild_ && connector_->ReaderIsNull()) {
      // start rebuild
      jobrunner()->Topology_RebuildForDelta();
      jobrunner()->Topology_WarmUp(true);
      connector_->InitReader();
      daemon_->DoneRebuild();
    } else if (daemon_->quit_ || daemon_->rebuild_) {
      // receive command. stop reader and finish queue requests in reader first.
      connector_->StopReader();
      connector_->ReadFromKafka(ready_queue_);  // read all msg.
      connector_->DeleteReader();
    } else {
      connector_->ReadFromKafka(ready_queue_);
    }
    if (ready_queue_.size() == 0) {
      jobrunner()->Topology_PullDelta(NULL);
      usleep(1000);
    } else {
      debugstr_.str("");
      bool debug = false;
      for (size_t i = 0; i < ready_queue_.size(); ++i) {
        if (boost::algorithm::ends_with(ready_queue_[i].get<1>(),
                                        "true")) {
          debug = true;
          break;
        }
      }
      if (debug) {
        debugstr_ << "Host name: " << hostname_ << "\n";
        debugstr_ << "{\n";
        for (size_t i = 0; i < ready_queue_.size(); ++i)
          debugstr_ << "Receieve (" << ready_queue_[i].get<0>() << ","
              << ready_queue_[i].get<1>() << ") on "
              << ready_queue_[i].get<2>() << "\n";
        debugstr_ << "}\n";
        debugstr_ << "Start process above requests at "
                  << gutil::GTimer::now_str() << "\n";
      }
      // retreive will be from back.
      std::reverse(ready_queue_.begin(), ready_queue_.end());
      jobrunner()->Topology_PullDelta(debug ? &debugstr_ : NULL);
    }
  }
}

void EngineJobListener::SetResponse(std::string& requestid,
                                    std::string& response) {
  // POC modification: Not Likely.
  if (response.size() == 0)
    return;
  GPROFILER(requestid) << "GPE|EngineJobListener|SetResponse_enter|" << "\n";
  Lock_t lock(mutex_);
  connector_->SetResponse(requestid, response);
  GPROFILER(requestid) << "GPE|EngineJobListener|SetResponse_done|"<< response.length() << "\n";
}

}  // namespace gperun

