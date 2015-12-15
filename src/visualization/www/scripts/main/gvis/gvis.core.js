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

    _this.layouts.setLayout(layoutName);

    return this
  }

  gvis.prototype.addLayout = function(layoutName, Fn) {
    var _this = this.scope;

    _this.layouts.addLayout(layoutName, Fn);

    return this
  }

  gvis.prototype.render = function(total_time, between_delay, init_delay) {
    var _this = this.scope;
    
    total_time = total_time || 1000;
    between_delay = between_delay || 500;
    init_delay = init_delay || 0;

    _this.renderer.update(0, 0);
    
    _this.renderer.render(total_time, between_delay, init_delay);
    
    return this
  }

  gvis.prototype.update = function(duration, delay) {
    var _this = this.scope

    _this.renderer.update(duration, delay);

    return this
  }

  gvis.prototype.showNodeLabel = function(type, id, labels) {
    var _this = this.scope;

    var node = _this.graph.nodes(type, id)

    for (var key in node[gvis.settings.styles].labels) {
      node[gvis.settings.styles].labels[key] = false;
    }

    for (var i in labels) {
      var label = labels[i];

      node[gvis.settings.styles].labels[label] = true;
    }

    return this;
  }

  gvis.prototype.showNodeLabelByType = function(type, labels) {

    var _this = this.scope;

    var nodes = _this.graph.nodes();

    for (var i in nodes) {
      var node = nodes[i];

      if (node.type == type) {
        this.showNodeLabel(node.type, node.id, labels);
      }
    }

    return this;
  }

  gvis.prototype.hideNodeLabel = function(type, id, labels) {
    var _this = this.scope;

    var node = _this.graph.nodes(type, id)

    for (var i in labels) {
      var label = labels[i];

      node[gvis.settings.styles].labels[label] = false;
    }

    return this;
  }

  gvis.prototype.hideNodeLabelByType = function(type, labels) {

    var _this = this.scope;

    var nodes = _this.graph.nodes();

    for (var i in nodes) {
      var node = nodes[i];

      if (node.type == type) {
        this.hideNodeLabel(node.type, node.id, labels);
      }
    }

    return this;
  }

  gvis.prototype.showLinkLabel = function(sourceType, sourceID, targetType, targetID, linkType, labels) {
    var _this = this.scope;

    var link = _this.graph.links(sourceType, sourceID, targetType, targetID, linkType);

    for (var key in link[gvis.settings.styles].labels) {
      link[gvis.settings.styles].labels[key] = false;
    }

    for (var i in labels) {
      var label = labels[i];

      link[gvis.settings.styles].labels[label] = true;
    }
  }

  gvis.prototype.showLinkLabelByType = function(linkType, labels) {
    var _this = this.scope;

    var links = _this.graph.links();

    for (var i in links) {
      var link = links[i];

      if (link.type == linkType) {
        this.showLinkLabel(link.source.type, link.source.id, link.target.type, link.target.id, link.type, labels);
      }
    }
  }

  gvis.prototype.hideLinkLabel = function(sourceType, sourceID, targetType, targetID, linkType, labels) {
    var _this = this.scope;

    var link = _this.graph.links(sourceType, sourceID, targetType, targetID, linkType);

    for (var i in labels) {
      var label = labels[i];

      link[gvis.settings.styles].labels[label] = false;
    }
  }

  gvis.prototype.hideLinkLabelByType = function(linkType, labels) {
    var _this = this.scope;

    var links = _this.graph.links();

    for (var i in links) {
      var link = links[i];

      if (link.type == linkType) {
        this.hideLinkLabel(link.source.type, link.source.id, link.target.type, link.target.id, link.type, labels);
      }
    }
  }

  gvis.prototype.setNodeToolTipFormat = function(fn) {
    gvis.behaviors.style.nodeToolTips.customized = fn;

    return this
  }

  gvis.prototype.setLinkToolTipFormat = function(fn) {
    gvis.behaviors.style.linkToolTips.customized = fn;

    return this
  }

  gvis.prototype.on = function(event, fn) {
    var _this = this.scope;
    _this.renderer.setEventHandler(event, fn);

    return this;
  }

  gvis.prototype.test = function() {
    var _this = this.scope;

    _this.graph.addNode({id:'0',type:'0'});
    _this.graph.addNode.call(window, {id:'0',type:'0'});
  }
}).call(this)
  
