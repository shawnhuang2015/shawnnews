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
//package org.neo4j.examples.server;

import java.io.*;
import java.util.*;

import java.net.URI;
import java.net.URISyntaxException;

import javax.ws.rs.core.MediaType;

import com.sun.jersey.api.client.Client;
import com.sun.jersey.api.client.ClientResponse;
import com.sun.jersey.api.client.WebResource;

public class RestLoadTraversalExample
{
    private static final String SERVER_ROOT_URI = "http://localhost:7474/db/data/";
    //private static final String twitter_source_path = "/Users/eric/project/graph_sql/neo4j/twitter_neo4j_test/source/eric_twitter_test.txt";
    private static final String twitter_source_path = "./source/eric_twitter_test_1000.txt";
    private static Map<String, URI> node_to_uri = new HashMap<String, URI>();

    public static void main( String[] args ) throws URISyntaxException
    {
        System.out.println("Before call checking the database");
        checkDatabaseIsRunning();

        // Eric Twitter graph loading stuff
        loadTwitterGraph();

        String startNodeName = "12";
        if (!node_to_uri.containsKey(startNodeName)) {
            return ;
        }
        // Eric traverse the test graph
        URI startNode = node_to_uri.get(startNodeName);
        twitterTraversalTest(startNode);

        /*
        // START SNIPPET: nodesAndProps
        System.out.println("Before call inserting the nodes");
        URI firstNode = createNode();
        addProperty( firstNode, "name", "Joe Strummer" );
        URI secondNode = createNode();
        addProperty( secondNode, "band", "The Clash" );
        // END SNIPPET: nodesAndProps

        // START SNIPPET: addRel
        URI relationshipUri = addRelationship( firstNode, secondNode, "singer",
                "{ \"from\" : \"1976\", \"until\" : \"1986\" }" );
        // END SNIPPET: addRel
        */

        /*
        // START SNIPPET: addMetaToRel
        addMetadataToProperty( relationshipUri, "stars", "5" );
        // END SNIPPET: addMetaToRel

        // START SNIPPET: queryForSingers
        System.out.println("Call findSingersInBands");
        findSingersInBands( firstNode );
        // END SNIPPET: queryForSingers

        sendTransactionalCypherQuery( "MATCH (n) WHERE has(n.name) RETURN n.name AS name" );
        */
    }

    private static void loadTwitterGraph() throws URISyntaxException {
        System.out.println("***************************************");
        System.out.println("Start to loading the temporary twitter graph");
        System.out.println("***************************************");
        // load all the souce graph file into memory
        long startTime = System.currentTimeMillis();
        List<String> source_content_list = new ArrayList<String>();
        try {
            BufferedReader br = new BufferedReader(new FileReader(twitter_source_path));
            String line;
            while ((line = br.readLine()) != null) {
                StringTokenizer st = new StringTokenizer(line, "\t");
                // at least two IDs found
                String firstNodeName = st.nextToken();
                String secondNodeName = st.nextToken();
                source_content_list.add(firstNodeName);
                source_content_list.add(secondNodeName);
            }
        } catch (FileNotFoundException e) {
            System.err.println("FileNotFoundException: " + e.getMessage());
        } catch (IOException e) {
            System.err.println("IOException: " + e.getMessage());
        }
        long endTime = System.currentTimeMillis();
        System.out.println("Load graph souce file into memeory took " + (endTime - startTime) + " milliseconds");

        // load the graph into neo4j
        startTime = System.currentTimeMillis();
        int total_edges = source_content_list.size()/2;
        for (int i = 0; i < total_edges; ++i) {
            int first_idx = 2*i;
            int second_idx = 2*i + 1;

            String firstNodeName = source_content_list.get(first_idx);
            URI firstNode;
            if (!node_to_uri.containsKey(firstNodeName)) {
                firstNode = createNode();
                addProperty(firstNode, "name", firstNodeName);
                node_to_uri.put(firstNodeName, firstNode);
            } else {
                firstNode = node_to_uri.get(firstNodeName);
            }

            String secondNodeName = source_content_list.get(second_idx);
            URI secondNode;
            if (!node_to_uri.containsKey(secondNodeName)) {
                secondNode = createNode();
                addProperty(secondNode, "name", secondNodeName);
                node_to_uri.put(secondNodeName, secondNode);
            } else {
                secondNode = node_to_uri.get(secondNodeName);
            }

            // add edge info
            URI relationshipUri = addRelationship(firstNode, secondNode, "KNOWS", null);
        }
        endTime = System.currentTimeMillis();
        System.out.println("Load graph souce into NEO4J took " + (endTime - startTime) + " milliseconds");
        System.out.println("***************************************");
        System.out.println("End of loading Twitter Graph");
        System.out.println("***************************************");
    }

