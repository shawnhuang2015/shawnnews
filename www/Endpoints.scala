package com.graphsql.endpoints

import java.io._;
/**
  * Create endpoints using Endpoints.register, which  takes a single argument, an Endpoint object.
  *
  * ==Overview==
  * It has flexible constructor that takes the following sequnce of arguments:
  *
  * 1.  Endpoint type:  Either POST() GET() or DELETE().  These are objects
  *     that functionally are used as constants to describe the endpoint type.
  *
  * 2.  Endpoint name, as a string.
  *
  * 3.  Additional endpoint extensions that may be strings or VAR() types.
  *     For example for the endpoint:  https://SERVERNAME/ *endpoint1* /uid/full,
  *     you would have the arguments "endpoint1",VAR("uid"), "full"
  *
  * 4.  A function whose arguments are:  ( X1: String, X2: String, ... , queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {}
  *    --- X1, X2, etc refer to the VAR() types you passed in the earlier arguments to the endpoint.
  *    --- the body of your endpoint implementation goes inside the brackets.
  *
  *  % An endpoint is parsed as follows:  the request comes in, and the server
  *     matches the request against all registered endpoints.  If one matches,
  *     the server will recall the function you created in step 4, providing the VAR() arguments,
  *     as well as a map of any key/value pairs passed in the query string, a data payload (POST only)
  *    and a reference to the global context object which gives you access to the IDS/GPE interface.
  *
  *  % the body of your function in part 4 above will:
  *   1.  validate the inputs (optional)
  *   2.  construct an appropriate request object (GPE or GSE--see documentation in
  *       realtime/restserver/app/util/Context.scala for more info)
  *   3.  Submit the request to the appropriate Context queue (context.gpe or context.gse or context.redis)
  **/
import com.graphsql.graph._
import com.graphsql.util._
import play.api.libs.json._

object EndpointDefinitions {
  //////////////////////////
  // HELPER FUNCTIONS     //
  //////////////////////////

  // TODO: add helper functions here

  //////////////////////////
  // REST ENDPOINTS       //
  //////////////////////////

  def Append(file: String, msg : String)
  {
    val fw = new FileWriter(file, true)
    try {
      fw.write( msg + "\n")
    }
    finally fw.close()
  }


  ///UI END POINTS
  
