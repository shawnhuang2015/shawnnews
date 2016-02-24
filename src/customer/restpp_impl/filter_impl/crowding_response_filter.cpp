/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing a response filter.
 *
 * Created on: May 9, 2015
 *      Author: jzhong
 ******************************************************************************/

/******************************************************************************
 * Do NOT edit on this file. Follow this file to create you own filters in a 
 * new file. Directly editting on this file may lead to merge conflicts with
 * poc4.3_base in the future.
 ******************************************************************************/

#include <restpp/librestpp.hpp>

#include <sstream>
#include <cstdio>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif


RequestObject* PostOntologyTreeDeprecated(FilterHelper *filter_helper,
          GsqlResponse *gsql_response,
          UserResponse *user_response){
  std::string str(gsql_response->response);
  Json::Reader reader;
  Json::Value root;

  if (reader.parse(str, root) && (root["error"].asBool() == false)) {
    RequestObject* req = new RequestObject();
    req->method = "POST";
    req->url = "import_tree";

    UserRequest* user_request = gsql_response->GetUserRequest();
    req->params = user_request->params;
    req->data = user_request->data;

    std::cout << "root: " << root << std::endl;

    req->params["vtype"] = std::vector<std::string>();
    req->params["vtype"].push_back(root["results"]["vtype"].asString());
    req->params["up_etype"] = std::vector<std::string>();
    req->params["up_etype"].push_back(root["results"]["etype"]["up"].asString());
    req->params["down_etype"] = std::vector<std::string>();
    req->params["down_etype"].push_back(root["results"]["etype"]["down"].asString());

//    std::cout << "params: " << std::endl;
//    typedef std::map<std::string, std::vector<std::string> > map_t;
//    for (map_t::iterator it = req->params.begin(); it != req->params.end(); ++it) {
//      std::cout << it->first << ": ";
//      int size = it->second.size();
//      for (int i = 0; i < size; ++i) {
//        std::cout << it->second[i] << ", ";
//      }
//      std::cout << std::endl;
//    }
//    std::cout << "data: " << req->data << std::endl;

    return NULL;
  } else {
    user_response->content = str;
    return NULL;
  }
}


#ifdef __cplusplus
}
#endif

