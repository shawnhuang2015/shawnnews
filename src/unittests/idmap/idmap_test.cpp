/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-CSE
 * idmap_test.cpp: /gdbms/src/unittests/idmap/idmap_test.cpp
 *
 *  Created on: Oct 12, 2013
 *      Author: like
 ******************************************************************************/

#include <boost/crc.hpp>  // for boost::crc_32_type
#include <gtest/gtest.h>
#include <gutil/gtimer.hpp>
#include <gstore/id_adaptor/id_map.hpp>
#include <gstore/id_adaptor/string_id_map.hpp>
#include <algorithm>
#include "../idmap/words.h"

#define NumberIntKeys 1000000

template<class VERTEX_TYPE>
class TestIDMap {
 public:
  explicit TestIDMap(bool idIsString)
      : idIsString_(idIsString) {
    if (!idIsString_) {
      idMap_ = new gstore::IdMap<uint64_t, VERTEX_TYPE>(false,
                                                        456789,
                                                        false,
                                                        2);
    } else {
      strIdMap_ = new gstore::StringIdMap<uint64_t, VERTEX_TYPE>(false,
                                                                 456789,
                                                                 false,
                                                                 2);
    }
  }
  ~TestIDMap() {
    if (!idIsString_) {
      delete idMap_;
    } else {
      delete strIdMap_;
    }
  }

  VERTEX_TYPE get_or_insertID(uint64_t IDLong, bool &newID) {
    return idMap_->insert(IDLong, newID);
  }

  VERTEX_TYPE get_or_insertID(std::string IDString, bool &newID) {
    return strIdMap_->insert(IDString.c_str(), newID);
  }

 private:
  gstore::IdMap<uint64_t, VERTEX_TYPE> *idMap_;
  gstore::StringIdMap<uint64_t, VERTEX_TYPE> *strIdMap_;
  bool idIsString_;
};

static void validIdMap(std::vector<uint32_t> &keyVector,
                       uint64_t sum_val,
                       uint32_t expectedCRC,
                       uint64_t expectedSUM,
                       uint64_t expectedMIN,
                       uint64_t expectedMAX
                       ) {
  uint32_t min_val = * std::min_element(keyVector.begin(), keyVector.end());
  uint32_t max_val = * std::max_element(keyVector.begin(), keyVector.end());
  boost::crc_32_type result;
  result.process_bytes(reinterpret_cast<char *>(keyVector.data()),
                       keyVector.size() * sizeof(uint32_t));
  EXPECT_EQ(result.checksum(), expectedCRC);
  EXPECT_EQ(sum_val, expectedSUM);
  EXPECT_EQ(min_val, expectedMIN);
  EXPECT_EQ(max_val, expectedMAX);
}

TEST(IDMAP_TEST, INTERGER_INCREASE) {
  TestIDMap<uint32_t> intIdMap(false);
  std::vector<uint32_t> keyVector;
  bool newID;
  uint64_t sum_val = 0;
  for (uint32_t i = 0; i < NumberIntKeys; i++) {
    keyVector.push_back(intIdMap.get_or_insertID(i*2, newID));
    sum_val += intIdMap.get_or_insertID(i*2, newID);
  }
  validIdMap(keyVector, sum_val, 2542833288, 727782023776, 0, 1262141);
}

TEST(IDMAP_TEST, INTERGER_DECREASE) {
  TestIDMap<uint32_t> intIdMap(false);
  std::vector<uint32_t> keyVector;
  bool newID;
  uint64_t sum_val = 0;
  for (int32_t i = (NumberIntKeys -1); i >= 0; i--) {
    keyVector.push_back(intIdMap.get_or_insertID((uint32_t)i*2, newID));
    sum_val += intIdMap.get_or_insertID((uint32_t)i*2, newID);
  }
  validIdMap(keyVector, sum_val, 1520598145, 727782023776, 0, 1262141);
}



TEST(IDMAP_TEST, STRING) {
  struct membuf : std::streambuf {
    membuf(char* begin, char* end) {
      this->setg(begin, begin, end);
    }
  };

  TestIDMap<uint32_t> stringIdMap(true);
  std::vector<uint32_t> keyVector;
  bool newID;
  uint64_t sum_val = 0;
  std::string keyStr = "";

  membuf sbuf(words_txt, words_txt + words_txt_len);
  std::istream in(&sbuf);
  while (std::getline(in, keyStr)) {
    keyVector.push_back(stringIdMap.get_or_insertID(keyStr.c_str(), newID));
    sum_val += stringIdMap.get_or_insertID(keyStr.c_str(), newID);
  }
  validIdMap(keyVector, sum_val, 3458673556, 67499187015, 0, 545074);
}
