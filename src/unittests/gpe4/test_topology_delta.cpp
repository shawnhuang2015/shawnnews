/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * resource_test.cpp: /gperun/test/resource_test.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: lichen
 ******************************************************************************/

#include <topology4/attribute.hpp>
#include <topology4/deltaattribute.hpp>
#include <topology4/deltaitembinary.hpp>
#include <topology4/deltarecords.hpp>
#include <topology4/deltarebuilder.hpp>
#include <topology4/topologygraph.hpp>
#include <topology4/topologyprinter.hpp>
#include <gutil/gdiskutil.hpp>
#include <gutil/gcharbuffer.hpp>
#include <gtest/gtest.h>

using namespace topology4;

struct FixSizeStruct {
  uint32_t i_;
  uint32_t j_;

  static void Add(uint8_t* oldptr, uint8_t* newptr) {
    FixSizeStruct* oldstruct = reinterpret_cast<FixSizeStruct*>(oldptr);
    FixSizeStruct* newstruct = reinterpret_cast<FixSizeStruct*>(newptr);
    oldstruct->i_ += newstruct->i_;
    oldstruct->j_ += newstruct->j_;
  }
};

void BuildAttributesMeta(topology4::AttributesMeta& attributesmeta) {
  for (int i = 1; i < topology4::Attribute::AttributeEnd; ++i) {
    for (int j = 0; j < 4; ++j) {
      topology4::AttributeMeta attributemeta;
      attributemeta.type_ = i;
      attributemeta.isnullable_ = true;
      attributemeta.name_ = boost::lexical_cast<std::string>(i) + "_"
                            + boost::lexical_cast<std::string>(j);
      attributemeta.fixedsize_ = sizeof(FixSizeStruct);
      if (j == 3
          && (i == topology4::Attribute::  FIXED_BINARY
              || i == topology4::Attribute:: UINT32_UDT_KV_LIST))
        attributemeta.merge_func_ = boost::bind(&FixSizeStruct::Add, _1,
                                                _2);
      attributesmeta.attributelist_.push_back(attributemeta);
    }
  }
  attributesmeta.Build();
}

void InsertAttribute(int type, uint8_t*& ptr, int sequence_id) {
  switch ((topology4::Attribute::ValueType) type) {
    case topology4::Attribute::BOOL_TYPE:
      *ptr = sequence_id % 2 == 0;
      ++ptr;
      break;
    case topology4::Attribute::UINT_TYPE:
      *ptr = sequence_id + 1;
      ++ptr;
      break;
    case topology4::Attribute::REAL_TYPE: {
      float value = (sequence_id + 1) * 100.0f;
      memcpy(ptr, &value, sizeof(value));
      ptr += sizeof(value);
    }
      break;
    case topology4::Attribute::STRING_TYPE:
      *ptr = 1;
      ++ptr;
      *ptr = 'A' + sequence_id;
      ++ptr;
      break;
    case topology4::Attribute::UINT32_UINT32_KV_LIST:
      *ptr = 2;
      ++ptr;
      *ptr = sequence_id;
      ++ptr;
      *ptr = sequence_id + 1;
      ++ptr;
      *ptr = sequence_id + 1;
      ++ptr;
      *ptr = sequence_id + 2;
      ++ptr;
      break;
    case topology4::Attribute::FIXED_BINARY: {
      FixSizeStruct fixed;
      fixed.i_ = sequence_id;
      fixed.j_ = sequence_id + 1;
      memcpy(ptr, &fixed, sizeof(fixed));
      ptr += sizeof(fixed);
    }
      break;
    case topology4::Attribute::UINT32_UDT_KV_LIST: {
      *ptr = 2;
      ++ptr;
      *ptr = sequence_id;
      ++ptr;
      FixSizeStruct fixed;
      fixed.i_ = sequence_id;
      fixed.j_ = sequence_id + 1;
      memcpy(ptr, &fixed, sizeof(fixed));
      ptr += sizeof(fixed);
      *ptr = sequence_id + 1;
      ++ptr;
      fixed.i_ = sequence_id + 1;
      fixed.j_ = sequence_id + 2;
      memcpy(ptr, &fixed, sizeof(fixed));
      ptr += sizeof(fixed);
    }
      break;
    case topology4::Attribute::UINT_SET:
      *ptr = 2;
      ++ptr;
      *ptr = sequence_id;
      ++ptr;
      *ptr = sequence_id + 1;
      ++ptr;
      break;
    case topology4::Attribute::AttributeEnd:
      break;
  }
}

