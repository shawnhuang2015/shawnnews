/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * resource_test.cpp: /gperun/test/resource_test.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: lichen
 ******************************************************************************/

#include <topology4/testpartition.hpp>
#include <topology4/topologygraph.hpp>
#include <topology4/topologyprinter.hpp>
#include <gtest/gtest.h>
#include <gmmnt/globalinstances.hpp>
#include <gmmnt/globalmemoryserver.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>
#include <gapi4/graphapi.hpp>
#include <queue>
#include <vector>
#include "partitiondatafeeder_unittest.hpp"
#include "udfs_generic.hpp"

std::string output_gpe4_path_ = "/tmp/unittest/gpe4.2_udf/";
VertexLocalId_t num_gpe4_vertices_ = 10238;
uint32_t num_worker_ = 3;

std::string GetConfigFile() {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";
  return enginecfgfile;
}

TEST(GPE4UDFTEST, Partition_UD) {
  unittest_gpe4::TestPartitionDataFeeder_UD feeder(num_gpe4_vertices_);
  topology4::PartitionMatrix partition(&feeder,
                                       output_gpe4_path_ + "partition/ud/",
                                       10);
  partition.Run();
  partition.MakeDistributedCopy(output_gpe4_path_ + "partition/ud_dist/",
                                num_worker_);
  ASSERT_EQ(partition.num_edges(), 2 * (num_gpe4_vertices_ - 1));
  gmmnt::GlobalInstances instance(GetConfigFile());
  topology4::TopologyGraph topology(&instance, output_gpe4_path_ + "partition/ud/");
  topology4::TopologyPrinter topologyprinter(&instance, &topology);
  topologyprinter.PrintEdges(topology4::EdgeBlockReaderSetting(false, false, true));
}

TEST(BLUEAPITEST, NoFilter) {
  gmmnt::GlobalInstances instance(GetConfigFile());
  topology4::TopologyGraph topology(&instance,
                                    output_gpe4_path_ + "partition/ud/",
                                    true);
  std::string callerid = "GraphAPI"
                         + topology.GetTopologyMeta()->topologyfolder_;
  topology.LoadTopologyIntoMemory(true, callerid);
  gapi4::GraphAPI api(&topology);
  ASSERT_EQ(topology.GetTopologyMeta()->edgecount_, api.GetEdgeCount());
  ASSERT_EQ(topology.GetTopologyMeta()->vertexcount_,
            api.GetVertexCount());
  gapi4::VerticesCollection vertexresults;
  api.GetAllVertices(NULL, vertexresults);
  size_t totalcount = 0;
  while (vertexresults.NextVertex()) {
    totalcount++;
  }
  ASSERT_EQ(totalcount, api.GetVertexCount());
  for (size_t i = 0; i < totalcount; i += 10) {
    topology4::VertexAttribute v1;
    api.GetOneVertex(i, v1);
    ASSERT_EQ(v1.GetUInt(1, -1), i);
  }
  std::vector<VertexLocalId_t> vidlist;
  vidlist.push_back(totalcount - 1);
  vidlist.push_back(totalcount - 3);
  vidlist.push_back(totalcount - 2);
  vidlist.push_back(totalcount - 3);  // duplicate
  vidlist.push_back(totalcount + 1);  // not existed
  api.GetVertices(vidlist, NULL, vertexresults);
  totalcount = 0;
  while (vertexresults.NextVertex()) {
    totalcount++;
  }
  ASSERT_EQ(totalcount, 3u);
  // test on edge
  gapi4::EdgesCollection edgeresults;
  api.GetAllEdges(NULL, edgeresults);
  totalcount = 0;
  while (edgeresults.NextEdge()) {
    totalcount++;
  }
  ASSERT_EQ(totalcount, api.GetEdgeCount());
  api.GetOneVertexEdges(3, NULL, edgeresults);
  totalcount = 0;
  while (edgeresults.NextEdge()) {
    std::cout << edgeresults;
    totalcount++;
  }
  ASSERT_EQ(totalcount, 3u);
  vidlist.clear();
  vidlist.push_back(api.GetVertexCount() + 1);  // not existed
  vidlist.push_back(5);
  vidlist.push_back(3);
  vidlist.push_back(3);  // duplcaite one
  api.GetMultipleVerticesEdges(vidlist, NULL, edgeresults);
  totalcount = 0;
  while (edgeresults.NextEdge()) {
    std::cout << edgeresults;
    totalcount++;
  }
  ASSERT_EQ(totalcount, 6u);
  api.GetSpecifiedEdges(3, 1, edgeresults);
  totalcount = 0;
  while (edgeresults.NextEdge()) {
    std::cout << edgeresults;
    totalcount++;
  }
  ASSERT_EQ(totalcount, 1u);
  std::vector<std::pair<VertexLocalId_t, VertexLocalId_t> > idpairlist;
  idpairlist.push_back(std::make_pair(5, 2));
  idpairlist.push_back(std::make_pair(3, 1));
  idpairlist.push_back(std::make_pair(5, 2));  // duplciate
  idpairlist.push_back(std::make_pair(3, 100));  // not there
  api.GetMutipleSpecifiedEdges(idpairlist, edgeresults);
  totalcount = 0;
  while (edgeresults.NextEdge()) {
    std::cout << edgeresults;
    totalcount++;
  }
  ASSERT_EQ(totalcount, 2u);
  instance.memserver_->FreeCaller(callerid);
}

