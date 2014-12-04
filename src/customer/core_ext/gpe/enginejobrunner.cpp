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
#include <gpelib4/enginedriver/degreehistogram.hpp>
#include <gpelib4/engine/master.hpp>
#include <gpelib4/engine/worker.hpp>
#include <gapi4/graphapi.hpp>
#include <topology4/deltarecords.hpp>
#include <topology4/topologygraph.hpp>
#include <json/json.h>
#include "serviceapi.hpp"
#include "serviceimplbase.hpp"

namespace gperun {

  void EngineJobRunner::Topology_Prepare() {
    // UDIMPL::GPE_UD_Impl::Topology_Prepare(topology_);
    if (postListener_ != NULL)
      topology_->GetTopologyMeta()->SetEnumeratorMapping(
            postListener_->enumMappers()->GetEnumeratorMappings());
  }

  void EngineJobRunner::Topology_PullDelta() {
    if(postListener_ == NULL)
      return;
    PullDelta();
    // start pull delta thread
    pulldeltathread_ = new boost::thread(boost::bind(&EngineJobRunner::PullDeltaThread, this));
  }

  std::string EngineJobRunner::RunInstance(gpelib4::EngineServiceRequest* instance) {
    gutil::GTimer timer;
    instance->querystate_ = new topology4::QueryState();
    instance->querystate_->tid_ = gutil::GTimer::GetTotalMicroSecondsSinceEpoch();
#ifdef ENABLETRANSACTION
    instance->querystate_->tid_ = gutil::extract_transactionid(instance->requestid_);
#else
    instance->querystate_->tid_ = tid_;
#endif
    topology_->GetCurrentSegementMeta(instance->querystate_->query_segments_meta_);
    instance->udfstatus_ = GetUdfStatus();
    instance->outputwriter_ = new gutil::JSONWriter();
    instance->output_num_iterations_ = 0;
    instance->outputwriter_->WriteStartObject();
    instance->outputwriter_->WriteName("results");
    std::ostringstream debugmsg;
    if (!GPEConfig::CheckTimeOut(instance->requestid_)) {
      instance->message_ = "error_: request timed out";
      instance->error_ = true;
    } else {
      bool debugmode = gutil::extract_rest_request_debugflag(instance->requestid_);
      if (debugmode) {
        debugmsg << this->joblistener_->GetDebugString();
        debugmsg << "Service running " << this->maxthreads_  << " instances \n";
        debugmsg << "Topology: " << topology_->GetTopologyMeta()->vertexcount_ << " nodes, "
                 << topology_->GetTopologyMeta()->edgecount_ << " edges\n";
        debugmsg << "{\nRequest start run at " << gutil::GTimer::now_str() <<"\n";
        debugmsg << "CPU usage: " << gutil::GSystem::get_sys_cpu_usage() << "%\n";
        debugmsg << "Free memory: " << gutil::GSystem::get_sys_free_memory() << "\n}\n";
      }
      if(gutil::extract_rest_request_idrequestflag(instance->requestid_)){
        instance->requestid_maps_ = idconverter_->GetIdMaps(instance->requestid_);
        if (instance->requestid_maps_ == NULL) {
          instance->message_ = "error_: Request  " + instance->requestid_ + " failed to retrieve id map.";
          instance->error_ = true;
        }
      }
      if(!instance->error_)
        Run(instance);
      if(instance->requestid_maps_ != NULL)
        delete instance->requestid_maps_;
      delete instance->querystate_;
      if (debugmode) {
        debugmsg << "{\nRequest end at " << gutil::GTimer::now_str() << " with "
                 << instance->output_num_iterations_ << " iterations in "
                 << timer.GetTotalMilliSeconds() << " ms\n";
        debugmsg << "CPU usage: " << gutil::GSystem::get_sys_cpu_usage() << "%\n";
        debugmsg << "Free memory: " << gutil::GSystem::get_sys_free_memory() << "\n}\n";
      }
    }
    if(instance->udfstatus_ != NULL) {
      instance->udfstatus_->SetFinished();
      if(instance->error_){
        instance->udfstatus_->SetError();
        instance->udfstatus_->AddMessage(instance->message_);
      }
    }
    if(instance->error_){
      // if error. Write start object and end object for "results".
      instance->outputwriter_->WriteStartObject();
      instance->outputwriter_->WriteEndObject();
    }
    instance->outputwriter_->WriteName("error").WriteBool(instance->error_);
    instance->outputwriter_->WriteName("message").WriteString(instance->message_);
    std::string debugstrmsg = debugmsg.str();
    instance->outputwriter_->WriteName("debug").WriteString(debugstrmsg);
    instance->outputwriter_->WriteEndObject(); // end of everything
    GVLOG(Verbose_UDFHigh) << "Finished " << instance->requestid_ << "|"
                           << timer.GetTotalMilliSeconds() << " ms";
    return "";
  }

