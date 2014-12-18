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

  class PrintOneVertex : public gpelib4::BaseUDF {
  public:
    // These consts are required by the engine.
    //Mode_SingleValue or Mode_MultipleValue or Mix Value
    static const gpelib4::ValueTypeFlag ValueTypeMode_ = gpelib4::Mode_SingleValue;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineInitializeFlag InitializeMode_ = gpelib4::Mode_Initialize_Globally;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag AggregateReduceMode_ = gpelib4::Mode_Defined;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag CombineReduceMode_ = gpelib4::Mode_NotDefined;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag VertexMapMode_ = gpelib4::Mode_Defined;
    //Mode_NotDefined or Mode_Defined
    static const gpelib4::UDFDefineFlag PrintMode_ = gpelib4::Mode_NotDefined;

    // These typedefs are required by the engine.
    typedef topology4::VertexAttribute V_ATTR;
    typedef topology4::EdgeAttribute E_ATTR;
    typedef uint8_t V_VALUE;
    typedef uint8_t MESSAGE;

    VertexLocalId_t start_node_;
    gutil::JSONWriter* writer_;
    bool vertexlevel_;
    std::vector<VertexLocalId_t>& idservice_vids_;

    PrintOneVertex(VertexLocalId_t start_node, gutil::JSONWriter* jsonwriter, bool vertexlevel, std::vector<VertexLocalId_t>& idservice_vids)
      : gpelib4::BaseUDF(gpelib4::EngineProcessMode_ActiveVertices, 1),
        start_node_(start_node),
        writer_(jsonwriter),
        vertexlevel_(vertexlevel),
        idservice_vids_(idservice_vids){
    }

    void Initialize(gpelib4::GlobalSingleValueContext<V_VALUE>* valuecontext) {
      writer_->WriteStartObject();
      if(!vertexlevel_){
        writer_->WriteName("edges");
        writer_->WriteStartArray();
      }
      valuecontext->Write(start_node_,1,true);
    }

    ALWAYS_INLINE void VertexMap( const VertexLocalId_t& id,
                                  V_ATTR* vertexattr,
                                  const V_VALUE& vertexvalue,
                                  gpelib4::SingleValueMapContext<MESSAGE>* context) {
      if(!vertexlevel_)
        return;
      writer_->WriteName("degree").WriteUnsignedInt(vertexattr->outgoing_degree());
      writer_->WriteName("type").WriteUnsignedInt(vertexattr->type());
      writer_->WriteName("attr");
      vertexattr->WriteToJson(*writer_);
    }

    ALWAYS_INLINE void EdgeMap( const VertexLocalId_t& srcvid,
                                V_ATTR* srcvertexattr,
                                const V_VALUE& srcvertexvalue,
                                const VertexLocalId_t& targetvid,
                                V_ATTR* targetvertexattr,
                                const V_VALUE& targetvertexvalue,
                                E_ATTR* edgeattr,
                                gpelib4::SingleValueMapContext<MESSAGE>* context) {
      if(vertexlevel_)
        return;
      writer_->WriteStartObject();
      writer_->WriteName("to_vid").WriteMarkVId(targetvid);
      writer_->WriteName("type").WriteUnsignedInt(edgeattr->type());
      idservice_vids_.push_back(targetvid);
      writer_->WriteName("to_degree").WriteUnsignedInt(targetvertexattr->outgoing_degree());
      writer_->WriteName("to_attr");
      targetvertexattr->WriteToJson(*writer_);
      writer_->WriteName("edgeattr");
      edgeattr->WriteToJson(*writer_);
      writer_->WriteEndObject();
    }

    ALWAYS_INLINE void Reduce( const VertexLocalId_t& vid,
                               V_ATTR* vertexattr,
                               const V_VALUE& singlevalue,
                               const MESSAGE& accumulator,
                               gpelib4::SingleValueContext<V_VALUE>* context) {
    }

    void EndRun(gpelib4::BasicContext* context) {
      if(!vertexlevel_)
        writer_->WriteEndArray();
      writer_->WriteEndObject();
    }

  };

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
    if(func == "vattr" || func == "vattr_udf"){
      std::string vidstr = "";
      if(!dataRoot["translate_ids"].isNull())
        vidstr = dataRoot["translate_ids"][0].asString();
      else
        vidstr = boost::lexical_cast<std::string>(dataRoot["translate_typed_ids"][0]["type"].asInt()) + "_" +  dataRoot["translate_typed_ids"][0]["id"].asString();
      VertexLocalId_t vid = 0;
      if (!serviceapi_->UIdtoVId(*request, vidstr, vid))
        return;
      if(func == "vattr"){
        gapi4::GraphAPI api(topology_, request->querystate_);
        topology4::VertexAttribute* v1 = api.GetOneVertex(vid);
        request->outputwriter_->WriteStartObject();
        if(v1 != NULL){
          request->outputwriter_->WriteName("degree").WriteUnsignedInt(v1->outgoing_degree());
          request->outputwriter_->WriteName("type").WriteUnsignedInt(v1->type());
          request->outputwriter_->WriteName("attr");
          v1->WriteToJson(*request->outputwriter_);
        }
        request->outputwriter_->WriteEndObject();
      } else {
        typedef PrintOneVertex UDF_t;
        UDF_t udf(vid, request->outputwriter_, true, request->output_idservice_vids);
        serviceapi_->RunUDF(request, serviceapi_->GetAdaptor(request, &udf));
      }
    } else if(func == "edges" || func == "edges_udf"){
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
      if(func == "edges"){
        gapi4::GraphAPI api(topology_, request->querystate_);
        gapi4::EdgesCollection edgeresults;
        api.GetOneVertexEdges(vid, NULL, edgeresults);
        request->outputwriter_->WriteStartObject();
        request->outputwriter_->WriteName("edges");
        request->outputwriter_->WriteStartArray();
        uint32_t count = 0;
        while (edgeresults.NextEdge()) {
          VertexLocalId_t toid = edgeresults.GetCurrentToVId();
          topology4::VertexAttribute* v1 = api.GetOneVertex(toid);
          if(v1 == NULL)
            continue;
          request->outputwriter_->WriteStartObject();
          request->outputwriter_->WriteName("to_vid").WriteMarkVId(toid);
          topology4::EdgeAttribute* edgeattr = edgeresults.GetCurrentEdgeAttribute();
          request->outputwriter_->WriteName("type").WriteUnsignedInt(edgeattr->type());
          request->output_idservice_vids.push_back(toid);
          if(writetgtvattr){
            request->outputwriter_->WriteName("to_degree").WriteUnsignedInt(v1->outgoing_degree());
            request->outputwriter_->WriteName("to_attr");
            v1->WriteToJson(*request->outputwriter_);
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
      } else{
        typedef PrintOneVertex UDF_t;
        UDF_t udf(vid, request->outputwriter_, false, request->output_idservice_vids);
        serviceapi_->RunUDF(request, serviceapi_->GetAdaptor(request, &udf));
      }
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
    }else if(func == "edge_exists"){
      std::string src_str = "";
      std::string tgt_str = "";
      if(!dataRoot["translate_ids"].isNull()){
        src_str = dataRoot["translate_ids"][0].asString();
        tgt_str = dataRoot["translate_ids"][1].asString();
      }else{
        src_str = boost::lexical_cast<std::string>(dataRoot["translate_typed_ids"][0]["type"].asInt()) + "_" +  dataRoot["translate_typed_ids"][0]["id"].asString();
        tgt_str  = boost::lexical_cast<std::string>(dataRoot["translate_typed_ids"][1]["type"].asInt()) + "_" +  dataRoot["translate_typed_ids"][1]["id"].asString();
      }


      if(!dataRoot["etype"].isNull()){
        request->error_ = true;
        request->message_ = "edge_exists function requires you to specify type:int";
        return;
      }

      uint32_t edge_type =dataRoot["etype"].asUInt();

      VertexLocalId_t src = 0;
      VertexLocalId_t tgt = 0;

      if (!serviceapi_->UIdtoVId(*request, src_str, src))
        return;
      if (!serviceapi_->UIdtoVId(*request, tgt_str, tgt))
        return;
      gapi4::GraphAPI api(topology_, request->querystate_);
      gapi4::EdgesCollection edgeresults;
      api.GetSpecifiedEdges(src,tgt, edgeresults);

      bool edge_exists = false;
      // two fail cases:  the edge exists but is the wrong type.
      // the edge does not exist for any type.
      while(edgeresults.NextEdge() && !edge_exists){
        topology4::EdgeAttribute* edgeattr = edgeresults.GetCurrentEdgeAttribute();
        edge_exists = edgeattr->type() == edge_type;
      }


      request->outputwriter_->WriteStartObject();
      request->outputwriter_->WriteName("edge_exists");
      request->outputwriter_->WriteBool(edge_exists);
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
