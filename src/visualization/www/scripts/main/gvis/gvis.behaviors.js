(function(undefined) {
  "use strict";
  // Doing something for behaviors for nodes and links by using customized configuration. Such as style structure, pre definition of the style base on attributes.
  // highlighting, coloring, hiding behaviors.
  console.log('Loading gvis.behaviors')

  gvis.behaviors = gvis.behaviors || {};

  gvis.behaviors.render = {
    viewportBackgroundColor : '#fafafa',
    viewportBackgroundOpacity : 1,
    
    nodeRadius : 20,
    nodeRadiusMargin : 4,
    nodeMaskRadius : 19,

    nodeBackgroundFillColor : '#fafafa',
    nodeBackgroundFillOpacity : 1,
    nodeBackgroundStrokeColor : 'black',
    nodeBackgroundStrokeOpacity : 0.5,
    nodeBackgroundStrokeWidth : 0.3,

    legendNodeRadius : 15,
    legendNodeRadiusMargin : 3
  }

  gvis.behaviors.icons = {
    usr : 'user',
    movie : 'movie'
  }

}).call(this)