  void EngineJobRunner::Run(gpelib4::EngineServiceRequest* request) {
    try {
      // parse request json string
      Json::Value requestRoot;
      Json::Reader jsonReader;
      jsonReader.parse(request->requeststr_, requestRoot);
      Json::Value jsArgv = requestRoot["argv"];
      for (uint32_t i = 0; i < jsArgv.size(); ++i) {
        request->request_argv_.push_back(jsArgv[i].asString());
      }
      request->jsoptions_ = requestRoot["options"];
      if(request->request_argv_.size() == 0){
        request->message_ = "error_: Request  " + request->requestid_
                            + " does not contain argv.";
        request->error_ = true;
        return;
      }
      request->request_function_ = request->request_argv_[0];
      if (request->request_function_ == "builtins") {
        RunStandardAPI(request);
      } else if(serviceapi_->serviceimpl_ != NULL){
        bool succeed = serviceapi_->serviceimpl_->RunQuery(*serviceapi_, *request);
        if (!succeed && !request->error_) {
          request->message_ = "error_: unknown function error " + request->requeststr_;
          request->error_ = true;
        }
      } else{
        request->message_ = "error_: no handler to handle query requests " + request->requeststr_;
        request->error_ = true;
      }
      GPROFILER(request->requestid_) << "GPE|Run|sendRequest2IDS|" << "\n";
      idconverter_->sendRequest2IDS(request->requestid_, request->output_idservice_vids);
      GPROFILER(request->requestid_) << "GPE|Run|sendRequest2IDS|" << request->output_idservice_vids.size() << "\n";
    } catch(const boost::exception& bex) {
      request->message_ = "error_: unexpected error " +  boost::diagnostic_information(bex);
      request->error_ = true;
    } catch(const std::exception& ex) {
      request->message_ = "error_: unexpected error " + std::string(ex.what());
      request->error_ = true;
    } catch(...) {
      request->message_ = "error_: unexpected error ";
      request->error_ = true;
    }
  }