uint8_t* BuildDeltaBinary(int sequence_id) {
  uint8_t* binary = new uint8_t[4192];
  uint8_t* ptr = binary;
  if (sequence_id == -1) {
    for (int i = 1; i < topology4::Attribute::AttributeEnd * 4; ++i) {
      *ptr = static_cast<int>(topology4::DeltaAttributeOperator_Delete);
      ++ptr;
    }
    return binary;
  }
  for (int i = 1; i < topology4::Attribute::AttributeEnd; ++i) {
    *ptr = static_cast<int>(topology4::DeltaAttributeOperator_Ignore);
    ++ptr;
    *ptr = static_cast<int>(topology4::DeltaAttributeOperator_Delete);
    ++ptr;
    *ptr = static_cast<int>(topology4::DeltaAttributeOperator_Overwrite);
    ++ptr;
    InsertAttribute(i, ptr, sequence_id);
    *ptr = static_cast<int>(topology4::DeltaAttributeOperator_Add);
    ++ptr;
    InsertAttribute(i, ptr, sequence_id);
  }
  return binary;
}

void ValidResult(topology4::Attribute& attribute, uint32_t sequence_id) {
  for (int index = 1; index < topology4::Attribute::AttributeEnd; ++index) {
    if (sequence_id == (uint32_t) -1) {
      EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 0), true);
      EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 1), true);
      EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 2), true);
      EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 3), true);
      continue;
    }
    EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 0), true);
    EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 1), true);
    EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 2), false);
    EXPECT_EQ(attribute.IsNull(4 * (index - 1) + 3), false);
    switch ((topology4::Attribute::ValueType) index) {
      case topology4::Attribute::BOOL_TYPE:
        if (sequence_id % 2 == 0) {
          EXPECT_EQ(attribute.GetBool(4 * (index - 1) + 2, false), true);
          EXPECT_EQ(attribute.GetBool(4 * (index - 1) + 3, false), true);
        } else {
          EXPECT_EQ(attribute.GetBool(4 * (index - 1) + 2, false), false);
          EXPECT_EQ(attribute.GetBool(4 * (index - 1) + 3, false), true);
        }
        break;
      case topology4::Attribute::UINT_TYPE:
        EXPECT_EQ(attribute.GetUInt(4 * (index - 1) + 2, 0),
                  sequence_id + 1);
        EXPECT_EQ(attribute.GetUInt(4 * (index - 1) + 3, 0),
                  (sequence_id + 1) * (sequence_id + 2) / 2);
        break;
      case topology4::Attribute::REAL_TYPE:
        EXPECT_EQ(
              static_cast<uint32_t>(attribute.GetDouble(4 * (index - 1) + 2,
                                                        0) + 0.1),
              100 * (sequence_id + 1));
        EXPECT_EQ(
              static_cast<uint32_t>(attribute.GetDouble(4 * (index - 1) + 3,
                                                        0) + 0.1),
              100 * (sequence_id + 1) * (sequence_id + 2) / 2);
        break;
      case topology4::Attribute::STRING_TYPE: {
        std::stringstream ss;
        ss << static_cast<char>('A' + sequence_id);
        EXPECT_EQ(attribute.GetString(4 * (index - 1) + 2), ss.str());
        ss.str("");
        for (uint32_t i = 0; i <= sequence_id; ++i) {
          ss << static_cast<char>('A' + i);
        }
        EXPECT_EQ(attribute.GetString(4 * (index - 1) + 3), ss.str());
      }
        break;
      case topology4::Attribute::UINT32_UINT32_KV_LIST: {
        topology4::UInt32_KV_Pair_Reader reader = attribute
                                                  .GetUInt32KVPairReader(4 * (index - 1) + 2);
        EXPECT_EQ(reader.count(), 2u);
        reader.MoveNext();
        EXPECT_EQ(reader.key_, sequence_id);
        EXPECT_EQ(reader.value_, sequence_id + 1);
        reader.MoveNext();
        EXPECT_EQ(reader.key_, sequence_id + 1);
        EXPECT_EQ(reader.value_, sequence_id + 2);
        reader = attribute.GetUInt32KVPairReader(4 * (index - 1) + 3);
        EXPECT_EQ(reader.count(), sequence_id + 2);
        uint32_t index1 = 0;
        while (reader.MoveNext()) {
          EXPECT_EQ(reader.key_, index1);
          EXPECT_EQ(reader.value_, index1 + 1);
          index1++;
        }
      }
        break;
      case topology4::Attribute::FIXED_BINARY: {
        FixSizeStruct* fptr1 = reinterpret_cast<FixSizeStruct*>(attribute
                                                                .GetFixedSizeBinary(4 * (index - 1) + 2));
        EXPECT_EQ(fptr1->i_, sequence_id);
        EXPECT_EQ(fptr1->j_, sequence_id + 1);
        FixSizeStruct* fptr2 = reinterpret_cast<FixSizeStruct*>(attribute
                                                                .GetFixedSizeBinary(4 * (index - 1) + 3));
        EXPECT_EQ(fptr2->i_, sequence_id * (sequence_id + 1) / 2);
        EXPECT_EQ(fptr2->j_, (sequence_id + 1) * (sequence_id + 2) / 2);
      }
        break;
      case topology4::Attribute::UINT32_UDT_KV_LIST: {
        topology4::UDT_KV_Pair_Reader<FixSizeStruct> reader = attribute
                                                              .GetUDTKVPairReader<FixSizeStruct>(4 * (index - 1) + 2);
        EXPECT_EQ(reader.count(), 2u);
        reader.MoveNext();
        EXPECT_EQ(reader.key_, sequence_id);
        EXPECT_EQ(reader.value_->i_, sequence_id);
        EXPECT_EQ(reader.value_->j_, sequence_id + 1);
        reader.MoveNext();
        EXPECT_EQ(reader.key_, sequence_id + 1);
        EXPECT_EQ(reader.value_->i_, sequence_id + 1);
        EXPECT_EQ(reader.value_->j_, sequence_id + 2);
        reader = attribute.GetUDTKVPairReader<FixSizeStruct>(
                   4 * (index - 1) + 3);
        EXPECT_EQ(reader.count(), sequence_id + 2);
        uint32_t index1 = 0;
        while (reader.MoveNext()) {
          EXPECT_EQ(reader.key_, index1);
          EXPECT_EQ(
                reader.value_->i_,
                (index1 == 0 || index1 == sequence_id + 1) ?
                  index1 : 2 * index1);
          EXPECT_EQ(
                reader.value_->j_,
                (index1 == 0 || index1 == sequence_id + 1) ?
                  index1 + 1 : 2 * (index1 + 1));
          index1++;
        }
      }
        break;
      case topology4::Attribute::UINT_SET: {
        topology4::UInt_Reader reader = attribute
                                          .GetUIntReader(4 * (index - 1) + 2);
        EXPECT_EQ(reader.count(), 2u);
        reader.MoveNext();
        EXPECT_EQ(reader.value_, sequence_id);
        reader.MoveNext();
        EXPECT_EQ(reader.value_, sequence_id + 1);
        reader = attribute.GetUIntReader(4 * (index - 1) + 3);
        EXPECT_EQ(reader.count(), sequence_id + 2);
        uint32_t index1 = 0;
        while (reader.MoveNext()) {
          EXPECT_EQ(reader.value_, index1);
          index1++;
        }
      }
        break;
      case topology4::Attribute::AttributeEnd:
        break;
    }
  }
}

