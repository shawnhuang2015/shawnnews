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
* @param new_response - the response that actually sent back to customer.
* @return - (1) If you want to initiate a cascaded request, return a pointer to a ResponseRequest object. In this case
*               new_response won't be sent back to customer.
*               The ResponseRequest must be allocated on heap using new operation; otherwise it would deallocate
*               when the filter function returns.
*           (2) Return NULL to return new_response to customer.
*/
RequestObject* ResponseFilterExample(const std::string &response, std::string &new_response){
  new_response = std::string("Add something new") + response;
  return NULL;
}

#ifdef __cplusplus
}
#endif


