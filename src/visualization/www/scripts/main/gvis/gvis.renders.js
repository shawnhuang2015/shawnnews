/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
require(['ol'], function(ol){
  "use strict";
  // Doing somthing for renders
  console.log('Loading gvis.render')

  gvis.renders = function(_this) {

    this._this = _this; // gvis pointer.
    this.graph = _this.graph;

    this.render_container = _this.conf.container;
    this.render_type = !_this.conf.render_type ? 'svg' : _this.conf.render_type;    

    this.domain_width = gvis.settings.domain_width;
    this.domain_height = gvis.settings.domain_height;

    this.range_width = +d3.select('#'+this.render_container).style('width').slice(0, -2);
    this.range_height = +d3.select('#'+this.render_container).style('height').slice(0, -2);

    this.zoomRange = [0.1, 10];

    this.xScale = d3.scale.linear()
    .domain([0, this.domain_width])
    .range([0, this.range_width]);

    this.yScale = d3.scale.linear()
    .domain([0, this.domain_height])
    .range([0, this.range_height])


    switch (this.render_type) {
      case 'canvas':
        this.renderer = 'render ' + this.render_container + ' by using canvas. Coming soon.';
      break;
      case 'map':
        //this.renderer = 'render ' + this.render_container + ' by using map. Coming soon.';
        this.renderer = new gvis.renders.map(this);
      break;
      case 'svg':
      default:
        this.renderer = new gvis.renders.svg(this);
      break;
    }

    this.setEventHandler = function(eventName, fn) {
      this.renderer.events.setEventHandler(eventName, fn)
    }

    this.generateCurvedLinkPoints = function(link) {
      var a = 0.25;
      var b = gvis.behaviors.render.nodeRadius * 2; // initNodeSetting.r * 4;
      var edgeMargin = gvis.behaviors.render.nodeRadius * 1.2;
      var markMargin = 0//gvis.behaviors.render.nodeRadius / 10;

      var C0 = [0, 0]
      var C1 = [0, 0]

      var x0 = this.xScale(link.source.x)
      var y0 = this.yScale(link.source.y)
      var x1 = this.xScale(link.target.x)
      var y1 = this.yScale(link.target.y)

      var n = 0;
      var i = -1;

      var graph = this.graph.data();

      var source_key = link.source[gvis.settings.key];
      var target_key = link.target[gvis.settings.key];

      var link_key = link[gvis.settings.key];

      n = Object.keys(graph.neighbors.all[source_key][target_key]).length;

      i = Object.keys(graph.neighbors.out[source_key][target_key]).indexOf(link_key)

      if (n == 0 || i == -1) {
        return [];
      }
      else if (n == 2) {
        // customize two line case.
        n = 3;
        if (i == 1) {
          i = 2
        }
      }

      var t = (n-1-2*i) * b / 2;
      var td = Math.sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1));
      var t0 = a * (td - edgeMargin) + edgeMargin * 0.5;
      var t1 = (1-a) * (td - edgeMargin) + edgeMargin * 0.5;

      var angle = Math.atan2(y1-y0, x1-x0)*180/Math.PI;

      C0[0] = x0 + t0;
      C0[1] = y0 + t;
      C1[0] = x0 + t1;
      C1[1] = y0 + t;

      C0 = gvis.utils.rotate(x0, y0, C0[0], C0[1], angle);
      C1 = gvis.utils.rotate(x0, y0, C1[0], C1[1], angle);


      link._C0 = C0;
      link._C1 = C1;

      var tdc = Math.sqrt((C1[0]-x1)*(C1[0]-x1)+(C1[1]-y1)*(C1[1]-y1));

      var tx0 = x0 - edgeMargin * (x0 - C0[0]) / tdc
      var ty0 = y0 - edgeMargin * (y0 - C0[1]) / tdc;

      var tx1 = x1 - (edgeMargin+markMargin) * (x1 - C1[0]) / tdc
      var ty1 = y1 - (edgeMargin+markMargin) * (y1 - C1[1]) / tdc;

      if (td < 2*(edgeMargin+markMargin)) {
        return []
      }

      if (tdc < edgeMargin+markMargin) {
        C1[0] = x1 * 0.5 + x0 *0.5;
        C1[1] = y1 * 0.5 + y0 * 0.5;
        C0 = C1;
      }

      return [[tx0, ty0], C0, C1, [tx1, ty1]]
    }
  }

  gvis.renders.prototype.update = function(duration, delay) {
    duration = duration != undefined ? duration : 500;
    delay = delay != undefined ? delay : 0;

    this.renderer.update(duration, delay);

    return this;
  }

  gvis.renders.prototype.autoFit = function(duration, delay) {
    duration = duration != undefined ? duration : 500;
    delay = delay != undefined ? delay : 0;
    this.renderer.autoFit(duration, delay);

    return this
  }

  gvis.renders.prototype.render = function(total_time, between_delay, init_delay) {
    var _this = this;

    if (!this.graph.nodes().length) {
      this.update(); 
      return;
    }

    setTimeout(function() {
      var start = new Date().getTime();
      var end = new Date().getTime();
      var time = end - start;

      renderFunction();

      var myRender = setInterval(renderFunction, between_delay);

      function renderFunction() {
        var converged = _this._this.layouts.runLayoutIteratively(between_delay*0.2);

        _this.update(between_delay*0.8);
        _this.autoFit(300, between_delay*0.8);

        end = new Date().getTime();
        time = end - start;

        if (!!converged || time >= total_time) {
          window.clearInterval(myRender);
        }
      }
    }, init_delay);
  }

  gvis.renders.prototype.clear = function() {
    this.renderer.clear();
  }


  /********** renders sub class **********/

  /********** renders.svg **********/
  gvis.renders.svg = function(renders) {
    this.renders = renders;

    this.events = new gvis.events.svg(this);

    var _this = renders._this;

    var container_id = "#" + renders.render_container

    this.zoom = d3.behavior.zoom()
    .scaleExtent(renders.zoomRange)
    .on("zoomstart", this.events.zoomstart)
    .on("zoom", this.events.zoomed)
    .on("zoomend", this.events.zoomend)

    this.brush = d3.svg.brush()
    .x(this.renders.xScale)
    .y(this.renders.yScale)
    .on("brushstart", this.events.brushstart)
    .on("brush", this.events.brush)
    .on("brushend", this.events.brushend);

    this.drag = d3.behavior.drag()
    .origin(function(d) { return d; })
    .on("dragstart", this.events.dragstarted)
    .on("drag", this.events.dragging)
    .on("dragend", this.events.dragended);

    var svg = d3.select(container_id)
    .append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("tabindex", 1)
    .on("keydown", this.events.keydown)
    .on("keyup", this.events.keyup)
    .on("contextmenu", this.events.contextmenu)
    .call(this.zoom)
    .on("dblclick.zoom", null);

    var defs = svg.append("defs");

    defs.append("svg:clipPath")
    .attr("id", "node-mask")
    .append("svg:circle")
    .attr("r", gvis.behaviors.render.nodeMaskRadius-1)
    .attr("cx", 0)
    .attr("cy", 0);

    defs.append('g')
    .attr('id', 'filter_group')
    .html(function(){
      return '\
        <!-- a transparent grey drop-shadow that blends with the background colour -->\
        <filter id="filter_shadow" width="1.5" height="1.5" x="-.25" y="-.25">\
            <feGaussianBlur in="SourceAlpha" stdDeviation="2.5" result="blur"/>\
            <feColorMatrix result="bluralpha" type="matrix" values=\
                    "1 0 0 0   0\
                     0 1 0 0   0\
                     0 0 1 0   0\
                     0 0 0 0.4 0 "/>\
            <feOffset in="bluralpha" dx="3" dy="3" result="offsetBlur"/>\
            <feMerge>\
                <feMergeNode in="offsetBlur"/>\
                <feMergeNode in="SourceGraphic"/>\
            </feMerge>\
        </filter>\
        \
        <!-- a transparent grey glow with no offset -->\
        <filter id="filter_black-glow">\
            <feColorMatrix type="matrix" values=\
                        "0 0 0 0   0\
                         0 0 0 0   0\
                         0 0 0 0   0\
                         0 0 0 0.7 0"/>\
            <feGaussianBlur stdDeviation="2.5" result="coloredBlur"/>\
            <feMerge>\
                <feMergeNode in="coloredBlur"/>\
                <feMergeNode in="SourceGraphic"/>\
            </feMerge>\
        </filter>\
        \
        <!-- a transparent glow that takes on the colour of the object it\'s applied to -->\
        <filter id="filter_glow">\
            <feGaussianBlur stdDeviation="2.5" result="coloredBlur"/>\
            <feMerge>\
                <feMergeNode in="coloredBlur"/>\
                <feMergeNode in="SourceGraphic"/>\
            </feMerge>\
        </filter>\
        '
    })


    svg.append('rect')
    .classed('background_rect', true) 
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("fill", gvis.behaviors.render.viewportBackgroundColor)
    .attr("opacity", gvis.behaviors.render.viewportBackgroundOpacity)
    .attr("stroke", "transparent")

    var g_brush = svg.append('g')
    .classed('brush', true)
    .datum(function() {
      return {selected: false, preSlected: false};
    })
    .call(this.brush)

    g_brush
    .on("mousedown.brush", null)
    .on("touchstart.brush", null)
    .on("touchmove.brush", null)
    .on("touchend.brush", null)
    .select('.background').style('cursor', 'auto');

    var g_zoomer = svg.append('g')

    var g_links = g_zoomer.append('g').classed('links_group', true)

    var g_nodes = g_zoomer.append('g').classed('nodes_group', true)

    var g_legends = svg.append('g').classed('legends_group', true)

    this.g_svg = svg;

    this.g_brush = g_brush;

    this.g_zoomer = g_zoomer;

    this.g_links = g_links

    this.g_nodes = g_nodes

    this.g_legends = g_legends

    this.update = function(duration, delay) {
      //console.log('render.svg.update ' + container_id)

      duration = duration != undefined ? duration : 500;
      delay = delay != undefined ? delay : 0;

      this.renders.range_width = +d3.select(container_id).style('width').slice(0, -2);
      this.renders.range_height = +d3.select(container_id).style('height').slice(0, -2);

      this.renders.xScale = d3.scale.linear()
      .domain([0, this.renders.domain_width])
      .range([0, this.renders.range_width]);

      this.renders.yScale = d3.scale.linear()
      .domain([0, this.renders.domain_height])
      .range([0, this.renders.range_height]);

      this.brush
      .x(this.renders.xScale)
      .y(this.renders.yScale)

      // DATA BINDING
      // Binding new data with old elements, if any.
      var d_links = g_links.selectAll('.link').data(_this.graph.data().array.links, function(d) {
        return d[gvis.settings.key]
      })

      var d_nodes = g_nodes.selectAll('.node').data(_this.graph.data().array.nodes, function(d) {
        return d[gvis.settings.key]
      })

      var d_legends = g_legends.selectAll('.legend').data(Object.keys(_this.graph.data().idMap.nodesType))

    
      // ENTER
      // Create new elements as needed.
      d_nodes
      .enter()
      .append('g')
      .classed('node', true)
      .attr('id', function(d) {return 'node_'+d[gvis.settings.key]})
      .call(this.addNodeRenderer, this)

      d_legends
      .enter()
      .append('g')
      .classed('legend', true)
      .call(this.addLegendRenderer, this)

      d_links
      .enter()
      .append('g')
      .classed('link', true)
      .attr('id', function(d) {return 'link_'+d[gvis.settings.key]})
      .call(this.addLinkRenderer, this)

      // EXIT
      // Remove old elements as needed.
      
      d_nodes.exit().remove();
      d_legends.exit().remove();
      d_links.exit().remove();


      // ENTER + UPDATE
      // Appending to the enter selection expands the update selection to include
      // entering elements; so, operations on the update selection after appending to
      // the enter selection will apply to both entering and updating nodes.
      // Updating current elements
      
      d_nodes.call(this.updateNodeRenderer, this, duration, delay);
      d_legends.call(this.updateLegendRenderer, this, duration, delay);
      d_links.call(this.updateLinkRenderer, this, duration, delay)


      // remove tipsy
      d3.selectAll('.tipsy').remove();
    }   
  }

  gvis.renders.svg.prototype.clear = function() {
    this.g_links.selectAll('*').remove();
    this.g_nodes.selectAll('*').remove();
    this.g_legends.selectAll('*').remove();
  }

  gvis.renders.svg.prototype.addNodeRenderer = function() {
    //console.log("addNodeRenderer", this, arguments)

    var nodes = arguments[0];
    var _svg = arguments[1];

    nodes[0].forEach(function(n) {
      if (!n) {
        return;
      }

      // adding node
      var node = d3.select(n)
      .append('g')
      .classed('node_container', true);

      var data = node.data()[0];

      if (data.x == undefined) data.x = 0;
      if (data.y == undefined) data.y = 0;

      node
      .attr("transform", function(d) {
        return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
      })
      .on("dblclick", _svg.events.nodeDblClick)
      .on("click", _svg.events.nodeClick)
      .call(_svg.drag)

      

      // bacground circle for node icon
      node
      .append('circle')
      .classed('node_background_circle', true)
      .attr('stroke-opacity', gvis.behaviors.render.nodeBackgroundStrokeOpacity)
      .attr('stroke-width', gvis.behaviors.render.nodeBackgroundStrokeWidth)
      .attr('stroke', gvis.behaviors.render.nodeBackgroundStrokeColor)
      .attr('fill', gvis.behaviors.render.nodeBackgroundFillColor)
      .attr('fill-opacity', gvis.behaviors.render.nodeBackgroundFillOpacity)
      .attr('r', gvis.behaviors.render.nodeRadius-gvis.behaviors.render.nodeRadiusMargin)
      .attr('filter', 'url(#filter_'+gvis.behaviors.render.nodeBackgroundFilter+')')

      // add node icon
      var icon = gvis.utils.icons(gvis.behaviors.icons[data.type]);

      //transform="matrix(sx, 0, 0, sy, cx-sx*cx, cy-sy*cy)"
      //Where sx is the scaling factor in the x-axis, sy in the y-axis.

      var matrix = [];

      var sx = (gvis.behaviors.render.nodeRadius-gvis.behaviors.render.nodeRadiusMargin)*1.5/icon.width;
      var sy = (gvis.behaviors.render.nodeRadius-gvis.behaviors.render.nodeRadiusMargin)*1.5/icon.height;

      var scale = sx < sy ? sx : sy

      var cx = icon.width/2;
      var cy = icon.height/2;

      var matrix= [scale, 0, 0, scale, -sx*cx, -sy*cy]

      node
      .append('g')
      .classed('icon', true)
      .html(icon.svg)
      .attr('transform', 'matrix('+matrix+')')
      .attr('fill', function(d) {
        return d[gvis.settings.styles].fill
      })

      var label = node
                  .append('g')

      // add node labels
      var text = label.append('text')
      .attr('id', function(d) {
        return 'label_' + d[gvis.settings.key];
      })
      .attr("text-anchor", "middle")
      .attr('y', gvis.behaviors.render.nodeRadius + gvis.behaviors.render.nodeLabelsFontSize / 1.5 - gvis.behaviors.render.nodeLabelsFontSize)

      var bbox = text[0][0].getBBox();

      // add node label background rectangle
      label.insert('rect', '#label_' + data[gvis.settings.key])
      //label.append('rect')
      .classed('label_background_rect', true)
      .attr('rx', 4)
      .attr('ry', 4)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
      .attr('fill', 'red')
      .attr('opacity', 0.2)
      
      $(node[0][0]).tipsy({ 
        gravity: 'w',  // n s e w
        html: true,
        title: function() {
          var d = this.__data__;

          if (!!gvis.behaviors.style.nodeToolTips.customized) {
            return gvis.behaviors.style.nodeToolTips.customized(d.type, d.id, d[gvis.settings.attrs], d);
          }
          else {
            return gvis.behaviors.style.nodeToolTips.default(d.type, d.id, d[gvis.settings.attrs], d);
          }
        }
      });
    })
  }

  gvis.renders.svg.prototype.updateNodeRenderer = function() {
    var nodes = arguments[0];
    var _svg = arguments[1];
    var duration = arguments[2] || 0;
    var delay = arguments[3] || 0;

    nodes[0].forEach(function(n) {
      if (!n) {
        return;
      }

      // select node
      var node = d3.select(n);

      node
      .select('.node_container')
      .transition()
      .delay(delay)
      .duration(duration)
      .attr("transform", function(d) { 
        return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
      })

      // get data
      var data = node.data()[0];


      // update icon
      node.select('.icon')
      .attr('fill', function(d) {
        return d[gvis.settings.styles].fill
      })

      // select label
      var text = node.select('#label_' + data[gvis.settings.key])

      text
      .attr("font-size", gvis.behaviors.render.nodeLabelsFontSize)
      .each(function(d) {
        nodeLabelGenerator(this, d);
      })
      // .text(function(d) {
      //   return d.type+':'+d.id;
      // })

      var bbox = text[0][0].getBBox();

      //label.insert('rect', '#label_' + data[gvis.settings.key])
      node.select('.label_background_rect')
      .transition()
      .delay(delay)
      .duration(duration)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
    })

    function nodeLabelGenerator(target, data) {
      //var labels = _svg.renders.labels.nodes[data.type];
      var labels = data[gvis.settings.styles].labels || {};

      d3.select(target).selectAll('tspan').remove();

      if (!labels || Object.keys(labels).length === 0) {
        d3.select(target)
        .append("tspan")
        .attr("x", 0)
        .attr("dy", gvis.behaviors.render.nodeLabelsFontSize)
        .text('ID:'+data.id)
      }
      else {
        if (!!labels['type']) {
          d3.select(target)
          .append("tspan")
          .attr("x", 0)
          .attr("dy", gvis.behaviors.render.nodeLabelsFontSize)
          .text('Type:'+data.type)
        }

        if (!!labels['id']) {
          d3.select(target)
          .append("tspan")
          .attr("x", 0)
          .attr("dy", gvis.behaviors.render.nodeLabelsFontSize)
          .text('ID:'+data.id)
        }

        for (var attr in data[gvis.settings.attrs]) {
          if (!!labels[attr]) {
            d3.select(target)
            .append("tspan")
            .attr("x", 0)
            .attr("dy", gvis.behaviors.render.nodeLabelsFontSize)
            .text(attr+':'+data[gvis.settings.attrs][attr])
          }
        }
      }
    }
  }

  gvis.renders.svg.prototype.addLinkRenderer = function() {
    var links = arguments[0];
    var _svg = arguments[1];

    links[0].forEach(function(l) {
      if (!l) {
        return;
      }

      var link = d3.select(l)
                 .append('g')
                 .classed('link_container', true)
                 .on("dblclick", _svg.events.linkDblClick)
                 .on("click", _svg.events.linkClick)
                 .on('mousedown', _svg.events.linkmousedown)


      var data = link.data()[0]

      if (data.source.x == undefined) data.source.x = 0;
      if (data.source.y == undefined) data.source.y = 0;
      if (data.target.x == undefined) data.target.x = 0;
      if (data.target.y == undefined) data.target.y = 0;

      var points = _svg.renders.generateCurvedLinkPoints(data);

      var path = link.append('g')
                 .classed('link_path', true)

      var display;
      var x;
      var y; 

      // base on points to determing whether show the links.
      if (points.length == 0) {
        display = 'none';

        var x0 = _svg.renders.xScale(data.source.x)
        var y0 = _svg.renders.yScale(data.source.y)
        var x1 = _svg.renders.xScale(data.target.x)
        var y1 = _svg.renders.yScale(data.target.y)
        
        points = [[x0, y0], [x0, y0], [x1, y1], [x1, y1]]
      } 
      else {
        display = '';
      }

      // if (points[0][0] == points[1][0] &&  points[1][0] == points[2][0] && points[2][0] == points[3][0]) {
      //   points[2][0] += 10;
      //   points[3][0] += 10;
      //   points[2][1] += 10;
      //   points[3][1] += 10;
      // }

      x = 0.5 * points[1][0] + 0.5 * points[2][0];
      y = 0.5 * points[1][1] + 0.5 * points[2][1];

      path.append('defs')
      .append("marker")
      .classed('link_marker', true)
      .attr("id", "link_marker_"+data[gvis.settings.key])
      .each(function(d) {
        var link_width = Math.sqrt(gvis.behaviors.render.linkStrokeWidth);

        var template = '<path d="M0,0 L0,{{a}} L{{b}},{{c}} L0,0"/>';
        var marker = gvis.utils.applyTemplate(template, {a:6*link_width, b:7*link_width, c:3*link_width});

        d3.select(this)
        .attr("markerWidth", 10*link_width)
        .attr("markerHeight", 10*link_width)
        .attr("refX", 5*link_width)
        .attr("refY", 3*link_width)
        .attr("orient", "auto")
        .attr("fill", 'black')
        .attr("markerUnits", "userSpaceOnUse") // User for "strokeWidth"
        .attr("stroke-width", 3*link_width)
        .html(marker)
      })

      path.append('path')
      .classed('link_line_background', true)
      .attr("id", 'link_line_background_'+data[gvis.settings.key])
      .attr("fill", 'none')
      .attr("stroke", gvis.behaviors.render.highlightColor)
      .attr("stroke-linecap", "round")
      .attr("stroke-width", gvis.behaviors.render.linkHighlightStrokeWidth)
      .attr("stroke-opacity", 0)
      .attr("d", function(d) {
        var line = d3.svg.line().interpolate("basis");
        return line(points);
      })

      path.append('path')
      .classed('link_line', true)
      .attr("id", 'link_line_'+data[gvis.settings.key])
      .attr("fill", 'none')
      .attr("stroke", 'black')
      .attr("stroke-width", function(d) {
        var width = d[gvis.settings.styles]['stroke-width'] || gvis.behaviors.render.linkStrokeWidth;
        return width;
      })
      .attr("stroke-linecap", "round")
      .attr("stroke-dasharray", function(d) {
        if (!!d[gvis.settings.styles].dashed) {
          var a = 5 * d[gvis.settings.styles]['stroke-dasharray'][0] * gvis.behaviors.render.linkStrokeWidth;
          var b = 5 * d[gvis.settings.styles]['stroke-dasharray'][0] * gvis.behaviors.render.linkStrokeWidth;
          return a + ' ' + b;
        }
        else {
          return ""
        }
      })
      .attr("opacity", function(d) {
        var opacity = d[gvis.settings.styles]['stroke-opacity'] || gvis.behaviors.render.linkStrokeOpacity;
        return opacity;
      })
      .attr("d", function(d) {
        var line = d3.svg.line().interpolate("basis");
        return line(points);
      })
      //.attr("marker-end", "url(#link_marker_"+data[gvis.settings.key] + ")")
      .attr(gvis.behaviors.render.linkMarker, function(d) {
        if (!!d.directed)
          return "url(#link_marker_"+data[gvis.settings.key] + ")"
        else 
          return "";
      })

      // adding labels for links
      var label = link.append('g')
      .attr('id', 'label_' + data[gvis.settings.key])
      .attr("transform", function(d) { 
        return "translate(" + x + "," + (y-gvis.behaviors.render.linkLabelsFontSize) + ")"; 
      })

      link.attr('display', display);

      var text = label.append('text')
      .attr('id', 'label_text_' + data[gvis.settings.key])
      .attr("text-anchor", "middle")

      var bbox = text[0][0].getBBox();

      label.insert('rect', '#label_text_' + data[gvis.settings.key])
      //label.append('rect')
      .classed('label_background_rect', true)
      .attr('rx', 4)
      .attr('ry', 4)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
      .attr('fill', '#9ecae1')
      .attr('fill-opacity', 0.5)
      .attr('stroke', gvis.behaviors.render.highlightColor)
      .attr('stroke-width', Math.pow(gvis.behaviors.render.linkHighlightStrokeWidth, 0.5))
      .attr('stroke-opacity', 0)
      //.attr('filter', 'url(#filter_'+gvis.behaviors.render.linkBackgroundFilter+')')

      $(label[0][0]).tipsy({ 
        gravity: 'w',  // n s e w
        html: true,
        title: function() {
          var d = this.__data__;

          if (!!gvis.behaviors.style.linkToolTips.customized) {
            return gvis.behaviors.style.linkToolTips.customized(d.type, d[gvis.settings.attrs], d);
          }
          else {
            return gvis.behaviors.style.linkToolTips.default(d.type, d[gvis.settings.attrs], d);
          }
        }
      });
    })
  }

  gvis.renders.svg.prototype.updateLinkRenderer = function() {
    var links = arguments[0]
    var _svg = arguments[1];
    var duration = arguments[2] || 0;
    var delay = arguments[3] || 0;

    links[0].forEach(function(l) {
      if (!l) {
        return;
      }

      var link = d3.select(l)

      var data = link.data()[0]

      var points = _svg.renders.generateCurvedLinkPoints(data);

      var x;
      var y;
      var display;
      var visibility;

      if (points.length == 0) {
        display = 'none';
        visibility = 'hidden';

        var x0 = _svg.renders.xScale(data.source.x)
        var y0 = _svg.renders.yScale(data.source.y)
        var x1 = _svg.renders.xScale(data.target.x)
        var y1 = _svg.renders.yScale(data.target.y)
        
        points = [[x0, y0], [x0, y0], [x1, y1], [x1, y1]]
      } 
      else {
        display = 'inline';
        visibility = 'visible';
      }

      // for label position.
      x = 0.5 * points[1][0] + 0.5 * points[2][0];
      y = 0.5 * points[1][1] + 0.5 * points[2][1];

      link.select('#link_line_'+data[gvis.settings.key])
      .transition()
      .delay(delay)
      .duration(duration)
      .attr("d", function(d) {
        var line = d3.svg.line().interpolate("basis");
        
        return line(points);
      })

      link.select('#link_line_background_'+data[gvis.settings.key])
      .transition()
      .delay(delay)
      .duration(duration)
      .attr("d", function(d) {
        var line = d3.svg.line().interpolate("basis");
        
        return line(points);
      })

      var label = link.select('#label_'+data[gvis.settings.key])
      .transition()
      .delay(delay)
      .duration(duration)
      .attr("transform", function(d) { 
        return "translate(" + x + "," + (y-gvis.behaviors.render.linkLabelsFontSize) + ")"; 
      })

      link.attr('display', display);
      link.select('.link_container')
      .attr('display', display);

      var text = link.select('#label_text_' + data[gvis.settings.key]);

      text
      .attr('font-size', gvis.behaviors.render.linkLabelsFontSize)
      .each(function(d) {
        linkLabelGenerator(this, d);
      })

      var bbox = text[0][0].getBBox();

      //label.insert('rect', '#label_' + data[gvis.settings.key])
      link.select('.label_background_rect')
      .transition()
      .delay(delay)
      .duration(duration)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
    })

    function linkLabelGenerator(target, data) {
      //var labels = _svg.renders.labels.links[data.type];

      var labels = data[gvis.settings.styles].labels || {};

      d3.select(target).selectAll('tspan').remove();

      if (!labels || Object.keys(labels).length === 0) {
        // d3.select(target)
        // .append("tspan")
        // .attr("x", 0)
        // .attr("dy", gvis.behaviors.render.linkLabelsFontSize)
        // .text('Type:'+data.type)
      }
      else {
        if (!!labels['type']) {
          d3.select(target)
          .append("tspan")
          .attr("x", 0)
          .attr("dy", gvis.behaviors.render.linkLabelsFontSize)
          .text('Type:'+data.type)
        }

        for (var attr in data[gvis.settings.attrs]) {

          if (!!labels[attr]) {
            d3.select(target)
            .append("tspan")
            .attr("x", 0)
            .attr("dy", gvis.behaviors.render.linkLabelsFontSize)
            .text(attr+':'+data[gvis.settings.attrs][attr])
          }
        }
      }
    }
  }

  gvis.renders.svg.prototype.addLegendRenderer = function() {
    var legends = arguments[0] 
    var _svg = arguments[1];

    legends[0].forEach(function(l, i) {
      if (!l) {
        return;
      }

      // adding legend
      var legend = d3.select(l)
      .append('g')
      .classed('legend_container', true)

      var type = legend.data()[0];

      // bacground circle for legend node icon
      legend
      .append('circle')
      .classed('legend_background_circle', true)
      .attr('stroke-opacity', 0)
      .attr('fill', gvis.behaviors.render.nodeBackgroundFillColor)
      .attr('fill-opacity', gvis.behaviors.render.nodeBackgroundFillOpacity)
      .attr('r', gvis.behaviors.render.legendNodeRadius-gvis.behaviors.render.legendNodeRadiusMargin)

      // add legend icon
      var icon = gvis.utils.icons(gvis.behaviors.icons[type]);

      var matrix = [];

      var sx = (gvis.behaviors.render.legendNodeRadius-gvis.behaviors.render.legendNodeRadiusMargin)*1.5/icon.width;
      var sy = (gvis.behaviors.render.legendNodeRadius-gvis.behaviors.render.legendNodeRadiusMargin)*1.5/icon.height;

      var scale = sx < sy ? sx : sy

      var cx = icon.width/2;
      var cy = icon.height/2;

      var matrix= [scale, 0, 0, scale, -sx*cx, -sy*cy]

      legend
      .append('g')
      .classed('icon', true)
      .html(icon.svg)
      .attr('transform', 'matrix('+matrix+')')
      .attr('fill', 'red')

      var label = legend
                  .append('g')

      // add legend labels
      var text = label.append('text')
      .attr('id', function(d) {
        return 'legendLabel_' + d;
      })
      .attr("text-anchor", "left")
      .attr('x', gvis.behaviors.render.legendNodeRadius * 1.00)
      .attr('y', gvis.behaviors.render.legendNodeRadius * 0.25)
      .text(function(d) {
        return d;
      })

      var bbox = text[0][0].getBBox();

      // add node label background rectangle
      label.insert('rect', '#legendLabel_' + type)
      //label.append('rect')
      .classed('legendLabel_background_rect', true)
      .attr('rx', 4)
      .attr('ry', 4)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
      .attr('fill', '#ccc')
      .attr('opacity', 0.9)
    })
  }

  gvis.renders.svg.prototype.updateLegendRenderer = function() {
    var legends = arguments[0] 
    var _svg = arguments[1];
    var duration = arguments[2] || 0
    var delay = arguments[3] || 0

    legends[0].forEach(function(l, i) {
        if (!l) {
          return;
        }

        // get legend
        var legend = d3.select(l);

        legend
        .select('.legend_container')
        .transition()
        .delay(delay)
        .duration(duration)
        .attr("transform", function(d) {
          return "translate(" + gvis.behaviors.render.legendNodeRadius + "," + (gvis.behaviors.render.legendNodeRadius-gvis.behaviors.render.legendNodeRadiusMargin)*(1+i*2) + ")"; 
        })

        var type = legend.data()[0];

        // legend icon
        var icon = gvis.utils.icons(gvis.behaviors.icons[type]);

        var matrix = [];

        var sx = (gvis.behaviors.render.legendNodeRadius-gvis.behaviors.render.legendNodeRadiusMargin)*1.5/icon.width;
        var sy = (gvis.behaviors.render.legendNodeRadius-gvis.behaviors.render.legendNodeRadiusMargin)*1.5/icon.height;

        var scale = sx < sy ? sx : sy

        var cx = icon.width/2;
        var cy = icon.height/2;

        var matrix= [scale, 0, 0, scale, -sx*cx, -sy*cy]

        legend
        .select('.icon')
        .html(icon.svg)
        .attr('transform', 'matrix('+matrix+')')
        .attr('fill', 'red')

        // add legend labels
        var text = legend.select('text')
        .text(function(d) {
          return d;
        })

        var bbox = text[0][0].getBBox();

        // add node label background rectangle
        //label.insert('rect', '#label_' + data[gvis.settings.key])
        legend.select('.legendLabel_background_rect')
        .attr('x', bbox.x)
        .attr('y', bbox.y)
        .attr('width', bbox.width)
        .attr('height', bbox.height)
    })
  }

  gvis.renders.svg.prototype.autoFit = function(duration, delay) {
    duration = duration != undefined ? duration : 0;
    delay = delay != undefined ? delay : 0;

    var nodes = this.renders.graph.data().array.nodes;
    var width = this.renders.domain_width;
    var height = this.renders.domain_height;

    //no molecules, nothing to do
    if (nodes.length === 0)
    return;

    // Get the bounding box
    var min_x = d3.min(nodes.map(function(d) {return d.x;}));
    var min_y = d3.min(nodes.map(function(d) {return d.y;}));

    var max_x = d3.max(nodes.map(function(d) {return d.x;}));
    var max_y = d3.max(nodes.map(function(d) {return d.y;}));


    // The width and the height of the graph
    var mol_width = max_x - min_x;
    var mol_height = max_y - min_y;

    // how much larger the drawing area is than the width and the height
    var width_ratio = width / mol_width;
    var height_ratio = height / mol_height;

    // we need to fit it in both directions, so we scale according to
    // the direction in which we need to shrink the most
    var min_ratio = Math.min(width_ratio, height_ratio) * 0.75;
    if (min_ratio > this.renders.zoomRange[1]) min_ratio = this.renders.zoomRange[1];
    if (min_ratio < this.renders.zoomRange[0]) min_ratio = this.renders.zoomRange[0];

    // the new dimensions of the molecule
    var new_mol_width = mol_width * min_ratio;
    var new_mol_height = mol_height * min_ratio;

    // translate so that it's in the center of the window
    var x_trans = -(min_x) * min_ratio + (width - new_mol_width) / 2;
    var y_trans = -(min_y) * min_ratio + (height - new_mol_height) / 2;

    // do the actual moving
    this.g_zoomer
    .transition()
    .delay(delay)
    .duration(duration)
    .attr("transform", "translate(" + [this.renders.xScale(x_trans), this.renders.yScale(y_trans)] + ")" + " scale(" + min_ratio + ")");

    // tell the zoomer what we did so that next we zoom, it uses the
    // transformation we entered here
    this.zoom.translate([this.renders.xScale(x_trans), this.renders.yScale(y_trans)]);
    this.zoom.scale(min_ratio);
  }

  gvis.renders.svg.prototype.centerView = function(duration, delay) {

    duration = duration != undefined ? duration : 0;
    delay = delay != undefined ? delay : 0

    var xMass=0;
    var yMass=0;

    var nodes = this.renders.graph.data().array.nodes.filter(function(n) {
      return n.selected;
    });

    if (nodes.length == 0) {
      nodes = this.renders.graph.data().array.nodes;
    }

    if (nodes.length == 0) {
      return;
    }

    nodes.forEach(function(n) {
      xMass += n.x;
      yMass += n.y;
    })

    xMass /= nodes.length;
    yMass /= nodes.length;

    var scale = this.zoom.scale();

    var x_trans = this.renders.xScale(this.renders.domain_width/2 - xMass*scale) ;
    var y_trans = this.renders.yScale(this.renders.domain_height/2 - yMass*scale) ;

    this.g_zoomer
    .transition()
    .delay(delay)
    .duration(duration)
    .attr("transform", "translate(" + [x_trans, y_trans] + ")" + " scale(" + scale + ")");

    // tell the zoomer what we did so that next we zoom, it uses the
    // transformation we entered here
    this.zoom.translate([x_trans, y_trans]);
  }

  gvis.renders.svg.prototype.unselectAllElements = function() {
    this.renders.graph.nodes().forEach(function(n) {
      n.preSlected = n.selected = false;
    })

    this.renders.graph.links().forEach(function(n) {
      n.preSlected = n.selected = false;
    })

    this.updateSelectedNodes();
    this.updateSelectedLinks();
  }

  gvis.renders.svg.prototype.updateSelectedNodes = function() {

    this.g_svg.selectAll('.node')
    .each(function(d) {
      if (d.selected) {
        d3.select(this)
        .select('.node_background_circle')
        .attr('stroke-opacity', gvis.behaviors.render.nodeHighlightStrokeOpacity)
        .attr('stroke-width', gvis.behaviors.render.nodeHighlightStrokeWidth)
        .attr('stroke', 'red')
        .attr('filter', 'url(#filter_glow)')
      }
      else {
        d3.select(this)
        .select('.node_background_circle')
        .attr('stroke-opacity', gvis.behaviors.render.nodeBackgroundStrokeOpacity)
        .attr('stroke-width', gvis.behaviors.render.nodeBackgroundStrokeWidth)
        .attr('stroke', gvis.behaviors.render.nodeBackgroundStrokeColor)
        .attr('filter', 'url(#filter_'+gvis.behaviors.render.nodeBackgroundFilter+')')
      }
    }) 
  }

  gvis.renders.svg.prototype.updateSelectedLinks = function() {

    this.g_svg
    .selectAll('.link')
    .each(function(d) {
      if (d.selected) {
        d3.select(this).select('.link_line_background')
        .attr('stroke-opacity', gvis.behaviors.render.linkHighlightStrokeOpacity)

        d3.select(this).select('.label_background_rect')
        .attr('stroke-opacity', gvis.behaviors.render.linkHighlightStrokeOpacity)
        //.attr('filter', 'url(#filter_glow)')
      }
      else {
        d3.select(this).select('.link_line_background')
        .attr('stroke-opacity', 0)

        d3.select(this).select('.label_background_rect')
        .attr('stroke-opacity', 0)
        //.attr('filter', 'url(#filter_'+gvis.behaviors.render.linkBackgroundFilter+')')
      }
    })   
  }



  /********** renders.canvas **********/
  gvis.renders.canvas = function(renders) {

  }


  /********** renders.map **********/
  gvis.renders.map = function(renders) {
    var that = this;
    this.renders = renders;

    this.events = new gvis.events.svg(this);

    var _this = renders._this;

    var container_id = renders.render_container;

    var UIObject = _this.UIObject;

    this.linkvaluename = 'hB';
    
    //this.format = new ol.format.GeoJSON();

    this.color = d3.scale.pow().exponent(0.6).domain([0, 1]).range(['blue','red']); // link color
    this.nodeColor = d3.scale.linear().domain([0, 345, 765]).range(['blue', 'green', 'red']).clamp(true);

    var radiusScale = d3.scale.linear().domain([4000, 100]).range([3, 10]).clamp(true)

    var styleFunction = function(feature, resolution){
      var type = feature.getGeometry().getType();
      var level = feature.getProperties().level;

      var nodergbcolor = d3.rgb(that.nodeColor(level));
      var opacityNodeColor = [nodergbcolor.r, nodergbcolor.g, nodergbcolor.b, 0.4]

      var linkrgbcolor = d3.rgb(that.color(level));
      var opacityLinkColor = [linkrgbcolor.r, linkrgbcolor.g, linkrgbcolor.b, 0.5]

      switch (type) {
        case 'Point':
          var image = new ol.style.Circle({
            radius: function(r){
              return radiusScale(r);
            }(resolution),
            fill: new ol.style.Fill({
                color: opacityNodeColor
              }),
            stroke: new ol.style.Stroke({color: '#000', width: 0.1})
          });
          return new ol.style.Style({
                    image: image,
                    text: textStyle(feature, resolution)
                  });
          break
        case 'Circle':
          return new ol.style.Style({
            text: textStyle(feature, resolution),
            stroke: new ol.style.Stroke({
              color: 'black',
              width: 1
            }),
            fill: new ol.style.Fill({
              color: that.nodeColor(level)//'rgba(0, 255, 0, 1)'
            })
          })
        break;
        case 'LineString':
        return new ol.style.Style({
          text: //textStyle(feature, resolution),
          new ol.style.Text({
                  text: function(feature, resolution) {
                    if (resolution > 2000) {
                      return ''
                    }
                    else {
                      return feature.getProperties().level.toString();
                    }
                  }.call(this, feature, resolution),
                  offsetY: 0, 
                  textAlign: 'center',
                  textBaseline : 'middle',
                  font: '13px',
                  stroke: new ol.style.Stroke({color: '#fff', width: 2}),
                }),
          stroke: new ol.style.Stroke({
            color: opacityLinkColor,
            width: 3
          })
        })
        break;
        default:
        break;
      }
    }

    var getText = function(feature, resolution) {
      if (resolution > 250) {
        return ''
      }
      else {
        return feature.getProperties().name + '-' + feature.getProperties().level
      }
    }

    var textStyle = function(feature, resolution) {
      return new ol.style.Text({
        text: getText(feature, resolution),
        offsetY: 8,
        textAlign: 'center',
        textBaseline : 'top',
        font: '13px',
        stroke: new ol.style.Stroke({color: '#fff', width: 2})
      })
    }

    this.vectorSource = new ol.source.Vector();

    this.vectorLayer = new ol.layer.Vector({
      source: this.vectorSource,
      style: styleFunction
    });

    this.vectorNodeSource = new ol.source.Vector();

    this.vectorNodeLayer = new ol.layer.Vector({
      source: this.vectorNodeSource,
      style: styleFunction
    });


    var resolutions = [];
    var tileSize = 256;
    var projection = ol.proj.get("EPSG:3857");
    for (var i = 0; i < 19; i++) {
        resolutions[i] = Math.pow(2, 18 - i);
    }
    var projectionExtent = projection.getExtent();// [-tileSize*resolusions[1],-tileSize*resolusions[1],tileSize*resolusions[1],tileSize*resolusions[1]];//projection.getExtent();
    var tilegrid = new ol.tilegrid.TileGrid({
        origin: [0,0],
        resolutions: resolutions
    });
    var tilesource = new ol.source.TileImage({
        projection: projection,
        tileGrid: tilegrid,
        tileUrlFunction: function (xyz, obj1, obj2) {
            if (!xyz) {
                return "";
            }
            var z = xyz[0];
            var x = xyz[1];
            var y = xyz[2];
//            y = -y - 1;  默认轴方向是右下的，但是百度的轴方向是右上。 所以y值 应该变的，但是为何没有 ？
            if (x < 0) {
                x = "M" + (-x);
            }
            if (y < 0) {
                y = "M" + (-y);
            }
            return "http://online3.map.bdimg.com/tile/?qt=tile&x=" + x + "&y=" + y + "&z=" + z + "&styles=pl&udt=20141119&scaler=1";
        }
    });

    var map = new ol.Map({
      layers: [
        new ol.layer.Tile({
          source: new ol.source.OSM(),
          opacity: 0.9
        }),
        // new ol.layer.Tile({
        //     extent: projectionExtent,
        //     source: tilesource,
        //     opacity: 0.2
        // }),
        this.vectorLayer,
        this.vectorNodeLayer
      ],
      target: container_id,
      controls: ol.control.defaults({
        attributionOptions: /** @type {olx.control.AttributionOptions} */ ({
          collapsible: false
        })
      }),
      view: new ol.View({
        center: [0, 0]/*ol.proj.fromLonLat([-68.75, 44.74])*/,
        zoom: 1
      })
    }); 

    this.map = map;

    map.on(['moveend', 'zoomend'], onInteractEnd);

    function onInteractEnd(evt) {
      var map = evt.map;
      var UIObject = _this.currentObj;

      if (UIObject.setting.customized) {
        if (typeof visualization == 'undefined') return;
        var map = visualization.scope.renderer.renderer.map;
        var extent = map.getView().calculateExtent(map.getSize());
        extent = ol.proj.transformExtent(extent, 'EPSG:3857', 'EPSG:4326');

        extent = function(oldExtent) {
          var lonmin=oldExtent[0] 
          var latmin=oldExtent[1] 
          var lonmax=oldExtent[2] 
          var latmax=oldExtent[3]

          var a,b,c,d;

          var alpha = 0.5;

          a = lonmin - (lonmax - lonmin)*alpha;
          b = latmin - (latmax - latmin)*alpha;
          c = lonmax + (lonmax - lonmin)*alpha;
          d = latmax + (latmax - latmin)*alpha;

          return [a,b,c,d];
        }(extent)
        
        var scale = d3.scale.linear()
        .domain([11,10, 9,8,7,6,5])
        .range([0,70,100,135,160,220,340])
        .clamp(true);

        console.log(scale(map.getView().getZoom()));

        var template = UIObject.initialization.urlTemplate;
        
        var value = {minlevel:scale(map.getView().getZoom()), lonmin:extent[0], latmin:extent[1], lonmax:extent[2], latmax:extent[3]};

        var initialurl = gvis.utils.applyTemplate(template, value);

        $.ajax({
          url : initialurl,
          type : "get",
          data : {},//JSON.stringify(submit_payload),//'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
          dataType   : 'json',
          error: function(jqXHR, textStatus, errorThrown){
            // will fire when timeout is reached
            // mygv.clean();
            if (jqXHR.responseText.indexOf('error_: invalid / deleted') != -1) {
              alert("输入的节点不存在，请重新输入。");
            }
            else if (jqXHR.responseText.indexOf('Adding ID failed.') != -1) {
              alert("输入的节点类型不存在，请重新输入。");
            }
            else {
              alert(jqXHR.responseText);
            }   
          },
          success: function(message, textStatus, jqXHR){
            if (message.error) {
              console.log("query language error.")
            }
            else {
              var length = message.results.length
              var graph = visualization.scope.graph;

              graph.clear();

              for (var i=0; i<length-1; ++i) {
                var rawnode = message.results[i];
                var node = {id:rawnode['all.id'], type:'BUS'}
                node[gvis.settings.attrs] = {}
                for (var key in rawnode['all.att']) {
                  var newKey = key.split('.')[1];
                  node[gvis.settings.attrs][newKey] = rawnode['all.att'][key];
                }

                graph.addNode(node);
              }

              var links = message.results[length-1]['@@edgeSet'].split(' ');
              links = links.map(function(l) {
                return l.split(',');
              })

              for (var i in links) {
                if (links[i][0] > links[i][1] || links[i][0] == "" || links[i][1] == "") continue;
                var link = {source:{id:links[i][0], type:'BUS'}, target:{id:links[i][1], type:'BUS'}, type:'BRANCH'}

                link[gvis.settings.attrs] = {hB:links[i][2]}

                graph.addLink(link);
              }

              var stop = 0;
              visualization.render();
            }
          }
        })
      }// end customied.
    }

    map.getView().on('propertychange', function(e) {
      switch (e.key) {
        case 'resolution':
          //console.log(e.oldValue, this.getZoom());
          break;
      }

      // var extent = map.getView().calculateExtent(map.getSize());
      // extent = ol.proj.transformExtent(extent, 'EPSG:3857', 'EPSG:4326');

      // console.log(extent, this.getZoom());
    })

    var info = $('#info');
    info.tooltip({
      animation: false,
      trigger: 'manual'
    });

    var displayFeatureInfo = function(pixel) {
      info.css({
        left: (pixel[0] + 13) + 'px',
        top: (pixel[1] + 60) + 'px'
      });
      var feature = map.forEachFeatureAtPixel(pixel, function(feature, layer) {
        return feature;
      });
      if (feature && feature.get('type') == 'node') {
        info.tooltip('hide')
            .attr('data-original-title', feature.get('name')+'-'+feature.get('level'))
            .tooltip('fixTitle')
            .tooltip('show');
      } else {
        info.tooltip('hide');
      }
    };

    map.on('pointermove', function(evt) {
      if (evt.dragging) {
        
        return;
      }
      
    });

    map.on('pointermove', function(evt) {
      if (evt.dragging) {
        info.tooltip('hide');
        return;
      }
      // var pixel = map.getEventPixel(evt.originalEvent);
      // var feature = map.forEachFeatureAtPixel(pixel, function(feature, layer) {
      //       return feature;
      //     });
      // if(feature) {
      //   //console.log(feature);
      // }
      displayFeatureInfo(map.getEventPixel(evt.originalEvent));

    });

    map.on('click', function(evt) {
      //console.log(evt.pixel);
      var feature = map.forEachFeatureAtPixel(evt.pixel, function(feature, layer) {
            return feature;
          });
      if(feature) {
        var prop = feature.getProperties();
        var type = prop.type;
        if(type == 'link') {
          var link = that.renders.graph.links(prop.key);
          that.events.linkClick(link);
        }
        else if (type == 'node'){
          var node = that.renders.graph.nodes(prop.key);
          that.events.nodeClick(node);
        }
      }

      var lonlat = ol.proj.transform(evt.coordinate, 'EPSG:3857', 'EPSG:4326');
      console.log(map.getView().getResolution());
      console.log(map.getView().getZoom());
      console.log(lonlat);
    });

    map.getView().setZoom(4);
    map.getView().setCenter([-10703629.944829801, 4236445.855677608])

    this.clear = function() {
      var that = this;
      that.renders.graph.clear();
      that.vectorSource.clear();
      that.vectorNodeSource.clear();
    }

    this.update = function() {
      //console.log("Map Update");
      var graph = this.renders.graph;
      var that = this;
      that.vectorSource.clear();
      that.vectorNodeSource.clear();

      var linkLevel = [+Number.MAX_VALUE, -Number.MAX_VALUE];
      var nodeLevel = [+Number.MAX_VALUE, -Number.MAX_VALUE];

      graph.links().forEach(function(l, i) {
        var source = l.source[gvis.settings.attrs];
        var target = l.target[gvis.settings.attrs];
        var attrs = l[gvis.settings.attrs]

        //var level = parseFloat(attrs['hB']); //parseFloat(attrs['MVALimitA'])
        var level = parseFloat(attrs[that.linkvaluename]);

        if (isNaN(level)) {
          var p = 0.3;
          level = (parseFloat(l.source.id[l.source.id.length-2]) / 10.0 * (1-p) +p+0.1).toFixed(2);
        }

        linkLevel[0] = Math.min(linkLevel[0], level);
        linkLevel[1] = Math.max(linkLevel[1], level);


        if (parseFloat(source.Longitude) < 1 && parseFloat(source.Longitude) > -1) {
          return;
        }
        if (parseFloat(target.Longitude) < 1 && parseFloat(target.Longitude) > -1) {
          return;
        }

        var newLinkFeature = new ol.Feature({
          geometry: new ol.geom.LineString([ol.proj.fromLonLat([parseFloat(source.Longitude), parseFloat(source.Latitude)]), ol.proj.fromLonLat([parseFloat(target.Longitude), parseFloat(target.Latitude)])]),
            key: l[gvis.settings.key],
            level: level,
            type: 'link'
        })

        that.vectorSource.addFeature(newLinkFeature);
      })

      //that.color.domain([linkLevel[0], linkLevel[0]*0.6+linkLevel[1]*0.4, linkLevel[1]]);

      //console.log(linkLevel)
      if (linkLevel[0] == -Number.MAX_VALUE || linkLevel[1] == +Number.MAX_VALUE) {
        linkLevel[0] = 0;
        linkLevel[1] = 1;

        that.color = d3.scale.linear().domain([0, 0.5, 1]).range(['blue','green', 'red']);
      }
      else {
        that.color = d3.scale.pow().exponent(0.6).domain(linkLevel).range(['blue','red']);
      }

      that.vectorLayer.setStyle(styleFunction);

      graph.nodes().forEach(function(n) {
        var attrs = n[gvis.settings.attrs];
        var id = n.id;
        var level = parseFloat(attrs.NominalV)

        nodeLevel[0] = Math.min(nodeLevel[0], level);
        nodeLevel[1] = Math.max(nodeLevel[1], level);

        if (parseFloat(attrs.Longitude) < 1 && parseFloat(attrs.Longitude) > -1) {
          return;
        }

        
        var newNodeGeom;
        var UIObject = _this.currentObj;
        if (UIObject.setting.customized) {
          newNodeGeom = {
            geometry : new ol.geom.Point(ol.proj.fromLonLat([parseFloat(attrs.Longitude), parseFloat(attrs.Latitude)])),
            key: n[gvis.settings.key],
            name: attrs.BusName,
            level: level,
            type: 'node'
          }
        }
        else {
          newNodeGeom = {
            geometry: new ol.geom.Circle(ol.proj.fromLonLat([parseFloat(attrs.Longitude), parseFloat(attrs.Latitude)]), 500),
            key: n[gvis.settings.key],
            name: attrs.BusName,
            level: level,
            type: 'node'
          }
        }

        var newNodeFeature = new ol.Feature(newNodeGeom);

        that.vectorNodeSource.addFeature(newNodeFeature);
      })

      // var nodeFeature = new ol.Feature({
      //   geometry: new ol.geom.Circle(ol.proj.fromLonLat([-68.85, 44.74]), 500),
      //   color: '#0000ff'
      // })

      // var linkFeature = new ol.Feature({
      //   geometry: new ol.geom.LineString([ol.proj.fromLonLat([-68.85, 44.74]), ol.proj.fromLonLat([-67.85, 45.74])]),
      //     color: '#00ff00'
      // })


      // this.vectorSource.addFeature(nodeFeature);
      // this.vectorSource.addFeature(linkFeature);
    }

    this.autoFit = function() {
      //console.log("Map Auto Fit")
    }
  }
})




