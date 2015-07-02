/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: RESTPP
 * register_builtin_filter.hpp:
 *
 *  Created on: Apr 24, 2015
 *      Author: jzhong
 *  See https://graphsql.atlassian.net/wiki/pages/viewpage.action?pageId=65831094
 *  (Section 3.5) for details about using builtin filters.
 ******************************************************************************/
#ifndef REGISTER_BUILTIN_FILTER_HPP
#define REGISTER_BUILTIN_FILTER_HPP

#include <librestpp.hpp>

#include "update_request_filter_example.hpp"

/*----------------------------------------------------------------------------------------
Ste 1:  All filter code are included in this file and compiled into one object file.
        Include your .cpp here.
-----------------------------------------------------------------------------------------*/


void RegisterBuiltinFilters(){
  /*--------------------------------------------------------------------------------------
  Step 2: register the function pointer of your filter with the filter name.
  Register your filter here by calling:
  RESTPP_COMPONENTS::filtermanager->RegisterBuiltinFilter("FilterName", (UpdateRequestFilter)&FilterName);
  ---------------------------------------------------------------------------------------*/
}


#endif
