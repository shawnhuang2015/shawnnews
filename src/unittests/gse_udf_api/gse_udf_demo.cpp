/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-CSE
 * gse_udf_demo.cpp: /gdbms/src/graphsql/server/gse_udf_api/gse_udf_demo.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: like
 ******************************************************************************/

#include <gse_udf_api/connection.hpp>
#include <gse_udf_api/graph.hpp>
#include <gse_udf_api/graphyaml.hpp>
#include <iostream>

/**
 * This demo creates a graph "Graph1", and grapv view "GV1" on it and
 * then partitioning
 * The current GSE_UDF API actually just has 2 classes:
 * - Connection: to invoke any GSQL statement.
 * - Graph: single wrapper of Graph, Graphview and partition. Mainly
 *          used for loading graph, and get partitioned data.
 * What is missing in the API is to return various objects from GSQL statement.
 * I defer these to later CLI so to standardize all these objects. These objects
 * can be complicated or very large -- think of ResultSet in JDBC...
 *
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char ** argv) {
  /* below checking can be REMOVED ->> */
#ifdef BUILDVERSION
  std::cout << "gse_udf_demo: Storage Engine Version: " <<
  MAKE_BOLD << BUILDVERSION << RESET_BOLD << std::endl;
#endif
  Gsql_Fd_Checker gsql_fd_checker(65532);
  if (!gsql_fd_checker.passed_)
    exit(-1);
  /* validate the graph store path */
  if (argc < 2) {
    printf("USAGE: %s <store_path>\n ", argv[0]);
    exit(1);
  }
  /* <<--- end of CAN BE REMOVED */

  /* Step 1. make the connection: need the store path which can be a relative one */
  gse_udf::Connection connection(argv[1]);

  if (!connection.getStatus()) {
    /* connection failure, store doesn't exist */
    std::cout << connection.getError() << std::endl;
    exit(-1);
  }

  

  /* 4. Loading data. */
  /* 4.1 setup the graph config which will convert to loading yaml */

  /* Below four file names will be in "Load graph ..." statement */
  /* You can give any filename to yaml. The yaml file(s) will be
   * deleted in demoGraph destuctor
   */
  std::string inputVertexFile = "/tmp/nodes.csv";   // if "", won't load
  std::string inputEdgeFile = "/tmp/edges.csv";     // if "", won't load
  std::string inputVertexYaml = "/tmp/nodes.yaml";
  std::string inputEdgeYaml = "/tmp/edges.yaml";

  /* 4.1 These are the settings. */
  gse_udf::GraphYamlMaker vertYamlMaker(inputVertexYaml,  //YAML file name
                                        inputVertexFile,  //input data file name
                                        "vertex_with_properties", // source format
                                        ',',   // separator
                                        5,     // number of vertices in millions
                                        4,     // number of bucket in graph
                                        true,  // allow duplicate?
                                        true,  // directed graph?
                                        false);  // false: vertex type is long/int, true: is string
  
  gse_udf::GraphYamlMaker edgeYamlMaker(inputEdgeYaml,  //YAML file name
                                        inputEdgeFile,  //input data file name
                                        "edge_only", //source format
                                        ',',   // separator
                                        5,     // number of vertices in millions
                                        4,     // number of bucket in graph
                                        true,  // allow duplicate?
                                        true,  // directed graph?
                                        false);  // false: vertex type is long/int, true: is string


  /* set attributes. Assume vertex has 2 attributes and edge doesn't have any */
  vertYamlMaker.addVertexAttributes("name", "CHARARRAY");
  vertYamlMaker.addVertexAttributes("age", "INT");
  
  vertYamlMaker.publish();
  edgeYamlMaker.publish(); 
  
  /* Note in above, we don't specify the file format (edge_only, edge_with_property).
   * Why? We know this by just seeing if any attributes are defined. */

  /* 2. Create a new graph.  The final parameter, when true, indicates the graph is new, 
   *    or if it already exists, should be deleted.  If set to false, then the data will 
   *    be added to the graph as in an update mode.  
   *    Note that if either YAML filename is blank, that part of the import process is skipped.  
   */
  bool success = connection.importGraphData("graph1",vertYamlMaker.filename_, edgeYamlMaker.filename_, true);
  
  if(success){
    gse_udf::Graph demoGraph = connection.loadGraphFromStore("graph1");
  }else{
    std::cout<<"error importing data into the store"<<std::endl; 
  }
  
  
  /* 5. We can also do the batch execution. E.g., create view and partition */
  std::vector<std::string> batchSQL;
  batchSQL.push_back(
      "create graphview gv1 from graph graph1 "
      "as ( node_attributes (none) edge_attributes (none) );");
  batchSQL.push_back(
      "partition graph gv1  -partNum 4 -partalg streaming;");
  connection.execute(batchSQL);

  /* 6. show info about the partition, with the graph view handler.
   * Note: you may see that I don't distinguish Graph and Graphview.
   *  The reason is that in GSE 2.0 we may create a materialized graphview
   *  just like a graph, e.g. in a single statement. So we don't
   *  have separate handle for graphview. The developer may know
   *  the graph or graphview in advance, or can check if a graphname
   *  refers to a graph or view by calling connection.isGraphView(). */

  gse_udf::Graph demoGV = connection.loadGraphFromStore("GV1");
  std::cout << "Partition - path   : " << demoGV.getPartitionPath()
            << std::endl;
  std::cout << "Partition - #edges : " << demoGV.getCountEdges()
            << std::endl;
  std::cout << "Partition - #nodes : " << demoGV.getCountVertices()
            << std::endl;
  std::cout << "Partition - #e_atts: " << demoGV.getNumberEdgeAttributes()
            << std::endl;
  std::cout << "Partition - #v_atts: "
            << demoGV.getNumberVertexAttributes() << std::endl;

  /* Do I need to close the connection? Not necessary. The close() will be
   * called in Connection's desctructor.
   * Also, the yaml file created by loading will also be removed at
   * Graph's desctrution.
   */
}

