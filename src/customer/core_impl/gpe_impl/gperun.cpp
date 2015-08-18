/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/

#include <gpe/serviceapi.hpp>
#ifndef OPENCODE
#include <core_impl/gse_impl/post_impl.hpp>
#endif
#include "impl.hpp"

int main(int argc, char** argv) {
#ifdef BUILDVERSION
    std::string versStr = BUILDVERSION;
    std::replace(versStr.begin(), versStr.end(), ',', '\n');
    std::cerr << "GPE Server: Version\n" << versStr << std::endl;
#endif


  UDIMPL::UDFRunner impl;
#ifndef OPENCODE
  UDIMPL::UD_PostJson2Delta postdelta_impl;
  gperun::ServiceAPI::Run(argc, argv, &impl, &postdelta_impl);
#else
  gperun::ServiceAPI::Run(argc, argv, &impl);
#endif
}

