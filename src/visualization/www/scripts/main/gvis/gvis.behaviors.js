/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
(function(undefined) {
  "use strict";
  // Doing something for behaviors for nodes and links by using customized configuration. Such as style structure, pre definition of the style base on attributes.
  // highlighting, coloring, hiding behaviors.
  console.log('Loading gvis.behaviors')

  gvis.behaviors = gvis.behaviors || {};

  gvis.behaviors.render = {
    linkMarker : 'marker-end', // marker-mid marker-end
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

    nodeBackgroundFilter : 'black-glow', // black-glow, glow, shadow
    linkBackgroundFilter : 'shadow',

    legendNodeRadius : 15,
    legendNodeRadiusMargin : 3,

    nodeLabelsFontSize : 15,
    linkLabelsFontSize : 13,

    linkStrokeWidth : 2,
    linkStrokeOpacity : 0.6,

    highlightColor : '#ff0000',
    nodeHighlightStrokeWidth : 5,
    nodeHighlightStrokeOpacity : 0.6,
    linkHighlightStrokeWidth : 15,
    linkHighlightStrokeOpacity : 0.6,


  }

  gvis.behaviors.icons = {
    usr : 'user',
    movie : 'card',
    tag : 'bank',
    phone : 'phone',    
    bankcard : 'card',
    user : 'user',
    client : 'bank',
    merchant : '',
    transaction : 'transaction'
  }

  gvis.behaviors.style = gvis.behaviors.style || {};

  gvis.behaviors.style.node = {
    labels : {

    },
    fill : '#f00'

  }

  gvis.behaviors.style.link = {
    labels : {

    },
    "stroke" : '#000',
    "stroke-dasharray" :  [0.5, 0.5]

  }

  gvis.behaviors.style.initializeNode = function(node) {
    for (var key in gvis.behaviors.style.node) {
      node[gvis.settings.styles] = node[gvis.settings.styles] || {};
      node[gvis.settings.styles][key] = node[gvis.settings.styles][key] || gvis.utils.clone(gvis.behaviors.style.node[key]);
    }
  }


  gvis.behaviors.style.initializeLink = function(link) {
    for (var key in gvis.behaviors.style.link) {
      link[gvis.settings.styles] = link[gvis.settings.styles] || {};
      link[gvis.settings.styles][key] = link[gvis.settings.styles][key] || gvis.utils.clone(gvis.behaviors.style.link[key]);
    }
  }

  gvis.behaviors.style.nodeToolTips = {
    'default' : function(type, id, attrs) {
      var template = '<span style="color:{{color}}">{{key}}</span>:{{value}}<br />'; 

      var result = '';

      result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'id', value:type});
      result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'type', value:id})

      for (var key in attrs) {
        result += gvis.utils.applyTemplate(template, {color:'#99d8c9', key:key, value:attrs[key]})
      }
      
      return result;
    },
    'customized' : undefined
  } 

  gvis.behaviors.style.linkToolTips = {
    'default' : function(type, attrs) {
      var template = '<span style="color:{{color}}">{{key}}</span>:{{value}}<br />'; 

      var result = '';

      //result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'id', value:d.id});
      result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'type', value:type})

      for (var key in attrs) {
        result += gvis.utils.applyTemplate(template, {color:'#99d8c9', key:key, value:attrs[key]})
      }
      
      return result;
    },
    'customized' : undefined
  }
}).call(this)
