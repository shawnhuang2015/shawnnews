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

    var handlers = {};

    this.setEventHandler = function(eventName, fn) {
      handlers[eventName] = fn;
    }

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

    this.dragging = function(draggingNodeData) {

      var nodes = _svg.renders.graph.nodes().filter(function(d) {
        return d.selected;
      })

      for (var i in nodes) {
        var d = nodes[i];
        d.x += _svg.renders.xScale.invert(d3.event.dx);
        d.y += _svg.renders.yScale.invert(d3.event.dy);

        var node_key = d[gvis.settings.key]
        var node = d3.select('#node_'+node_key);

        _svg.updateNodeRenderer(node, _svg)

        var graph = _svg.renders.graph.data();

        for (var other_key in graph.neighbors.all[node_key]) {
          for (var link_key in graph.neighbors.all[node_key][other_key]) {
            var link= d3.select('#link_'+link_key)

            _svg.updateLinkRenderer(link, _svg)
          }
        }
      }

            
      // remove tipsy
      d3.selectAll('.tipsy').remove();
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
      _svg.unselectAllElements();
    }

    this.brush = function() {
      var extent = d3.event.target.extent();
      
      var translate = _svg.zoom.translate();
      var scale = _svg.zoom.scale();

      translate[0] = _svg.renders.xScale.invert(translate[0] / scale)
      translate[1] = _svg.renders.yScale.invert(translate[1] / scale)

      extent[0][0] = extent[0][0] / scale - translate[0]
      extent[0][1] = extent[0][1] / scale - translate[1]
      extent[1][0] = extent[1][0] / scale - translate[0]
      extent[1][1] = extent[1][1] / scale - translate[1]

      _svg.g_svg.selectAll('.node')
      .each(function(d) {
        d.preSelected = d.selected = (extent[0][0] <= d.x && d.x < extent[1][0]
                && extent[0][1] <= d.y && d.y < extent[1][1]);
      })

      _svg.updateSelectedNodes();      
    }

    this.brushend = function() {
      _svg.g_svg.selectAll('.link')
      .each(function(d) {
        d.preSelected = d.selected = d.source.selected && d.target.selected;
      })

      _svg.updateSelectedLinks();

      // var extent = d3.event.target.extent();
      
      // var translate = _svg.zoom.translate();
      // var scale = _svg.zoom.scale();

      // translate[0] = _svg.renders.xScale.invert(translate[0] / scale)
      // translate[1] = _svg.renders.yScale.invert(translate[1] / scale)

      // extent[0][0] = extent[0][0] / scale - translate[0]
      // extent[0][1] = extent[0][1] / scale - translate[1]
      // extent[1][0] = extent[1][0] / scale - translate[0]
      // extent[1][1] = extent[1][1] / scale - translate[1]

      var nodes = _svg.renders.graph.nodes().filter(function(d) {
        return d.selected;
      })

      var links = _svg.renders.graph.links().filter(function(d) {
        return d.selected;
      })

      if(!handlers['multiSelect']) {
        console.log('multiSelect Events :')
      }
      else {
        handlers['multiSelect'](nodes, links, _svg);
      }

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

        _svg.renders.render(500, 500); 
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

        _svg.renders.render(500, 500); 
        // _svg.update()
        // _svg.autoFit(500)
      }
      // key '1'
      else if (d3.event.keyCode == 49) {
        _svg.renders._this.layouts.setLayout('circle');
        _svg.renders.render(500, 500); 
      }
      // key '2'
      else if (d3.event.keyCode == 50) {
        _svg.renders._this.layouts.setLayout('tree');
        _svg.renders.render(500, 500); 
      }
      // key '3'
      else if (d3.event.keyCode == 51) {
        _svg.renders._this.layouts.setLayout('DFStree');
        _svg.renders.render(500, 500);      
      }
      // key '4'
      else if (d3.event.keyCode == 52) {
        _svg.renders._this.layouts.setLayout('cluster');
        _svg.renders.render(500, 500);   
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

    this.nodeClick = function(d) {
      var that = this;
      var d = d;

      setTimeout(function() {
          var dblclick = parseInt($(that).data('doubleClickCount'), 10);

          if (dblclick > 0) {
              $(that).data('doubleClickCount', dblclick-1);
          } else {
            if(!handlers['nodeClick']) {
              console.log('node click')
            }
            else {
              handlers['nodeClick'](d, _svg);
            }
          }
      }, 200);
    }

    this.nodeDblClick = function(d) {
      $(this).data('doubleClickCount', 2);

      if(!handlers['nodeDblClick']) {
        console.log('node DblClick')
      }
      else {
        handlers['nodeDblClick'](d, _svg);
      }
    }   

    this.linkClick = function(d) {
      var that = this;
      var d = d;

      setTimeout(function() {
          var dblclick = parseInt($(that).data('doubleClickCount'), 10);

          if (dblclick > 0) {
              $(that).data('doubleClickCount', dblclick-1);
          } else {
            if(!handlers['linkClick']) {
              console.log('linkClick')
            }
            else {
              handlers['linkClick'](d, _svg);
            }
          }
      }, 200);

    }

    this.linkDblClick = function(d) {
      $(this).data('doubleClickCount', 2);

      if(!handlers['linkDblClick']) {
        console.log('link DblClick')
      }
      else {
        handlers['linkDblClick'](d, _svg);
      }
    }
  }
}).call(this)