void CheckAttribute(topology4::AttributesMeta& attributesmeta,
                    topology4::DeltaAttributeCombiner& combiner,
                    int sequence_id) {
  topology4::Attribute attribute;
  topology4::AttributeFriendClass::SetAttributesMeta(&attribute,
                                                     &attributesmeta);
  uint8_t* deltaattr = combiner.GetCombinedBinary();
  // gutil::CompactWriter::PrintBytes(std::cout, "GetCombinedBinary", deltaattr,  12);
  topology4::AttributeFriendClass::SetAttributePtr(&attribute, deltaattr);
  std::cout << attribute << "\n";
  ValidResult(attribute, sequence_id);
}

void ApplyDelta(topology4::DeltaAttributeCombiner& combiner, int sequence_id) {
  uint8_t* deltabinary = BuildDeltaBinary(sequence_id);
  combiner.ApplyDelta(deltabinary);
  delete[] deltabinary;
}

void VerifyCombine(int applycount, bool testdelete){
  topology4::AttributesMeta attributesmeta;
  topology4::DeltaBuffer* deltabuffer = new topology4::DeltaBuffer();
  BuildAttributesMeta(attributesmeta);
  topology4::DeltaAttributeCombiner combiner(&attributesmeta, deltabuffer);
  uint8_t* standardbinary = new uint8_t[attributesmeta.headerbytes_]();
  combiner.SetOriginalAttributes(standardbinary);
  for(int i = 0; i < applycount; ++i)
    ApplyDelta(combiner, i);
  if(testdelete){
    ApplyDelta(combiner, -1);
    CheckAttribute(attributesmeta, combiner, -1);
    for(int i = 0; i < applycount; ++i)
      ApplyDelta(combiner, i);
  }
  CheckAttribute(attributesmeta, combiner, applycount - 1);
  delete deltabuffer;
  delete[] standardbinary;
}

