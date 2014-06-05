#include <gutil/gsystem.hpp>
#include "enginejobrunner_zmq.hpp"
#include "gpeconfig.hpp"

namespace gperun {

static inline void addError(Json::Value& root, const string &err_msg) {
    root["message"] = err_msg;
    root["error"] = true;
}

void ZMQEngineJobRunner::InitLookupMaps() {
    //string owner_id_map_file_path = "/tmp/owner_id_map_file.csv";

    // set up the owner_id map file
    //ifstream owner_id_map_file(owner_id_map_file_path.c_str());
    //for (string line; getline(owner_id_map_file, line);) {
    //    vector<std::string> tokens;
    //    boost::split(tokens, line, boost::is_any_of(","));
    //    string key = tokens[0];
   //     uint32_t owner_id = boost::lexical_cast<uint32_t>(tokens[1]);
   //     owner_id_map_[key] = owner_id;
    //}
}

string ZMQEngineJobRunner::RunInstance(gpelib4::ZMQEngineServiceRequest* instance) {
    Json::Value root;
    root["results"] = Json::arrayValue;
    if (!GPEConfig::CheckTimeOut(instance->requestid_)) {
        root["debug"] = "";
        addError(root, "error_: request timed out");
        string result = root.toStyledString();
        GVLOG(Verbose_UDFHigh) << "Request timeout " << instance->requestid_;
        return result;
    }
    
    gutil::GTimer timer;
    bool debugmode = false;
    if (boost::algorithm::ends_with(instance->requestid_, ":D")) {
        debugmode = true;
    }

    string debugmsg = "";
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

    // this call may block
    //cout << "before get id map" << endl;
    gpelib4::IdConverter_ZMQ::RequestIdMaps* maps = id_converter_->GetIdMaps(
                                           instance->requestid_);
    unsigned int iterations = Run(instance, root, maps);

    if (debugmode) {
        debugmsg += "{\nRequest end at " + gutil::GTimer::now_str()
            + " with " + boost::lexical_cast<std::string>(iterations)
            + " iterations in "
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
    std::string result = root.toStyledString();

    //cout << "result: " << root.toStyledString() << endl;

    GVLOG(Verbose_UDFHigh) << "Finished " << instance->requestid_ << "|"
                           << timer.GetTotalMilliSeconds() << " ms" << "|"
                           << result;
    return result;
}


unsigned int ZMQEngineJobRunner::Run(gpelib4::ZMQEngineServiceRequest* request,
                                     Json::Value& root,
                                     gpelib4::IdConverter_ZMQ::RequestIdMaps* maps) {
	size_t num_interations = 0;
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
      	Json::Value jsArgv = requestRoot["argv"];
      	for (uint32_t i = 0; i < jsArgv.size(); ++i) {
        	argv.push_back(jsArgv[i].asString());
      	}
      	Json::Value jsoptions = requestRoot["options"];
      	std::vector<VertexLocalId_t> idservice_vids;
      	if (argv[0] == "debug_neighbors") {
        	VertexLocalId_t vid = 0;
        	// XXX
        	//if (!Util::UIdtoVId(topology_, maps, root, argv[1], vid)){
          		return 0;
          	//}
        	ShowOneVertexInfo(request, root, vid, idservice_vids);
      	} else {
        	bool succeed =  UDIMPL::GPE_UD_Impl::RunQuery_ZMQ(request, 
        		                                          this, 
        		                                          maps, 
        		                                          argv,  
        		                                          jsoptions,
                                                          idservice_vids, 
                                                          root, 
                                                          num_interations,
                                                          GPEConfig::customizedsetttings_);
        	if (!succeed) {
          		root["message"] = "error_: unknown function error "
                            + request->requeststr_;
          		root["error"] = true;
          		return 0;
        	}
        }

        string msg_body = id_converter_->generateRequestIDS(idservice_vids, request->rest_id_);
        connector_->SetResponseToIDS(request->requestid_, msg_body);

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
    return num_interations;    
}

void ZMQEngineJobRunner::ShowOneVertexInfo(gpelib4::ZMQEngineServiceRequest* request,
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

}