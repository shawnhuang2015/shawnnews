/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
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

  gvis.layouts.prototype.force = function() {
    var force = d3.layout.force()
    .nodes(this.graph.data().array.nodes)
    .links(this.graph.data().array.links)
    .size([1, 1])
    .linkStrength(0.1)
    .friction(0.9)
    .linkDistance(0.1)
    .charge(-30)
    .gravity(0.1)
    .theta(0.8)
    .alpha(0.1)

    force.start();
    for (var i = 0; i < 20; ++i) force.tick();
    force.stop();
  }
  