    private static void twitterTraversalTest( URI startNode )
            throws URISyntaxException
    {
        System.out.println("***************************************");
        System.out.println("Start to traverse Twitter Graph");
        System.out.println("***************************************");
        // START SNIPPET: traversalDesc
        // TraversalDefinition turns into JSON to send to the Server
        TraversalDefinition t = new TraversalDefinition();
        //t.setOrder( TraversalDefinition.DEPTH_FIRST );
        t.setOrder( TraversalDefinition.BREADTH_FIRST );
        t.setUniqueness( TraversalDefinition.NODE );
        //t.setUniqueness( TraversalDefinition.NONE );
        t.setMaxDepth( 5 );
        t.setReturnFilter( TraversalDefinition.ALL );
        t.setRelationships( new Relation( "KNOWS", Relation.BOTH ) );
        // END SNIPPET: traversalDesc

        // START SNIPPET: traverse
        //URI traverserUri = new URI( startNode.toString() + "/traverse/node" );
        URI traverserUri = new URI( startNode.toString() + "/traverse/path" );
        WebResource resource = Client.create()
                .resource( traverserUri );
        String jsonTraverserPayload = t.toJson();
        long startTime = System.currentTimeMillis();
        ClientResponse response = resource.accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( jsonTraverserPayload )
                .post( ClientResponse.class );
        long endTime = System.currentTimeMillis();
        System.out.println("query graph in NEO4J took " + (endTime - startTime) + " milliseconds");

        System.out.println( String.format(
                "POST [%s] to [%s], status code [%d], returned data: "
                        + System.getProperty( "line.separator" ) + "%s",
                jsonTraverserPayload, traverserUri, response.getStatus(),
                response.getEntity( String.class ) ) );
        response.close();

        System.out.println("***************************************");
        System.out.println("End of Traversal Twitter Graph");
        System.out.println("***************************************");
        // END SNIPPET: traverse
    }

    private static void sendTransactionalCypherQuery(String query) {
        // START SNIPPET: queryAllNodes
        final String txUri = SERVER_ROOT_URI + "transaction/commit";
        WebResource resource = Client.create().resource( txUri );

        String payload = "{\"statements\" : [ {\"statement\" : \"" +query + "\"} ]}";
        ClientResponse response = resource
                .accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( payload )
                .post( ClientResponse.class );
        
        System.out.println( String.format(
                "POST [%s] to [%s], status code [%d], returned data: "
                        + System.getProperty( "line.separator" ) + "%s",
                payload, txUri, response.getStatus(),
                response.getEntity( String.class ) ) );
        
        response.close();
        // END SNIPPET: queryAllNodes
    }

    private static void findSingersInBands( URI startNode )
            throws URISyntaxException
    {
        // START SNIPPET: traversalDesc
        // TraversalDefinition turns into JSON to send to the Server
        TraversalDefinition t = new TraversalDefinition();
        t.setOrder( TraversalDefinition.DEPTH_FIRST );
        t.setUniqueness( TraversalDefinition.NODE );
        t.setMaxDepth( 10 );
        t.setReturnFilter( TraversalDefinition.ALL );
        t.setRelationships( new Relation( "singer", Relation.OUT ) );
        // END SNIPPET: traversalDesc

        // START SNIPPET: traverse
        URI traverserUri = new URI( startNode.toString() + "/traverse/node" );
        WebResource resource = Client.create()
                .resource( traverserUri );
        String jsonTraverserPayload = t.toJson();
        ClientResponse response = resource.accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( jsonTraverserPayload )
                .post( ClientResponse.class );

        System.out.println( String.format(
                "POST [%s] to [%s], status code [%d], returned data: "
                        + System.getProperty( "line.separator" ) + "%s",
                jsonTraverserPayload, traverserUri, response.getStatus(),
                response.getEntity( String.class ) ) );
        response.close();
        // END SNIPPET: traverse
    }