  Endpoints.register(Endpoint(GET(), "UIpages", (queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => { 
    
    val formElements: JsValue = Json.parse("""
    [ 
      {
        "tabname": "Explore One Step Neighborhood",
        "index": 0,
        "elements": [ { "label": { "name": "Type" } }, { "textbox": {"name" : "type", "length" : 4 } }, { "label": { "name" : "Key"} }, { "textbox": {"name": "primaryKey", "length" : 10}}],
        "attributes": {
          "depth" : 1
        },
        "setting" : {
          "layout" : "force"
        },
        "initialization":{
          "coloring" : [
              {"selection":"some_int < 3", "selectionType":"nodes", "color":"#00ff00"},
              {"selection":"some_int <= 4", "selectionType":"edges", "color":"#0000ff"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "http://uitest.graphsql.com:8080/engine/kneighborhood_full_type",
            "URL_attrs" : {
                "id" : {"usage":"input", "name":"primaryKey"},
                "type" : {"usage":"input", "name":"type"},
                "depth" : {"usage":"attributes", "name":"depth"}
            }
          },
          "node_dblclick" : {
            "URL_head" : "http://uitest.graphsql.com:8080/engine/kneighborhood_full_type",
            "URL_attrs" : {
                "id" : {"usage":"select", "name":"id"},
                "type" : {"usage":"select", "name":"type"},
                "depth" : {"usage":"attributes", "name":"depth"}
            }
          }
        }  
      },
      {
        "tabname": "Explore Two Step Neighborhood",
        "index": 1,
        "elements": [ { "label": { "name": "Type" } }, { "textbox": {"name" : "type", "length" : 4 } }, { "label": { "name" : "Key"} }, { "textbox": {"name": "primaryKey", "length" : 10}}],
        "attributes": {
          "depth" : 2
        },
        "setting" : {
          "layout" : "tree"
        },
        "initialization":{
          "coloring" : [
              {"selection":"some_int > 6 && some_float > 0.6", "selectionType":"edges", "color":"#FE2E64"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "http://uitest.graphsql.com:8080/engine/kneighborhood_full_type",
            "URL_attrs" : {
                "id" : {"usage":"input", "name":"primaryKey"},
                "type" : {"usage":"input", "name":"type"},
                "depth" : {"usage":"attributes", "name":"depth"}
            }
          },
          "node_dblclick" : {
            "URL_head" : "http://uitest.graphsql.com:8080/engine/kneighborhood_full_type",
            "URL_attrs" : {
                "id" : {"usage":"select", "name":"id"},
                "type" : {"usage":"select", "name":"type"},
                "depth" : {"usage":"attributes", "name":"depth"}
            }
          }
        } 
      }
    ]
    """);

    ////"http://uitest.graphsql.com:8080/engine/kneighborhood_full_type?id*primaryKey&type*type&depth@depth",
 
    Some(Json.stringify(formElements));
    //Some(Json.stringify(Json.obj("results" -> Json.stringify(formElements), "error" -> false, "message" -> ""))) 
    //Some(Json.stringify(Json.obj("error" -> false, "message" -> "dummy!")))
  }))
  ///FINISH UI END POINTS




  Endpoints.register(Endpoint(GET(), "kneighborhood", VAR(), (uid: String, queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    val request = GpeRequest(List("kneighborhood", uid), queryString)
    context.gpe.writeAndWait(request, List(uid))
  }))


  /*
   * This endpoint is re-usable because it assumes that you are following a fixed format for graph update:
   * {"nodeList":[{"id":"id1","att1":v1,...}, {"id":"id2","a1":v2,...},...],
   * "edgeList":[{"startNode":"id1","endNode":"id2","a1":v1,...},...] }
   *
   * Any vertex appearing in the edgeList must have a corresponding entry in the nodeList.
   *
   * You do not have to give a full attribute specification for a vertex/edge.  You can give just id's,
   * or a partial spec.
   */
  Endpoints.register(Endpoint(POST(), "updategraph", (queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    if (!dataPayload.keys.contains("nodeList")) {
      Some(Json.stringify(Json.obj("error" -> true, "message" -> "You must pass a node list!")))
    } else {

    // probably a cleaner way to do this, but this works.
      val vertexSeq: Seq[String] = if (!dataPayload.keys.contains("nodeList")) List() else {
        val rawList = (dataPayload \ "nodeList").as[JsArray].value.map((x: JsValue) => {
          val vertexId = (x \ "id").as[String]
          vertexId
        })
        rawList
      }

      val edgeSeq: Seq[Edge] = if (!dataPayload.keys.contains("edgeList")) List() else {
        val rawList = (dataPayload \ "edgeList").as[JsArray].value.map((x: JsValue) => {
          val fromId = (x \ "startNode").as[String]
          val toId = (x \ "endNode").as[String]
          val xObj = x.as[JsObject]
          var attributes = Map[String,Any]()

          Edge(fromId, toId, attributes)
        })
        rawList
      }

      val vertexIdSet: Set[String] = vertexSeq.toSet
      val containsAll: Boolean = edgeSeq.forall((e: Edge) => vertexIdSet.contains(e.fromId) && vertexIdSet.contains(e.toId))

      if (containsAll) {
        // odds are vertexSeq has no duplicates.  But to be safe, since we just made that set up
        // above, we'll convert that back to a list and send it across.
        val request = PassthroughGseRequest(Json.stringify(dataPayload),vertexIdSet.toList)
        val response = context.gse.passthrough_writeAndWait(request)
        response
      } else {
        Some(Json.stringify(Json.obj("error" -> true, "message" -> "Not all of the edges had corresponding vertices in the node list.")))
      }
    }
  })) 

  Endpoints.register(Endpoint(GET(), "kneighborhood_full_type", (queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    val node_id = queryString("id")(0);
    var node_type="";
    Append("/tmp/amol.out", "Before if \n");
    if(queryString.contains("type") && queryString("type").length == 1 && queryString("type")(0) != "")
      node_type = queryString("type")(0);
    Append("/tmp/amol.out", "After if \n");
    val depth = queryString("depth")(0);
    Append("/tmp/amol.out", "Got depth \n");
    val request = GpeRequest(List("kneighborhood_full", node_id, node_type, depth), Map());
    Append("/tmp/amol.out", "Creating gpe \n");
    Append("/tmp/amol.out", node_id + " " + node_type + " " + depth);
    if(node_type != "") {
       Append("/tmp/amol.out", "before typed call \n");
       context.gpe.writeAndWait_TypedIds(request, List(node_id -> node_type.toInt)) 
    }
    else {
      Append("/tmp/amol.out", "before regular call \n");
      context.gpe.writeAndWait(request, List(node_id)) 
    }
     
  }))


  // Helper function that forces the JVM to initialize this object.
  // Must be left untouched. It is required by the RestServer object.
  def warmUp() = { }
}

