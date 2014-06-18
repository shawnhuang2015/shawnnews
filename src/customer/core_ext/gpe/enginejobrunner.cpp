/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * enginejobrunner.cpp: /gdbms/src/customers/gpe_base/service/enginejobrunner.cpp
 *
 *  Created on: May 6, 2014
 *      Author: lichen
 ******************************************************************************/

#include "enginejobrunner.hpp"
#include <gutil/gsystem.hpp>
#include <gapi4/graphapi.hpp>
#include <json/json.h>
#include "gpeconfig.hpp"
#include "util.hpp"
#include "../../core_impl/gpe_impl/impl.hpp"

namespace gperun {

  void EngineJobRunner::Topology_Prepare() {
    UDIMPL::GPE_UD_Impl::Topology_Prepare(topology_);
    if (postListener_ != NULL)
      topology_->GetTopologyMeta()->SetEnumeratorMapping(
            postListener_->enumMappers()->GetEnumeratorMappings());
  }

  void EngineJobRunner::Topology_PullDelta(std::stringstream* debugstr) {
    // POC modification: Not Likely.
    if (postListener_ == NULL)
      return;
    uint64_t deltasize = 0;
    char* deltadata = postListener_->getAllDelta(deltasize);
    if (deltadata == NULL || deltasize == 0)
      return;
    topology_->GetDeltaRecords()->ReadDeltas(
          reinterpret_cast<uint8_t*>(deltadata), deltasize);
    delete[] deltadata;
    topology_->HandleNewDelta();
  }

  std::string EngineJobRunner::RunInstance(EngineServiceRequest* instance) {
    gutil::GTimer timer;
    instance->outputstream_ = new std::ostringstream();
    gutil::JSONStringWriter jsonwriter(*instance->outputstream_);
    jsonwriter.WriteStartObject();
    jsonwriter.WriteName("results");
    std::ostringstream debugmsg;
    if (!GPEConfig::CheckTimeOut(instance->requestid_)) {
      instance->message_ = "error_: request timed out";
      instance->error_ = true;
    } else {
      bool debugmode = false;
      if (boost::algorithm::ends_with(instance->requestid_, ":D"))
        debugmode = true;
      else if (boost::algorithm::ends_with(instance->requestid_, ":N"))
        debugmode = false;
      if (debugmode) {
        debugmsg << this->joblistener_->GetDebugString();
        debugmsg << "Service running " << this->maxthreads_  << " instances \n";
        debugmsg << "Topology: " << topology_->GetTopologyMeta()->vertexcount_ << " nodes, "
                    << topology_->GetTopologyMeta()->edgecount_ << " edges\n";
        debugmsg << "{\nRequest start run at " << gutil::GTimer::now_str() <<"\n";
        debugmsg << "CPU usage: " << gutil::GSystem::get_sys_cpu_usage() << "%\n";
        debugmsg << "Free memory: " << gutil::GSystem::get_sys_free_memory() << "\n}\n";
      }
      gse2::IdConverter::RequestIdMaps* maps = idconverter_->GetIdMaps(
                                                 instance->requestid_);
      unsigned int iterations = Run(instance, maps, jsonwriter);
      delete maps;
      if (debugmode) {
        debugmsg << "{\nRequest end at " << gutil::GTimer::now_str() << " with "
                    << iterations << " iterations in "
                    << timer.GetTotalMilliSeconds() << " ms\n";
        debugmsg << "CPU usage: " << gutil::GSystem::get_sys_cpu_usage() << "%\n";
        debugmsg << "Free memory: " << gutil::GSystem::get_sys_free_memory() << "\n}\n";
      }
    }
    jsonwriter.WriteName("error").WriteBool(instance->error_);
    jsonwriter.WriteName("message").WriteString(instance->message_);
    std::string debugstrmsg = debugmsg.str();
    jsonwriter.WriteName("debug").WriteString(debugstrmsg);
    jsonwriter.WriteEndObject(); // end of everything
    GVLOG(Verbose_UDFHigh) << "Finished " << instance->requestid_ << "|"
                           << timer.GetTotalMilliSeconds() << " ms";
    return "";
  }

