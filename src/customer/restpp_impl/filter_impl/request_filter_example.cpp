/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing an endpoint filter.
 *
 * Created on: Feb 5, 2015
 *      Author: jzhong
 ******************************************************************************/
#include <restpp/librestpp.hpp>

#include <sstream>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

/**
* This function is where you implement your filter. The name of this function does not matter.
* You can use any function name. Just remember to update the driver function.
*
* @param schema_manager The graph schema.

  Use the following four parameters to access the HTTP request:
* @param method - The HTTP method
* @param url - The URL of the request
* @param data - The post data of the request
* @param data_length  - Length of the POST data
* @param params - The query string parsed into key->vector map
  The following parameters are your output:
* @param jsoptions - The jsoptions object you will access on impl.hpp
* @param id_set The set of ids for translation
* @param error_message if anything wrong happens, set the error_message. If error_message is set to
*        nonempty, the request will be immediately returned without forwarding to GPE.
* @return
*/
bool RequestFilterExample(
               const SchemaManager *schemamanager,
               const std::string &method,
               const std::string &url,
               const char *data, size_t data_length,
               const std::map<std::string, std::vector<std::string> > &params,
               Json::Value &jsoptions,
               IdSet &idset,
               std::string &errormessage){



  return true;
}



#ifdef __cplusplus
}
#endif


