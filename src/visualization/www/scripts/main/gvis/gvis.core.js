(function(undefined) {

  var gvis = function() {
    var self = this;

    /**
    * The current version of gvis:
    */
    self.version = '0.2.0';

    /**
    * Here can be use for customizing every thing, base on the default behavior.
    *
    * @method customized
    * @param {Object} customized Argument 1
    */
    self.customized = function(customized) {
      // do something customized.
      // 
    }

    console.log(self);
  }

  gvis.prototype.data = function(newData) {
    gvis.data.refineDataName(newData);
  }

  this.gvis = gvis;

}).call(this)