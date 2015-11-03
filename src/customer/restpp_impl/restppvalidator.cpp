/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: REST++ validator
 * restpp.cpp: entry of REST++ validator.
 *
 *  Created on: Oct 29, 2015
 *  Made with ♥ by jzhong@graphsql
 ******************************************************************************/

#include <restpp/librestpp.hpp>
#include "builtin_filter_impl/register_builtin_filter.hpp"

int main(int argc, char **argv){
  return Validate(argc, argv, &RegisterBuiltinFilters);
}
