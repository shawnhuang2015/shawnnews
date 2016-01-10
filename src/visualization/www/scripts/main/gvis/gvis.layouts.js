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

  var force = d3.layout.force()
  .gravity(.1)
  .distance(0.005)
  .charge(-0.02)
  .size([1, 1]);

  gvis.layouts = function(_this) {
    this._this = _this;

    this._graph = _this.graph;
    this._tree = {};
    this._rootNodeKey = '';

    this._layoutName = 'random';

    this.addLayout('random', function() {
      this._graph.data().array.nodes.forEach(function(n) {
        n.x = Math.random();
        n.y = Math.random();
      })
    });

    this.addLayout('allZero', function() {
      this._graph.data().array.nodes.forEach(function(n) {
        n.x = 0
        n.y = 0
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

    this.addLayout('treemap', function() {
      var size = this.createTreeBFS();

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = 0

      var treeNodes = d3.layout.treemap().value(function(d) {return 1*d.children.length;})
      .size([size[0] * gvis.settings.domain_width / 4.0, size[1] * gvis.settings.domain_height / 4.0])
      .nodes(this._tree)

      treeNodes.forEach(function(node) {
        node.node.x = !node.parent ? 0 : node.parent.node.x + node.parent.dx * 0.3
        node.node.y = !node.parent ? 0 : node.parent.node.y + node.y - node.parent.y + node.dy * 0.5
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

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = gvis.settings.domain_height / 2.0;

      var treeNodes = d3.layout.tree()
      .size([360, size[1] * gvis.settings.domain_height / 4.0])
      .separation(function(a, b) {
        return (a.parent == b.parent ? 1 : 2) / a.depth / a.depth;
      })
      .nodes(this._tree)

      treeNodes.forEach(function(node, i) {
        node.x = isNaN(node.x) ? 0 : node.x;
        node.y = isNaN(node.y) ? i * 30: node.y;

        var position = gvis.utils.rotate(0, 0, node.y, 0, node.x-Math.PI/2.0)
        node.node.x = position[0] + gvis.settings.domain_width / 2.0;
        node.node.y = position[1] + gvis.settings.domain_height / 2.0;
      })
    })

    this.addLayout('moreLayoutIfNeeded', function() {
      var size = this.createTreeBFS();

      var rootNode = this._graph.nodes(this._rootNodeKey);
      rootNode.x = gvis.settings.domain_width / 2.0;
      rootNode.y = 0

      var treeNodes = [];

      treeNodes.forEach(function(node) {
        node.node.x = node.x;
        node.node.y = node.y;
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

  gvis.layouts.prototype.setLayout = function(layoutName) {
    layoutName = layoutName ||  this._layoutName;
    this._layoutName = layoutName;
  }

  gvis.layouts.prototype.runLayout = function(layoutName) {
    layoutName = layoutName ||  this._layoutName;
    this._layoutName = layoutName;

    if (!this[layoutName]) {
      return this.tree();
      throw 'layout ' + layoutName + ' does not exist.'  
    }
    else {
      return this[layoutName]();
    }
  }

  gvis.layouts.prototype.runLayoutIteratively = function(duration) {
    if (!this[this._layoutName]) {
      return this.tree();
      throw 'layout ' + layoutName + ' does not exist.'  
    }
    else {
      return this[this._layoutName](duration);
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

    this._tree = {"children":[], "node":rootNode, "depth":0};
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

    //console.log([maxWidth-1, maxDepth-1])
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

    //console.log([maxWidth-1, maxDepth-1])
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

  gvis.layouts.prototype.force = function(duration) {

    var _this = this;

    force
    .nodes(this._graph.nodes())
    .links(this._graph.links())

    var start = new Date().getTime();
    var end = new Date().getTime();
    var time = end - start;

    force.start();

    while (time < duration) {
      end = new Date().getTime();
      time = end - start;
      force.tick();
    }

    force.stop();

    return false;
  }
}).call(this)
