(function(undefined) {
  "use strict";
  // ALL the prototype functions.
  // Public functions.

  gvis.prototype.data = function(newData) {
    newData = gvis.data.refineDataName(newData);
    rawData = gvis.utils.clone(newData);
  }


}).call(this)