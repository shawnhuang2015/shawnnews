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
    var _this = renders._this;

    var container_id = "#" + renders.render_container

    var zoom = d3.behavior.zoom()
                .scaleExtent([0.1, 10])
                .on("zoom", zoomed);

    var svg = d3.select(container_id)
              .append("svg")
              .attr("width", "100%")
              .attr("height", "100%")
              .call(zoom);

    var g_zoomer = svg.append('g')

    g_zoomer.append("g")
      .attr("class", "x axis")
    .selectAll("line")
      .data(d3.range(-renders.range_width, renders.range_width, 10))
    .enter().append("line")
      .attr("x1", function(d) { return d; })
      .attr("y1", -renders.range_height)
      .attr("x2", function(d) { return d; })
      .attr("y2", renders.range_height);


    g_zoomer.append("g")
      .attr("class", "y axis")
    .selectAll("line")
      .data(d3.range(-renders.range_height, renders.range_height, 10))
    .enter().append("line")
      .attr("x1", -renders.range_width)
      .attr("y1", function(d) { return d; })
      .attr("x2", renders.range_width)
      .attr("y2", function(d) { return d; });

    this.update = function() {
      console.log('render.svg.update ' + container_id)

      renders.range_width = +d3.select(container_id).style('width').slice(0, -2);
      renders.range_height = +d3.select(container_id).style('height').slice(0, -2);

      renders.xScale = d3.scale.linear()
                    .domain([0, renders.domain_width])
                    .range([0, renders.range_width]);

      renders.yScale = d3.scale.linear()
                   .domain([0, renders.domain_height])
                   .range([0, renders.range_height])

    }

    function zoomed() {
      g_zoomer.attr("transform", "translate(" + d3.event.translate + ")scale(" + d3.event.scale + ")");
    }
  }



  /********** renders.canvas **********/
  gvis.renders.canvas = function(renders) {

  }


  /********** renders.map **********/
  gvis.renders.map = function(renders) {

  }
  
}).call(this)




