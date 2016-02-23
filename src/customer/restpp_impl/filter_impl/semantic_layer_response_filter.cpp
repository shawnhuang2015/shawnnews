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
  if (reader.parse(str, root) && root["error"].asBool() == false) {
    std::cout << "resp filter" << std::endl;
    return NULL;
  } else {
    user_response->content = str;
    return NULL;
  }
}


#ifdef __cplusplus
}
#endif


