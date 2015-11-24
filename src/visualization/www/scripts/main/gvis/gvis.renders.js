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
      var b = 30; // initNodeSetting.r * 4;
      var edgeMargin = 20;
      var markMargin = 2;

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
      var t0 = a * (td - 20) + 10;
      var t1 = (1-a) * (td - 20) + 10;

      //var angle = 90 - Math.atan2(x1-x0, y1-y0)*180/Math.PI;
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
      //return [[this.xScale(tx0), this.yScale(ty0)], [this.xScale(C0[0]), this.yScale(C0[1])], [this.xScale(C1[0]), this.yScale(C1[1])], [this.xScale(tx1), this.yScale(ty1)]];
    }
  }

  gvis.renders.prototype.update = function() {
    this.renderer.update();
  }


  /********** renders sub class **********/

  /********** renders.svg **********/
  gvis.renders.svg = function(renders) {

    this.renders = renders;
    var _this = renders._this;

    var container_id = "#" + renders.render_container

    var zoom = d3.behavior.zoom()
                .scaleExtent([0.1, 10])
                .on("zoom", zoomed);

    var svg = d3.select(container_id)
              .append("svg")
              .attr("width", "100%")
              .attr("height", "100%")
              // This is used for a custom right-click 
              .on("contextmenu", function() {
                //console.log('oncontextmenu svg')
                d3.event.preventDefault();
              })
              .call(zoom);

    svg.append('rect')
      .classed('background_rect', true) 
      .attr("width", "100%")
      .attr("height", "100%")
      .attr("fill", "#dadaeb")
      .attr("opacity", 0.2)
      .attr("stroke", "transparent")

    var g_zoomer = svg.append('g')

    var g_links = g_zoomer.append('g').classed('links_group', true)

    var g_nodes = g_zoomer.append('g').classed('nodes_group', true)

    var g_legends = g_zoomer.append('g').classed('legends_group', true)

    this.update = function() {
      console.log('render.svg.update ' + container_id)

      this.renders.range_width = +d3.select(container_id).style('width').slice(0, -2);
      this.renders.range_height = +d3.select(container_id).style('height').slice(0, -2);

      this.renders.xScale = d3.scale.linear()
                    .domain([0, this.renders.domain_width])
                    .range([0, this.renders.range_width]);

      this.renders.yScale = d3.scale.linear()
                   .domain([0, this.renders.domain_height])
                   .range([0, this.renders.range_height]);

      // DATA JOIN
      // Join new data with old elements, if any.
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

      // ENTER + UPDATE
      // Appending to the enter selection expands the update selection to include
      // entering elements; so, operations on the update selection after appending to
      // the enter selection will apply to both entering and updating nodes.
      // Updating current elements
      d_links.call(this.updateLinkRenderer, this, 500)
      d_nodes.call(this.updateNodeRenderer, this, 500);
      d_legends.call(this.updateLegendRenderer, this);
  

      // EXIT
      // Remove old elements as needed.
      d_links.exit().remove();
      d_nodes.exit().remove();
      d_legends.exit().remove();
    }

    function zoomed() {
      g_zoomer.attr("transform", "translate(" + d3.event.translate + ")scale(" + d3.event.scale + ")");
    }    
  }

  gvis.renders.svg.prototype.addNodeRenderer = function() {
    console.log("addNodeRenderer", this, arguments)

    var _svg = arguments[1];

    var drag = d3.behavior.drag()
        .origin(function(d) { return d; })
        .on("dragstart", dragstarted)
        .on("drag", dragging)
        .on("dragend", dragended);

    this[0].forEach(function(n) {
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
                .call(drag)

      var data = node.data()[0];
      // bacground circle for node icon
      node
      .append('g')
      .classed('node_background_circle', true)
      .append('circle')
      .attr('stroke-opacity', 0)
      .attr('fill-opacity', 0)
      .attr('r', 20)

      // add node icon
      var icon = gvis.utils.icons('default');

      node
      .append('g')
      .classed('icon', true)
      .html(icon.svg)
      .attr('transform', 'translate('+icon.translate+') scale('+icon.scale+')')
      .attr('fill', 'red')

      var label = node
                  .append('g')

      var text = label.append('text')
      .attr('id', function(d) {
        return 'label_' + d[gvis.settings.key];
      })
      .attr("text-anchor", "middle")
      .attr('y', 31)
      .text(function(d) {
        return d.type;
      })

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
      .attr('fill', 'red')
      .attr('opacity', 0.2)
      
    })


    function dragstarted() {
      d3.event.sourceEvent.stopPropagation();
      d3.select(this).classed("dragging", true);
    }

    function dragging(d) {
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

          _svg.updateLinkRenderer.call(link, null, _svg)
          // var data = link.data()[0]
          // var points = _svg.renders.generateCurvedLinkPoints(data)

          // link.select('#link_path_'+data[gvis.settings.key])
          // .attr("d", function(d) {
          //   var line = d3.svg.line().interpolate("basis");
          //   return line(points);
          // })

          //link.select('#link')
        }
      }
    }

    function dragended() {
      d3.select(this).classed("dragging", false);
    }
  }

  gvis.renders.svg.prototype.updateNodeRenderer = function() {
    var _svg = arguments[1];
    var duration = arguments[2] || 0;

    this[0].forEach(function(n) {
      if (!n) {
        return;
      }

      // adding node
      var node = d3.select(n);

      node
      .select('.node_container')
      .transition()
      .duration(duration)
      .attr("transform", function(d) { 
        return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
      })

      var data = node.data()[0];

      var text = node.select('#label_' + data[gvis.settings.key])
      .text(function(d) {
        return d.type;
      })

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
  }

  gvis.renders.svg.prototype.addLinkRenderer = function() {
    var _svg = arguments[1];

    this[0].forEach(function(l) {
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

      var label = link.append('g')
                  .attr('id', 'label_' + data[gvis.settings.key])


      var display;
      var x;
      var y; 

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


      label.attr('display', display);

      var text = label.append('text')
      .attr('id', 'label_text_' + data[gvis.settings.key])
      .attr('x', x)
      .attr('y', y)
      .attr("text-anchor", "middle")
      .text(function(d) {
        return d.type;
      })


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
    var _svg = arguments[1];
    var duration = arguments[2] || 0;

    this[0].forEach(function(l) {
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

      label.attr('display', display);

      var text = label.select('#label_text_' + data[gvis.settings.key])
      .attr('x', x)
      .attr('y', y)
      .text(function(d) {
        return d.type;
      })

      var bbox = text[0][0].getBBox();

      //label.insert('rect', '#label_' + data[gvis.settings.key])
      label
      .select('.label_background_rect')
      .transition()
      .duration(duration)
      .attr('x', bbox.x)
      .attr('y', bbox.y)
      .attr('width', bbox.width)
      .attr('height', bbox.height)
    })
  }

  gvis.renders.svg.prototype.addLegendRenderer = function() {

  }

  gvis.renders.svg.prototype.updateLegendRenderer = function() {

  }



  /********** renders.canvas **********/
  gvis.renders.canvas = function(renders) {

  }


  /********** renders.map **********/
  gvis.renders.map = function(renders) {

  }
  
}).call(this)