  unsigned int EngineJobRunner::Run(EngineServiceRequest* request,
                                    gse2::IdConverter::RequestIdMaps* maps,
                                    gutil::JSONStringWriter& jsonwriter) {
    size_t num_iterations = 0;
    try {
      // check id map first
      if (maps == NULL) {
        request->message_ = "error_: Request  " + request->requestid_
                          + " failed to retrieve id map.";
        request->error_ = true;
        return 0;
      }
      // parse request json string
      Json::Value requestRoot;
      Json::Reader jsonReader;
      jsonReader.parse(request->requeststr_, requestRoot);
      std::vector<std::string> argv;

      Json::Value jsArgv = requestRoot["argv"];
      for (uint32_t i = 0; i < jsArgv.size(); ++i) {
        argv.push_back(jsArgv[i].asString());
      }
      Json::Value jsoptions = requestRoot["options"];
      std::vector<VertexLocalId_t> idservice_vids;
      if (argv[0] == "debug_neighbors") {
        VertexLocalId_t vid = 0;
        if (!Util::UIdtoVId(topology_, maps, request->message_, request->error_, argv[1], vid))
          return 0;
        ShowOneVertexInfo(request, jsonwriter, vid, idservice_vids);
      } else {
        bool succeed =  UDIMPL::GPE_UD_Impl::RunQuery(request,
                                                      this,
                                                      maps,
                                                      argv,
                                                      jsoptions,
                                                      idservice_vids,
                                                      jsonwriter,
                                                      num_iterations,
                                                      GPEConfig::customizedsetttings_);
        if (!succeed) {
          request->message_ = "error_: unknown function error "
                            + request->requeststr_;
          request->error_ = true;
          return 0;
        }
      }
      idconverter_->sendRequest2IDS(request->requestid_, idservice_vids);
    } catch(const boost::exception& bex) {
      request->message_ = "error_: unexpected error " +  boost::diagnostic_information(bex);
      request->error_ = true;
      return 0;
    } catch(const std::exception& ex) {
      request->message_ = "error_: unexpected error " + std::string(ex.what());
      request->error_ = true;
      return 0;
    } catch(...) {
      request->message_ = "error_: unexpected error ";
      request->error_ = true;
      return 0;
    }
    return num_iterations;
  }

  void EngineJobRunner::ShowOneVertexInfo(EngineServiceRequest* request,
                                          gutil::JSONStringWriter& jsonwriter,
                                          VertexLocalId_t vid,
                                          std::vector<VertexLocalId_t>& idservice_vids) {
    gapi4::GraphAPI api(topology_);
    topology4::VertexAttribute* v1 = api.GetOneVertex(vid);
    jsonwriter.WriteName("source");
    jsonwriter.WriteStartObject();
    std::string markvid = Util::MarkVId(vid);
    jsonwriter.WriteName("vid").WriteString(markvid);
    idservice_vids.push_back(vid);
    jsonwriter.WriteName("vertexattr");
    v1->WriteToJson(jsonwriter);
    jsonwriter.WriteEndObject(); // source
    gapi4::EdgesCollection edgeresults;
    api.GetOneVertexEdges(vid, NULL, edgeresults);
    jsonwriter.WriteName("edges");
    jsonwriter.WriteStartArray();
    while (edgeresults.NextEdge()) {
      jsonwriter.WriteStartObject();
      VertexLocalId_t toid = edgeresults.GetCurrentToVId();
      topology4::EdgeAttribute* edgeattr = edgeresults
                                           .GetCurrentEdgeAttribute();
      markvid = Util::MarkVId(toid);
      jsonwriter.WriteName("to_vid").WriteString(markvid);
      idservice_vids.push_back(toid);
      topology4::VertexAttribute* v2 = api.GetOneVertex(toid);
      jsonwriter.WriteName("to_vertexattr");
      v2->WriteToJson(jsonwriter);
      jsonwriter.WriteName("edgeattr");
      edgeattr->WriteToJson(jsonwriter);
      jsonwriter.WriteEndObject();
    }
    jsonwriter.WriteEndArray();
  }

}  // namespace gperun

