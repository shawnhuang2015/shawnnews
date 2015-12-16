/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
(function(undefined) {
  "use strict";
  console.log('Loading gvis')

  var __instances = 0;

  var gvis = function(conf) {

    //local data object.
    var _this = {};

    var version = '0.1.2'

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
    _this.conf = conf;

    _this.graph = new gvis.graph();
    _this.layouts = new gvis.layouts(_this);
    
    //_this.events = {};

    _this.behaviors = gvis.behaviors;
    _this.renderer = new gvis.renders(_this);
  }

  this.gvis = gvis;
}).call(this)
