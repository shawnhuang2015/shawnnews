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

using namespace gperun;

namespace UDIMPL {

  class UDFRunner : public ServiceImplBase{
  public:
    bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request){
      std::cout<<request.request_function_<<std::endl;
      if(request.request_function_== "kneighborsize")
        return RunUDF_KNeighborSize(serviceapi, request);
      else if(request.request_function_ == "transactionfraud") {
        return RunUDF_TransactionFraud(serviceapi, request);
      }
      return false; /// not a valid request
    }

  private:
    bool RunUDF_TransactionFraud(ServiceAPI& serviceapi, EngineServiceRequest& request) {
std::cout<<"Transaction Fraud"<<std::endl;
      VertexLocalId_t local_start;
      if (!serviceapi.UIdtoVId(request, "0_" + request.request_argv_[1], local_start))
        return false;
      typedef TransactionFraudScore UDF_t;
      std::cout<<"local_start"<<std::endl;
      UDF_t udf(3, local_start, request.outputwriter_);
      serviceapi.RunUDF(&request, &udf);

      boost::unordered_set<EdgePair,EdgePairHash> edges = udf.getEdges();
      boost::unordered_set<Vertex,VertexHash> vertices = udf.getVertices();
      
      gutil::JSONWriter* writer_ = request.outputwriter_; 

      
      std::vector <VertexLocalId_t> vids;
      writer_->WriteStartObject();
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
          writer_ ->WriteUnsignedInt(it ->type);
          writer_ ->WriteName("isFraud");
          writer_ ->WriteBool(it ->isFraud);
          writer_ ->WriteEndObject();
      }
      writer_->WriteEndArray();
      writer_->WriteName("Edges");
      writer_->WriteStartArray();

      gapi4::EdgesCollection results;

      for(boost::unordered_set<EdgePair,EdgePairHash> ::iterator it = edges.begin();
            it != edges.end();
            ++it){
          writer_->WriteStartObject();

          writer_->WriteName("src");
          writer_->WriteMarkVId(it->src);

          writer_->WriteName("tgt");
          writer_->WriteMarkVId(it->tgt);

          writer_ ->WriteEndObject();
      }
      writer_->WriteEndArray();
      writer_->WriteEndObject();
      request.output_idservice_vids.insert(request.output_idservice_vids.begin(), vids.begin(), vids.end());

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
