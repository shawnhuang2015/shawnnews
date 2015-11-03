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


  RequestObject* ResponseFilterExample(FilterHelper *filter_helper,
            GsqlResponse *gsql_response,
            UserResponse *user_response){
    return NULL;
    /*
    // generate cascaded request from response filter
    RequestObject *cascaded_request = new RequestObject();
    cascaded_request->method = "GET";
    cascaded_request->url = "/cascade/second";
    cascaded_request->data = gsql_response->response;
    cascaded_request->params["string_param"].push_back("randomString");
    return cascaded_request;
    */
  }



#ifdef __cplusplus
}
#endif


