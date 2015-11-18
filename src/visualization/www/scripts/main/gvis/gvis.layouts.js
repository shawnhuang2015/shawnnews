(function(undefined) {
  "use strict";
  // Doing somthing for layouts
  console.log('Loading gvis.layouts')

  gvis.layouts = function(graph) {
    this.graph = graph;
  }

  gvis.layouts.prototype.random = function() {
    this.graph.data().array.nodes.forEach(function(n) {
      n[gvis.settings.styles].x = Math.random();
      n[gvis.settings.styles].y = Math.random();
    })
  }
  
}).call(this)