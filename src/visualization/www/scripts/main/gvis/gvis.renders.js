(function(undefined) {
  "use strict";
  // Doing somthing for renders
  console.log('Loading gvis.render')

  gvis.renders = function(_this) {

    this._this = _this; // gvis pointer.
    this.graph = _this.graph;

    this.render_container = _this.conf.container;
    this.render_type = !_this.conf.render_type ? 'svg' : _this.conf.render_type;    

    this.domain_width = 1;
    this.domain_height = 1;

    this.range_width = +d3.select('#'+this.render_container).style('width').slice(0, -2);
    this.range_height = +d3.select('#'+this.render_container).style('height').slice(0, -2);

    this.zoomRange = [0.1, 10];

    this.labels = {
      nodes: {
        usr: {
          a1:true,
          a2:true,
          id:true,
          type:true
        }
      },
      links: {
        bbb: {
          aa1:true,
          aa2:true,
          type:true
        }
      }
    }

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
        this.renderer = 'render ' + this.render_container + ' by using map. Coming soon.';
      break;
      case 'svg':
      default:
        this.renderer = new gvis.renders.svg(this);
      break;
    }

    this.generateCurvedLinkPoints = function(link) {
      var a = 0.25;
      var b = gvis.behaviors.render.nodeRadius * 2; // initNodeSetting.r * 4;
      var edgeMargin = gvis.behaviors.render.nodeRadius;
      var markMargin = gvis.behaviors.render.nodeRadius / 10;

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
        return ;
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

  gvis.renders.prototype.update = function(duration) {
    this.renderer.update(duration);
  }

  gvis.renders.prototype.autoFit = function() {
    var duration = 500;
    this.renderer.autoFit(duration);
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
    .on("zoom", this.events.zoomed);

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

    svg.append("defs").append("svg:clipPath")
    .attr("id", "node-mask")
    .append("svg:circle")
    .attr("r", gvis.behaviors.render.nodeMaskRadius-1)
    .attr("cx", 0)
    .attr("cy", 0);

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

    this.update = function(duration) {
      console.log('render.svg.update ' + container_id)

      duration = duration != undefined ? duration : 500;

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
        
      d_links
      .enter()
      .append('g')
      .classed('link', true)
      .attr('id', function(d) {return 'link_'+d[gvis.settings.key]})
      .call(this.addLinkRenderer, this)


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

      // EXIT
      // Remove old elements as needed.
      d_links.exit().remove();
      d_nodes.exit().remove();
      d_legends.exit().remove();


      // ENTER + UPDATE
      // Appending to the enter selection expands the update selection to include
      // entering elements; so, operations on the update selection after appending to
      // the enter selection will apply to both entering and updating nodes.
      // Updating current elements
      d_links.call(this.updateLinkRenderer, this, duration)
      d_nodes.call(this.updateNodeRenderer, this, duration);
      d_legends.call(this.updateLegendRenderer, this, duration);
    }   
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
      .classed('node_container', true)
      .attr("transform", function(d) {
        return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
      })
      .call(_svg.drag)

      var data = node.data()[0];

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
      .attr('fill', 'green')

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
      //label.insert('rect', '#label_' + data[gvis.settings.key])
      label.append('rect')
      .classed('label_background_rect', true)
      .attr('rx', 4)
      .attr('ry', 4)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
      .attr('fill', 'red')
      .attr('opacity', 0.2)
      
    })
  }

  gvis.renders.svg.prototype.updateNodeRenderer = function() {
    var nodes = arguments[0];
    var _svg = arguments[1];
    var duration = arguments[2] || 0;

    nodes[0].forEach(function(n) {
      if (!n) {
        return;
      }

      // select node
      var node = d3.select(n);

      node
      .select('.node_container')
      .transition()
      .duration(duration)
      .attr("transform", function(d) { 
        return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
      })

      // get data
      var data = node.data()[0];

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
      .duration(duration)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
    })

    function nodeLabelGenerator(target, data) {
      var labels = _svg.renders.labels.nodes[data.type];

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

        for (var attr in labels) {
          if (attr == 'type' || attr == 'id') {
            continue;
          }
          else {
            if (labels[attr]) {
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

      var data = link.data()[0]

      var points = _svg.renders.generateCurvedLinkPoints(data);

      var path = link.append('g')
                 .classed('link_path', true)

      var display;
      var x;
      var y; 

      // base on points to determing whether show the links.
      if (points.length == 0) {
        display = 'none';
        x = _svg.renders.xScale(0.5 * data.source.x+ 0.5 * data.target.x);
        y = _svg.renders.yScale(0.5 * data.source.y+ 0.5 * data.target.y);
      } 
      else {
        display = '';
        x = 0.5 * points[1][0] + 0.5 * points[2][0];
        y = 0.5 * points[1][1] + 0.5 * points[2][1];
      }

      path.append('defs')
      .append("marker")
      .attr("id", "link_marker_"+data[gvis.settings.key])
      .attr("markerWidth", "10")
      .attr("markerHeight", "10")
      .attr("refX", 5)
      .attr("refY", 3)
      .attr("orient", "auto")
      .attr("fill", 'black')
      .attr("markerUnits", "userSpaceOnUse") // User for "strokeWidth"
      .attr("stroke-width", "3px")
      .html('<path d="M0,0 L0,6 L7,3 L0,0"/>')

      path.append('path')
      .attr("id", 'link_path_'+data[gvis.settings.key])
      .attr("fill", 'transparent')
      .attr("stroke", 'black')
      .attr("stroke-width", 1)
      .attr("stroke-linecap", "round")
      .attr("stroke-dasharray", "5 5")
      .attr("opacity", 0.8)
      .attr("d", function(d) {
        var line = d3.svg.line().interpolate("basis");
        return line(points);
      })
      .attr("style", "marker-end:url(#link_marker_"+data[gvis.settings.key] + ")")

      // adding labels for links
      var label = link.append('g')
      .attr('id', 'label_' + data[gvis.settings.key])
      .attr("transform", function(d) { 
        return "translate(" + x + "," + y + ")"; 
      })


      label.attr('display', display);

      var text = label.append('text')
      .attr('id', 'label_text_' + data[gvis.settings.key])
      .attr("text-anchor", "middle")

      var bbox = text[0][0].getBBox();

      //label.insert('rect', '#label_' + data[gvis.settings.key])
      label.append('rect')
      .classed('label_background_rect', true)
      .attr('rx', 4)
      .attr('ry', 4)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
      .attr('fill', 'blue')
      .attr('opacity', 0.2)
    })
  }

  gvis.renders.svg.prototype.updateLinkRenderer = function() {
    var links = arguments[0]
    var _svg = arguments[1];
    var duration = arguments[2] || 0;

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

      if (points.length == 0) {
        display = 'none';
        x = _svg.renders.xScale(0.5 * data.source.x+ 0.5 * data.target.x);
        y = _svg.renders.yScale(0.5 * data.source.y+ 0.5 * data.target.y);
      } 
      else {
        display = '';
        x = 0.5 * points[1][0] + 0.5 * points[2][0];
        y = 0.5 * points[1][1] + 0.5 * points[2][1];
      }

      link.select('#link_path_'+data[gvis.settings.key])
      .transition()
      .duration(duration)
      .attr("d", function(d) {
        var line = d3.svg.line().interpolate("basis");
        
        return line(points);
      })

      var label = link.select('#label_'+data[gvis.settings.key])
      .transition()
      .duration(duration)
      .attr("transform", function(d) { 
        return "translate(" + x + "," + y + ")"; 
      })

      label.attr('display', display);

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
      .duration(duration)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
    })

    function linkLabelGenerator(target, data) {
      var labels = _svg.renders.labels.links[data.type];

      d3.select(target).selectAll('tspan').remove();

      if (!labels || Object.keys(labels).length === 0) {
        d3.select(target)
        .append("tspan")
        .attr("x", 0)
        .attr("dy", gvis.behaviors.render.linkLabelsFontSize)
        .text('Type:'+data.type)
      }
      else {
        if (!!labels['type']) {
          d3.select(target)
          .append("tspan")
          .attr("x", 0)
          .attr("dy", gvis.behaviors.render.linkLabelsFontSize)
          .text('Type:'+data.type)
        }

        for (var attr in labels) {
          if (attr == 'type') {
            continue;
          }
          else {
            if (labels[attr]) {
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

    legends[0].forEach(function(l, i) {
        if (!l) {
          return;
        }

        // get legend
        var legend = d3.select(l);

        legend
        .select('.legend_container')
        .transition()
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

  gvis.renders.svg.prototype.autoFit = function(duration) {
    duration = duration || 0;

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
    var min_ratio = Math.min(width_ratio, height_ratio) * 0.85;
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
    .duration(duration)
    .attr("transform", "translate(" + [this.renders.xScale(x_trans), this.renders.yScale(y_trans)] + ")" + " scale(" + min_ratio + ")");

    // tell the zoomer what we did so that next we zoom, it uses the
    // transformation we entered here
    this.zoom.translate([this.renders.xScale(x_trans), this.renders.yScale(y_trans)]);
    this.zoom.scale(min_ratio);
  }

  gvis.renders.svg.prototype.centerView = function(duration) {

    duration = duration || 0;

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
    .duration(duration)
    .attr("transform", "translate(" + [x_trans, y_trans] + ")" + " scale(" + scale + ")");

    // tell the zoomer what we did so that next we zoom, it uses the
    // transformation we entered here
    this.zoom.translate([x_trans, y_trans]);
  }



  /********** renders.canvas **********/
  gvis.renders.canvas = function(renders) {

  }


  /********** renders.map **********/
  gvis.renders.map = function(renders) {

  }
  
}).call(this)




