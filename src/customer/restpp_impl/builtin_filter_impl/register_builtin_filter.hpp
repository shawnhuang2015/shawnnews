/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: RESTPP
 * Brief: register builtin filters in this file.
 *
 *  Created on: Apr 24, 2015
 *      Author: jzhong
 *  See "Builtin filters" https://graphsql.atlassian.net/wiki/pages/viewpage.action?pageId=65831094
 *  (Section 3.5) for details about using builtin filters.
 ******************************************************************************/
#ifndef REGISTER_BUILTIN_FILTER_HPP
#define REGISTER_BUILTIN_FILTER_HPP

#include <restpp/librestpp.hpp>

/*----------------------------------------------------------------------------------------
  Steps for registering builtin filters:
  
  Step 1:  Include your *.hpp.
-----------------------------------------------------------------------------------------*/
#include "builtin_filter_example.hpp"


void RegisterBuiltinFilters(){
  /*--------------------------------------------------------------------------------------
  Step 2: register the function pointer of your filter under a unique filter name.
  the following two lines just domonstrate how to register builtin filters, 
  you can safely delete it on your own project.
  ---------------------------------------------------------------------------------------*/
  RegisterBuiltinFilter("BuiltinRequestFilterExample", (RequestFilter)&BuiltinRequestFilterExample);
  RegisterBuiltinFilter("BuiltinResponseFilterExample", (RequestFilter)&BuiltinResponseFilterExample);
}


#endif
