/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing an endpoint filter.
 *
 * Created on: Feb 5, 2015
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
#include <map>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

  // Just put payload into jsoptions such that GPE could access it.
  void WrapPayloadFilter(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request) {
    if (user_request->data_length == 0) {
      std::string msg("Payload missing.");  // TODO: msg not properly returned?
      gsql_request->Respond(msg);
      return;
    }
    std::string payload(user_request->data, user_request->data_length);

    Json::Value &jsoptions = gsql_request->jsoptions;
    Json::Value js_payload;
    Json::Reader reader;

    if (reader.parse(payload, js_payload)) {
      jsoptions["payload"] = js_payload;
    } else {
      std::string msg("Payload is not Json.");
      gsql_request->Respond(msg);
    }
  }

#ifdef __cplusplus
}
#endif


