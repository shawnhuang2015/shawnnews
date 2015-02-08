/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * testpartitiondatafeeder.hpp: /gperun2/src/test/testpartitiondatafeeder.hpp
 *
 *  Created on: Aug 22, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPELIB4_TESTPARTITIONDATAFEEDER_HPP_
#define GPELIB4_TESTPARTITIONDATAFEEDER_HPP_

#include <topology4/testpartition.hpp>
#include <sstream>

using topology4::PartitionDataFeeder;
using topology4::Type_IdMap;
using topology4::PartitionEdgeInfo;
using topology4::Attribute;

namespace unittest_gpe4 {

/**
 * partition data feed based on sample dense directed graph.
 */
class TestPartitionDataFeeder_DenseD : public PartitionDataFeeder {
 public:
  VertexLocalId_t edgecount_;

  TestPartitionDataFeeder_DenseD()
      : PartitionDataFeeder(true, false, 8, 8),
        edgecount_(256) {
    this->num_vertices_ = edgecount_ * edgecount_;
    buf_ = new char[1024];
  }

  ~TestPartitionDataFeeder_DenseD() {
    delete[] buf_;
    for (size_t i = 0; i < bufvec1_.size(); ++i)
      delete[] bufvec1_[i];
  }

  // for id map.
  Type_IdMap InitIdMap() {
    return topology4::Type_IdMap_Long;
  }

  uint64_t GetUserIntId(VertexLocalId_t id) {
    return id * 2;
  }

  char* GetUserStringId(VertexLocalId_t id) {
    return NULL;
  }

  void CloseIdMap() {
  }

  std::vector<int> GetVertexAttributeType() {
    std::vector<int> vec;
    vec.push_back(Attribute::BOOL_TYPE);
    vec.push_back(Attribute::UINT_TYPE);
    vec.push_back(Attribute::REAL_TYPE);
    vec.push_back(Attribute::STRING_TYPE);
    vec.push_back(Attribute::BOOL_TYPE);
    vec.push_back(Attribute::UINT_TYPE);
    vec.push_back(Attribute::REAL_TYPE);
    vec.push_back(Attribute::STRING_TYPE);
    return vec;
  }

  std::vector<int> GetEdgeAttributeType() {
    std::vector<int> vec;
    vec.push_back(Attribute::BOOL_TYPE);
    vec.push_back(Attribute::UINT_TYPE);
    vec.push_back(Attribute::REAL_TYPE);
    vec.push_back(Attribute::STRING_TYPE);
    vec.push_back(Attribute::BOOL_TYPE);
    vec.push_back(Attribute::UINT_TYPE);
    vec.push_back(Attribute::REAL_TYPE);
    vec.push_back(Attribute::STRING_TYPE);
    return vec;
  }

  // for vertex attributes
  void FetchVertexProp(VertexLocalId_t id, char*& prop_ptr,
                       size_t& prop_length) {
    PrepreProp(id, prop_ptr, prop_length, buf_);
  }

  void PrepreProp(VertexLocalId_t id, char*& prop_ptr, size_t& prop_length,
                  char* buf) {
    prop_ptr = buf;
    char* offset = buf;
    *offset = 255;   // header
    offset++;
    for (uint32_t i = 0; i < 2; ++i) {
      *offset = id % 2 == 0 ? 1 : 0;
      offset++;
      gutil::CompactWriter::writeCompressed(id, offset);
      float f = id + 0.5f;
      memcpy(offset, &f, sizeof(f));
      offset += sizeof(f);
      std::stringstream ss;
      ss << id;
      std::string sid = ss.str();
      unsigned int length = sid.size();
      gutil::CompactWriter::writeCompressed(length, offset);
      memcpy(offset, sid.c_str(), length);
      offset += length;
      ++id;
    }
    prop_length = offset - buf;
  }

// for edges

  void InitEdges(unsigned int num_rowpartition) {
    for (unsigned int i = 0; i < num_rowpartition; i++) {
      bufvec1_.push_back(new char[1024 * edgecount_ * 2]);
    }
  }

  void FetchOutgoingEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<PartitionEdgeInfo>& edgeinfo) {
    std::vector<VertexLocalId_t> nids;
    VertexLocalId_t id1 = id / edgecount_;
    VertexLocalId_t id2 = id % edgecount_;
    for (VertexLocalId_t i = 0; i < edgecount_; ++i) {
      nids.push_back(id1 * edgecount_ + i);
      nids.push_back(i * edgecount_ + id2);
    }
    size_t asize = 1024;
    for (size_t i = 0; i < nids.size(); i++) {
      char* charptr = bufvec1_[rowpartitionindex] + asize * i;
      char* prop_ptr;
      size_t prop_length;
      PrepreProp(id + nids[i], prop_ptr, prop_length, charptr);
      edgeinfo.push_back(
          PartitionEdgeInfo(nids[i], prop_length, charptr));
      // std::cout << id << "," << nids[i] << "," << d << "\n";
    }
  }