TEST(GPE4UDFTEST, Partition_D) {
  unittest_gpe4::TestPartitionDataFeeder_D feeder(num_gpe4_vertices_);
  topology4::PartitionMatrix partition(&feeder,
                                       output_gpe4_path_ + "partition/d/",
                                       10);
  partition.Run();
  partition.MakeDistributedCopy(output_gpe4_path_ + "partition/d_dist/",
                                num_worker_);
  ASSERT_EQ(partition.num_edges(), num_gpe4_vertices_ - 1);
}

TEST(GPE4UDFTEST, Partition_DenseD) {
  std::string path = output_gpe4_path_ + "partition/dense_d/";
  if (!boost::filesystem::exists(path + "config.yaml")) {// running too long.
    unittest_gpe4::TestPartitionDataFeeder_DenseD feeder;
    topology4::PartitionMatrix partition(&feeder, path, 13);
    partition.Run();
  }
  gmmnt::GlobalInstances instance(GetConfigFile());
  topology4::TopologyGraph topology(&instance, path);
  topology4::TopologyPrinter topologyprinter(&instance, &topology);
  topologyprinter.PrintOneVertexEdges(0);
}

TEST(GPE4UDFTEST, Partition_DenseD_TgtPart) {
  std::string path = output_gpe4_path_ + "partition/dense_d_tgtpart/";
  if (!boost::filesystem::exists(path + "config.yaml")){ // running too long.
    unittest_gpe4::TestPartitionDataFeeder_DenseD feeder;
    topology4::PartitionMatrix partition(&feeder, path, 13, 3, 1);
    partition.Run();
  }
  gmmnt::GlobalInstances instance(GetConfigFile());
  topology4::TopologyGraph topology(&instance, path);
  topology4::TopologyPrinter topologyprinter(&instance, &topology);
  topologyprinter.PrintOneVertexEdges(0);
}

TEST(GPE4UDFTEST, Partition_DenseD_EdgeTypes) {
  std::string path = output_gpe4_path_ + "partition/dense_d_edgetypes/";
  if (!boost::filesystem::exists(path + "config.yaml")){ // running too long.
    unittest_gpe4::TestPartitionDataFeeder_DenseD feeder;
    topology4::PartitionMatrix partition(&feeder, path, 13, 3, 0, true);
    partition.Run();
  }
  gmmnt::GlobalInstances instance(GetConfigFile());
  topology4::TopologyGraph topology(&instance, path);
  topology4::TopologyPrinter topologyprinter(&instance, &topology);
  topologyprinter.PrintOneVertexEdges(0);
}

void GPE4UDFTEST_UDF_DenseD(std::string  partitionpath, std::string output, bool sparse_mode,
                            bool edgedata_inmemory) {
  std::cout << "\n\n\nGPE4UDFTEST_UDF_DenseD sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  typedef unittest_gpe4::UDF_DenseD UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               partitionpath,
                               sparse_mode, edgedata_inmemory);
  driver.topology()->WarmUp(true);
  driver.RunSingleVersion<UDF_t>(&udf, output);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);
}

void GPE4UDFTEST_UDF_DenseD_TypeFilter(std::string  partitionpath,
                                       bool edgedata_inmemory) {
  std::cout << "GPE4UDFTEST_UDF_DenseD_TypeFilter edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  typedef unittest_gpe4::UDF_DenseD_TypeFilter UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               partitionpath,
                               false, edgedata_inmemory);
  driver.topology()->WarmUp(true);
  driver.RunSingleVersion<UDF_t>(&udf, "");
}

TEST(GPE4UDFTEST, UDF_DenseD) {
  std::string output = output_gpe4_path_ + "denseD";
  boost::filesystem::remove_all(output + "_bak");
  std::string partitionpath = output_gpe4_path_ + "partition/dense_d/";
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, true, true);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, true, false);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, false, true);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, false, false);
  partitionpath = output_gpe4_path_ + "partition/dense_d_tgtpart/";
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, true, true);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, true, false);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, false, true);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, false, false);
  partitionpath = output_gpe4_path_ + "partition/dense_d_edgetypes/";
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, true, true);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, true, false);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, false, true);
  GPE4UDFTEST_UDF_DenseD(partitionpath, output, false, false);
}

