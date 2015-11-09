(function(undefined) {
  "use strict";
  console.log('Loading gvis')

  var __instances = 0;

  var gvis = function(conf) {
    var self = this,
        _conf = conf || {};

    self.version = '0.2.0';

    var data = {"nodes":[], "links":[]};

    console.log(self);
  }

  this.gvis = gvis;

}).call(this)