    // START SNIPPET: insideAddMetaToProp
    private static void addMetadataToProperty( URI relationshipUri,
            String name, String value ) throws URISyntaxException
    {
        URI propertyUri = new URI( relationshipUri.toString() + "/properties" );
        String entity = toJsonNameValuePairCollection( name, value );
        WebResource resource = Client.create()
                .resource( propertyUri );
        ClientResponse response = resource.accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( entity )
                .put( ClientResponse.class );

        System.out.println( String.format(
                "PUT [%s] to [%s], status code [%d]", entity, propertyUri,
                response.getStatus() ) );
        response.close();
    }

    // END SNIPPET: insideAddMetaToProp

    private static String toJsonNameValuePairCollection( String name,
            String value )
    {
        return String.format( "{ \"%s\" : \"%s\" }", name, value );
    }

    private static URI createNode()
    {
        // START SNIPPET: createNode
        final String nodeEntryPointUri = SERVER_ROOT_URI + "node";
        // http://localhost:7474/db/data/node

        WebResource resource = Client.create()
                .resource( nodeEntryPointUri );
        // POST {} to the node entry point URI
        ClientResponse response = resource.accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( "{}" )
                .post( ClientResponse.class );

        final URI location = response.getLocation();
        System.out.println( String.format(
                "POST to [%s], status code [%d], location header [%s]",
                nodeEntryPointUri, response.getStatus(), location.toString() ) );
        response.close();

        return location;
        // END SNIPPET: createNode
    }

    // START SNIPPET: insideAddRel
    private static URI addRelationship( URI startNode, URI endNode,
            String relationshipType, String jsonAttributes )
            throws URISyntaxException
    {
        URI fromUri = new URI( startNode.toString() + "/relationships" );
        String relationshipJson = generateJsonRelationship( endNode,
                relationshipType, jsonAttributes );

        WebResource resource = Client.create()
                .resource( fromUri );
        // POST JSON to the relationships URI
        ClientResponse response = resource.accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( relationshipJson )
                .post( ClientResponse.class );

        final URI location = response.getLocation();
        System.out.println( String.format(
                "POST to [%s], status code [%d], location header [%s]",
                fromUri, response.getStatus(), location.toString() ) );

        response.close();
        return location;
    }
    // END SNIPPET: insideAddRel

    private static String generateJsonRelationship( URI endNode,
            String relationshipType, String... jsonAttributes )
    {
        StringBuilder sb = new StringBuilder();
        sb.append( "{ \"to\" : \"" );
        sb.append( endNode.toString() );
        sb.append( "\", " );

        sb.append( "\"type\" : \"" );
        sb.append( relationshipType );
        if ( jsonAttributes == null || jsonAttributes.length < 1 )
        {
            sb.append( "\"" );
        }
        else
        {
            sb.append( "\", \"data\" : " );
            for ( int i = 0; i < jsonAttributes.length; i++ )
            {
                sb.append( jsonAttributes[i] );
                if ( i < jsonAttributes.length - 1 )
                { // Miss off the final comma
                    sb.append( ", " );
                }
            }
        }

        sb.append( " }" );
        return sb.toString();
    }

    private static void addProperty( URI nodeUri, String propertyName,
            String propertyValue )
    {
        // START SNIPPET: addProp
        String propertyUri = nodeUri.toString() + "/properties/" + propertyName;
        // http://localhost:7474/db/data/node/{node_id}/properties/{property_name}

        WebResource resource = Client.create()
                .resource( propertyUri );
        ClientResponse response = resource.accept( MediaType.APPLICATION_JSON )
                .type( MediaType.APPLICATION_JSON )
                .entity( "\"" + propertyValue + "\"" )
                .put( ClientResponse.class );

        System.out.println( String.format( "PUT to [%s], status code [%d]",
                propertyUri, response.getStatus() ) );
        response.close();
        // END SNIPPET: addProp
    }

    private static void checkDatabaseIsRunning()
    {
        // START SNIPPET: checkServer
        WebResource resource = Client.create()
                .resource( SERVER_ROOT_URI );
        ClientResponse response = resource.get( ClientResponse.class );

        System.out.println( String.format( "GET on [%s], status code [%d]",
                SERVER_ROOT_URI, response.getStatus() ) );
        response.close();
        // END SNIPPET: checkServer
    }
}