  void EngineJobRunner::RunStandardAPI(gpelib4::EngineServiceRequest* request){
    GPROFILER(request->requestid_) << request->requeststr_ << "\n";
    Json::Value dataRoot;
    Json::Reader jsonReader;
    jsonReader.parse(request->jsoptions_["payload"][0].asString(), dataRoot);
    std::string func = dataRoot["function"].asString();
    if(func == "vattr"){
      std::string vidstr = "";
      if(!dataRoot["translate_ids"].isNull())
        vidstr = dataRoot["translate_ids"][0].asString();
      else
        vidstr = boost::lexical_cast<std::string>(dataRoot["translate_typed_ids"][0]["type"].asInt()) + "_" +  dataRoot["translate_typed_ids"][0]["id"].asString();
      VertexLocalId_t vid = 0;
      if (!serviceapi_->UIdtoVId(*request, vidstr, vid)){
        return;
      }
      gapi4::GraphAPI api(topology_, request->querystate_);
      topology4::VertexAttribute* v1 = api.GetOneVertex(vid);
      request->outputwriter_->WriteStartObject();
      if(v1 != NULL){
        request->outputwriter_->WriteName("attr");
        v1->WriteToJson(*request->outputwriter_);
      }
      request->outputwriter_->WriteEndObject();
    } else if(func == "edges"){
      std::string vidstr = "";
      if(!dataRoot["translate_ids"].isNull())
        vidstr = dataRoot["translate_ids"][0].asString();
      else
        vidstr = boost::lexical_cast<std::string>(dataRoot["translate_typed_ids"][0]["type"].asInt()) + "_" +  dataRoot["translate_typed_ids"][0]["id"].asString();
      bool writedgeeattr = dataRoot["edgeattr"].asBool();
      bool writetgtvattr = dataRoot["tgtvattr"].asBool();
      size_t limit = dataRoot["limit"].isNull() ? std::numeric_limits<size_t>::max() : dataRoot["limit"].asUInt();
      VertexLocalId_t vid = 0;
      if (!serviceapi_->UIdtoVId(*request, vidstr, vid))
        return;
      gapi4::GraphAPI api(topology_, request->querystate_);
      gapi4::EdgesCollection edgeresults;
      api.GetOneVertexEdges(vid, NULL, edgeresults);
      request->outputwriter_->WriteStartObject();
      request->outputwriter_->WriteName("edges");
      request->outputwriter_->WriteStartArray();
      uint32_t count = 0;
      while (edgeresults.NextEdge()) {
        request->outputwriter_->WriteStartObject();
        VertexLocalId_t toid = edgeresults.GetCurrentToVId();
        request->outputwriter_->WriteName("to_vid").WriteMarkVId(toid);
        topology4::EdgeAttribute* edgeattr = edgeresults.GetCurrentEdgeAttribute();
        request->outputwriter_->WriteName("type").WriteUnsignedInt(edgeattr->type());
        request->output_idservice_vids.push_back(toid);
        if(writetgtvattr){
          topology4::VertexAttribute* v1 = api.GetOneVertex(toid);
          if(v1 != NULL){
            request->outputwriter_->WriteName("to_degree").WriteUnsignedInt(v1->outgoing_degree());
            request->outputwriter_->WriteName("to_attr");
            v1->WriteToJson(*request->outputwriter_);
          }
        }
        if(writedgeeattr){
          request->outputwriter_->WriteName("edgeattr");
          edgeattr->WriteToJson(*request->outputwriter_);
        }
        request->outputwriter_->WriteEndObject();
        if(++count == limit)
          break;
      }
      request->outputwriter_->WriteEndArray();
      request->outputwriter_->WriteEndObject();
    } else if(func == "randomids"){
      bool writeattr = dataRoot["attr"].asBool();
      uint32_t count = dataRoot["count"].isNull() ? 10 : dataRoot["count"].asUInt();
      uint32_t vtype = dataRoot["vtype"].isNull() ? 0 : dataRoot["vtype"].asUInt();
      DegreeType_t mindegree = dataRoot["mindegree"].isNull() ? 0 : dataRoot["mindegree"].asUInt();
      DegreeType_t maxdegree = dataRoot["maxdegree"].isNull() ? std::numeric_limits<DegreeType_t>::max() : dataRoot["maxdegree"].asUInt();
      request->outputwriter_->WriteStartObject();
      request->outputwriter_->WriteName("ids");
      request->outputwriter_->WriteStartArray();
      gapi4::GraphAPI api(topology_, request->querystate_);
      gapi4::VerticesFilter_ByOneType filter(vtype);
      gapi4::VerticesCollection vertexresults;
      api.GetAllVertices(&filter, vertexresults);
      uint32_t matched = 0;
      while(vertexresults.NextVertex()){
        topology4::VertexAttribute* vattr = vertexresults.GetCurrentVertexAttribute();
        DegreeType_t degree = vattr->outgoing_degree();
        if(degree >= mindegree && degree <= maxdegree){
          VertexLocalId_t id = vertexresults.GetCurrentVId();
          request->outputwriter_->WriteStartObject();
          request->outputwriter_->WriteName("vid").WriteMarkVId(id);
          if(writeattr){
            request->outputwriter_->WriteName("attr");
            vattr->WriteToJson(*request->outputwriter_);
          }
          request->output_idservice_vids.push_back(id);
          request->outputwriter_->WriteEndObject();
          if(++matched == count)
            break;
        }
      }
      request->outputwriter_->WriteEndArray();
      request->outputwriter_->WriteEndObject();
    }  else if(func == "degreehistogram"){
      std::vector<std::string> strs;
      std::string degreelist = dataRoot["degreelist"].asString();
      boost::split(strs, degreelist, boost::is_any_of(","));
      std::vector<size_t> limits;
      for (size_t i = 0; i < strs.size(); ++i)
        limits.push_back(boost::lexical_cast<size_t>(strs[i]));
      typedef gpelib4::DegreeHistogram UDF_t;
      UDF_t udf(limits);
      gpelib4::SingleMachineAdaptor<UDF_t> adaptor(&udf, globalinstance_, topology_, false, request->querystate_);
      gpelib4::Master master(&adaptor, "");
      gpelib4::Worker worker(&adaptor);
      std::string result = master.RunUDF();
      request->outputwriter_->WriteStartObject();
      request->outputwriter_->WriteName("degreehistogram").WriteString(result);
      request->outputwriter_->WriteEndObject();
    } else {
      request->message_ = "error_: incorrect function " + func;
      request->error_ = true;
    }
  }

  void EngineJobRunner::PullDeltaThread() {
    if (postListener_ == NULL)
      return;
    while (isrunning_) {
      bool ok = PullDelta();
      if(!ok)
        usleep(1000);
    }
  }

  bool EngineJobRunner::PullDelta(){
    uint64_t deltasize = 0;
    topology4::TransactionId_t tid = 0;
    size_t current_postqueue_pos = 0, current_idresponsequeue_pos = 0;
    char* deltadata = postListener_->getAllDelta(deltasize, tid,
                                                 current_postqueue_pos, current_idresponsequeue_pos);
    if (deltadata != NULL){
      topology_->GetDeltaRecords()->ReadDeltas(reinterpret_cast<uint8_t*>(deltadata), deltasize,
                                               current_postqueue_pos, current_idresponsequeue_pos);
      delete[] deltadata;
      boost::mutex::scoped_lock lock(mpiid_mutex_);
      topology_->GetCurrentSegementMeta(segmentmetas_);
      postq_pos_ = current_postqueue_pos;
      idresq_pos_ = current_idresponsequeue_pos;
      tid_ = tid;
      return true;
    }
    return false;
  }

}  // namespace gperun
