(function(undefined) {
  "use strict";
  // ALL the prototype functions.
  // Public functions.

  console.log('Loading gvis.core')


  gvis.prototype.data = function(newData) {
    if (arguments.length) {
      return this.data;
    }
    else {
      newData = gvis.data.initialization(newData);
      var rawData = gvis.utils.clone(newData);
    }
  }


}).call(this)