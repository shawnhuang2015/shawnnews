(function(undefined) {

  var gvis = function() {
    var self = this;

    console.log(self);
  }

  this.gvis = gvis;
}).call(this)