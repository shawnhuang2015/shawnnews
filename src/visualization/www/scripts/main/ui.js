/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
require([], function(){
  
  var data = {
    "nodes": [
      {"id": "0", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 100, "shape": "square", "fill": "#ff5500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "1", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 100, "shape": "circle", "fill": "#005500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.8}},
      {"id": "2", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "cross", "fill": "#0055ff", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.7}},
      {"id": "3", "type": "movie", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "circle", "fill": "#ff5500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.6}},
      {"id": "4", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "diamond", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "5", "type": "movie", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "circle", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}},
      {"id": "6", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10,"shape": "cross", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.5}},
      {"id": "7", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 20, "shape": "circle", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.4}},
      {"id": "8", "type": "usr", "_attrs": {"a1": 1, "a2": 2, "a3": 4, "a4": 5}, "_styles": {"size": 60, "shape": "square", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "9", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "diamond", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.9}},
      {"id": "10", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "square", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.8}},
      {"id": "11", "type": "movie", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "diamond", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.7}},
      {"id": "12", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "cross", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.6}}],
    "links": [
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "aaa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "bbb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "ccc", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "2", "type": "usr"}, "type": "aaa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "2", "type": "usr"}, "type": "bbb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "3", "type": "movie"}, "type": "dds", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": true},
      {"source": {"id": "4", "type": "tag"}, "target": {"id": "0", "type": "usr"}, "type": "adx", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "0", "type": "usr"}, "type": "fdd", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "6", "type": "usr"}, "type": "eew", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.5}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "3", "type": "movie"}, "type": "eer", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.4}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "4", "type": "tag"}, "type": "dff", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.3}, "directed": false},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "1", "type": "tag"}, "type": "sdf", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "6", "type": "usr"}, "type": "sdd", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "4", "type": "tag"}, "type": "eeq", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": false},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "5", "type": "movie"}, "type": "cca", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "6", "type": "usr"}, "type": "dfa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": true},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "5", "type": "movie"}, "type": "ccq", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.5}, "directed": false},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "6", "type": "usr"}, "type": "ccb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": true, "opacity": 0.4}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "6", "type": "usr"}, "type": "ccr", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.3}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "7", "type": "tag"}, "type": "ccw", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": false},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "8", "type": "usr"}, "type": "ssa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "9", "type": "tag"}, "type": "ssb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": true},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "10", "type": "usr"}, "type": "bbc", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": false},
      {"source": {"id": "4", "type": "tag"}, "target": {"id": "11", "type": "movie"}, "type": "bbs", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "12", "type": "usr"}, "type": "bbc", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.5}, "directed": true}]
  };


  d3.select('#prototype1').style('height', ($(window).height()-220)+'px');

  this.test = new gvis({
    configure: config,
    container: 'prototype1',
    //render_type: 'svg',
  });


  // for (var node in data.nodes) {
  //   test.addNode(data.nodes[node]);
  // }

  // for (var link in data.links) {
  //   test.addLink(data.links[link]);
  // }

  test
  .read(data)
  .addLayout('cluster', function() {
      var size = this.createTreeBFS();

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = gvis.settings.domain_height / 2.0;

      var layoutNodes = d3.layout.cluster()
      .size([size[0] * gvis.settings.domain_width / 4.0, size[1] * gvis.settings.domain_height / 4.0])
      .separation(function(a, b) {
        return (a.parent == b.parent ? 1 : 2) / a.depth / a.depth;
      })
      .nodes(this._tree)

      layoutNodes.forEach(function(node) {
        node.node.x = node.x
        node.node.y = node.y
      })
    })
  .layout('cluster')
  .render()

  window.addEventListener('resize', function() {
    console.log('resizing')
    d3.select('#prototype1').style('height', $(window).height()-220+'px');
    test.update(0);
  });
});
