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
    _this.layouts.random();

    console.log(_this.graph.data())

    return this
  }

  gvis.prototype.layout = function(layoutName) {
    var _this = this.scope;

    _this.layouts.runLayout(layoutName);

    return this
  }

  gvis.prototype.addLayout = function(layoutName, Fn) {
    var _this = this.scope;

    _this.layouts.addLayout(layoutName, Fn);

    return this
  }

  gvis.prototype.render = function() {
    var _this = this.scope;

    _this.renderer.update();
    _this.renderer.autoFit();

    return this
  }

  gvis.prototype.update = function(duration, delay) {
    var _this = this.scope

    _this.renderer.update(duration, delay);

    return this
  }

  gvis.prototype.test = function() {
    var _this = this.scope;

    _this.graph.addNode({id:'0',type:'0'});
    _this.graph.addNode.call(window, {id:'0',type:'0'});
  }
}).call(this)
  
