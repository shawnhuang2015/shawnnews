/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing a response filter.
 *
 * Created on: May 9, 2015
 *      Author: jzhong
 ******************************************************************************/
#include <restpp/librestpp.hpp>

#include <sstream>
#include <cstdio>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/**
* This function is where you implement your response filter. The name of this function does not matter.
* You can use any function name. Just remember to update the "response_filter" filed of your endpoint configuration.
*
* @param response - the response sent from GPE to RESTPP
* @param new_response - the response that actually sent back to user.
* @return
*/
bool ResponseFilterExample(const std::string &response, std::string &new_response){
  new_response = std::string("Add something new") + response;
  return true;
}



#ifdef __cplusplus
}
#endif


