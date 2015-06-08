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
#include "udf/yeepay.hpp"

using namespace gperun;

namespace UDIMPL {

  class UDFRunner : public ServiceImplBase{
  public:
    bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request){
      if(request.request_function_== "kneighborsize") {
        return RunUDF_KNeighborSize(serviceapi, request);
      } else if (request.request_function_ == "check_ip") {
        return RunUDF_CheckIP(serviceapi, request);
      }
      return false; /// not a valid request
    }

  private:
    bool RunUDF_KNeighborSize(ServiceAPI& serviceapi, EngineServiceRequest& request){
      // sample to convert vid.
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start))
        return false;
      request.outputwriter_->WriteStartObject();
      request.outputwriter_->WriteName("vertex");
      // sample to use GraphAPI
      GraphAPI* graphapi = serviceapi.CreateGraphAPI(&request);
      graphapi->GetOneVertex(local_start)->WriteToJson(*request.outputwriter_);
      delete graphapi;
      request.outputwriter_->WriteName("neighborsize");
      // sample to run one UDF
      typedef KNeighborSize UDF_t;
      UDF_t udf(3, local_start, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);
      if(udf.abortmsg_.size() > 0){
        request.error_ = true;
        request.message_ += udf.abortmsg_;
        return true;
      }
      request.outputwriter_->WriteEndObject();
      request.output_idservice_vids.push_back(local_start);
      return true;
    }


    bool RunUDF_CheckIP(ServiceAPI& serviceapi, EngineServiceRequest& request) {
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[1], local_start)) {
        return false;
      }

      typedef yeepay_ns::YeepaySubGraphExtractUDF UDF_t;

      size_t depth = request.jsoptions_.isMember("depth") ? 
        strtoul(request.jsoptions_["depth"][0].asString().c_str(), NULL, 10) : 1;
      size_t start_time = request.jsoptions_.isMember("starttime") ? 
        strtoul(request.jsoptions_["starttime"][0].asString().c_str(), NULL, 10) : 0;
      size_t end_time = request.jsoptions_.isMember("endtime") ? 
        strtoul(request.jsoptions_["endtime"][0].asString().c_str(), NULL, 10) : yeepay_ns::INF;

      GraphAPI* graphapi = serviceapi.CreateGraphAPI(&request);

      UDF_t udf(depth * 2, local_start, start_time, end_time, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);

      boost::unordered_set<VertexLocalId_t> vertices = udf.vertices_;
      yeepay_ns::vertex_map_t edges = udf.group_by_vertex;

      gutil::JSONWriter* writer_ = request.outputwriter_; 

      std::vector <VertexLocalId_t> vids;

      writer_->WriteStartObject();

      // fill vertices info into json.
      writer_->WriteName("vertices");
      writer_->WriteStartArray();

      for (boost::unordered_set<VertexLocalId_t>::const_iterator cit = vertices.begin(); cit != vertices.end(); ++cit) {
          vids.push_back(*cit);

          writer_->WriteStartObject();
          writer_->WriteName("id");
          writer_->WriteMarkVId(*cit);
          writer_->WriteName("type");
          writer_->WriteUnsignedInt<unsigned>(0);

          writer_->WriteName("attr");
          writer_->WriteStartObject();
          //graphapi->GetOneVertex(*cit)->WriteAttributeToJson(*request.outputwriter_);
          writer_->WriteEndObject();

          writer_->WriteName("other");
          writer_->WriteStartObject();
          writer_->WriteName("log");
          writer_->WriteString(graphapi->GetOneVertex(*cit)->GetString(0));
          writer_->WriteEndObject();
          writer_->WriteEndObject();
      }

      // end filling vertices.
      writer_->WriteEndArray();

      // fill edges info into json.
      writer_->WriteName("edges");
      writer_->WriteStartArray();

      for (yeepay_ns::vertex_map_t::const_iterator cit = edges.begin();
           cit != edges.end(); ++cit) {
        writer_->WriteStartObject();

        writer_->WriteName("type");
        writer_->WriteUnsignedInt<unsigned>(0);

        writer_->WriteName("src");
        writer_->WriteStartObject();
        writer_->WriteName("id");
        writer_->WriteMarkVId(cit->first.first);
        writer_->WriteName("type");
        writer_->WriteUnsignedInt<unsigned>(0);
        writer_->WriteEndObject();

        writer_->WriteName("tgt");
        writer_->WriteStartObject();
        writer_->WriteName("id");
        writer_->WriteMarkVId(cit->first.second);
        writer_->WriteName("type");
        writer_->WriteUnsignedInt<unsigned>(0);
        writer_->WriteEndObject();

        writer_->WriteName("attr");
        writer_->WriteStartObject();
        writer_->WriteName("sids");
        writer_->WriteStartArray();
        for (boost::unordered_set<VertexLocalId_t>::const_iterator cit1 = cit->second.begin();
             cit1 != cit->second.end(); ++cit1) {
          writer_->WriteMarkVId(*cit1);
          vids.push_back(*cit1);
        }
        writer_->WriteEndArray();
        writer_->WriteEndObject();

        writer_->WriteEndObject();
      }

      // end filling edges.
      writer_->WriteEndArray();

      // end writing json.
      writer_->WriteEndObject();

      request.output_idservice_vids.push_back(local_start);
      for (std::vector<VertexLocalId_t>::const_iterator cit = vids.begin();
           cit != vids.end(); ++cit) {
        request.output_idservice_vids.push_back(*cit);
      }
      return true;
    }
  };
}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
