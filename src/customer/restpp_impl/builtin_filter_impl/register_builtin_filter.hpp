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
#include "../sysinit.hpp"
#include "../filtermanager.hpp"

/*----------------------------------------------------------------------------------------
Instruction:  All filter code are included in this file and compiled into one object file.
              Include your .cpp here.
-----------------------------------------------------------------------------------------*/
//#include "aliagfilter.cpp"
//#include "aliegfilter.cpp"

void RegisterBuiltinFilters(){
  /*--------------------------------------------------------------------------------------
  Instruction: register the function pointer of your filter with the filter name.
  Register your filter here.
  ---------------------------------------------------------------------------------------*/
  //RESTPP_COMPONENTS::filtermanager->RegisterBuiltinFilter("AliEgFilter", (UpdateRequestFilter)&AliEgFilter);
  //RESTPP_COMPONENTS::filtermanager->RegisterBuiltinFilter("AliAgFilter", (UpdateRequestFilter)&AliAgFilter);
}


#endif
