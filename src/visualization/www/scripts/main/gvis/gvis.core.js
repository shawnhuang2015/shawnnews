(function(undefined) {
  "use strict";
  // ALL the prototype functions.
  // Public functions.

  console.log('Loading gvis.core')

  gvis.prototype.data = function(newData) {
    var _this = this.localScope();

    _this.graph = new gvis.data.graph(newData);
    _this = {test:'test'}
  }
  
}).call(this)