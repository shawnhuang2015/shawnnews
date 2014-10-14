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
  GSQLLogger logger(argv[0], "/tmp/log/unit");
  return RUN_ALL_TESTS();
}
