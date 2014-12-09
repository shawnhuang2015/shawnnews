/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Oct 14, 2013
 *      Author: lichen
 ******************************************************************************/

#include <core_ext/gpe/serviceapi.hpp>
#include <core_impl/gpe_impl/impl.hpp>
#include <core_impl/gse_impl/post_impl.hpp>

int main(int argc, char** argv) {
  UDIMPL::GPE_UD_Impl impl;
  UDIMPL::UD_PostJson2Delta postdelta_impl;
  gperun::ServiceAPI api(argc, argv, &impl, &postdelta_impl);
  api.RunMain();
}