  void FetchIncominggEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<PartitionEdgeInfo>& edgeinfo) {
  }

  void CloseEdges(unsigned int rowpartitionindex) {
  }

  typedef std::pair<std::string, std::pair<char*, size_t> > FileInfo_t;
  // for others
  void WriteOtherFiles(std::vector<FileInfo_t>& otherfiles_towrite) {
  }

 private:
  char* buf_;
  std::vector<char*> bufvec1_;
};

/**
 * partition data feed based on sample undirected graph.
 * extenal id is unsigned long = id * 2
 * Each vertex (except 0) will have (vid >> 1) neighbor as incoming edge. (undirected graph will double edges)
 * Vertex has 3 attributes: id is even (bool), id itself (unsigned int), id string (string)
 * Edge has one attribute: sid + vid (double)
 */
class TestPartitionDataFeeder_UD : public PartitionDataFeeder {
 public:
  explicit TestPartitionDataFeeder_UD(VertexLocalId_t num_vertices)
      : PartitionDataFeeder(false, false, 3, 1) {
    this->num_vertices_ = num_vertices;
    buf_ = new char[1024];
  }

  ~TestPartitionDataFeeder_UD() {
    delete[] buf_;
    for (size_t i = 0; i < bufvec1_.size(); ++i)
      delete[] bufvec1_[i];
    for (size_t i = 0; i < bufvec2_.size(); ++i)
      delete[] bufvec2_[i];
  }

  // for id map.
  Type_IdMap InitIdMap() {
    return topology4::Type_IdMap_Long;
  }

  uint64_t GetUserIntId(VertexLocalId_t id) {
    return id * 2;
  }

  char* GetUserStringId(VertexLocalId_t id) {
    return NULL;
  }

  void CloseIdMap() {
  }

  std::vector<int> GetVertexAttributeType() {
    std::vector<int> vec;
    vec.push_back(Attribute::BOOL_TYPE);
    vec.push_back(Attribute::UINT_TYPE);
    vec.push_back(Attribute::STRING_TYPE);
    return vec;
  }

  std::vector<int> GetEdgeAttributeType() {
    std::vector<int> vec;
    vec.push_back(Attribute::REAL_TYPE);
    return vec;
  }

  // for vertex attributes
  void FetchVertexProp(VertexLocalId_t id, char*& prop_ptr,
                       size_t& prop_length) {
    prop_ptr = buf_;
    char* offset = buf_;
    *offset = 7;   // header
    offset++;
    *offset = id % 2 == 0 ? 1 : 0;
    offset++;
    gutil::CompactWriter::writeCompressed(id, offset);
    std::stringstream ss;
    ss << id;
    std::string sid = ss.str();
    unsigned int length = sid.size();
    gutil::CompactWriter::writeCompressed(length, offset);
    memcpy(offset, sid.c_str(), length);
    offset += length;
    prop_length = offset - buf_;
  }

// for edges

  void InitEdges(unsigned int num_rowpartition) {
    for (unsigned int i = 0; i < num_rowpartition; i++) {
      bufvec1_.push_back(new char[1000]);
      bufvec2_.push_back(new char[1000]);
    }
  }

  void FetchOutgoingEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<PartitionEdgeInfo>& edgeinfo) {
    std::vector<VertexLocalId_t> nids;
    if (id > 0)
      nids.push_back(id << 1);
    nids.push_back((id << 1) + 1);
    size_t asize = 5;
    for (size_t i = 0; i < nids.size(); i++) {
      if (nids[i] >= this->num_vertices_)
        continue;
      char* charptr = bufvec1_[rowpartitionindex] + asize * i;
      charptr[0] = 1;  // header bit
      float d = id + nids[i];
      memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
      edgeinfo.push_back(
          PartitionEdgeInfo(nids[i], asize, charptr));
      // std::cout << id << "," << nids[i] << "," << d << "\n";
    }
  }

  void FetchIncominggEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<PartitionEdgeInfo>& edgeinfo) {
    if (id == 0)
      return;
    size_t asize = 5;
    VertexLocalId_t nid = id >> 1;
    char* charptr = bufvec2_[rowpartitionindex];
    charptr[0] = 1;  // header bit
    float d = nid + id;
    memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
    edgeinfo.push_back(PartitionEdgeInfo(nid, asize, charptr));
    // std::cout << id << "," << nid << "," << d << "\n";
  }

  void CloseEdges(unsigned int rowpartitionindex) {
  }

  typedef std::pair<std::string, std::pair<char*, size_t> > FileInfo_t;
  // for others
  void WriteOtherFiles(std::vector<FileInfo_t>& otherfiles_towrite) {
  }

 private:
  char* buf_;
  std::vector<char*> bufvec1_;
  std::vector<char*> bufvec2_;
};

