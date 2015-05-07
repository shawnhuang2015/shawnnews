/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing an endpoint filter.
 *
 * Created on: Feb 5, 2015
 *      Author: jzhong
 ******************************************************************************/
#include <librestpp.hpp>

#include <sstream>
#include <cstdio>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/**
* This function is where you implement your filter. The name of this function does not matter.
* You can use any function name. Just remember to update the driver function.
*
* @param schema_manager The graph schema.

  Use the following four parameters to access the HTTP request:
* @param method The HTTP method
* @param url The URL of the request
* @param data The post data of the request
* @param params The query string parsed into key->vector map

* @param id_set The set of ids for translation
* @param error_message if anything wrong happens, set the error_message. If error_message is set to
*        nonempty, the request will be immediately returned without forwarding to GPE.
*
* @return
*/
bool ResponseFilterExample(const std::string &response, std::string &new_response){
  std::stringstream ss;
  ss.str(response);
  std::vector<size_t> nos;
  size_t no;
  while(ss>>no){
    nos.push_back(no);
  }
  if(nos.size() == 0){
    new_response = "Received 0 line.";
    return true;
  }
  std::ostringstream convert;
  convert<<"Received lines: "<<nos[nos.size() - 1]<<std::endl;
  if(nos.size() > 1){
    convert<<"Failed lines:";
    for(int i = 0; i < nos.size() - 1; i ++){
      //on the request filter the lines are counted from 0
      convert<<" "<<nos[i] + 1;
    }
  }
  convert<<".";
  new_response = convert.str();
  return true;
}



#ifdef __cplusplus
}
#endif


