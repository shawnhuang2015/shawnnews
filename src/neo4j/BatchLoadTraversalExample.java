/**
 * Licensed to Neo Technology under one or more contributor
 * license agreements. See the NOTICE file distributed with
 * this work for additional information regarding copyright
 * ownership. Neo Technology licenses this file to you under
 * the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
// START SNIPPET: sampleDocumentation
// START SNIPPET: _sampleDocumentation
//package org.neo4j.examples;

import static java.lang.System.out;

import java.io.*;
import java.util.*;
import java.io.File;
import java.util.concurrent.TimeUnit;

import org.neo4j.cypher.javacompat.ExecutionEngine;
import org.neo4j.cypher.javacompat.ExecutionResult;
import org.neo4j.graphdb.Direction;
import org.neo4j.graphdb.GraphDatabaseService;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Path;
import org.neo4j.graphdb.Relationship;
import org.neo4j.graphdb.RelationshipType;
import org.neo4j.graphdb.Transaction;
import org.neo4j.graphdb.factory.GraphDatabaseFactory;
import org.neo4j.graphdb.traversal.Evaluators;
import org.neo4j.graphdb.traversal.TraversalDescription;
import org.neo4j.graphdb.traversal.Uniqueness;
import org.neo4j.graphdb.traversal.*;

import org.neo4j.graphdb.DynamicLabel;
import org.neo4j.graphdb.Label;
import org.neo4j.graphdb.ResourceIterator;
import org.neo4j.graphdb.schema.IndexDefinition;
import org.neo4j.graphdb.schema.Schema;

public class BatchLoadTraversalExample
{
    public enum RelTypes implements RelationshipType
    {
        KNOWS
        //KNOWS,
        //NEO_NODE,
        //CODED_BY
    }

    private GraphDatabaseService db;
    private TraversalDescription friendsTraversal;
    private static final String twitter_source_path = "./source/small_twitter_edge.txt";
    private static final int total_lines = 2286909;
    private static Map<String, Node> name_to_node = new HashMap<String, Node>();

    private static final String DB_PATH = "target/eric-twitter-traversal-example";
    //private static final String nameToFind = "11";
    private static final String nameToFind = "67864340";

    public static void main( String[] args )
    {
        System.out.println("Delete the old database!!!");
        deleteFileOrDirectory( new File( DB_PATH ) );
        System.out.println("Finish Delete the old database!!!");
        System.out.println("Create new empty database!!!");
        GraphDatabaseService database = new GraphDatabaseFactory().newEmbeddedDatabase( DB_PATH );
        System.out.println("Finish new empty database!!!");
        System.out.println("Create Indexing for the graph!!!");
        // create index for the existed graph-db
        ///*
        {
            IndexDefinition indexDefinition;
            try ( Transaction tx = database.beginTx() )
            {
                Schema schema = database.schema();
                indexDefinition = schema.indexFor( DynamicLabel.label( "User" ) )
                        .on( "name" )
                        .create();
                tx.success();
            }
            // END SNIPPET: createIndex
            // START SNIPPET: wait
            try ( Transaction tx = database.beginTx() )
            {
                Schema schema = database.schema();
                schema.awaitIndexOnline( indexDefinition, 10, TimeUnit.SECONDS );
            }
            // END SNIPPET: wait
        }
        //*/
        System.out.println("Finish Indexing for the graph!!!");

        BatchLoadTraversalExample example = new BatchLoadTraversalExample( database );
        example.loadTwitterGraph(database);
        example.traverseTwitterGraph(database);
        //Node joe = example.createData();
        //example.run( joe );
    }

    public BatchLoadTraversalExample( GraphDatabaseService db )
    {
        this.db = db;
        // START SNIPPET: basetraverser
        friendsTraversal = db.traversalDescription()
                .depthFirst()
                .relationships( Rels.KNOWS )
                .uniqueness( Uniqueness.RELATIONSHIP_GLOBAL );
        // END SNIPPET: basetraverser
    }

    // added by Eric Chu@07/23/2014
    private void loadTwitterGraph(GraphDatabaseService db) {
        System.out.println("***************************************");
        System.out.println("Start to loading the temporary twitter graph");
        System.out.println("***************************************");
        // load all the souce graph file into memory
        long startTime = System.currentTimeMillis();
        Map<String, List<String>> graph_adj_list = new HashMap<String, List<String>>();
        try {
            BufferedReader br = new BufferedReader(new FileReader(twitter_source_path));
            String line;
            int line_idx = 0;
            while ((line = br.readLine()) != null) {
                StringTokenizer st = new StringTokenizer(line, "\t");
                // at least two IDs found
                String firstNodeName = st.nextToken();
                String secondNodeName = st.nextToken();
                if (!graph_adj_list.containsKey(firstNodeName)) {
                    List<String> end_vertex_list = new ArrayList<String> ();
                    end_vertex_list.add(secondNodeName);
                    graph_adj_list.put(firstNodeName, end_vertex_list);
                } else {
                    graph_adj_list.get(firstNodeName).add(secondNodeName);
                }
                System.out.print("\rProcess raw file [" + ++line_idx + "]/[" + total_lines + "]");
            }
        } catch (FileNotFoundException e) {
            System.err.println("FileNotFoundException: " + e.getMessage());
        } catch (IOException e) {
            System.err.println("IOException: " + e.getMessage());
        }
        long endTime = System.currentTimeMillis();
        System.out.println("\nLoad graph souce file into memeory took " + (endTime - startTime) + " milliseconds");

        // load the graph into neo4j
        startTime = System.currentTimeMillis();
        //try (Transaction tx = db.beginTx()) {
        {
            Label label = DynamicLabel.label( "User" );
            int record_idx = 0;
            for (String firstNodeName : graph_adj_list.keySet()) {
                // get the start vertex node
                try (Transaction tx = db.beginTx()) {
                Node firstNode;
                if (!name_to_node.containsKey(firstNodeName)) {
                    firstNode = db.createNode(label);
                    firstNode.setProperty("name", firstNodeName);
                    name_to_node.put(firstNodeName, firstNode);
                } else {
                    firstNode = name_to_node.get(firstNodeName);
                }

                // process the outgoing list
                for (String secondNodeName : graph_adj_list.get(firstNodeName)) {
                    Node secondNode;
                    if (!name_to_node.containsKey(secondNodeName)) {
                        secondNode = db.createNode(label);
                        secondNode.setProperty("name", secondNodeName);
                        name_to_node.put(secondNodeName, secondNode);
                    } else {
                        secondNode = name_to_node.get(secondNodeName);
                    }

                    // add edge info
                    firstNode.createRelationshipTo(secondNode, RelTypes.KNOWS);
                    System.out.print("\rProcess insert record [" + ++record_idx + "]/[" + total_lines + "]");
                }
                tx.success();
                }
            }

        }
        //    tx.success();
        //}
        endTime = System.currentTimeMillis();
        System.out.println("\nLoad graph souce into NEO4J took " + (endTime - startTime) + " milliseconds");
        System.out.println("***************************************");
        System.out.println("End of loading Twitter Graph");
        System.out.println("***************************************");
    }

    // added by Eric Chu@07/23/2014
    private void traverseTwitterGraph(GraphDatabaseService db) {
        Node startNode;

        // find the users
        {
            // START SNIPPET: findUsers
            Label label = DynamicLabel.label( "User" );
            try ( Transaction tx = db.beginTx() )
            {
                try ( ResourceIterator<Node> users =
                    db.findNodesByLabelAndProperty( label, "name", nameToFind ).iterator() )
                {
                    ArrayList<Node> userNodes = new ArrayList<>();
                    while ( users.hasNext() )
                    {
                        userNodes.add( users.next() );
                    }
                    /*
                    for ( Node node : userNodes )
                    {
                        System.out.println( "The username of user " + nameToFind + " is " + node.getProperty( "name" ) );
                    }
                    */
                    if (userNodes.size() != 0) {
                        startNode = userNodes.get(0);
                        System.out.println( "The username of user " + nameToFind + " is " + startNode.getProperty( "name" ) );
                    } else {
                        System.out.println("The username of user " + nameToFind + " doesn't exist");
                        return ;
                    }
                }
            }
            // END SNIPPET: findUsers
        }

        // start to traverse the test graph
        try ( Transaction tx = db.beginTx() )
        {
            String output = "";
            System.out.println("***************************************");
            System.out.println("Start to traverse Twitter Graph start at[" + nameToFind + "]");
            System.out.println("***************************************");
            long startTime = System.currentTimeMillis();
            Traverser paths = db.traversalDescription()
                                            .depthFirst()
                                            .relationships(Rels.KNOWS, Direction.OUTGOING)
                                            .evaluator(Evaluators.toDepth( 5 ))
                                            .traverse(startNode);
            long endTime = System.currentTimeMillis();
            System.out.println("query graph in NEO4J took " + (endTime - startTime) + " milliseconds");
            for (Path position : paths) {
                output += position + "\n";
            }
            System.out.println(output);
        }
    }
    
    private Node createData()
    {
        ExecutionEngine engine = new ExecutionEngine( db );
        String query = "CREATE (joe {name: 'Joe'}), (sara {name: 'Sara'}), "
           + "(lisa {name: 'Lisa'}), (peter {name: 'PETER'}), (dirk {name: 'Dirk'}), "
                       + "(lars {name: 'Lars'}), (ed {name: 'Ed'}),"
           + "(joe)-[:KNOWS]->(sara), (lisa)-[:LIKES]->(joe), "
           + "(peter)-[:KNOWS]->(sara), (dirk)-[:KNOWS]->(peter), "
           + "(lars)-[:KNOWS]->(drk), (ed)-[:KNOWS]->(lars), " 
           + "(lisa)-[:KNOWS]->(lars) " 
           + "RETURN joe";
        ExecutionResult result = engine.execute( query );
        Object joe = result.columnAs( "joe" ).next();
        if ( joe instanceof Node )
        {
            return (Node) joe;
        }
        else
        {
            throw new RuntimeException( "Joe isn't a node!" );
        }
    }

    private void run( Node joe )
    {
        try (Transaction tx = db.beginTx())
        {
            out.println( knowsLikesTraverser( joe ) );
            out.println( traverseBaseTraverser( joe ) );
            out.println( depth3( joe ) );
            out.println( depth4( joe ) );
            out.println( nodes( joe ) );
            out.println( relationships( joe ) );
        }
    }

    public String knowsLikesTraverser( Node node )
    {
        String output = "";
        // START SNIPPET: knowslikestraverser
        for ( Path position : db.traversalDescription()
                .depthFirst()
                .relationships( Rels.KNOWS )
                .relationships( Rels.LIKES, Direction.INCOMING )
                .evaluator( Evaluators.toDepth( 5 ) )
                .traverse( node ) )
        {
            output += position + "\n";
        }
        // END SNIPPET: knowslikestraverser
        return output;
    }

    public String traverseBaseTraverser( Node node )
    {
        String output = "";
        // START SNIPPET: traversebasetraverser
        for ( Path path : friendsTraversal.traverse( node ) )
        {
            output += path + "\n";
        }
        // END SNIPPET: traversebasetraverser
        return output;
    }

    public String depth3( Node node )
    {
        String output = "";
        // START SNIPPET: depth3
        for ( Path path : friendsTraversal
                .evaluator( Evaluators.toDepth( 3 ) )
                .traverse( node ) )
        {
            output += path + "\n";
        }
        // END SNIPPET: depth3
        return output;
    }

    public String depth4( Node node )
    {
        String output = "";
        // START SNIPPET: depth4
        for ( Path path : friendsTraversal
                .evaluator( Evaluators.fromDepth( 2 ) )
                .evaluator( Evaluators.toDepth( 4 ) )
                .traverse( node ) )
        {
            output += path + "\n";
        }
        // END SNIPPET: depth4
        return output;
    }

    public String nodes( Node node )
    {
        String output = "";
        // START SNIPPET: nodes
        for ( Node currentNode : friendsTraversal
                .traverse( node )
                .nodes() )
        {
            output += currentNode.getProperty( "name" ) + "\n";
        }
        // END SNIPPET: nodes
        return output;
    }

    public String relationships( Node node )
    {
        String output = "";
        // START SNIPPET: relationships
        for ( Relationship relationship : friendsTraversal
                .traverse( node )
                .relationships() )
        {
            output += relationship.getType().name() + "\n";
        }
        // END SNIPPET: relationships
        return output;
    }

    // START SNIPPET: sourceRels
    private enum Rels implements RelationshipType
    {
        LIKES, KNOWS
    }
    // END SNIPPET: sourceRels

    private static void deleteFileOrDirectory( File file )
    {
        if ( file.exists() )
        {
            if ( file.isDirectory() )
            {
                for ( File child : file.listFiles() )
                {
                    deleteFileOrDirectory( child );
                }
            }
            file.delete();
        }
    }
}
