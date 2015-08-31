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
#include "gpath_base/graphtypeinfo.hpp"
#include "udfs/generic_traversal.hpp"
#include "kstepneighborhoodsubgraph.hpp"
#include <boost/lexical_cast.hpp>

using namespace gperun;

namespace UDIMPL {

class UDFRunner : public ServiceImplBase {
 public:
  bool RunQuery(ServiceAPI& serviceapi, EngineServiceRequest& request) {
    if (request.request_function_ == "kneighborsize") {
      return RunUDF_KNeighborSize(serviceapi, request);
    }
    if(request.request_function_ == "gpath") {
      return RunUDF_GPath(serviceapi, request);
    }
    
    return false;  /// not a valid request
  }

 private:
  void convertUid2Vid(ServiceAPI& serviceapi, EngineServiceRequest& request,
                      uint32_t typeId, Json::Value& memberList, Json::Value& output) { 
    Json::Value memberShipList = Json::Value(Json::arrayValue);
    for (uint32_t j = 0; j < memberList.size(); ++j) {
      std::string nodeIdStr = boost::lexical_cast<std::string>(typeId) + "_" + memberList[j].asString();
      VertexLocalId_t localNode;
      if (!serviceapi.UIdtoVId(request, nodeIdStr, localNode, false)) {
        request.message_ = nodeIdStr + " can not be converted into the internal Id in impl.hpp";
        request.error_ = true;
      }   
      memberShipList.append(localNode);//adding the converted id into the list.
    }   
    output["membership_vid_list"] = memberShipList;//replace the old list with the converted list
  }   


  bool RunUDF_GPath(ServiceAPI& serviceapi,
                            EngineServiceRequest& request) {
    std::cout << " GPath invoked " << std::endl;
    std::cout << " Jsoptions is " << request.jsoptions_.toStyledString() << std::endl;

    typedef GenericTraversal UDF_t;
    GraphMetaData graphMetaData;
    graphMetaData.getGraphMeta(serviceapi);//get the graph meta data
    //GCOUT(Verbose_UDFLow) << graphMetaData << std::endl;
    std::cout << graphMetaData << std::endl;

    Json::Value jsoptions = request.jsoptions_["parsedQuery"];
  
    // Convert anchor node membership_list 
    uint32_t typeId=0;
    graphMetaData.getVertexTypeIndex(jsoptions["anchor_node"]["vtypecheck"].asString(), typeId);
    convertUid2Vid(serviceapi, request, typeId, jsoptions["anchor_node"]["membership_list"], jsoptions["anchor_node"]);

    // Convert steps membership_list
    for (uint32_t i = 0; i < jsoptions["steps"].size(); ++i) {
      uint32_t typeId = 0;
      graphMetaData.getVertexTypeIndex(jsoptions["steps"][i]["vtypecheck"].asString(), typeId);
      convertUid2Vid(serviceapi, request, typeId, jsoptions["steps"][i]["membership_list"], jsoptions["steps"][i]);
    }

    if (request.error_) {
      return true;
    }

    std::cout << jsoptions << std::endl;

    UDF_t udf(graphMetaData, jsoptions, request.outputwriter_);
    serviceapi.RunUDF(&request, &udf);

    return true;
  }
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
};
}  // namespace UDIMPL
#endif  // SRC_CUSTOMER_COREIMPL_GPEIMPL_IMPL_HPP_
