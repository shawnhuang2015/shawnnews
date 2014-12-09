/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GPE
 * resource_test.cpp: /gperun/test/resource_test.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: lichen
 ******************************************************************************/

#include <partition/partitionmatrix.hpp>
#include <engine_matrix/enginedriver.hpp>
#include <engine_matrix/enginedriverservice.hpp>
#include <topologydelta/topologyrebuilder.hpp>
#include <gtest/gtest.h>
#include <queue>
#include <vector>
#include "partitiondatafeeder_unittest.hpp"
#include "udfs_generic.hpp"
#include "udfs_job.hpp"
#include "udfs_delta.hpp"

std::string output_path_ = "/tmp/unittest/udf/";
VertexLocalId_t num_vertices_ = 10238;

TEST(UDFTEST, Partition_UD) {
  unittest::TestPartitionDataFeeder_UD feeder(num_vertices_);
  gpelib::PartitionMatrix partition(&feeder,
                                    output_path_ + "partition/ud/", 8, 8,
                                    gpelib::Degree_Type_None);
  partition.Run();
  ASSERT_EQ(partition.num_edges(), 2 * (num_vertices_ - 1));
}

TEST(UDFTEST, Partition_D) {
  unittest::TestPartitionDataFeeder_D feeder(num_vertices_);
  gpelib::PartitionMatrix partition(&feeder, output_path_ + "partition/d/",
                                    8, 8);
  partition.Run();
  ASSERT_EQ(partition.num_edges(), num_vertices_ - 1);
}

TEST(UDFTEST, UDF_UD) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  typedef gpelib::VertexAttribute V_ATTR;
  typedef gpelib::EdgeAttribute_Writable E_ATTR;
  typedef unittest::UDF_UD<size_t, V_ATTR, E_ATTR> UDF_t;
  UDF_t udf;
  gpelib::EngineDriver<UDF_t> driver(enginecfgfile,
                                     output_path_ + "partition/ud/", &udf);
  driver.run(&udf, output_path_ + "ud.txt");
}

TEST(UDFTEST, UDF_D) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  typedef gpelib::VertexAttribute V_ATTR;
  typedef gpelib::EdgeAttribute_Writable E_ATTR;
  typedef unittest::UDF_D<char*, V_ATTR, E_ATTR> UDF_D_t;
  UDF_D_t udf;
  gpelib::EngineDriver<UDF_D_t> driver(enginecfgfile,
                                       output_path_ + "partition/d/",
                                       &udf);
  driver.run(&udf, output_path_ + "d.txt");
}

TEST(UDFTEST, UDF_Abort) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  typedef gpelib::VertexAttribute V_ATTR;
  typedef gpelib::EdgeAttribute_Writable E_ATTR;
  typedef unittest::UDF_Abort<char*, V_ATTR, E_ATTR> UDF_D_t;
  for (int i = 0; i < 7; ++i) {
    UDF_D_t udf((unittest::AbortLocation) i);
    gpelib::EngineDriver<UDF_D_t> driver(enginecfgfile,
                                         output_path_ + "partition/d/",
                                         &udf);
    driver.run(&udf, output_path_ + "abort.txt");
  }
}

TEST(UDFTEST, UDF_MixValue) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  typedef gpelib::VertexAttribute V_ATTR;
  typedef gpelib::EdgeAttribute_Writable E_ATTR;
  typedef unittest::UDF_MixValue<size_t, V_ATTR, E_ATTR> UDF_D_t;
  UDF_D_t udf;
  gpelib::EngineDriver<UDF_D_t> driver(enginecfgfile,
                                       output_path_ + "partition/ud/",
                                       &udf);
  driver.run(&udf, output_path_ + "mixvalue.txt");
}

TEST(UDFTEST, UDF_Composite) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  gmmnt::GlobalInstances* globalinstance = new gmmnt::GlobalInstances(
      enginecfgfile);
  gpelib::EngineSetting* enginesetting = new gpelib::EngineSetting(
      enginecfgfile);
  gpelib::GPEGraphRegistry* graphregistry = new gpelib::GPEGraphRegistry(
      globalinstance);
  {
    unittest::UnitTestComposite composite(globalinstance, graphregistry,
                                          enginesetting,
                                          output_path_ + "partition/ud/",
                                          output_path_ + "composite1.txt");
    composite.Run();
  }
  /*{
   unittest::UnitTestComposite composite(globalinstance, graphregistry,
   enginesetting,
   output_path_ + "partition/ud/",
   output_path_ + "composite2.txt");
   composite.Run_JobNoReuse();
   }*/
  delete graphregistry;
  delete enginesetting;
  delete globalinstance;
}

