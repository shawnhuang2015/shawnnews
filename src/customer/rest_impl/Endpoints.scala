package com.graphsql.endpoints
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
  * 3.  Additional endpoint extensions that may be strings or VAR() typ€es.
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

  ///UI END POINTS
  
  Endpoints.register(Endpoint(GET(), "UIpages", (queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => { 
    // User defined configure object for the web base graph visualization (gsqlv)
    val formElements: JsValue = Json.parse("""
    [ 
      {
        "tabname": "Explore N Step Neighborhood",
        "index": 0,
        "elements": [ { "label": { "name": "IP" } }, { "textbox": {"name" : "id", "length" : 20 } },
            { "label": { "name" : "Depth"} }, { "textbox": {"name": "depth", "length" : 10}},
            { "label": { "name": "Start Time" } }, { "textbox": {"name" : "starttime", "length" : 20 } },
            { "label": { "name": "End Time" } }, { "textbox": {"name" : "endtime", "length" : 20 } }
        ],
        "attributes": {
          "depth" : 1
        },
        "setting" : {
          "layout" : "circle",
          "graphType" : "undirected" 
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/check_ip",
            "URL_attrs" : {
                "id" : {"usage":"input", "name":"id"},
                "depth" : {"usage":"input", "name":"depth"},
                "starttime" : {"usage":"input", "name":"starttime"},
                "endtime" : {"usage":"input", "name":"endtime"}
            }
          },
          "node_dblclick" : {
            "URL_head" : "/engine/check_ip",
            "URL_attrs" : {
                "id" : {"usage":"select", "name":"id"},
                "depth" : {"usage":"attributes", "name":"depth"},
                "starttime" : {"usage":"input", "name":"starttime"},
                "endtime" : {"usage":"input", "name":"endtime"}
            }
          }
        } 
      }
    ]
    """);
 
    Some(Json.stringify(formElements));
  }))
  ///FINISH UI END POINTS

  //////////////////////////
  // REST ENDPOINTS       //
  //////////////////////////
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

  Endpoints.register(Endpoint(GET(), "debug_neighbors", VAR(), (uid: String, queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    val request = GpeRequest(List("debug_neighbors", uid), Map())
    context.gpe.writeAndWait(request, List(uid))
  }))

  Endpoints.register(Endpoint(GET(), "rec", VAR(), (uid: String, queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    val request = GpeRequest(List("rec", uid), Map())
    context.gpe.writeAndWait(request, List(uid))
  }))

  Endpoints.register(Endpoint(GET(), "check_ip", VAR(), (uid: String, queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    val request = GpeRequest(List("check_ip", uid), Map())
    context.gpe.writeAndWait(request, List(uid))
  }))

  // Helper function that forces the JVM to initialize this object.
  // Must be left untouched. It is required by the RestServer object.
  def warmUp() = { }
}

