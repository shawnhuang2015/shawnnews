/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++
 * Brief: template for writing an endpoint filter.
 *
 * Created on: June 16, 2015
 *      Author: jzhong
 ******************************************************************************/
#ifndef UPDATE_REQUEST_FILTER_EXAMPLE_HPP
#define UPDATE_REQUEST_FILTER_EXAMPLE_HPP

#include <fstream>

bool BuiltinFilterExample(FilterHelper *filter_helper,
           UserRequest *user_request,
           GsqlRequest *gsql_request)
{
  return true;
}


#endif
