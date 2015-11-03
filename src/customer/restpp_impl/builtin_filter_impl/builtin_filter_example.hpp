/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: Example of builtin request/response filter.
 * Builtin filters have exactly the same prototype as regular filters. To use it,
 * you need to register your filter in register_builtin_filter.hpp
 *
 * Created on: June 16, 2015
 *      Author: jzhong
 ******************************************************************************/
#ifndef BUILTIN_FILTER_EXAMPLE_HPP
#define BUILTIN_FILTER_EXAMPLE_HPP

/******************************************************************************
* Do NOT edit on this file. Follow this file to create you own filters in a 
* new file (must be a header file). Directly editting on this file may lead to 
* merge conflicts with poc4.3_base.
******************************************************************************/


bool BuiltinRequestFilterExample(FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request){
  return true;
}


RequestObject* BuiltinResponseFilterExample(FilterHelper *filter_helper,
            GsqlResponse *gsql_response,
            UserResponse *user_response){
    return NULL;
}


#endif
