(function(undefined) {
  "use strict";
  console.log('Loading gvis')

  var __instances = 0;

  var gvis = function(conf) {

    //local data object.
    var _this = {};

    var version = '0.2.0'

    //return the version information.
    Object.defineProperty(this, 'version', {
      get: function() { return 'gvis version('+ version + ')'; },
      set: function(newValue) { },
    })

    //Retuen local data object _this.
    Object.defineProperty(this, 'scope', {
      get: function() { return _this; },
      set: function(newValue) {
        try {
          throw new Error("scope is read-only.")
        }
        catch (err) {
          console.log(err)
        }
      }
    })

    _this.settings = gvis.settings;
    _this.settings.conf = conf;

    _this.graph = new gvis.graph();
    _this.behaviors = {};
    _this.events = {};
    _this.renders = {};
    _this.layouts = {};
   
  }

  this.gvis = gvis;

}).call(this)