/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: Example of builtin request/response filter.
 * Builtin filters have exactly the same prototype as regular filters. To use it,
 * you need to register your filter in register_builtin_filter.hpp
 *
 * Created on: June 16, 2015
 *      Author: jzhong
 ******************************************************************************/
#ifndef BUILTIN_CROWDING_FILTER_HPP
#define BUILTIN_CROWDING_FILTER_HPP
#include <boost/lexical_cast.hpp>

/******************************************************************************
* Do NOT edit on this file. Follow this file to create you own filters in a 
* new file (must be a header file). Directly editting on this file may lead to 
* merge conflicts with poc4.3_base.
******************************************************************************/

std::string errorMsg(std::string msg) {
  Json::Value root;
  root["error"] = true;
  root["message"] = msg;
  return root.toStyledString();
}

bool CreateCrowdUpdateFilter(FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
  char* data = user_request->data;
  size_t data_length = user_request->data_length;

  std::string error_message;
  Json::Reader reader; 
  Json::Value root;

  if (reader.parse(data, data + data_length, root)) {
    //parse request parameters
    std::string crowdName = root["name"].asString();
    std::string vtype = root["vtype"].asString();
    std::string etype = root["etype"].asString();
    std::string target = root["target"].asString();
    Json::Value &results = root["results"];
    uint32_t userCount = results["count"].asUInt();
    std::cout << "CreateCrowdUpdateFilter|name:" + crowdName \
        + "|vtype:" + vtype + "|etype:" + etype + "|target:" + target \
        + "|count:" + boost::lexical_cast<std::string>(userCount) << "\n";
    std::vector<std::string> userIds;
    for (uint32_t k = 0 ; k < results["userIds"].size(); ++k) {
      userIds.push_back(results["userIds"][k].asString());
    }

    //update graph - add index
    bool success = true;
    AttributesDelta attr;
    attr.SetUInt64("date_time", time(NULL));
    if (gsql_request->UpsertVertex(attr, vtype, crowdName, error_message)) {
      for (size_t k = 0; k < userIds.size(); ++k) {
        attr.Clear();
        if (!gsql_request->UpsertEdge(attr, vtype, crowdName, etype, target, userIds[k], error_message)) {
          success = false;
          break;
        }
        if (k == 5000) {  //hack - need fix
          std::cout << "Connect 5000 vertices to Crowd Index" << "\n";
          break;
        }
      }
    } else {
      success = false;
    }

    if (success && !gsql_request->FlushDelta(error_message)) {
      success = false;
    }
    if (success) {
      Json::Value root;
      root["error"] = false;
      root["message"] = "";
      //root["results"]["count"] = userCount;
      root["results"] = results;

      gsql_request->Respond(root.toStyledString());
    } else {
      gsql_request->Respond(errorMsg(error_message));
    }
  }
  

  return true;
}

#endif
