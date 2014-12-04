/* Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 *
 *  Created on: Jul 31, 2013
 *      Author: lichen
 ******************************************************************************/

#include <cmath>
#include "gtest/gtest.h"

TEST(SquareRootTest, PositiveNos) {
  EXPECT_EQ(18.0, sqrt(324.0));
  EXPECT_EQ(25.4, sqrt(645.16));
//    EXPECT_EQ (50.3321, sqrt (2533.310224));
}

TEST(SquareRootTest, ZeroAndNegativeNos) {
  ASSERT_EQ(0.0, sqrt(0.0));
//    ASSERT_EQ (-1, sqrt (-22.0));
}