TEST(GPE4UDFTEST, UDF_DenseD_TypeFilter) {
  std::string partitionpath = output_gpe4_path_ + "partition/dense_d_edgetypes/";
  GPE4UDFTEST_UDF_DenseD_TypeFilter(partitionpath, true);
  GPE4UDFTEST_UDF_DenseD_TypeFilter(partitionpath, false);
}

void GPE4UDFTEST_UDF_UD(std::string output, bool sparse_mode,
                        bool edgedata_inmemory) {
  std::cout << "\n\n\nGPE4UDFTEST_UDF_UD sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  typedef unittest_gpe4::UDF_UD UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/ud/",
                               sparse_mode, edgedata_inmemory);
  driver.RunSingleVersion<UDF_t>(&udf, output);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);
}

TEST(GPE4UDFTEST, UDF_UD) {
  std::string output = output_gpe4_path_ + "uD";
  boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_UDF_UD(output, true, true);
  GPE4UDFTEST_UDF_UD(output, true, false);
  GPE4UDFTEST_UDF_UD(output, false, true);
  GPE4UDFTEST_UDF_UD(output, false, false);
}

void GPE4UDFTEST_DIST_UDF_UD(std::string output, bool sparse_mode,
                             bool edgedata_inmemory,
                             bool needtargetvalue) {
  /*std::cout << "\n\n\nGPE4UDFTEST_DIST_UDF_UD sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory
            << " needtargetvalue " << needtargetvalue << "\n\n\n";
  typedef unittest_gpe4::UDF_UD UDF_t;
  UDF_t udf;
  udf.targetvaluecheck_ = needtargetvalue;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/ud_dist/",
                               sparse_mode, edgedata_inmemory);
  driver.RunDistributedVersion<UDF_t>(&udf, output, needtargetvalue);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);*/
}

TEST(GPE4UDFTEST, DIST_UDF_UD) {
  std::string output = output_gpe4_path_ + "uD";
  // boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_DIST_UDF_UD(output, true, true, true);
  GPE4UDFTEST_DIST_UDF_UD(output, true, false, true);
  GPE4UDFTEST_DIST_UDF_UD(output, false, true, true);
  GPE4UDFTEST_DIST_UDF_UD(output, false, false, true);
  GPE4UDFTEST_DIST_UDF_UD(output, true, true, false);
  GPE4UDFTEST_DIST_UDF_UD(output, true, false, false);
  GPE4UDFTEST_DIST_UDF_UD(output, false, true, false);
  GPE4UDFTEST_DIST_UDF_UD(output, false, false, false);
}

void GPE4UDFTEST_UDF_D(std::string output, bool sparse_mode,
                       bool edgedata_inmemory) {
  std::cout << "\n\n\nGPE4UDFTEST_UDF_D sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  typedef unittest_gpe4::UDF_D UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/d/",
                               sparse_mode, edgedata_inmemory);
  driver.RunSingleVersion<UDF_t>(&udf, output);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);
}

TEST(GPE4UDFTEST, UDF_D) {
  std::string output = output_gpe4_path_ + "D";
  boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_UDF_D(output, true, true);
  GPE4UDFTEST_UDF_D(output, true, false);
  GPE4UDFTEST_UDF_D(output, false, true);
  GPE4UDFTEST_UDF_D(output, false, false);
}

void GPE4UDFTEST_DIST_UDF_D(std::string output, bool sparse_mode,
                            bool edgedata_inmemory) {
  /*std::cout << "\n\n\nGPE4UDFTEST_DIST_UDF_D sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  typedef unittest_gpe4::UDF_D UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/d_dist/",
                               sparse_mode, edgedata_inmemory);
  driver.RunDistributedVersion<UDF_t>(&udf, output, false);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);*/
}

TEST(GPE4UDFTEST, DIST_UDF_D) {
  std::string output = output_gpe4_path_ + "D";
  // boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_DIST_UDF_D(output, true, true);
  GPE4UDFTEST_DIST_UDF_D(output, true, false);
  GPE4UDFTEST_DIST_UDF_D(output, false, true);
  GPE4UDFTEST_DIST_UDF_D(output, false, false);
}

void GPE4UDFTEST_UDF_Abort(bool sparse_mode, bool edgedata_inmemory) {
  std::cout << "\n\n\nGPE4UDFTEST_UDF_Abort sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/d/",
                               sparse_mode, edgedata_inmemory);
  typedef unittest_gpe4::UDF_Abort UDF_t;
  for (int i = 0; i < 7; ++i) {
    UDF_t udf((unittest_gpe4::AbortLocation) i);
    driver.RunSingleVersion<UDF_t>(
          &udf,
          output_gpe4_path_ + "abort_" + boost::lexical_cast<std::string>(i)
          + ".txt");
  }
}

