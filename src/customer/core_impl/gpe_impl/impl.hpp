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
#include "TransactionFraudScore.hpp"
#include "fraud_score.hpp"

using namespace gperun;

namespace UDIMPL {

  class UDFRunner : public ServiceImplBase{
  public:
    bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request){
      if(request.request_function_== "kneighborsize")
        return RunUDF_KNeighborSize(serviceapi, request);
      else if(request.request_function_ == "transactionfraud") {
        return RunUDF_TransactionFraud(serviceapi, request);
      }
      return false; /// not a valid request
    }

  private:
    std::string typestring(int i) {
      switch(i) {
        case 0: return "TXN";
        case 1: return "USERID";
        case 2: return "SSN";
        case 3: return "BANKID";
        case 4: return "CELL";
        case 5: return "IMEI";
        case 6: return "IP";
      }
      return "UNKNOWN";
    }
    bool RunUDF_TransactionFraud(ServiceAPI& serviceapi, EngineServiceRequest& request) {
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, request.request_argv_[2] + "_" + request.request_argv_[1], local_start)) {
        return false;
      }
//      typedef TransactionFraudScore UDF_t;
      typedef lianlian_ns::FraudScoreUDF UDF_t;
      UDF_t udf(6, local_start, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);

      /*
      boost::unordered_set<EdgePair,EdgePairHash> edges = udf.getEdges();
      boost::unordered_set<Vertex,VertexHash> vertices = udf.getVertices();
      
      gutil::JSONWriter* writer_ = request.outputwriter_; 

      GraphAPI* graphapi = serviceapi.CreateGraphAPI(&request);

      std::vector <VertexLocalId_t> vids;
      writer_->WriteStartObject();
      writer_->WriteName("score");
      writer_->WriteFloat(udf.getScore());
      writer_->WriteName("vertices");
      writer_->WriteStartArray();
      for(boost::unordered_set<Vertex,VertexHash> :: iterator it = vertices.begin();
            it != vertices.end();
            ++it){
          vids.push_back(it ->vid);
          writer_ ->WriteStartObject();
          writer_ ->WriteName("id");
          writer_ ->WriteMarkVId(it->vid);
          writer_ ->WriteName("type");
          std::string tmp = typestring(graphapi->GetOneVertex(it ->vid)->type());
          writer_ ->WriteString(tmp);
          writer_ ->WriteName("attr");
          writer_ ->WriteStartObject();
          if(it -> vid == local_start) {
            writer_ ->WriteName("score");
            writer_ ->WriteFloat(udf.getScore()); 
          }
          graphapi->GetOneVertex(it ->vid)->WriteAttributeToJson(*request.outputwriter_);
          writer_ ->WriteEndObject();
          writer_ ->WriteEndObject();
      }
      writer_->WriteEndArray();
      writer_->WriteName("edges");
      writer_->WriteStartArray();

      

      for(boost::unordered_set<EdgePair,EdgePairHash> ::iterator it = edges.begin();
            it != edges.end();
            ++it){

          gapi4::EdgesCollection results;
          graphapi ->GetSpecifiedEdges(it->src, it->tgt, results);

          while(results.NextEdge()) {
            writer_->WriteStartObject();

            writer_->WriteName("src");
            writer_->WriteStartObject();
            writer_->WriteName("id");
            writer_->WriteMarkVId(it->src);
            writer_ ->WriteName("type");
            std::string tmp =  typestring(graphapi->GetOneVertex(it ->src) ->type());
            writer_ ->WriteString(tmp);
            writer_->WriteEndObject();

            writer_->WriteName("tgt");
            writer_->WriteStartObject();
            writer_->WriteName("id");
            writer_->WriteMarkVId(it->tgt);
            writer_ ->WriteName("type");
            tmp = typestring( graphapi->GetOneVertex(it ->tgt) ->type());
            writer_ ->WriteString(tmp);
            writer_->WriteEndObject();

            writer_ ->WriteName("type");
            tmp = typestring(results.GetCurrentEdgeAttribute() ->type());
            writer_ ->WriteString(tmp);
            results.GetCurrentEdgeAttribute()->WriteAttributeToJson(*request.outputwriter_);

            writer_ ->WriteName("attr");
            writer_ ->WriteStartObject();
            writer_ ->WriteEndObject();

            writer_ ->WriteEndObject();
          }
      }
      writer_->WriteEndArray();
      writer_->WriteEndObject();
      delete(graphapi);
      request.output_idservice_vids.insert(request.output_idservice_vids.begin(), vids.begin(), vids.end());
      */
      return true;
    }

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

  };
}    // namespace UDIMPL
#endif    // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
