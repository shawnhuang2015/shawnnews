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
      .attr('id', function(d) {return d[gvis.settings.key]})
      .call(this.addLinkRenderer)


      d_nodes
      .enter()
      .append('g')
      .classed('node', true)
      .attr('id', function(d) {return d[gvis.settings.key]})
      .call(this.addNodeRenderer, this)

      d_legends
      .enter()
      .append('g')
      .classed('legend', true)
      .call(this.addLegendRenderer)

      // ENTER + UPDATE
      // Appending to the enter selection expands the update selection to include
      // entering elements; so, operations on the update selection after appending to
      // the enter selection will apply to both entering and updating nodes.
      // Updating current elements
      d_links.call(this.updateLinkRenderer)
      d_nodes.call(this.updateNodeRenderer);
      d_legends.call(this.updateLegendRenderer);
  

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
      // adding node
      var node = d3.select(n)
                .append('g')
                .attr("transform", function(d) { 
                  return "translate(" + _svg.renders.xScale(d.x) + "," + _svg.renders.yScale(d.y) + ")"; 
                })
                .call(drag)

      var data = node.data()[0];
      // bacground circle for node icon
      node
      .append('g')
      .classed('background_circle', true)
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
    }

    function dragended() {
        d3.select(this).classed("dragging", false);
    }
  }

  gvis.renders.svg.prototype.updateNodeRenderer = function() {

  }

  gvis.renders.svg.prototype.addLinkRenderer = function() {

  }

  gvis.renders.svg.prototype.updateLinkRenderer = function() {

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




