(function(undefined) {
  "use strict";
  // Doing somthing for layouts
  console.log('Loading gvis.layouts')

  gvis.layouts = function(graph) {
    this.graph = graph;
  }

  gvis.layouts.prototype.random = function() {
    this.graph.data().array.nodes.forEach(function(n) {
      n.x = Math.random();
      n.y = Math.random();
    })
  }
  
}).call(this)