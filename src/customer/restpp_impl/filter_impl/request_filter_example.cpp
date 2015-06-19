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
* This function is where you implement your request filter. The name of this function does not matter.
* You can use any function name.

  Use the following parameter to access the graph schema.
* @param schema_manager

  Use the following four parameters to access the HTTP request:
* @param method - The HTTP method
* @param url - The URL of the request
* @param data - The post data of the request
* @param data_length  - Length of the POST data
* @param params - The query string parsed into (parameter name)->(values) map

  The following parameters are your output:
* @param jsoptions - The jsoptions object you will access on impl.hpp
* @param id_set The - set of ids for translation
* @param message - message is directly sent back to user if the filter returns false. Otherwise, setting message
*                 has no effects.
* @return - true if every is right and the request is expected to be sent to GPE, false if something wrong happen
*         and the error_message will be returned to customer.
*/
bool RequestFilterExample(
               const SchemaManager *schemamanager,
               const std::string &method,
               const std::string &url,
               const char *data, size_t data_length,
               const std::map<std::string, std::vector<std::string> > &params,
               Json::Value &jsoptions,
               IdSet &idset,
               std::string &message){



  return true;
}



#ifdef __cplusplus
}
#endif


