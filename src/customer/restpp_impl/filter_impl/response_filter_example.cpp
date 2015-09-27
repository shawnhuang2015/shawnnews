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


  RequestObject* CascadeRequestGenerator(FilterHelper *filter_helper,
            GsqlResponse *gsql_response,
            UserResponse *user_response){
    RequestObject *cascade_request = new RequestObject();
    cascade_request->method = "GET";
    cascade_request->url = "/cascade/second";
    cascade_request->data = gsql_response->response;
    cascade_request->params["string_param"].push_back("randomString");
    return cascade_request;
  }

  RequestObject* CascadeRequestResponseFilter(FilterHelper *filter_helper,
            GsqlResponse *gsql_response,
            UserResponse *user_response){
    user_response->content = "CascadeRequestResponseFilter: "
      + gsql_response->response;
    return NULL;
    /*
    RequestObject *cascade_request = new RequestObject();
    cascade_request->method = "GET";
    cascade_request->url = "/cascade/second";
    cascade_request->data = gsql_response->response;
    cascade_request->params["string_param"].push_back("randomString");
    return cascade_request;
    */
  }



#ifdef __cplusplus
}
#endif