TEST(GPE4UDFTEST, UDF_Abort) {
  GPE4UDFTEST_UDF_Abort(true, true);
  GPE4UDFTEST_UDF_Abort(true, false);
  GPE4UDFTEST_UDF_Abort(false, true);
  GPE4UDFTEST_UDF_Abort(false, false);
}

void GPE4UDFTEST_DIST_UDF_Abort(bool sparse_mode, bool edgedata_inmemory) {
  /*std::cout << "\n\n\nGPE4UDFTEST_DIST_UDF_Abort sparse_mode "
            << sparse_mode << " edgedata_inmemory " << edgedata_inmemory
            << "\n\n\n";
  typedef unittest_gpe4::UDF_Abort UDF_t;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/d_dist/",
                               sparse_mode, edgedata_inmemory);
  for (int i = 0; i < 7; ++i) {
    UDF_t udf((unittest_gpe4::AbortLocation) i);
    driver.RunDistributedVersion<UDF_t>(
          &udf,
          output_gpe4_path_ + "abort_dist_"
          + boost::lexical_cast<std::string>(i) + ".txt",
          true);
  }*/
}

TEST(GPE4UDFTEST, DIST_UDF_Abort) {
  GPE4UDFTEST_DIST_UDF_Abort(true, true);
  GPE4UDFTEST_DIST_UDF_Abort(true, false);
  GPE4UDFTEST_DIST_UDF_Abort(false, true);
  GPE4UDFTEST_DIST_UDF_Abort(false, false);
}

void GPE4UDFTEST_UDF_MixValue(std::string output, bool sparse_mode,
                              bool edgedata_inmemory) {
  std::cout << "\n\n\nGPE4UDFTEST_UDF_MixValue sparse_mode " << sparse_mode
            << " edgedata_inmemory " << edgedata_inmemory << "\n\n\n";
  typedef unittest_gpe4::UDF_MixValue UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/ud/",
                               sparse_mode, edgedata_inmemory);
  driver.RunSingleVersion<UDF_t>(&udf, output);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);
}

TEST(GPE4UDFTEST, UDF_MixValue) {
  std::string output = output_gpe4_path_ + "mixvalue";
  boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_UDF_MixValue(output, true, true);
  GPE4UDFTEST_UDF_MixValue(output, true, false);
  GPE4UDFTEST_UDF_MixValue(output, false, true);
  GPE4UDFTEST_UDF_MixValue(output, false, false);
}

void GPE4UDFTEST_UDF_JumpSegments(std::string output, bool sparse_mode,
                                  bool edgedata_inmemory) {
  std::cout << "\n\n\nGPE4UDFTEST_UDF_JumpSegments sparse_mode "
            << sparse_mode << " edgedata_inmemory " << edgedata_inmemory
            << "\n\n\n";
  typedef unittest_gpe4::UDF_JumpSegments UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/ud/",
                               sparse_mode, edgedata_inmemory);
  driver.topology()->WarmUp(true);
  driver.RunSingleVersion<UDF_t>(&udf, output);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);
}

TEST(GPE4UDFTEST, UDF_JumpSegments) {
  std::string output = output_gpe4_path_ + "jumpsegments";
  boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_UDF_JumpSegments(output, true, true);
  GPE4UDFTEST_UDF_JumpSegments(output, true, false);
  GPE4UDFTEST_UDF_JumpSegments(output, false, true);
  GPE4UDFTEST_UDF_JumpSegments(output, false, false);
}

void GPE4UDFTEST_DIST_UDF_JumpSegments(std::string output,
                                       bool sparse_mode,
                                       bool edgedata_inmemory,
                                       bool needtargetvalue) {
  /*std::cout << "\n\n\nGPE4UDFTEST_DIST_UDF_JumpSegments sparse_mode "
            << sparse_mode << " edgedata_inmemory " << edgedata_inmemory
            << " needtargetvalue " << needtargetvalue << "\n\n\n";
  typedef unittest_gpe4::UDF_JumpSegments UDF_t;
  UDF_t udf;
  gpelib4::EngineDriver driver(GetConfigFile(),
                               output_gpe4_path_ + "partition/ud_dist/",
                               sparse_mode, edgedata_inmemory);
  driver.RunDistributedVersion<UDF_t>(&udf, output, needtargetvalue);
  GASSERT(gutil::GDiskUtil::CompareAndBackup(output), output);*/
}

TEST(GPE4UDFTEST, DIST_UDF_JumpSegments) {
  std::string output = output_gpe4_path_ + "jumpsegments";
  // boost::filesystem::remove_all(output + "_bak");
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, true, true, true);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, true, false, true);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, false, true, true);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, false, false, true);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, true, true, false);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, true, false, false);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, false, true, false);
  GPE4UDFTEST_DIST_UDF_JumpSegments(output, false, false, false);
}
