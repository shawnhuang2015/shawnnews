(function(undefined) {
  "use strict";
  console.log('Loading gvis.data')

  gvis.data = gvis.data || {};

  gvis.data.graph = function(data) {

    this.graph = "updated Graph."

    this.data = function() {
      return data;
    }
  }


}).call(this)