(function(undefined) {
  "use strict";
  // Doing something for behaviors for nodes and links by using customized configuration. Such as style structure, pre definition of the style base on attributes.
  // highlighting, coloring, hiding behaviors.
  console.log('Loading gvis.behaviors')

  gvis.behaviors = gvis.behaviors || {};

  gvis.behaviors.render = {
    nodeRadius : 20,
  }

  gvis.behaviors.icons = {
    usr : 'user',
    movie : 'movie'
  }

}).call(this)