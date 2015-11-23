(function(undefined) {
  "use strict";

  console.log('Loading gvis.settings')

  var settings = {
    nodeKeyConcChar : '-'
    ,linkKeyConcChar : '_'
    ,attrs: '_attrs'
    ,styles: '_styles'
    ,key: '_key'
  }

  gvis.settings = gvis.utils.extend(gvis.settings || {}, settings);
}).call(this)