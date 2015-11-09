(function(undefined) {
  "use strict";
  gvis.data = gvis.data || {};


  gvis.data.refineDataName = function (newData) {
    // the result should be as nodes and edges.
    var result = {"nodes":[], "edges":[]};
    var arrResult = []
    this.topology4VertexAttribute_to_json();
    // newData.vertices.forEach(function(n) {
    //   // Example of a node :
    //   tempNode = {type:'', id:'', attr: {}, style: {}, other: {}};
    //   tempNode.type = n.type
    //   tempNode.id = n.id
    //   tempNode.attr = topology4VertexAttribute_to_json(n.attr);
    //   tempNode.other = clone(n.other)

    //   arrResult.push(tempNode);
    // }) 

    // var tempResult = {};
    // arrResult.forEach(function(n, i){
    //   tempResult[n.id + '-'+ n.type] = n; 
    // })

    // for (var key in tempResult) {
    //   result.nodes.push(tempResult[key]);
    // }

    // newData.edges.forEach(function(e) {
    //   // Example of a link :
    //   // {"source":{"type":"newType1","id":"newKey1"},"target":{"type":"type0","id":"key45"},"attr":{"weight":"0.01","name":"name0.7"}, "type":0};
    //   tempEdge = {"source":{"type":"","id":""},"target":{"type":"","id":""},"attr":""};
    //   tempEdge.source.type = e.src.type
    //   tempEdge.source.id = e.src.id
    //   tempEdge.target.type = e.tgt.type
    //   tempEdge.target.id = e.tgt.id

    //   // Check whether a edge includes 'type' attribute. 
    //   if ("type" in e) {
    //     tempEdge.type = e.type
    //   }
    //   else {
    //     tempEdge.type = "Undefined"
    //   }
      
    //   //deep copy the object of the attributes.
    //   tempEdge.attr = topology4VertexAttribute_to_json(e.attr) //The way to parse edge and vertex attrs are same.
    //   tempEdge.other = clone(e.other)

    //   result.edges.push(tempEdge);
    // })
    
    return result
  }

  gvis.data.topology4VertexAttribute_to_json = function (vattr) {
    var result = {};
    var reggie = /(.*)\((\d*)\)$/;  // for degree(0) => degree, 0

    if (vattr == "" || vattr == undefined) {
      return result;
    }
    else if (typeof(vattr) == 'object') {
      return clone(vattr);
    }
    else {
      var attrs = vattr.split('|');
      attrs.forEach(function(attr, i) {
        //var items = attr.split(':');
        var items = attr.split(':', 1);
        items[1] = attr.substring(attr.indexOf(":")+1);

        if (items.length > 1 && items[0] != "" && items[0] != items[1]) {
          if (items[0] == 'type') { //|| items[0].indexOf("od_et") > -1 || items[0] == 'outdegree') {
            //items[0] = 'type_index';
            //console.log(items[0])
            return; // ignore the type attributes.
          }

          var tempAttr = reggie.exec(items[0]); // "degree(0)" => "degree(0)", "degree", "0";
          if (tempAttr == null) {
            result[items[0]] = items[1];
          }
          else {
            result[tempAttr[1]] = items[1];
          }
        }
      })
      return result; 
    }
  }

}).call(this)