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
    // POC modification: Not Likely.
    Json::Value root;
    root["results"] = Json::arrayValue;
    if (!GPEConfig::CheckTimeOut(instance->requestid_)) {
      root["debug"] = "";
      root["message"] = "error_: request timed out";
      root["error"] = true;
      std::string result = root.toStyledString();
      GVLOG(Verbose_UDFHigh) << "Request timeout " << instance->requestid_;
      return result;
    }
    gutil::GTimer timer;
    bool debugmode = false;
    if (boost::algorithm::ends_with(instance->requestid_, ":D"))
      debugmode = true;
    else if (boost::algorithm::ends_with(instance->requestid_, ":N"))
      debugmode = false;
    std::string debugmsg = "";
    if (debugmode) {
      debugmsg += "Service running "
                  + boost::lexical_cast<std::string>(this->maxthreads_)
                  + " instances \n";
      debugmsg += "Topology: "
                  + boost::lexical_cast<std::string>(
                    topology_->GetTopologyMeta()->vertexcount_) + " nodes, "
                  + boost::lexical_cast<std::string>(
                    topology_->GetTopologyMeta()->edgecount_) + " edges\n";
      debugmsg += "{\nRequest start run at " + gutil::GTimer::now_str()
                  + "\n";
      debugmsg += "CPU usage: "
                  + boost::lexical_cast<std::string>(
                    gutil::GSystem::get_sys_cpu_usage()) + "%\n";
      debugmsg += "Free memory: "
                  + boost::lexical_cast<std::string>(
                    gutil::GSystem::get_sys_free_memory()) + "\n}\n";
    }
    gse2::IdConverter::RequestIdMaps* maps = idconverter_->GetIdMaps(
                                               instance->requestid_);
    unsigned int iterations = Run(instance, root, maps);
    delete maps;
    if (debugmode) {
      debugmsg += "{\nRequest end at " + gutil::GTimer::now_str() + " with "
                  + boost::lexical_cast<std::string>(iterations) + " iterations in "
                  + boost::lexical_cast<std::string>(timer.GetTotalMilliSeconds())
                  + " ms\n";
      debugmsg += "CPU usage: "
                  + boost::lexical_cast<std::string>(
                    gutil::GSystem::get_sys_cpu_usage()) + "%\n";
      debugmsg += "Free memory: "
                  + boost::lexical_cast<std::string>(
                    gutil::GSystem::get_sys_free_memory()) + "\n}\n";
      debugmsg = this->joblistener_->GetDebugString() + debugmsg;
    }
    root["debug"] = debugmsg;
    Json::FastWriter writer;
    std::string result = writer.write(root);
    GVLOG(Verbose_UDFHigh) << "Finished " << instance->requestid_ << "|"
                           << timer.GetTotalMilliSeconds() << " ms" << "|"
                           << result;
    return result;
  }

  unsigned int EngineJobRunner::Run(EngineServiceRequest* request,
                                    Json::Value& root,
                                    gse2::IdConverter::RequestIdMaps* maps) {
    try {
      // check id map first
      if (maps == NULL) {
        root["message"] = "error_: Request  " + request->requestid_
                          + " failed to retrieve id map.";
        root["error"] = true;
        return 0;
      }
      // parse request json string
      Json::Value requestRoot;
      Json::Reader jsonReader;
      jsonReader.parse(request->requeststr_, requestRoot);
      std::vector<std::string> argv;
      std::map<std::string,std::string> options;
      Json::Value jsArgv = requestRoot["argv"];
      for (uint32_t i = 0; i < jsArgv.size(); ++i) {
        argv.push_back(jsArgv[i].asString());
      }
      Json::Value jsoptions = requestRoot["options"];
      std::vector<std::string> members = jsoptions.getMemberNames();
      for (uint32_t i = 0; i < members.size(); ++i) {
        options.insert(std::pair<std::string,std::string>(members[i], jsoptions[members[i]].asString()));
      }
      std::vector<VertexLocalId_t> idservice_vids;
      if (argv[0] == "debug_neighbors") {
        VertexLocalId_t vid = 0;
        if (!Util::UIdtoVId(topology_, maps, root, argv[1], vid))
          return 0;
        ShowOneVertexInfo(request, root, vid, idservice_vids);
      } else {
        bool succeed =  UDIMPL::GPE_UD_Impl::RunQuery(this, maps, argv, options,
                                                      idservice_vids, root, GPEConfig::customizedsetttings_);
        if (!succeed) {
          root["message"] = "error_: unknown function error "
                            + request->requeststr_;
          root["error"] = true;
          return 0;
        }
      }
      idconverter_->sendRequest2IDS(request->requestid_, idservice_vids);
    } catch(const boost::exception& bex) {
      root["message"] = "error_: unexpected error " +  boost::diagnostic_information(bex);
      root["error"] = true;
      return 0;
    } catch(const std::exception& ex) {
      root["message"] = "error_: unexpected error " + std::string(ex.what());
      root["error"] = true;
      return 0;
    } catch(...) {
      root["message"] = "error_: unexpected error ";
      root["error"] = true;
      return 0;
    }
    return 0;
  }

  void EngineJobRunner::ShowOneVertexInfo(EngineServiceRequest* request,
                                          Json::Value& root,
                                          VertexLocalId_t vid,
                                          std::vector<VertexLocalId_t>& idservice_vids) {
    gapi4::GraphAPI api(topology_);
    topology4::VertexAttribute* v1 = api.GetOneVertex(vid);
    root["source"]["vid"] = Util::MarkVId(vid);
    idservice_vids.push_back(vid);
    v1->WriteToJson(root["source"]["vertexattr"]);
    gapi4::EdgesCollection edgeresults;
    api.GetOneVertexEdges(vid, NULL, edgeresults);
    root["edges"] = Json::arrayValue;
    while (edgeresults.NextEdge()) {
      Json::Value thisRec;
      VertexLocalId_t toid = edgeresults.GetCurrentToVId();
      topology4::EdgeAttribute* edgeattr = edgeresults
                                           .GetCurrentEdgeAttribute();
      thisRec["to_vid"] = Util::MarkVId(toid);
      idservice_vids.push_back(toid);
      topology4::VertexAttribute* v2 = api.GetOneVertex(toid);
      v2->WriteToJson(thisRec["to_vertexattr"]);
      edgeattr->WriteToJson(thisRec["edgeattr"]);
      root["edges"].append(thisRec);
    }
  }

}  // namespace gperun

