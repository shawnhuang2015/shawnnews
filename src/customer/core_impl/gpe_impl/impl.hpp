/******************************************************************************
 * Copyright (c) 2014, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *      Author: lichen
 ******************************************************************************/

#ifndef SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
#define SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_

#include <gpe/serviceimplbase.hpp>
#include "kneighborsize.hpp"
#include "kstepneighborhoodsubgraph.hpp"
#include "querydispatcher.hpp"

using namespace gperun;

namespace UDIMPL {

class UDFRunner : public ServiceImplBase {
 public:
  bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    if (request.request_function_ == "kneighborsize") {
      return RunUDF_KNeighborSize(serviceapi, request);
    } else if (request.request_function_ == "queryDispatcher") {
#ifdef RUNQUERY
        std::string queryName = request.jsoptions_["query_name"][0].asString();
        QueryDispatcher qdp(queryName,serviceapi, request);
        return qdp.RunQuery();
#else
        return false;
#endif
    } else if (request.request_function_ == "semantic_def") {
      return SemanticDef(request);
    }
    
    return false;  /// not a valid request
  }

 private:
  bool RunUDF_KNeighborSize(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    // sample to convert vid.
    VertexLocalId_t local_start;
    // if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start))
    if (!serviceapi.UIdtoVId(request, request.request_data_, local_start))
      return false;
    request.outputwriter_->WriteStartObject();
    request.outputwriter_->WriteName("vertex");
    {
      // sample to use GraphAPI
      boost::shared_ptr<GraphAPI> graphapi =
          serviceapi.CreateGraphAPI(&request);
      VertexAttribute v1;
      graphapi->GetOneVertex(local_start, v1);
      v1.WriteToJson(*request.outputwriter_);
    }
    request.outputwriter_->WriteName("neighborsize");
    // sample to run one UDF
    typedef KNeighborSize UDF_t;
    UDF_t udf(3, local_start, request.outputwriter_);
    serviceapi.RunUDF(&request, &udf);
    if (udf.abortmsg_.size() > 0) {
      request.error_ = true;
      request.message_ += udf.abortmsg_;
      return true;
    }
    request.outputwriter_->WriteEndObject();
    request.output_idservice_vids.push_back(local_start);
    return true;
  }

  bool SemanticDef(EngineServiceRequest &request) {
    const Json::Value &jsoptions = request.jsoptions_;

    if (! jsoptions.isMember("payload")) {
      request.error_ = true;
      request.message_ += "Payload missing.";
      return true;
    }

    bool invalid = false;
    // copy the payload, modify
    Json::Value payload = jsoptions["payload"];

    const std::string OBJ("object");
    const std::string ONTO("ontology");
    const std::string OBJ_ONTO("object_ontology");
    const std::string PROF("profile");
    const std::string ONTO_VTYPE_PREF = "__onto_v_";
    const std::string ONTO_ETYPE_PREF_UP = "__onto_e_up_";
    const std::string ONTO_ETYPE_PREF_DOWN = "__onto_e_down_";
    const std::string OBJ_ONTO_ETYPE_PREF = "__obj_onto_e_";

    // check for "object", should be present
    if (payload.isMember(OBJ) && payload[OBJ].isArray()) {
    } else {
      request.error_ = true;
      request.message_ += OBJ + " missing or not array.";
      invalid = true;
    }

    // check for "ontology" & "object_ontology", might be absent
    if (payload.isMember(ONTO) == payload.isMember(OBJ_ONTO)) {
      if (payload.isMember(ONTO)) {
        if (payload[ONTO].isArray() && payload[OBJ_ONTO].isArray()) {
        } else {
          request.error_ = true;
          request.message_ += ONTO + " or " + OBJ_ONTO + " not array.";
          invalid = true;
        }

        // assign vtype/etype names
        Json::Value onto;
        int size = payload[ONTO].size();
        for (int i = 0; i < size; ++i) {
          std::string name(payload[ONTO][i].asString());
          onto[name] = Json::Value();
          onto[name]["vtype"] = ONTO_VTYPE_PREF + name;
          onto[name]["etype"] = Json::Value(Json::arrayValue);
          onto[name]["etype"].append(ONTO_ETYPE_PREF_UP + name);
          onto[name]["etype"].append(ONTO_ETYPE_PREF_DOWN + name);
        }
        payload[ONTO] = onto;
      }
    }
 
    // check for "profile", should be present
    if (payload.isMember(PROF) && payload[PROF].isArray()) {
    } else {
      request.error_ = true;
      request.message_ += PROF + " missing or not array.";
      invalid = true;
    }

    std::cout << payload << std::endl;
    return ! invalid;
  }
};
}  // namespace UDIMPL
#endif  // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
