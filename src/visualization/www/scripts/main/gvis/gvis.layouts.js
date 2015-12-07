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
    this._graph = graph;
    this._tree = {};
    this._rootNodeKey = '';

    this.addLayout('random', function() {
      graph.data().array.nodes.forEach(function(n) {
        n.x = Math.random();
        n.y = Math.random();
      })
    });


    this.addLayout('tree', function() {
      this.createTree();

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = 0

      var tree = d3.layout.tree()
      .size([1, 1])
      .separation(function(a, b) {
        return (a.parent == b.parent ? 1 : 2);
      })
      .nodes(this._tree)

      tree.forEach(function(node) {
        node.node.x = node.x;
        node.node.y = node.y;
      })

    });
  }

  gvis.layouts.prototype.addLayout = function(layoutName, layoutFn) {
    if ( typeof layoutName !== 'string' ||
      typeof layoutFn !== 'function' ||
      arguments.length !== 2) {
      throw 'addLayout: Wrong arguments.'
    }

    if (this[layoutName]) {
      throw 'layout ' + layoutName + ' already exists.'  
    }

    this[layoutName] = layoutFn;

    return this;
  }

  gvis.layouts.prototype.createTree = function() {
    if (this._rootNodeKey === '' || !this._rootNodeKey) {
      this._rootNodeKey = this._graph.nodes()[0][gvis.settings.key];
    }

    this._graph.nodes().forEach(function(node) {
      node[gvis.settings.iterated] = false;
    })

    var rootNode = this._graph.nodes(this._rootNodeKey);
    rootNode[gvis.settings.iterated] = true;

    this._tree = {"children":[], "node":rootNode}

    function iterateTree (parents, depth) {
      depth += 1;

      var _graph = this._graph;

      var nextDepthParents = []

      parents.forEach(function(parent) {
        var outlinks = _graph.outgoingLinks(parent.node[gvis.settings.key]);
        var inlinks = _graph.incomingLinks(parent.node[gvis.settings.key]);

        for (var key in outlinks) {
          var node = outlinks[key][Object.keys(outlinks[key])[0]].target || {};
          if (!node[gvis.settings.iterated]) {
            node[gvis.settings.iterated] = true;

            var treeNode = {"children":[], "node":node, "depth":depth};
            parent.children.push(treeNode);
            nextDepthParents.push(treeNode);
          }
        }

        for (var key in inlinks) {
          var node = inlinks[key][Object.keys(inlinks[key])[0]].source || {};
          if (!node[gvis.settings.iterated]) {
            node[gvis.settings.iterated] = true;

            var treeNode = {"children":[], "node":node, "depth":depth};
            parent.children.push(treeNode);
            nextDepthParents.push(treeNode);
          }
        }
      })

      if (!nextDepthParents.length) {
        return;
      }
      
      iterateTree.call(this, nextDepthParents, depth);
    }

    iterateTree.call(this, [this._tree], 0)
  }

  gvis.layouts.prototype.setRootNode = function(type, id) {
    try {
      var node = this.graph.nodes(type, id);
      this.rootNodeKey = node[gvis.settings.key]
    }
    catch (err) {
      console.log(err);
    }
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
}).call(this)
