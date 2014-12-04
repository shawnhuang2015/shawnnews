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
#include <gutil/jsonwriter.hpp>
#include <json/json.h>
#include <gpelib4/enginebase/udfstatus.hpp>

namespace gperun {

  bool EngineJobListener::HandleOffLineRequest(std::string& requestid, std::string& request){
    Json::Value requestRoot;
    Json::Reader jsonReader;
    jsonReader.parse(request, requestRoot);
    Json::Value jsArgv = requestRoot["argv"];
    gpelib4::UDFStatus* udfstatus = this->jobrunner()->GetUdfStatus();
    bool ok = false;
    bool error = false;
    std::string msg;
    gutil::JSONWriter jsonwriter;
    jsonwriter.WriteStartObject();
    jsonwriter.WriteName("results");
    if(jsArgv[0].asString() == "offline"){
      error = udfstatus == NULL || udfstatus->RequestId() != jsArgv[2].asString();
      msg = error ? "failed to retrieve corresponding udf status" : "";
      if(!error){
        udfstatus->WriteToJSON(jsonwriter);
      }
    } else {
      GASSERT(requestRoot["mode"].asString() == "offline", requestRoot["mode"].asString());
      ok = udfstatus == NULL || udfstatus->Finished();
      if(ok) {
        gpelib4::UDFStatus* newudfstatus = new gpelib4::UDFStatus("offline_req");
        newudfstatus->WriteToJSON(jsonwriter);
        this->jobrunner()->SetUdfStatus(newudfstatus);
        if(deltarebuilder_ != NULL)
          deltarebuilder_->StopRebuildThread();
      } else {
        error = true;
        msg = "last request has not finished yet.";
      }
    }
    if(error) {
      jsonwriter.WriteStartObject().WriteEndObject();
    }
    jsonwriter.WriteName("error").WriteBool(error);
    jsonwriter.WriteName("message").WriteString(msg);
    jsonwriter.WriteEndObject();
    SetResponse(requestid, jsonwriter.GetBuffer(), jsonwriter.GetSize());
    return ok;
  }

void EngineJobListener::ReadRequest(std::string& requestid,
                                    std::string& request) {
  // POC modification: Not Likely.
  std::vector<boost::tuple<std::string, std::string, int64_t> > read_queue;
  while (true) {
    if(GPEConfig::udfmode_ == "offline"){
      if (daemon_->quit_) {
        connector_->stopReader();
        requestid = "";
        request = "";
        return;
      }
      if(deltarebuilder_ != NULL && !deltarebuilder_->RebuildThreadRunning() && jobrunner()->GetExecutingAndQueuedJobCount() == 0)
       deltarebuilder_->StartRebuildThread();
      connector_->readFromKafka(ready_queue_, 1);
      if(ready_queue_.size() == 0) {
        usleep(1000);
      } else {
        GASSERT(ready_queue_.size() == 1, ready_queue_.size());
        boost::tuple<std::string, std::string, int64_t>& requestPair =
            ready_queue_.back();
        requestid = requestPair.get<0>();
        request = requestPair.get<1>();
        ready_queue_.pop_back();
        if(HandleOffLineRequest(requestid, request)){
          GVLOG(Verbose_UDFHigh) << "running " << requestid << ", " << request;
          return;
        }
      }
      continue;
    }
    // process requests fit on current delta
    if (!ready_queue_.empty()) {
      boost::tuple<std::string, std::string, int64_t>& requestPair =
          ready_queue_.back();
      requestid = requestPair.get<0>();
      request = requestPair.get<1>();
      ready_queue_.pop_back();
      GVLOG(Verbose_UDFHigh) << "running " << requestid << ", " << request;
      return;
    }
    read_queue.clear();
    if (daemon_->quit_ && connector_->isReaderNull(0)) {
      // quite and no more requests. tell listener to stop
      requestid = "";
      request = "";
      return;
    }
    if (daemon_->quit_) {
      // receive command. stop reader and finish queue requests in reader first.
      connector_->stopReader();
      connector_->readFromKafka(read_queue);  // read all msg.
    } else {
      connector_->readFromKafka(read_queue);
    }
    if (read_queue.size() == 0) {
      usleep(1000);
    } else {
      debugstr_.str("");
      bool debug = false;
      for (size_t i = 0; i < read_queue.size(); ++i) {
        if (gutil::extract_rest_request_debugflag(read_queue[i].get<1>())) {
          debug = true;
          break;
        }
      }
      if (debug) {
        debugstr_ << "Host name: " << hostname_ << "\n";
        debugstr_ << "{\n";
        for (size_t i = 0; i < read_queue.size(); ++i)
          debugstr_ << "Receieve (" << read_queue[i].get<0>() << ","
              << read_queue[i].get<1>() << ") on "
              << read_queue[i].get<2>() << "\n";
        debugstr_ << "}\n";
        debugstr_ << "Start process above requests at "
                  << gutil::GTimer::now_str() << "\n";
      }
      // retreive will be from back.
      std::reverse(read_queue.begin(), read_queue.end());
      ready_queue_.insert(ready_queue_.begin(), read_queue.begin(), read_queue.end());
    }
  }
}

void EngineJobListener::SetResponse(std::string& requestid, char* response, size_t response_size) {
  // POC modification: Not Likely.
  if (response_size == 0)
    return;
  GPROFILER(requestid) << "GPE|EngineJobListener|SetResponse_enter|" << "\n";
  Lock_t lock(mutex_);
  connector_->setResponse(requestid, response, response_size);
  GPROFILER(requestid) << "GPE|EngineJobListener|SetResponse_done|"<< response_size << std::endl;
}

}  // namespace gperun