TEST(GPE4DELTATEST, Delta_Combine) {
  for(int count = 1; count <= 10; ++count){
    VerifyCombine(count, false);
    VerifyCombine(count, true);
  }
}

void TestDeltaItemBinaryReader(topology4::DeltaItemBinary& binary,
                               topology4::TransactionId_t topologytid,
                               topology4::TransactionId_t querytid,
                               topology4::TransactionId_t start,
                               topology4::TransactionId_t  end){
  topology4::DeltaItemBinary_Reader reader(&binary, topologytid, querytid);
  for(topology4::TransactionId_t i = start; i <= end; i+=1000){
    EXPECT_EQ(reader.Next(), true);
    EXPECT_EQ(reader.size_, sizeof(topology4::TransactionId_t));
    EXPECT_EQ(*(topology4::TransactionId_t*)reader.binary_, i);
  }
  EXPECT_EQ(reader.Next(), false);
}

TEST(GPE4DELTATEST, DeltaItemBinary) {
  topology4::DeltaItemBinary binary;
  for(topology4::TransactionId_t i = 1000; i <= 10000; i+=1000)
    binary.AddBinary(i, (uint8_t*)&i, sizeof(topology4::TransactionId_t));
  std::cout << "Print one DeltaItemBinary " << binary;
  TestDeltaItemBinaryReader(binary, 0, 99999, 1000, 10000);
  TestDeltaItemBinaryReader(binary, 1100, 9900, 2000, 9000);
  TestDeltaItemBinaryReader(binary, 3000, 9000, 4000, 9000);
  EXPECT_EQ(binary.Compact_Until(1500), false);
  TestDeltaItemBinaryReader(binary, 0, 99999, 2000, 10000);
  EXPECT_EQ(binary.Compact_Until(5000), false);
  TestDeltaItemBinaryReader(binary, 0, 99999, 6000, 10000);
  EXPECT_EQ(binary.Compact_Until(10001), true);
}

