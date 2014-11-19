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
#include "util.hpp"
#include "../../core_impl/gpe_impl/impl.hpp"

namespace gperun {

  void EngineJobRunner::Topology_Prepare() {
    UDIMPL::GPE_UD_Impl::Topology_Prepare(topology_);
    if (postListener_ != NULL)
      topology_->GetTopologyMeta()->SetEnumeratorMapping(
            postListener_->enumMappers()->GetEnumeratorMappings());
  }

  void EngineJobRunner::Topology_PullDelta() {
    if(postListener_ == NULL)
      return;
    // do first time (make up) delta pull
    uint64_t deltasize = 0;
    char* deltadata = postListener_->getAllDelta(deltasize, current_post_tid_,
                                                 current_postqueue_pos_, current_idresponsequeue_pos_);
    if (deltadata != NULL){
      topology_->GetDeltaRecords()->ReadDeltas(reinterpret_cast<uint8_t*>(deltadata), deltasize,
                                               current_postqueue_pos_, current_idresponsequeue_pos_);
      delete[] deltadata;
    }
    // start pull delta thread
    pulldeltathread_ = new boost::thread(boost::bind(&EngineJobRunner::PullDeltaThread, this));
  }

  std::string EngineJobRunner::RunInstance(EngineServiceRequest* instance) {
    gutil::GTimer timer;
    instance->querystate_.tid_ = gutil::GTimer::GetTotalMicroSecondsSinceEpoch();
#ifdef ENABLETRANSACTION
    instance->querystate_.tid_ = gutil::extract_transactionid(instance->requestid_);
#else
    instance->querystate_.tid_ = current_post_tid_;
#endif
    topology_->GetCurrentSegementMeta(instance->querystate_.query_segments_meta_);
    instance->udfstatus_ = GetUdfStatus();
    instance->writer_ = new gutil::JSONWriter();
    gutil::JSONWriter& jsonwriter = *instance->writer_;
    jsonwriter.WriteStartObject();
    jsonwriter.WriteName("results");
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
      gse2::IdConverter::RequestIdMaps* maps = NULL;
      if(gutil::extract_rest_request_idrequestflag(instance->requestid_)){
        maps = idconverter_->GetIdMaps(instance->requestid_);
        if (maps == NULL) {
          instance->message_ = "error_: Request  " + instance->requestid_ + " failed to retrieve id map.";
          instance->error_ = true;
        }
      }
      unsigned int iterations = 0;
      if(!instance->error_)
        Run(instance, maps, jsonwriter);
      if(maps != NULL)
        delete maps;
      if (debugmode) {
        debugmsg << "{\nRequest end at " << gutil::GTimer::now_str() << " with "
                 << iterations << " iterations in "
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
      jsonwriter.WriteStartObject();
      jsonwriter.WriteEndObject();
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
                                    gutil::JSONWriter& jsonwriter) {
    size_t num_iterations = 0;
    try {
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
      if(argv.size() == 0){
        request->message_ = "error_: Request  " + request->requestid_
                            + " does not contain argv.";
        request->error_ = true;
        return 0;
      }
      std::vector<VertexLocalId_t> idservice_vids;
      if (argv[0] == "debug_neighbors") {
        VertexLocalId_t vid = 0;
        if (!Util::UIdtoVId(topology_, maps, request->message_, request->error_, argv[1], vid, request->querystate_))
          return 0;
        ShowOneVertexInfo(request, jsonwriter, vid, idservice_vids);
      } else if (argv[0] == "builtins") {
        RunStandardAPI(request, maps, jsoptions, idservice_vids, jsonwriter);
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
        if (!succeed && !request->error_) {
          request->message_ = "error_: unknown function error "
                              + request->requeststr_;
          request->error_ = true;
          return 0;
        }
      }
      GPROFILER(request->requestid_) << "GPE|Run|sendRequest2IDS|" << "\n";
      idconverter_->sendRequest2IDS(request->requestid_, idservice_vids);
      GPROFILER(request->requestid_) << "GPE|Run|sendRequest2IDS|" << idservice_vids.size() << "\n";
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

  void EngineJobRunner::RunStandardAPI(gpelib4::EngineDriverService::EngineServiceRequest* request,
                                       gse2::IdConverter::RequestIdMaps* maps,
                                       Json::Value& jsoptions,
                                       std::vector<VertexLocalId_t>& idservice_vids,
                                       gutil::JSONWriter& response_writer){
    GPROFILER(request->requestid_) << request->requeststr_ << "\n";
    Json::Value dataRoot;
    Json::Reader jsonReader;
    jsonReader.parse(jsoptions["payload"][0].asString(), dataRoot);
    std::string func = dataRoot["function"].asString();
    if(func == "vattr"){
      std::string vidstr = "";
      if(!dataRoot["translate_ids"].isNull())
        vidstr = dataRoot["translate_ids"][0].asString();
      else
        vidstr = boost::lexical_cast<std::string>(dataRoot["translate_typed_ids"][0]["type"].asInt()) + "_" +  dataRoot["translate_typed_ids"][0]["id"].asString();
      VertexLocalId_t vid = 0;
      if (!Util::UIdtoVId(topology_, maps, request->message_, request->error_, vidstr, vid, request->querystate_))
        return;
      gapi4::GraphAPI api(topology_, &request->querystate_);
      topology4::VertexAttribute* v1 = api.GetOneVertex(vid);
      response_writer.WriteStartObject();
      if(v1 != NULL){
        response_writer.WriteName("degree").WriteUnsignedInt(v1->outgoing_degree());
        response_writer.WriteName("type").WriteUnsignedInt(v1->type());
        response_writer.WriteName("attr");
        v1->WriteToJson(response_writer);
      }
      response_writer.WriteEndObject();
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
      if (!Util::UIdtoVId(topology_, maps, request->message_, request->error_, vidstr, vid, request->querystate_))
        return;
      gapi4::GraphAPI api(topology_, &request->querystate_);
      gapi4::EdgesCollection edgeresults;
      api.GetOneVertexEdges(vid, NULL, edgeresults);
      response_writer.WriteStartObject();
      response_writer.WriteName("edges");
      response_writer.WriteStartArray();
      uint32_t count = 0;
      while (edgeresults.NextEdge()) {
        response_writer.WriteStartObject();
        VertexLocalId_t toid = edgeresults.GetCurrentToVId();
        response_writer.WriteName("to_vid").WriteMarkVId(toid);
        topology4::EdgeAttribute* edgeattr = edgeresults.GetCurrentEdgeAttribute();
        response_writer.WriteName("type").WriteUnsignedInt(edgeattr->type());
        idservice_vids.push_back(toid);
        if(writetgtvattr){
          topology4::VertexAttribute* v1 = api.GetOneVertex(toid);
          if(v1 != NULL){
            response_writer.WriteName("to_degree").WriteUnsignedInt(v1->outgoing_degree());
            response_writer.WriteName("to_attr");
            v1->WriteToJson(response_writer);
          }
        }
        if(writedgeeattr){
          response_writer.WriteName("edgeattr");
          edgeattr->WriteToJson(response_writer);
        }
        response_writer.WriteEndObject();
        if(++count == limit)
          break;
      }
      response_writer.WriteEndArray();
      response_writer.WriteEndObject();
    } else if(func == "randomids"){
      bool writeattr = dataRoot["attr"].asBool();
      uint32_t count = dataRoot["count"].isNull() ? 10 : dataRoot["count"].asUInt();
      uint32_t vtype = dataRoot["vtype"].isNull() ? 0 : dataRoot["vtype"].asUInt();
      DegreeType_t mindegree = dataRoot["mindegree"].isNull() ? 0 : dataRoot["mindegree"].asUInt();
      DegreeType_t maxdegree = dataRoot["maxdegree"].isNull() ? std::numeric_limits<DegreeType_t>::max() : dataRoot["maxdegree"].asUInt();
      response_writer.WriteStartObject();
      response_writer.WriteName("ids");
      response_writer.WriteStartArray();
      gapi4::GraphAPI api(topology_, &request->querystate_);
      gapi4::VerticesFilter_ByOneType filter(vtype);
      gapi4::VerticesCollection vertexresults;
      api.GetAllVertices(&filter, vertexresults);
      uint32_t matched = 0;
      while(vertexresults.NextVertex()){
        topology4::VertexAttribute* vattr = vertexresults.GetCurrentVertexAttribute();
        DegreeType_t degree = vattr->outgoing_degree();
        if(degree >= mindegree && degree <= maxdegree){
          VertexLocalId_t id = vertexresults.GetCurrentVId();
          response_writer.WriteStartObject();
          response_writer.WriteName("vid").WriteMarkVId(id);
          response_writer.WriteName("degree").WriteUnsignedInt(vattr->outgoing_degree());
          if(writeattr){
            response_writer.WriteName("attr");
            vattr->WriteToJson(response_writer);
          }
          idservice_vids.push_back(id);
          response_writer.WriteEndObject();
          if(++matched == count)
            break;
        }
      }
      response_writer.WriteEndArray();
      response_writer.WriteEndObject();
    }  else if(func == "degreehistogram"){
      std::vector<std::string> strs;
      std::string degreelist = dataRoot["degreelist"].asString();
      boost::split(strs, degreelist, boost::is_any_of(","));
      std::vector<size_t> limits;
      for (size_t i = 0; i < strs.size(); ++i)
        limits.push_back(boost::lexical_cast<size_t>(strs[i]));
      typedef gpelib4::DegreeHistogram UDF_t;
      UDF_t udf(limits);
      gpelib4::SingleMachineAdaptor<UDF_t> adaptor(&udf, globalinstance_, topology_, false, &request->querystate_);
      gpelib4::Master master(&adaptor, "");
      gpelib4::Worker worker(&adaptor);
      std::string result = master.RunUDF();
      response_writer.WriteStartObject();
      response_writer.WriteName("degreehistogram").WriteString(result);
      response_writer.WriteEndObject();
    } else {
      request->message_ = "error_: incorrect function " + func;
      request->error_ = true;
    }
  }

  void EngineJobRunner::ShowOneVertexInfo(EngineServiceRequest* request,
                                          gutil::JSONWriter& jsonwriter,
                                          VertexLocalId_t vid,
                                          std::vector<VertexLocalId_t>& idservice_vids) {
    gapi4::GraphAPI api(topology_, &request->querystate_);
    topology4::VertexAttribute* v1 = api.GetOneVertex(vid);
    jsonwriter.WriteStartObject();
    jsonwriter.WriteName("source");
    jsonwriter.WriteStartObject();
    jsonwriter.WriteName("vid").WriteMarkVId(vid);
    idservice_vids.push_back(vid);
    jsonwriter.WriteName("vertexattr");
    if(v1 != NULL)
      v1->WriteToJson(jsonwriter);
    else{
      jsonwriter.WriteStartObject();
      jsonwriter.WriteEndObject();
    }
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
      jsonwriter.WriteName("to_vid").WriteMarkVId(toid);
      idservice_vids.push_back(toid);
      topology4::VertexAttribute* v2 = api.GetOneVertex(toid);
      jsonwriter.WriteName("to_vertexattr");
      if(v2 != NULL)
        v2->WriteToJson(jsonwriter);
      else{
        jsonwriter.WriteStartObject();
        jsonwriter.WriteEndObject();
      }
      jsonwriter.WriteName("edgeattr");
      edgeattr->WriteToJson(jsonwriter);
      jsonwriter.WriteEndObject();
    }
    jsonwriter.WriteEndArray();
    jsonwriter.WriteEndObject();
  }

  void EngineJobRunner::PullDeltaThread() {
    if (postListener_ == NULL)
      return;
    while (isrunning_) {
      uint64_t deltasize = 0;
      char* deltadata = postListener_->getAllDelta(deltasize, current_post_tid_,
                                                   current_postqueue_pos_, current_idresponsequeue_pos_);
      if (deltadata != NULL){
        topology_->GetDeltaRecords()->ReadDeltas(reinterpret_cast<uint8_t*>(deltadata), deltasize,
                                                 current_postqueue_pos_, current_idresponsequeue_pos_);
        delete[] deltadata;
      }
      else
        usleep(1000);
    }
  }

}  // namespace gperun