/**
 * partition data feed based on sample directed graph
 * extenal id is string = "id_" + (id * 2)
 * Each vertex (except 0) will have (vid >> 1) neighbor as incoming edge.
 * Vertex has 3 attributes: id is even (bool), id itself (unsigned int), id string (string)
 * Edge has one attribute: id * 2 (double)
 */
class TestPartitionDataFeeder_D : public PartitionDataFeeder {
 public:
  explicit TestPartitionDataFeeder_D(VertexLocalId_t num_vertices)
      : PartitionDataFeeder(true, true, 3, 1) {
    num_vertices_ = num_vertices;
    buf_ = new char[1024];
  }

  ~TestPartitionDataFeeder_D() {
    delete[] buf_;
    for (size_t i = 0; i < bufvec1_.size(); ++i)
      delete[] bufvec1_[i];
    for (size_t i = 0; i < bufvec2_.size(); ++i)
      delete[] bufvec2_[i];
  }

  // for id map.
  Type_IdMap InitIdMap() {
    return topology4::Type_IdMap_String;
  }

  uint64_t GetUserIntId(VertexLocalId_t id) {
    return 0;
  }

  char* GetUserStringId(VertexLocalId_t id) {
    std::stringstream ss;
    ss << "id_" << (id * 2);
    memcpy(buf_, ss.str().c_str(), ss.str().size() + 1);
    return buf_;
  }

  void CloseIdMap() {
  }

  std::vector<int> GetVertexAttributeType() {
    std::vector<int> vec;
    vec.push_back(Attribute::BOOL_TYPE);
    vec.push_back(Attribute::UINT_TYPE);
    vec.push_back(Attribute::STRING_TYPE);
    return vec;
  }

  std::vector<int> GetEdgeAttributeType() {
    std::vector<int> vec;
    vec.push_back(Attribute::REAL_TYPE);
    return vec;
  }

  // for vertex attributes
  void FetchVertexProp(VertexLocalId_t id, char*& prop_ptr,
                       size_t& prop_length) {
    prop_ptr = buf_;
    char* offset = buf_;
    *offset = 7;   // header
    offset++;
    *offset = id % 2 == 0 ? 1 : 0;
    offset++;
    gutil::CompactWriter::writeCompressed(id, offset);
    std::stringstream ss;
    ss << id;
    std::string sid = ss.str();
    unsigned int length = sid.size();
    gutil::CompactWriter::writeCompressed(length, offset);
    memcpy(offset, sid.c_str(), length);
    offset += length;
    prop_length = offset - buf_;
  }

// for edges

  void InitEdges(unsigned int num_rowpartition) {
    for (unsigned int i = 0; i < num_rowpartition; i++) {
      bufvec1_.push_back(new char[1000]);
      bufvec2_.push_back(new char[1000]);
    }
  }

  void FetchOutgoingEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<PartitionEdgeInfo>& edgeinfo) {
    std::vector<VertexLocalId_t> nids;
    if (id > 0)
      nids.push_back(id << 1);
    nids.push_back((id << 1) + 1);
    size_t asize = 5;
    for (size_t i = 0; i < nids.size(); i++) {
      if (nids[i] >= this->num_vertices_)
        continue;
      char* charptr = bufvec1_[rowpartitionindex] + asize * i;
      charptr[0] = 1;  // header bit
      float d = id + nids[i];
      memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
      edgeinfo.push_back(
          PartitionEdgeInfo(nids[i], asize, charptr));
      // std::cout << id << "," << nids[i] << "\n";
    }
  }

  void FetchIncominggEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<PartitionEdgeInfo>& edgeinfo) {
    if (id == 0)
      return;
    size_t asize = 5;
    VertexLocalId_t nid = id >> 1;
    char* charptr = bufvec2_[rowpartitionindex];
    charptr[0] = 1;  // header bit
    float d = nid + id;
    memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
    edgeinfo.push_back(PartitionEdgeInfo(nid, asize, charptr));
  }

  void CloseEdges(unsigned int rowpartitionindex) {
  }

  typedef std::pair<std::string, std::pair<char*, size_t> > FileInfo_t;
  // for others
  void WriteOtherFiles(std::vector<FileInfo_t>& otherfiles_towrite) {
  }

 private:
  char* buf_;
  std::vector<char*> bufvec1_;
  std::vector<char*> bufvec2_;
};

}  // namespace unittest_gpe4

#endif /* GPELIB4_TESTPARTITIONDATAFEEDER_HPP_ */
