/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
(function(undefined) {
  "use strict";
  // Doing somthing for events which can be customized.
  console.log('Loading gvis.events')

  gvis.events = gvis.events || {};


  gvis.events.svg = function(svg) {
    var _svg = svg;

    var shiftKey;
    var ctrlKey;
    var altKey;

    var currentKey;

    //node mouse down events.
    this.dragstarted = function() {
      d3.event.sourceEvent.stopPropagation();
      d3.select(this).classed("dragging", true);

      if (shiftKey) {

      }
      else if(altKey) {

      }
      else {
        _svg.g_svg.selectAll('.node')
        .each(function(d) {
          d.preSelected = d.selected = false;
        })

        d3.select(this)
        .each(function(d) {
          d.preSelected = d.selected = true;
        })

        _svg.updateSelectedNodes();      

        _svg.g_svg.selectAll('.link')
        .each(function(d) {
          d.preSelected = d.selected = d.source.selected || d.target.selected;
        })

        _svg.updateSelectedLinks();
      }
    }

    this.dragging = function(d) {
      d.x += _svg.renders.xScale.invert(d3.event.dx);
      d.y += _svg.renders.yScale.invert(d3.event.dy);

      d3.select(this).attr("transform", function(d) { 
        return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
      })

      var node_key = d[gvis.settings.key]
      var graph = _svg.renders.graph.data();

      for (var other_key in graph.neighbors.all[node_key]) {
        for (var link_key in graph.neighbors.all[node_key][other_key]) {
          var link= d3.select('#link_'+link_key)

          _svg.updateLinkRenderer(link, _svg)
        }
      }
    }

    this.dragended = function() {
      d3.select(this).classed("dragging", false);
    }

    this.linkmousedown = function(d) {
      if (shiftKey) {

      }
      else if(altKey) {

      }
      else {
        _svg.g_svg.selectAll('.link')
        .each(function(d) {
          d.preSelected = d.selected = false;
        })

        d3.select(this)
        .each(function(d) {
          d.preSelected = d.selected = true;
        })

        _svg.updateSelectedLinks();      
      }
    }

    this.brushstart = function() {
      console.log('brush start')
    }

    this.brush = function() {
      var extent = d3.event.target.extent();
      console.log(extent[1]);
    }

    this.brushend = function() {
      d3.event.target.clear();
      d3.select(this).call(d3.event.target);
    }

    this.keydown = function() {
      shiftKey = d3.event.shiftKey;
      ctrlKey = d3.event.ctrlKey;
      altKey = d3.event.altKey;
      currentKey = d3.event.keyCode;

      console.log('d3.event.keyCode down: ', d3.event.keyCode);

      if (shiftKey) {
        _svg.g_svg.call(_svg.zoom)
        .on("dblclick.zoom", null)
        .on("mousedown.zoom", null)
        .on("touchstart.zoom", null)
        .on("touchmove.zoom", null)
        .on("touchend.zoom", null);

        _svg.g_svg.selectAll('.node_container')
        .on('mousedown.drag', null);

        _svg.g_brush.select('.background').style('cursor', 'crosshair')
        _svg.g_brush.call(_svg.brush);
      }

      // key 'f'
      if (d3.event.keyCode == 70) {
        _svg.autoFit(500);
      }
      // key 'c'
      else if (d3.event.keyCode == 67) {
        _svg.centerView(500);
      }
      // key 'r'
      else if (d3.event.keyCode == 82) {

        _svg.g_svg
        .selectAll('.node')
        .each(function(d) {
          if (d.selected) {
            _svg.renders._this.layouts.setRootNode(d.type, d.id);
          }

          return d.selected;
        })

        _svg.renders._this.layouts.runLayout();
        _svg.update()
        _svg.autoFit(500)
      }
      // key 'd'
      else if (d3.event.keyCode == 68) {
        _svg.g_svg
        .selectAll('.node')
        .each(function(d) {
          if (d.selected) {
            _svg.renders._this.graph.dropNode(d.type, d.id);
          }

          return d.selected;
        })

        _svg.update()
        _svg.autoFit(500)
      }
      // key '1'
      else if (d3.event.keyCode == 49) {
        _svg.renders._this.layouts.runLayout('circle');
        _svg.update()
        _svg.autoFit(500)
      }
      // key '2'
      else if (d3.event.keyCode == 50) {
        _svg.renders._this.layouts.runLayout('tree');
        _svg.update()
        _svg.autoFit(500)
      }
      // key '3'
      else if (d3.event.keyCode == 51) {
        _svg.renders._this.layouts.runLayout('DFStree');
        _svg.update()
        _svg.autoFit(500)        
      }
      // key '4'
      else if (d3.event.keyCode == 52) {
        _svg.renders._this.layouts.runLayout('cluster');
        _svg.update()
        _svg.autoFit(500)       
      }
      //key '5'
      else if (d3.event.keyCode == 53) {
        _svg.renders._this.layouts.setLayout('force');
        _svg.renders.render(5000, 1000);  
      }

    }

    this.keyup = function() {
      shiftKey = d3.event.shiftKey;
      ctrlKey = d3.event.ctrlKey;
      altKey = d3.event.altKey;

      currentKey = d3.event.keyCode;

      if (!shiftKey) {
        _svg.g_brush
        .call(_svg.brush)
        .on("mousedown.brush", null)
        .on("touchstart.brush", null)
        .on("touchmove.brush", null)
        .on("touchend.brush", null);

        _svg.g_brush.select('.background').style('cursor', 'auto');
        _svg.g_svg.call(_svg.zoom)
        .on("dblclick.zoom", null);

        _svg.g_svg.selectAll('.node_container')
        .call(_svg.drag)
      }

      console.log('d3.event.keyCode up: ', d3.event.keyCode);
    }

    this.contextmenu = function() {
      console.log('oncontextmenu _svg')
      d3.event.preventDefault();
    }

    this.zoomstart = function() {
      //d3.event.sourceEvent.stopPropagation();
      //d3.event.preventDefault();

      // console.log('zoomstart')
    }

    this.zoomed =  function() {
      _svg.g_zoomer.attr("transform", "translate(" + d3.event.translate + ")scale(" + d3.event.scale + ")");
    } 

    this.zoomend = function() {
      // d3.event.sourceEvent.preventDefault();

      // d3.event.sourceEvent.stopPropagation();
      // console.log('zoomend')
    }
  }
}).call(this)
