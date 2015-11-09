(function(undefined) {
  "use strict";
  console.log('Loading gvis.data')

  gvis.data = gvis.data || {};


  gvis.data.initialization = function (newData) {
    // the result should be as nodes and links.
    var result = {"nodes":[], "links":[]};
    var arrResult = []

    // newData.nodes.forEach(function(n) {
    //   // Example of a node :
    //   tempNode = {type:'', id:'', attr: {}, style: {}, other: {}};
    //   tempNode.type = n.type
    //   tempNode.id = n.id
    //   tempNode.attr = this.topology4VertexAttribute_to_json(n.attr);
    //   tempNode.other = gvis.utils.clone(n.other)

    //   arrResult.push(tempNode);
    // }) 

    // var tempResult = {};
    // arrResult.forEach(function(n, i){
    //   tempResult[n.id + '-'+ n.type] = n; 
    // })

    // for (var key in tempResult) {
    //   result.nodes.push(tempResult[key]);
    // }

    // newData.links.forEach(function(e) {
    //   // Example of a link :
    //   // {"source":{"type":"newType1","id":"newKey1"},"target":{"type":"type0","id":"key45"},"attr":{"weight":"0.01","name":"name0.7"}, "type":0};
    //   templink = {"source":{"type":"","id":""},"target":{"type":"","id":""},"attr":""};
    //   templink.source.type = e.src.type
    //   templink.source.id = e.src.id
    //   templink.target.type = e.tgt.type
    //   templink.target.id = e.tgt.id

    //   // Check whether a link includes 'type' attribute. 
    //   if ("type" in e) {
    //     templink.type = e.type
    //   }
    //   else {
    //     templink.type = "Undefined"
    //   }
      
    //   //deep copy the object of the attributes.
    //   templink.attr = topology4VertexAttribute_to_json(e.attr) //The way to parse link and vertex attrs are same.
    //   templink.other = clone(e.other)

    //   result.links.push(tempLink);
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