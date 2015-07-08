/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: RESTPP
 * restpp.cpp: entry of REST++ server.
 *
 *  Created on: June 23, 2015
 *      Author: jzhong
 ******************************************************************************/

#include <restpp/librestpp.hpp>
#include "builtin_filter_impl/register_builtin_filter.hpp"
#include "filterutil.cpp"

int main(int argc, char **argv){
  StartService(argc, argv, &RegisterBuiltinFilters);
  return 0;
}
