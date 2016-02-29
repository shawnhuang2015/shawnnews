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


RequestObject* PostOntologyTree(FilterHelper *filter_helper,
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
    // will crash if 0
    if (user_request->data_length > 0) {
      req->data = user_request->data;
    }

    req->params["vtype"] = std::vector<std::string>();
    req->params["vtype"].push_back(root["results"]["vtype"].asString());
    req->params["up_etype"] = std::vector<std::string>();
    req->params["up_etype"].push_back(root["results"]["etype"]["up"].asString());
    req->params["down_etype"] = std::vector<std::string>();
    req->params["down_etype"].push_back(root["results"]["etype"]["down"].asString());

    return req;
  } else {
    user_response->content = str;
    return NULL;
  }
}

RequestObject* SetUserTag(FilterHelper *filter_helper,
          GsqlResponse *gsql_response,
          UserResponse *user_response){
  std::string str(gsql_response->response);
  Json::Reader reader;
  Json::Value root;

  if (reader.parse(str, root) && (root["error"].asBool() == false)) {
    RequestObject* req = new RequestObject();
    req->method = "POST";
    req->url = "post_user_tag";

    UserRequest* user_request = gsql_response->GetUserRequest();
    req->params = user_request->params;

    // will crash if 0
    if (user_request->data_length > 0) {
      req->data = user_request->data;
    }

    req->params["ontology_vtype"] = std::vector<std::string>();
    req->params["ontology_vtype"].push_back(
        root["results"]["ontology"]["vtype"].asString());
    req->params["ontology_up_etype"] = std::vector<std::string>();
    req->params["ontology_up_etype"].push_back(
        root["results"]["ontology"]["etype"]["up"].asString());
    req->params["ontology_down_etype"] = std::vector<std::string>();
    req->params["ontology_down_etype"].push_back(
        root["results"]["ontology"]["etype"]["down"].asString());

    req->params["object_vtype"] = std::vector<std::string>();
    req->params["object_vtype"].push_back(user_request->params["object"][0]);

    req->params["object_ontology_etype"] = std::vector<std::string>();
    req->params["object_ontology_etype"].push_back(
        root["results"]["object_ontology"]["etype"].asString());

    return req;
  } else {
    user_response->content = str;
    return NULL;
  }
}


#ifdef __cplusplus
}
#endif