TEST(GPE4DELTATEST, DeltaRecords) {
  gutil::pthread_spinlock_t* ret_lock = NULL;
  topology4::DeltaSegmentRecords segmentrecord(0, 10);
  const uint32_t endid = 2;
  {
    topology4::TransactionId_t tid = 100;
    for(uint32_t id = 0; id < endid; ++id) {
      topology4::DeltaVertexAttributeRecord* v0 = segmentrecord.vertexattrrecords_.GetPointerForWrite(id, ret_lock, true);
      v0->AddVertexDelta(tid, (uint8_t*)&id, sizeof(id));
      //v0->AddVertexDegree(tid, 6);
      gutil::pthread_spin_writeunlock(ret_lock);
      topology4::DeltaVertexEdgesRecord* v1 = segmentrecord.vertexedgerecords_.GetPointerForWrite(id, ret_lock, true);
      v1->AddEdgeDelta(1, 75, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(0, 100, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(0, 50, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(1, 125, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(0, 200, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(1, 25, tid, (uint8_t*)&id, sizeof(id));
      gutil::pthread_spin_writeunlock(ret_lock);
    }
    std::cout << "After Transaction " << tid << "\n" << segmentrecord;
  }
  {
    topology4::TransactionId_t tid = 200;
    for(uint32_t id = 0; id < endid; ++id) {
      topology4::DeltaVertexAttributeRecord* v0 = segmentrecord.vertexattrrecords_.GetPointerForWrite(id, ret_lock, true);
      v0->AddVertexDelta(tid, (uint8_t*)&id, sizeof(id));
      //v0->AddVertexDegree(tid, 6);
      gutil::pthread_spin_writeunlock(ret_lock);
      topology4::DeltaVertexEdgesRecord* v1 = segmentrecord.vertexedgerecords_.GetPointerForWrite(id, ret_lock, true);
      v1->AddEdgeDelta(1, 125, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(0, 200, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(1, 25, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(1, 75, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(0, 100, tid, (uint8_t*)&id, sizeof(id));
      v1->AddEdgeDelta(0, 50, tid, (uint8_t*)&id, sizeof(id));
      gutil::pthread_spin_writeunlock(ret_lock);
    }
    std::cout << "After Transaction " << tid << "\n" << segmentrecord;
  }
  {
    segmentrecord.Compact_Until(101);
    std::cout << "After Compact_Until " << 101 << "\n" << segmentrecord;
  }
  {
    segmentrecord.Compact_Until(202);
    for(uint32_t id = 0; id < endid; ++id){
      EXPECT_EQ(segmentrecord.vertexattrrecords_.GetPointerForRead(id, ret_lock) == NULL, true);
      EXPECT_EQ(segmentrecord.vertexedgerecords_.GetPointerForRead(id, ret_lock) == NULL, true);
    }
  }
}

std::string GetConfigFile1() {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";
  return enginecfgfile;
}

void InsertOneDelta(gmmnt::GlobalInstances& instance, topology4::TopologyGraph& topology, VertexLocalId_t i,
                    bool checkid, bool print=false, bool idcheck = true){
  gutil::GTimer timer;
  gutil::GCharBuffer buffer;
  uint8_t firstbyte = (((uint8_t) DeltaRecord_Vertex)
                       << DeltaRecordTypeBits) + (((uint8_t) DeltaAction_Insert));
  buffer.write((char*) &firstbyte, 1);
  buffer.writeCompressed(i);
  uint8_t attributeoperator =
      ((uint8_t) DeltaAttributeOperator_Overwrite);
  buffer.write((char*) &attributeoperator, 1);
  buffer.writeCompressed(i);
  buffer.write((char*) &DeltaWatermarkByte_, 1);
  if(i > 0){
    firstbyte = (((uint8_t) DeltaRecord_Vertex)
                 << DeltaRecordTypeBits) + (((uint8_t) DeltaAction_Insert));
    buffer.write((char*) &firstbyte, 1);
    buffer.writeCompressed(0);
    uint8_t attributeoperator =
        ((uint8_t) DeltaAttributeOperator_Add);
    buffer.write((char*) &attributeoperator, 1);
    buffer.writeCompressed(1);
    buffer.write((char*) &DeltaWatermarkByte_, 1);
    firstbyte = (((uint8_t) DeltaRecord_Edge) << DeltaRecordTypeBits)
                + (((uint8_t) DeltaAction_Insert));
    buffer.write((char*) &firstbyte, 1);
    buffer.writeCompressed(i);
    buffer.writeCompressed(i - 1);
    buffer.writeCompressed(0); //type
    attributeoperator = ((uint8_t) DeltaAttributeOperator_Add);
    buffer.write((char*) &attributeoperator, 1);
    buffer.writeCompressed(1);
    buffer.write((char*) &DeltaWatermarkByte_, 1);
    {
      firstbyte = (((uint8_t) DeltaRecord_Edge) << DeltaRecordTypeBits)
                  + (((uint8_t) DeltaAction_Insert));
      buffer.write((char*) &firstbyte, 1);
      buffer.writeCompressed(0);
      buffer.writeCompressed(1);
      buffer.writeCompressed(1); //type
      attributeoperator = ((uint8_t) DeltaAttributeOperator_Add);
      buffer.write((char*) &attributeoperator, 1);
      buffer.writeCompressed(1);
      buffer.write((char*) &DeltaWatermarkByte_, 1);
    }
    for(VertexLocalId_t j = 0; j < i; ++j){
      firstbyte = (((uint8_t) DeltaRecord_Edge) << DeltaRecordTypeBits)
                  + (((uint8_t) DeltaAction_Insert));
      buffer.write((char*) &firstbyte, 1);
      buffer.writeCompressed(j);
      buffer.writeCompressed(i);
      buffer.writeCompressed(1); //type
      attributeoperator = ((uint8_t) DeltaAttributeOperator_Add);
      buffer.write((char*) &attributeoperator, 1);
      buffer.writeCompressed(1);
      buffer.write((char*) &DeltaWatermarkByte_, 1);
    }
  }
  TransactionId_t tid =  (i+1)*10;
  uint32_t binarysize = buffer.bufferpos_ - buffer.buffer_;
  size_t finalbinarysize = sizeof(TransactionId_t) + sizeof(binarysize) + binarysize;
  uint8_t* finalbinary = new uint8_t[finalbinarysize];
  uint8_t* tmpptr = finalbinary;
  memcpy(tmpptr, &tid, sizeof(tid));
  tmpptr += sizeof(tid);
  memcpy(tmpptr, &binarysize, sizeof(binarysize));
  tmpptr += sizeof(binarysize);
  memcpy(tmpptr, buffer.buffer_, binarysize);
  topology.GetDeltaRecords()->ReadDeltas(finalbinary, finalbinarysize, 0, 0);
  delete[] finalbinary;
  if(print){
    topology4::TopologyPrinter printer(&instance, &topology);
    std::cout << "\n\nPrint for " << i << " \n";
    std::cout << "Nothing to print \n";
    printer.PrintVertexAttributes(true, -1, 0);
    printer.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1, 0);
    std::cout << "Print half \n";
    printer.PrintVertexAttributes(true, -1, (i + 1) * 5);
    printer.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1, (i + 1) * 5);
    std::cout << "Print everything \n";
    printer.PrintVertexAttributes(true);
    printer.PrintEdges(EdgeBlockReaderSetting(true, true, true));
  }
  if(idcheck){
    topology4::QueryState query_state((i + 1) * 5);
    topology.GetCurrentSegementMeta(query_state.query_segments_meta_);
    if(checkid){
      for(VertexLocalId_t j = 0; j <= i ; ++j)
        EXPECT_EQ(topology.IsValidVertexId(j, query_state), j < (i + 1) / 2);
    }
    query_state.tid_ = 10000000;
    for(VertexLocalId_t j = 0; j <= i ; ++j)
      EXPECT_EQ(topology.IsValidVertexId(j, query_state), true);
  }
  timer.Stop("InsertOneDelta " + boost::lexical_cast<std::string>(i) + " Finished");
}

void MakeEmptyPartition(std::string topologypath, VertexLocalId_t segmentsizeinpowerof2){
  topology4::TopologyMeta meta(topologypath);
  meta.edgestoretype_ = EdgeStoreType_GroupByType;
  meta.segmentsizeinpowerof2_ = segmentsizeinpowerof2;
  {
    topology4::VertexTypeMeta vertextypemeta;
    vertextypemeta.typeid_ = 0;
    vertextypemeta.typename_ = "vertextype";
    meta.vertextypemeta_.push_back(vertextypemeta);
    topology4::AttributeMeta attrmeta1;
    attrmeta1.type_ = topology4::Attribute::UINT_TYPE;
    attrmeta1.name_ = "int1";
    attrmeta1.isnullable_ = false;
    meta.vertextypemeta_[0].attributes_.attributelist_.push_back(attrmeta1);
  }
  {
    topology4::EdgeTypeMeta edgetypemeta;
    edgetypemeta.typeid_ = 0;
    edgetypemeta.typename_ = "edgetype1";
    edgetypemeta.isdirected_ = false;
    meta.edgetypemeta_.push_back(edgetypemeta);
    topology4::AttributeMeta attrmeta2;
    attrmeta2.type_ = topology4::Attribute::UINT_TYPE;
    attrmeta2.name_ = "int2";
    attrmeta2.isnullable_ = false;
    meta.edgetypemeta_[0].attributes_.attributelist_.push_back(attrmeta2);
  }
  {
    topology4::EdgeTypeMeta edgetypemeta;
    edgetypemeta.typeid_ = 1;
    edgetypemeta.typename_ = "edgetype2";
    edgetypemeta.isdirected_ = true;
    meta.edgetypemeta_.push_back(edgetypemeta);
    topology4::AttributeMeta attrmeta2;
    attrmeta2.type_ = topology4::Attribute::UINT_TYPE;
    attrmeta2.name_ = "int3";
    attrmeta2.isnullable_ = false;
    meta.edgetypemeta_[1].attributes_.attributelist_.push_back(attrmeta2);
  }
  meta.WriteTo(topologypath);
}

TEST(GPE4DELTATEST, Topology_Small) {
  gmmnt::GlobalInstances instance(GetConfigFile1());
  std::string topologypath = "/tmp/deltatopology/";
  gutil::GDiskUtil::CreateEmptyFolder(topologypath);
  MakeEmptyPartition(topologypath, 2);
  {
    topology4::TopologyGraph topology(&instance, topologypath,true, true);
    std::cout << *topology.GetTopologyMeta() << "\n";
    {
      for(VertexLocalId_t i = 0; i < 6; ++i){
        InsertOneDelta(instance, topology, i, true);
      }
      topology4::QueryState query_state(61);
      topology.GetCurrentSegementMeta(query_state.query_segments_meta_);
      DeltaRebuilder rebuilder1(&instance, &topology, NULL);
      rebuilder1.Rebuild("/tmp/deltatopology_half/", &query_state);
    }
    {
      for(VertexLocalId_t i = 6; i < 12; ++i){
        InsertOneDelta(instance, topology, i, true);
      }
      topology4::QueryState query_state(121);
      topology.GetCurrentSegementMeta(query_state.query_segments_meta_);
      DeltaRebuilder rebuilder1(&instance, &topology, NULL);
      rebuilder1.Rebuild("/tmp/deltatopology_full/", &query_state);
    }
  }
  {
    topology4::TopologyGraph topology2(&instance, "/tmp/deltatopology_half/",true, true);
    // std::cout << "Print /tmp/deltatopology_half/\n";
    // topology4::TopologyPrinter printer(&instance, &topology2);
    // printer.PrintVertexAttributes(true, -1);
    // printer.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1);
    // return;
    for(VertexLocalId_t i = 6; i < 12; ++i){
      InsertOneDelta(instance, topology2, i, false);
    }
    topology4::QueryState query_state(121);
    topology2.GetCurrentSegementMeta(query_state.query_segments_meta_);
    DeltaRebuilder rebuilder3(&instance, &topology2, NULL);
    rebuilder3.Rebuild("/tmp/deltatopology_full2/", &query_state);
    //std::string cmd = "diff  -r /tmp/deltatopology_full /tmp/deltatopology_full2";
    //EXPECT_EQ(system(cmd.c_str()), 0);
  }
  {
    // topology4::TopologyGraph topology3(&instance, "/tmp/deltatopology_full2/",true, true);
    // std::cout << "Print /tmp/deltatopology_full2/\n";
    // topology4::TopologyPrinter printer2(&instance, &topology3);
    // printer2.PrintVertexAttributes(true, -1);
    // printer2.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1);
  }
  {
    topology4::TopologyGraph topology4(&instance, "/tmp/deltatopology_half/",true, true);
    MPIIDImpl mpiidimpl;
    DeltaRebuilder rebuilder4(&instance, &topology4, &mpiidimpl);
    // std::cout << "Print /tmp/deltatopology_half/\n";
    // TopologyPrinter printer(&instance, &topology4);
    // printer.PrintVertexAttributes(true, -1);
    // printer.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1);
    rebuilder4.StartRebuildThread();
    rebuilder4.rebuildsetting_.print_debug_msg_ = false;
    rebuilder4.rebuildsetting_.sleep_no_job_ = 1;
    for(VertexLocalId_t i = 0; i < 12; ++i){
      InsertOneDelta(instance, topology4, i, false, false);
      mpiidimpl.tid_ = (i + 1)* 10 + 1;
      usleep(100000);
      // TopologyPrinter printer(&instance, &topology4);
      // printer.PrintVertexAttributes(true, -1);
      // printer.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1);
    }
  }
  {
    topology4::TopologyGraph topology2(&instance, "/tmp/deltatopology_half/",true, true);
    std::cout << "Print /tmp/deltatopology_half/\n";
    topology4::TopologyPrinter printer(&instance, &topology2);
    printer.PrintVertexAttributes(true, -1);
    printer.PrintEdges(EdgeBlockReaderSetting(true, true, true), -1);
  }
  {
    // std::string cmd = "diff  -r /tmp/deltatopology_full /tmp/deltatopology_half";
    // EXPECT_EQ(system(cmd.c_str()), 0);
  }
}


TEST(GPE4DELTATEST, Topology_Large) {
  gmmnt::GlobalInstances instance(GetConfigFile1());
  size_t size = 1920;
  std::string topologypath = "/tmp/deltatopology_large/";
  gutil::GDiskUtil::CreateEmptyFolder(topologypath);
  MakeEmptyPartition(topologypath, 7);
  {
    topology4::TopologyGraph topology(&instance, topologypath,true, true);
    {
      for(VertexLocalId_t i = 0; i < size / 2; ++i){
        InsertOneDelta(instance, topology, i, false);
      }
      topology4::QueryState query_state(size * 5);
      topology.GetCurrentSegementMeta(query_state.query_segments_meta_);
      DeltaRebuilder rebuilder1(&instance, &topology, NULL);
      rebuilder1.Rebuild("/tmp/deltatopology_large_half/", &query_state);
    }
    {
      for(VertexLocalId_t i = size / 2; i < size; ++i){
        InsertOneDelta(instance, topology, i, false);
      }
      topology4::QueryState query_state(size * 10);
      topology.GetCurrentSegementMeta(query_state.query_segments_meta_);
      DeltaRebuilder rebuilder2(&instance, &topology, NULL);
      rebuilder2.Rebuild("/tmp/deltatopology_large_full/", &query_state);
    }
  }
  {
    topology4::TopologyGraph topology4(&instance, "/tmp/deltatopology_large_half/",true, true);
    MPIIDImpl mpiidimpl;
    DeltaRebuilder rebuilder4(&instance, &topology4, &mpiidimpl);
    rebuilder4.StartRebuildThread();
    rebuilder4.rebuildsetting_.print_debug_msg_ = false;
    rebuilder4.rebuildsetting_.sleep_no_job_ = 1;
    for(VertexLocalId_t i = size / 2 - 10; i < size; ++i){
      InsertOneDelta(instance, topology4, i, false, false, false);
      mpiidimpl.tid_ = (i + 1)* 10;
    }
    sleep(3); // wait until all rebuild done
  }
  {
    //std::string cmd = "diff  -r /tmp/deltatopology_large_half /tmp/deltatopology_large_full";
    //EXPECT_EQ(system(cmd.c_str()), 0);
  }
  {
    std::cout << "Delete Edge Sample: delete 1918 to 1919 \n";
    topology4::TopologyGraph topology5(&instance, "/tmp/deltatopology_large_full/",true, true);
    topology4::TopologyPrinter printer(&instance, &topology5);
    printer.PrintOneVertexEdges(1918);
    gutil::GCharBuffer buffer;
    uint8_t firstbyte = (((uint8_t) DeltaRecord_Edge)
                         << DeltaRecordTypeBits) + (((uint8_t) DeltaAction_Delete));
    buffer.write((char*) &firstbyte, 1);
    buffer.writeCompressed(1918);
    buffer.writeCompressed(1919);
    buffer.writeCompressed(0); //type
    buffer.write((char*) &DeltaWatermarkByte_, 1);
    TransactionId_t tid =  100000;
    uint32_t binarysize = buffer.bufferpos_ - buffer.buffer_;
    size_t finalbinarysize = sizeof(TransactionId_t) + sizeof(binarysize) + binarysize;
    uint8_t* finalbinary = new uint8_t[finalbinarysize];
    uint8_t* tmpptr = finalbinary;
    memcpy(tmpptr, &tid, sizeof(tid));
    tmpptr += sizeof(tid);
    memcpy(tmpptr, &binarysize, sizeof(binarysize));
    tmpptr += sizeof(binarysize);
    memcpy(tmpptr, buffer.buffer_, binarysize);
    topology5.GetDeltaRecords()->ReadDeltas(finalbinary, finalbinarysize, 0, 0);
    std::cout  << *topology5.GetDeltaRecords();
    delete[] finalbinary;
    printer.PrintOneVertexEdges(1918);
  }
  {
    std::cout << "Delete Vertex Sample: delete 1919\n";
    topology4::TopologyGraph topology5(&instance, "/tmp/deltatopology_large_full/",true, true);
    topology4::TopologyPrinter printer(&instance, &topology5);
    printer.PrintOneVertexEdges(1918);
    gutil::GCharBuffer buffer;
    uint8_t firstbyte = (((uint8_t) DeltaRecord_Vertex)
                         << DeltaRecordTypeBits) + (((uint8_t) DeltaAction_Delete));
    buffer.write((char*) &firstbyte, 1);
    buffer.writeCompressed(1919);
    buffer.write((char*) &DeltaWatermarkByte_, 1);
    TransactionId_t tid =  100000;
    uint32_t binarysize = buffer.bufferpos_ - buffer.buffer_;
    size_t finalbinarysize = sizeof(TransactionId_t) + sizeof(binarysize) + binarysize;
    uint8_t* finalbinary = new uint8_t[finalbinarysize];
    uint8_t* tmpptr = finalbinary;
    memcpy(tmpptr, &tid, sizeof(tid));
    tmpptr += sizeof(tid);
    memcpy(tmpptr, &binarysize, sizeof(binarysize));
    tmpptr += sizeof(binarysize);
    memcpy(tmpptr, buffer.buffer_, binarysize);
    topology5.GetDeltaRecords()->ReadDeltas(finalbinary, finalbinarysize, 0, 0);
    std::cout  << *topology5.GetDeltaRecords();
    delete[] finalbinary;
    printer.PrintOneVertexEdges(1918);
    printer.PrintOneVertexEdges(1919);
  }
}


