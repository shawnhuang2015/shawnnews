#ifndef REGISTER_BUILTIN_FILTER_HPP
#define REGISTER_BUILTIN_FILTER_HPP


/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.
 * All rights reserved.
 * Project: GHTTP
 * register_builtin_filter.hpp:
 *
 *  Created on: Apr 24, 2015
 *      Author: jzhong
 ******************************************************************************/
#include <restpp/sysinit.hpp>
#include <restpp/filtermanager.hpp>
#include <restpp/parse.hpp>

/*----------------------------------------------------------------------------------------
Instruction:  All filter code are included in this file and compiled into one object file.
              Include your .cpp here.
-----------------------------------------------------------------------------------------*/


void RegisterBuiltinFilters(){
  /*--------------------------------------------------------------------------------------
  Instruction: register the function pointer of your filter with the filter name.
  Register your filter here by calling:
  RESTPP_COMPONENTS::filtermanager->RegisterBuiltinFilter("FilterName", (UpdateRequestFilter)&FilterName);
  ---------------------------------------------------------------------------------------*/
}


#endif
