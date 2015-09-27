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

  void CascadeRequestRequestFilter(
           FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request){
    gsql_request->jsoptions["add"] = Json::Value(Json::arrayValue);
    gsql_request->jsoptions["add"].append(123);
  }

#ifdef __cplusplus
}
#endif


