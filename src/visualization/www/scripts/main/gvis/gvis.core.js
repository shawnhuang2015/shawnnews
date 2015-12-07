/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
(function(undefined) {
  "use strict";
  // ALL the prototype functions.
  // Public functions.

  console.log('Loading gvis.core')

  gvis.prototype.data = function(newData) {
    var _this = this.scope;
  }

  gvis.prototype.addNode = function(newNode) {
    var _this = this.scope;

    _this.graph.addNode(newNode);
  }

  gvis.prototype.addLink = function(newLink) {
    var _this = this.scope;

    _this.graph.addLink(newLink);
  }

  gvis.prototype.dropNode = function(node) {
    var _this = this.scope;

    _this.graph.dropNode(node.type, node.id);
  }

  gvis.prototype.dropLink = function(link) {
    var _this = this.scope;

    _this.graph.dropLink(link.source.type, link.source.id, link.target.type, link.target.id, link.type, true);
  }

  gvis.prototype.read = function(data) {
    var _this = this.scope;
    _this.graph.read(data);

    console.log(_this.graph.data())
  }

  gvis.prototype.render = function() {
    var _this = this.scope;
    _this.layouts.random();
    _this.renderer.update();
    _this.renderer.autoFit();
  }

  gvis.prototype.update = function(duration) {
    var _this = this.scope
    _this.renderer.update(duration);
  }

  gvis.prototype.test = function() {
    var _this = this.scope;

    _this.graph.addNode({id:0,type:0});
    _this.graph.addNode.call(window, {id:0,type:0});
  }
}).call(this)
  
