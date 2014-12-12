/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/

#include "serviceapi.hpp"
#include <core_impl/gpe_impl/impl.hpp>
#ifndef OPENCODE
#include <core_impl/gse_impl/post_impl.hpp>
#endif

int main(int argc, char** argv) {
  UDIMPL::UDFRunner impl;
#ifndef OPENCODE
  UDIMPL::UD_PostJson2Delta postdelta_impl;
  gperun::ServiceAPI::Run(argc, argv, &impl, &postdelta_impl);
#else
  gperun::ServiceAPI::Run(argc, argv, &impl);
#endif
}

