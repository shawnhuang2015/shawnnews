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
  * 3.  Additional endpoint extensions that may be strings or VAR() types.
  *     For example for the endpoint:  https://SERVERNAME/ *endpoint1* /uid/full,
  *     you would have the arguments "endpoint1",VAR(), "full"
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
  Endpoints.register(Endpoint(GET(), "kneighborhood", VAR(), (uid: String, queryString: Map[String,Seq[String]], dataPayload: JsObject, context: EndpointContext) => {
    val request = GpeRequest(List("kneighborhood", uid), queryString)
    context.gpe.writeAndWait(request, List(uid))
  }))

  // Helper function that forces the JVM to initialize this object.
  // Must be left untouched. It is required by the RestServer object.
  def warmUp() = { }
}