TEST(UDFTEST, UDF_Delta) {
  char path[1000];
  getcwd(path, 1000);
  std::string enginecfgfile = path;
  enginecfgfile += "/enginecfg.yaml";

  gmmnt::GlobalInstances* globalinstance_ = new gmmnt::GlobalInstances(
      enginecfgfile);
  gpelib::EngineSetting* enginesetting_ = new gpelib::EngineSetting(
      enginecfgfile);
  typedef unittest::UDF_Delta<char*, gpelib::VertexAttribute,
      gpelib::EdgeAttribute> UDF_t;
  typedef UDF_t::VID VID;
  typedef UDF_t::V_ATTR V_ATTR;
  typedef UDF_t::E_ATTR E_ATTR;
  typedef UDF_t::V_VALUE V_VALUE;
  typedef UDF_t::MESSAGE MESSAGE;
  if (true) {  // test delta
    unittest::TestPartitionDataFeeder_Delta feeder(num_vertices_);
    gpelib::PartitionMatrix partition(&feeder,
                                      output_path_ + "partition/delta/", 8,
                                      8, gpelib::Degree_Type_Outgoing);
    partition.Run();
    ASSERT_EQ(partition.num_edges(), 2 * (num_vertices_ - 1));
    boost::shared_ptr<gpelib::TopologyDeltaRetriever> deltaretriever(
        new unittest::TestTopologyDeltaRetriever(num_vertices_));
    gpelib::TopologyManagement* topology_ = new gpelib::TopologyManagement(
        globalinstance_->memallocator_, globalinstance_->diskio_,
        output_path_ + "partition/delta/", deltaretriever);
    for (size_t i = 0; i < 6; i++) {
      topology_->PullDelta();
      gpelib::GraphProcessing* graphprocessing_ =
          new gpelib::GraphProcessing(topology_,
                                      globalinstance_->memserver_,
                                      globalinstance_->membuffer_,
                                      globalinstance_->diskio_,
                                      enginesetting_);
      UDF_t udf(i + 1, num_vertices_, partition.num_edges());
      graphprocessing_
          ->Init<UDF_t, V_ATTR, E_ATTR, V_VALUE, MESSAGE,
              gpelib::Mode_MultipleValue, gpelib::Mode_NotDefined,
              gpelib::Mode_Defined & gpelib::Mode_MultipleValue,
              gpelib::Mode_NotDefined>(&udf);
      graphprocessing_
          ->Run<V_VALUE, MESSAGE, gpelib::Mode_MultipleValue,
              gpelib::Mode_Defined, UDF_t, V_ATTR>(&udf);
      graphprocessing_
          ->Print<UDF_t, V_ATTR, E_ATTR, V_VALUE, VID,
              gpelib::Mode_Defined, gpelib::Mode_MultipleValue>(
          output_path_ + "delta.txt");
      delete graphprocessing_;
    }
    delete topology_;
  }
  if (false) {  // test rebuild
    unittest::TestPartitionDataFeeder_Delta feeder(num_vertices_);
    gpelib::PartitionMatrix partition(&feeder,
                                      output_path_ + "partition/delta/", 8,
                                      8, gpelib::Degree_Type_Outgoing);
    partition.Run();
    ASSERT_EQ(partition.num_edges(), 2 * (num_vertices_ - 1));
    boost::shared_ptr<gpelib::TopologyDeltaRetriever> deltaretriever(
        new unittest::TestTopologyDeltaRetriever(num_vertices_));
    gpelib::TopologyManagement* topology_ = new gpelib::TopologyManagement(
        globalinstance_->memallocator_, globalinstance_->diskio_,
        output_path_ + "partition/delta/", deltaretriever);
    for (size_t i = 1; i <= 6; i++) {
      {
        gpelib::RebuildDataFeeder rebuilder(topology_, globalinstance_);
        rebuilder.Run();
        VertexLocalId_t vertexcount = topology_->countofvertex();
        delete topology_;
        topology_ = new gpelib::TopologyManagement(
            globalinstance_->memallocator_, globalinstance_->diskio_,
            output_path_ + "partition/delta/", deltaretriever);
      }
      if (true) {
        gpelib::GraphProcessing* graphprocessing_ =
            new gpelib::GraphProcessing(topology_,
                                        globalinstance_->memserver_,
                                        globalinstance_->membuffer_,
                                        globalinstance_->diskio_,
                                        enginesetting_);
        UDF_t udf(i, num_vertices_, partition.num_edges());
        graphprocessing_
            ->Init<UDF_t, V_ATTR, E_ATTR, V_VALUE, MESSAGE,
                gpelib::Mode_MultipleValue, gpelib::Mode_NotDefined,
                gpelib::Mode_Defined & gpelib::Mode_MultipleValue,
                gpelib::Mode_NotDefined>(&udf);
        graphprocessing_
            ->Run<V_VALUE, MESSAGE, gpelib::Mode_MultipleValue,
                gpelib::Mode_Defined, UDF_t, V_ATTR>(&udf);
        graphprocessing_
            ->Print<UDF_t, V_ATTR, E_ATTR, V_VALUE, VID,
                gpelib::Mode_Defined, gpelib::Mode_MultipleValue>(
            output_path_ + "delta_rebuild.txt");
        delete graphprocessing_;
      }
    }
    delete topology_;
  }
  delete enginesetting_;
  delete globalinstance_;
}
