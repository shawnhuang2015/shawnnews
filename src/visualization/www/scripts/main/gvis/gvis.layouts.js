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

    this._layoutName = 'random';

    this.addLayout('random', function() {
      graph.data().array.nodes.forEach(function(n) {
        n.x = Math.random();
        n.y = Math.random();
      })
    });


    this.addLayout('tree', function() {
      var size = this.createTreeBFS();

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = 0

      var treeNodes = d3.layout.tree()
      .size([size[0] * gvis.settings.domain_width / 4.0, size[1] * gvis.settings.domain_height / 4.0])
      .separation(function(a, b) {
        return (a.parent == b.parent ? 1 : 2);
      })
      .nodes(this._tree)

      treeNodes.forEach(function(node) {
        node.node.x = node.x;
        node.node.y = node.y;
      })
    });

    this.addLayout('DFStree', function() {
      var size = this.createTreeDFS();

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = 0

      var treeNodes = d3.layout.tree()
      .size([size[0] * gvis.settings.domain_width / 4.0, size[1] * gvis.settings.domain_height / 4.0])
      .separation(function(a, b) {
        return (a.parent == b.parent ? 1 : 2);
      })
      .nodes(this._tree)

      treeNodes.forEach(function(node) {
        node.node.x = node.x;
        node.node.y = node.y;
      })
    });

    this.addLayout('circle', function() {
      var size = this.createTreeBFS();
      console.log(size);

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = gvis.settings.domain_height / 2.0;

      var treeNodes = d3.layout.tree()
      .size([360, size[1] * gvis.settings.domain_height / 4.0])
      .separation(function(a, b) {
        return (a.parent == b.parent ? 1 : 2) / a.depth / a.depth;
      })
      .nodes(this._tree)

      treeNodes.forEach(function(node) {
        var position = gvis.utils.rotate(0, 0, node.y, 0, node.x-Math.PI/2.0)
        node.node.x = position[0];
        node.node.y = position[1];
      })
    })
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

  gvis.layouts.prototype.runLayout = function(layoutName) {
    layoutName = layoutName ||  this._layoutName;
    this._layoutName = layoutName;

    try {
      if (!this[layoutName]) {
        this.tree();
        throw 'layout ' + layoutName + ' does not exist.'  
      }
      this[layoutName]();
    }
    catch (err){
      console.log(err);
    }
  }

  // Breadth First Traverse
  gvis.layouts.prototype.createTreeBFS = function() {
    var maxDepth = 0;
    var maxWidth = 0;

    if (this._rootNodeKey === '' || !this._rootNodeKey) {
      this._rootNodeKey = this._graph.nodes()[0][gvis.settings.key];
    }

    this._graph.nodes().forEach(function(node) {
      node[gvis.settings.iterated] = false;
    })

    var rootNode = this._graph.nodes(this._rootNodeKey);
    rootNode[gvis.settings.iterated] = true;

    this._tree = {"children":[], "node":rootNode};
    rootNode[gvis.settings.children] = [];

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
            node[gvis.settings.children] = [];

            parent.children.push(treeNode);
            parent.node[gvis.settings.children].push(node);

            nextDepthParents.push(treeNode);
          }
        }

        for (var key in inlinks) {
          var node = inlinks[key][Object.keys(inlinks[key])[0]].source || {};
          if (!node[gvis.settings.iterated]) {
            node[gvis.settings.iterated] = true;

            var treeNode = {"children":[], "node":node, "depth":depth};
            node[gvis.settings.children] = [];

            parent.children.push(treeNode);
            parent.node[gvis.settings.children].push(node);

            nextDepthParents.push(treeNode);
          }
        }
      })

      maxWidth = maxWidth > nextDepthParents.length ? maxWidth : nextDepthParents.length;
      if (!nextDepthParents.length) {
        maxDepth = maxDepth > depth ? maxDepth : depth;
        return;
      }
      
      iterateTree.call(this, nextDepthParents, depth);
    }

    iterateTree.call(this, [this._tree], 0)

    console.log([maxWidth-1, maxDepth-1])
    return [maxWidth-1, maxDepth-1] 
  }

  // Depth First Traverse
  gvis.layouts.prototype.createTreeDFS = function() {
    var maxDepth = 0;
    var maxWidth = 0;

    if (this._rootNodeKey === '' || !this._rootNodeKey) {
      this._rootNodeKey = this._graph.nodes()[0][gvis.settings.key];
    }

    this._graph.nodes().forEach(function(node) {
      node[gvis.settings.iterated] = false;
    })

    var rootNode = this._graph.nodes(this._rootNodeKey);
    rootNode[gvis.settings.iterated] = true;

    this._tree = {"children":[], "node":rootNode};
    rootNode[gvis.settings.children] = [];

    function iterateTree (parents, depth) {
      depth += 1;

      var _graph = this._graph;
      var _this = this;

      var tempMax = 0;

      parents.forEach(function(parent) {
        var outlinks = _graph.outgoingLinks(parent.node[gvis.settings.key]);
        var inlinks = _graph.incomingLinks(parent.node[gvis.settings.key]);

        for (var key in outlinks) {
          var node = outlinks[key][Object.keys(outlinks[key])[0]].target || {};
          if (!node[gvis.settings.iterated]) {
            node[gvis.settings.iterated] = true;

            var treeNode = {"children":[], "node":node, "depth":depth};
            node[gvis.settings.children] = [];

            parent.children.push(treeNode);
            parent.node[gvis.settings.children].push(node);
          }
        }

        for (var key in inlinks) {
          var node = inlinks[key][Object.keys(inlinks[key])[0]].source || {};
          if (!node[gvis.settings.iterated]) {
            node[gvis.settings.iterated] = true;

            var treeNode = {"children":[], "node":node, "depth":depth};
            node[gvis.settings.children] = [];

            parent.children.push(treeNode);
            parent.node[gvis.settings.children].push(node);
          }
        }

        tempMax += parent.children.length;

        if (!parent.children.length) {
          maxDepth = maxDepth > depth ? maxDepth : depth;
          return;
        }
        
        iterateTree.call(_this, parent.children, depth);
      })

      maxWidth = maxWidth > tempMax ? maxWidth : tempMax;
    }

    iterateTree.call(this, [this._tree], 0)

    console.log([maxWidth-1, maxDepth-1])
    return [maxWidth-1, maxDepth-1] 
  }

  gvis.layouts.prototype.setRootNode = function(type, id) {
    try {
      var node = this._graph.nodes(type, id);
      this._rootNodeKey = node[gvis.settings.key]
    }
    catch (err) {
      console.log(err);
    }
  }

  // gvis.layouts.prototype.force = function() {
  //   var force = d3.layout.force()
  //   .nodes(this.graph.data().array.nodes)
  //   .links(this.graph.data().array.links)
  //   .size([1, 1])
  //   .linkStrength(0.1)
  //   .friction(0.9)
  //   .linkDistance(0.1)
  //   .charge(-30)
  //   .gravity(0.1)
  //   .theta(0.8)
  //   .alpha(0.1)

  //   force.start();
  //   for (var i = 0; i < 20; ++i) force.tick();
  //   force.stop();
  // }
}).call(this)
