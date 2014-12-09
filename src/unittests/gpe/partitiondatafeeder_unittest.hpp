/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * testpartitiondatafeeder.hpp: /gperun2/src/test/testpartitiondatafeeder.hpp
 *
 *  Created on: Aug 22, 2013
 *      Author: lichen
 ******************************************************************************/

#ifndef GPE_TESTPARTITIONDATAFEEDER_HPP_
#define GPE_TESTPARTITIONDATAFEEDER_HPP_

#include <partition/partitiondatafeeder.hpp>
#include <enginebase/attribute.hpp>
#include <sstream>

namespace unittest {

/**
 * partition data feed based on sample undirected graph.
 * extenal id is unsigned long = id * 2
 * Each vertex (except 0) will have (vid >> 1) neighbor as incoming edge. (undirected graph will double edges)
 * Vertex has 3 attributes: id is even (bool), id itself (unsigned int), id string (string)
 * Edge has one attribute: sid + vid (double)
 */
class TestPartitionDataFeeder_UD : public gpelib::PartitionDataFeeder {
 public:
  explicit TestPartitionDataFeeder_UD(VertexLocalId_t num_vertices)
      : gpelib::PartitionDataFeeder(false, false, 3, 1) {
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
  gpelib::Type_IdMap InitIdMap() {
    return gpelib::Type_IdMap_Long;
  }

  uint64_t GetUserIntId(VertexLocalId_t id) {
    return id * 2;
  }

  char* GetUserStringId(VertexLocalId_t id) {
    return NULL;
  }

  void CloseIdMap() {
  }

  // for vertex attributes
  void FetchVertexProp(VertexLocalId_t id, char*& prop_ptr,
                       size_t& prop_length) {
    prop_ptr = buf_;
    prop_ptr[0] = gpelib::Attribute::BOOL_TYPE;
    prop_ptr[1] = id % 2 == 0 ? 1 : 0;
    prop_ptr[2] = gpelib::Attribute::INT_TYPE;
    int iid = id;
    memcpy(prop_ptr + 3, reinterpret_cast<char *>(&iid), sizeof(iid));
    prop_ptr[7] = gpelib::Attribute::STRING_TYPE;
    std::stringstream ss;
    ss << id;
    std::string sid = ss.str();
    unsigned int length = sid.size();
    memcpy(prop_ptr + 8, reinterpret_cast<char *>(&length),
           sizeof(length));
    memcpy(prop_ptr + 12, sid.c_str(), length);
    prop_length = length + 12;
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
      std::vector<gpelib::PartitionEdgeInfo>& edgeinfo) {
    std::vector<VertexLocalId_t> nids;
    if (id > 0)
      nids.push_back(id << 1);
    nids.push_back((id << 1) + 1);
    size_t asize = 9;
    for (size_t i = 0; i < nids.size(); i++) {
      if (nids[i] >= this->num_vertices_)
        continue;
      char* charptr = bufvec1_[rowpartitionindex] + asize * i;
      charptr[0] = gpelib::Attribute::REAL_TYPE;
      double d = id + nids[i];
      memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
      edgeinfo.push_back(
          gpelib::PartitionEdgeInfo(nids[i], asize, charptr));
      // std::cout << id << "," << nids[i] << "," << d << "\n";
    }
  }

  void FetchIncominggEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<gpelib::PartitionEdgeInfo>& edgeinfo) {
    if (id == 0)
      return;
    size_t asize = 9;
    VertexLocalId_t nid = id >> 1;
    char* charptr = bufvec2_[rowpartitionindex];
    charptr[0] = gpelib::Attribute::REAL_TYPE;
    double d = nid + id;
    memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
    edgeinfo.push_back(gpelib::PartitionEdgeInfo(nid, asize, charptr));
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
class TestPartitionDataFeeder_D : public gpelib::PartitionDataFeeder {
 public:
  explicit TestPartitionDataFeeder_D(VertexLocalId_t num_vertices)
      : gpelib::PartitionDataFeeder(true, true, 3, 1) {
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
  gpelib::Type_IdMap InitIdMap() {
    return gpelib::Type_IdMap_String;
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

  // for vertex attributes
  void FetchVertexProp(VertexLocalId_t id, char*& prop_ptr,
                       size_t& prop_length) {
    prop_ptr = buf_;
    prop_ptr[0] = gpelib::Attribute::BOOL_TYPE;
    prop_ptr[1] = id % 2 == 0 ? 1 : 0;
    prop_ptr[2] = gpelib::Attribute::INT_TYPE;
    unsigned int iid = (unsigned int) id;
    memcpy(prop_ptr + 3, reinterpret_cast<char *>(&iid), sizeof(iid));
    prop_ptr[7] = gpelib::Attribute::STRING_TYPE;
    std::stringstream ss;
    ss << id;
    std::string sid = ss.str();
    unsigned int length = sid.size();
    memcpy(prop_ptr + 8, reinterpret_cast<char *>(&length),
           sizeof(length));
    memcpy(prop_ptr + 12, sid.c_str(), length);
    prop_length = length + 12;
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
      std::vector<gpelib::PartitionEdgeInfo>& edgeinfo) {
    std::vector<VertexLocalId_t> nids;
    if (id > 0)
      nids.push_back(id << 1);
    nids.push_back((id << 1) + 1);
    size_t asize = 9;
    for (size_t i = 0; i < nids.size(); i++) {
      if (nids[i] >= this->num_vertices_)
        continue;
      char* charptr = bufvec1_[rowpartitionindex] + asize * i;
      charptr[0] = gpelib::Attribute::REAL_TYPE;
      double d = id + nids[i];
      memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
      edgeinfo.push_back(
          gpelib::PartitionEdgeInfo(nids[i], asize, charptr));
      // std::cout << id << "," << nids[i] << "\n";
    }
  }

  void FetchIncominggEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<gpelib::PartitionEdgeInfo>& edgeinfo) {
    if (id == 0)
      return;
    size_t asize = 9;
    VertexLocalId_t nid = id >> 1;
    char* charptr = bufvec2_[rowpartitionindex];
    charptr[0] = gpelib::Attribute::REAL_TYPE;
    double d = nid + id;
    memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
    edgeinfo.push_back(gpelib::PartitionEdgeInfo(nid, asize, charptr));
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
 * partition data feed based on sample undirected graph.
 * extenal id is string = "id_" + (id * 2)
 * Each vertex (except 0) will have (vid >> 1) neighbor as incoming edge. (undirected graph will double edges)
 * Vertex has 3 attributes: id is even (bool), id itself (unsigned int), id string (string)
 * Edge has one attribute: sid + vid (double)
 */
class TestPartitionDataFeeder_Delta : public gpelib::PartitionDataFeeder {
 public:
  explicit TestPartitionDataFeeder_Delta(VertexLocalId_t num_vertices)
      : gpelib::PartitionDataFeeder(false, false, 3, 1) {
    this->num_vertices_ = num_vertices;
    buf_ = new char[1024];
    gvid_to_pid_ = NULL;
  }

  ~TestPartitionDataFeeder_Delta() {
    delete[] buf_;
    for (size_t i = 0; i < bufvec1_.size(); ++i)
      delete[] bufvec1_[i];
    for (size_t i = 0; i < bufvec2_.size(); ++i)
      delete[] bufvec2_[i];
    delete[] gvid_to_pid_;
  }

  // for id map.
  gpelib::Type_IdMap InitIdMap() {
    return gpelib::Type_IdMap_String;
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

  // for vertex attributes
  void FetchVertexProp(VertexLocalId_t id, char*& prop_ptr,
                       size_t& prop_length) {
    prop_ptr = buf_;
    prop_ptr[0] = gpelib::Attribute::BOOL_TYPE;
    prop_ptr[1] = id % 2 == 0 ? 1 : 0;
    prop_ptr[2] = gpelib::Attribute::INT_TYPE;
    int iid = id;
    memcpy(prop_ptr + 3, reinterpret_cast<char *>(&iid), sizeof(iid));
    prop_ptr[7] = gpelib::Attribute::STRING_TYPE;
    std::stringstream ss;
    ss << id;
    std::string sid = ss.str();
    unsigned int length = sid.size();
    memcpy(prop_ptr + 8, reinterpret_cast<char *>(&length),
           sizeof(length));
    memcpy(prop_ptr + 12, sid.c_str(), length);
    prop_length = length + 12;
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
      std::vector<gpelib::PartitionEdgeInfo>& edgeinfo) {
    std::vector<VertexLocalId_t> nids;
    if (id > 0)
      nids.push_back(id << 1);
    nids.push_back((id << 1) + 1);
    size_t asize = 9;
    for (size_t i = 0; i < nids.size(); i++) {
      if (nids[i] >= this->num_vertices_)
        continue;
      char* charptr = bufvec1_[rowpartitionindex] + asize * i;
      charptr[0] = gpelib::Attribute::REAL_TYPE;
      double d = id + nids[i];
      memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
      edgeinfo.push_back(
          gpelib::PartitionEdgeInfo(nids[i], asize, charptr));
      // std::cout << id << "," << nids[i] << "," << d << "\n";
    }
  }

  void FetchIncominggEdges(
      unsigned int rowpartitionindex, VertexLocalId_t id,
      std::vector<gpelib::PartitionEdgeInfo>& edgeinfo) {
    if (id == 0)
      return;
    size_t asize = 9;
    VertexLocalId_t nid = id >> 1;
    char* charptr = bufvec2_[rowpartitionindex];
    charptr[0] = gpelib::Attribute::REAL_TYPE;
    double d = nid + id;
    memcpy(charptr + 1, reinterpret_cast<char *>(&d), sizeof(d));
    edgeinfo.push_back(gpelib::PartitionEdgeInfo(nid, asize, charptr));
    // std::cout << id << "," << nid << "," << d << "\n";
  }

  void CloseEdges(unsigned int rowpartitionindex) {
  }

  typedef std::pair<std::string, std::pair<char*, size_t> > FileInfo_t;
  // for others
  void WriteOtherFiles(std::vector<FileInfo_t>& otherfiles_towrite) {
    gvid_to_pid_ = new VertexLocalId_t[num_vertices_ * 2];
    std::fill(gvid_to_pid_, gvid_to_pid_ + num_vertices_ * 2,
              (VertexLocalId_t) -1);
    for (VertexLocalId_t i = 0; i < (const VertexLocalId_t) num_vertices_;
        i++) {
      gvid_to_pid_[2 * i] = i;
    }
    otherfiles_towrite.push_back(
        std::pair<std::string, std::pair<char*, size_t> >(
            "gvid_to_pid.map",
            std::pair<char*, size_t>(
                reinterpret_cast<char*>(gvid_to_pid_),
                num_vertices_ * 2 * sizeof(VertexLocalId_t))));
  }

 private:
  char* buf_;
  std::vector<char*> bufvec1_;
  std::vector<char*> bufvec2_;
  VertexLocalId_t* gvid_to_pid_;
};

}  // namespace unittest

#endif /* GPE_TESTPARTITIONDATAFEEDER_HPP_ */
