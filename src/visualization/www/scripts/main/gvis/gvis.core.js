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

  gvis.prototype.test = function() {
    var _this = this.scope;

    _this.graph.addNode({id:0,type:0});
    _this.graph.addNode.call(window, {id:0,type:0});
  }

  
}).call(this)