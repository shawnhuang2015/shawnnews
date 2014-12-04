/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Jul 31, 2013
 *      Author: lichen
 ******************************************************************************/

#include <gutil/glogging.hpp>
#include <string>
#include <iostream>
#include "gtest/gtest.h"

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  gutil::GSQLLogger logger(argv[0], "/tmp/log/unit");
#ifndef REMOVETESTASSERT
  std::cout
      << "\n***Testing Assert Mode. To disable it, add cb=1 when build using scons\n\n";
#endif
  return RUN_ALL_TESTS();
}
