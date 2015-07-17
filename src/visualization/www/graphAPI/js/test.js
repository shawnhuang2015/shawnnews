var width = 960,
    height = 500;

function generateGraphData(tg) {
    var dataset = {}
    dataset.nodes = [];
    dataset.edges = [];

    tg.nodes.forEach(function(n, i) {
        var newNode = {};
            newNode.type = "type"+i%3;
            newNode.key = "key"+i.toString();
            newNode.attr = {}
            newNode.attr.weight = Math.random().toFixed(1);
            newNode.attr.name = "name" + Math.random().toFixed(1);
            //newNode.x = n.x;
            //newNode.y = n.y;
            //newNode.x = Math.random() * width;
            //newNode.y = Math.random() * height;

            dataset.nodes.push(newNode);
    })

    tg.links.forEach(function(l, i) {
        var newEdge = {};

        newEdge.source = {}
        newEdge.target = {}

        newEdge.source.type = dataset.nodes[l.source].type;
        newEdge.source.key = dataset.nodes[l.source].key;

        newEdge.target.type = dataset.nodes[l.target].type;
        newEdge.target.key = dataset.nodes[l.target].key;

        newEdge.attr = {}
        newEdge.attr.weight = Math.random().toFixed(1);
        newEdge.attr.name = "name"+Math.random().toFixed(1);

        dataset.edges.push(newEdge);
    })


    return dataset;
}

function generateRandomGraphData(nn, nt) {
    var dataset = {};
    dataset.nodes = [];
    dataset.edges = [];
    var numberofnodes = nn;
    var numberoftype = nt;

    var tempn = numberofnodes * numberoftype;

    // randomly generate nodes.
    for (var i=0; i<numberoftype; ++i) {
        for (var j=0; j<numberofnodes; ++j) {
            var newNode = {};
            newNode.type = "type"+i.toString();
            newNode.key = "key"+j.toString();
            newNode.attr = {}
            newNode.attr.weight = "weight" + Math.random().toFixed(1);
            newNode.attr.name = "name" + Math.random().toFixed(1);

            dataset.nodes.push(newNode);
        }
    }

    // randomly generate edges for all nodes.
    for (var i=0; i<tempn; ++i) {

        var newEdge = {};

        newEdge.source = {}
        newEdge.target = {}

        newEdge.source.type = "type"+Math.floor(Math.random()*numberoftype).toString();
        newEdge.source.key = "key"+Math.floor(Math.random()*numberofnodes).toString();

        newEdge.target.type = "type"+Math.floor(Math.random()*numberoftype).toString();
        newEdge.target.key = "key"+Math.floor(Math.random()*numberofnodes).toString();

        newEdge.attr = {}
        newEdge.attr.weight = "weight"+Math.random().toFixed(1);
        newEdge.attr.name = "name"+Math.random().toFixed(1);


        if (newEdge.source.type == newEdge.target.type &&
            newEdge.source.key == newEdge.target.key) {
            continue;
        }
        else {
            dataset.edges.push(newEdge)
        }
    }

    // randomly generate edges for several nodes;
    for (var i=0; i<tempn; ++i) {

        var newEdge = {};

        newEdge.source = {}
        newEdge.target = {}

        newEdge.source.type = "type"+Math.floor(Math.random()*numberoftype).toString();
        newEdge.source.key = "key"+Math.floor(Math.random()*numberofnodes).toString();

        newEdge.target.type = "type"+Math.floor(Math.random()*2).toString();
        newEdge.target.key = "key"+Math.floor(Math.random()*15).toString();

        newEdge.attr = {}
        newEdge.attr.weight = "weight"+Math.random().toFixed(1);
        newEdge.attr.name = "name"+Math.random().toFixed(1);

        if (newEdge.source.type == newEdge.target.type &&
            newEdge.source.key == newEdge.target.key) {
            continue;
        }
        else {
            dataset.edges.push(newEdge)
        }
    }

    // randomly generate edges for a small part of nodes which will be the sink nodes.
    for (var i=0; i<tempn; ++i) {

        var newEdge = {};

        newEdge.source = {}
        newEdge.target = {}

        newEdge.source.type = "type"+Math.floor(Math.random()*numberoftype).toString();
        newEdge.source.key = "key"+Math.floor(Math.random()*numberofnodes).toString();

        newEdge.target.type = "type"+Math.floor(Math.random()*1).toString();
        newEdge.target.key = "key"+Math.floor(Math.random()*5).toString();

        newEdge.attr = {}
        newEdge.attr.weight = "weight"+Math.random().toFixed(1);
        newEdge.attr.name = "name"+Math.random().toFixed(1);

        if (newEdge.source.type == newEdge.target.type &&
            newEdge.source.key == newEdge.target.key) {
            continue;
        }
        else {
            dataset.edges.push(newEdge)
        }
    }

    // remove the duplicate edges.
    dataset.edges = dataset.edges.filter(function(item, index) {
        var pos = 0;
        dataset.edges.some(function(d, i) {
            if (item.source.type ==  d.source.type &&
                item.source.key == d.source.key &&
                item.target.type == d.target.type &&
                item.target.key == d.target.key) {
                pos = i;
                return true;
            }
        })

        return index == pos;
    })
    

    // remove the duplicate bidirectional deges.
    dataset.edges = dataset.edges.filter(function(item, index) {
        var pos = 0;
        dataset.edges.some(function(d, i) {
            if (item.source.type ==  d.target.type &&
                item.source.key == d.target.key &&
                item.target.type == d.source.type &&
                item.target.key == d.source.key) {
                pos = i;
                return true;
            }
        })

        return 0 == pos;
    })

    return dataset;
}

function testInitializedGraphDataSet(rawData) {
    var result = {};
    result.nodes = []
    result.links = []

    rawData.nodes.forEach(function(d) {
        d.x = Math.random()*width;
        d.y = Math.random()*height;

        result.nodes.push(d);
    })

    rawData.edges.forEach(function(d) {
        var sourceID = d.source;
        var targetID = d.target;

        rawData.nodes.forEach(function(n, index) {
            if (d.target.key == n.key && d.target.type == n.type) targetID = index;
            if (d.source.key == n.key && d.source.type == n.type) sourceID = index;
        }) 

        var newLink = {};
        newLink.attr = d.attr;
        newLink.target = targetID;
        newLink.source = sourceID;

        newLink.source = sourceID;
        newLink.target = targetID;

        result.links.push(newLink);
    })

    return result;
}


function testForceDirectedLayoutGraph(div_id, graph_data, tabindex) {
    
    var shiftKey, ctrlKey, metaKey;

    var nodeGraph = graph_data;
    var xScale = d3.scale.linear()
    .domain([0,width]).range([0,width]);
    var yScale = d3.scale.linear()
    .domain([0,height]).range([0, height]);

    var svg = d3.select("#"+div_id)
    .attr("tabindex", tabindex)
    .on("keydown.brush", keydown)
    .on("keyup.brush", keyup)
    //.each(function() { this.focus(); })
    .append("svg")
    .attr("width", width)
    .attr("height", height)
    .attr("user-select", 'none')
    //.on("mousedown", zoomstart);

    var zoomer = d3.behavior.zoom().
        scaleExtent([0.1,5]).
        x(xScale).
        y(yScale).
        //size([1000, 1000]).
        on("zoomstart", zoomstart).
        on("zoom", redraw).
        on("zoomend", function() {
            console.log("zoomend")
        })

    function zoomstart() {
        //force.stop();
        console.log("zoomstart")
    }

    function redraw() {
        vis.attr("transform",
                 "translate(" + d3.event.translate + ")" + " scale(" + d3.event.scale + ")");

        //console.log(d3.event.scale)
    }

    var tempExtent = undefined;

    var brusher = d3.svg.brush()
    //.x(d3.scale.identity().domain([0, width]))
    //.y(d3.scale.identity().domain([0, height]))
    .x(xScale)
    .y(yScale)
    .on("brushstart", function(d) {
        node.each(function(d) { 
            d.previouslySelected = shiftKey && d.selected; });

        force.stop();
    })
    .on("brush", function() {
        var extent = d3.event.target.extent();
        tempExtent = extent;

        node.classed("selected", function(d) {
            return d.selected = d.previouslySelected ^
            (extent[0][0] <= d.x && d.x < extent[1][0]
             && extent[0][1] <= d.y && d.y < extent[1][1]);
        });

        if (metaKey) {
            ;
        }
        else {
            node.classed("fixed", function(d) {
                var temp = (extent[0][0] <= d.x && d.x < extent[1][0]
                 && extent[0][1] <= d.y && d.y < extent[1][1]);
                d.fixed |= temp;

                return d.fixed;
            })
        }

        

        console.log("brush event")
    })
    .on("brushend", function() {
        d3.event.target.clear();
        d3.select(this).call(d3.event.target);
        var extent = tempExtent

        if (metaKey) {
            node.classed("fixed", function(d) {
                var temp = (extent[0][0] <= d.x && d.x < extent[1][0]
                 && extent[0][1] <= d.y && d.y < extent[1][1]);
                d.fixed = temp ? !d.fixed: d.fixed;

                return d.fixed;
            })
        }

        force.resume();
    });

    var svg_graph = svg.append('svg:g')
    .on("mousedown", function() {
        console.log("svg graph mousedown")
        node.each(function(d) {
            d.selected = false;
            d.previouslySelected = false;
        });
        node.classed("selected", false);

        link.each(function(d) {
            d.selected = false;
            d.previouslySelected = false;
            d3.select(this).classed("selected", false);
        });

    })
    .call(zoomer)
    //.call(brusher)
    

    var rect = svg_graph.append('svg:rect')
    .attr('width', width)
    .attr('height', height)
    .attr('fill', '#22ff22')
    .attr('opacity', 0.1)
    .attr('stroke', 'transparent')
    .attr('stroke-width', 5)
    //.attr("pointer-events", "all")
    .attr("id", "zrect")
    

    var brush = svg_graph.append("g")
    .datum(function() { return {selected: false, previouslySelected: false}; })
    .attr("class", "brush");

    var vis = svg_graph.append("svg:g");

    vis.attr('fill', 'red')
    .attr('stroke', 'blue')
    .attr('stroke-width', 1)
    .attr('opacity', 0.5)
    .attr('id', 'vis')


    brush.call(brusher)
    .on("mousedown.brush", null)
    .on("touchstart.brush", null) 
    .on("touchmove.brush", null)
    .on("touchend.brush", null); 

    brush.select('.background').style('cursor', 'auto');

    var link = vis.append("g")
    .attr("class", "link")
    .selectAll("line");

    var node = vis.append("g")
    .attr("class", "node")
    .selectAll("circle");

    center_view = function() {
        // Center the view on the molecule(s) and scale it so that everything
        // fits in the window

        if (nodeGraph === null)
            return;

        var nodes = nodeGraph.nodes;

        //no molecules, nothing to do
        if (nodes.length === 0)
            return;

        // Get the bounding box
        min_x = d3.min(nodes.map(function(d) {return d.x;}));
        min_y = d3.min(nodes.map(function(d) {return d.y;}));

        max_x = d3.max(nodes.map(function(d) {return d.x;}));
        max_y = d3.max(nodes.map(function(d) {return d.y;}));


        // The width and the height of the graph
        mol_width = max_x - min_x;
        mol_height = max_y - min_y;

        // how much larger the drawing area is than the width and the height
        width_ratio = width / mol_width;
        height_ratio = height / mol_height;

        // we need to fit it in both directions, so we scale according to
        // the direction in which we need to shrink the most
        min_ratio = Math.min(width_ratio, height_ratio) * 0.8;

        // the new dimensions of the molecule
        new_mol_width = mol_width * min_ratio;
        new_mol_height = mol_height * min_ratio;

        // translate so that it's in the center of the window
        x_trans = -(min_x) * min_ratio + (width - new_mol_width) / 2;
        y_trans = -(min_y) * min_ratio + (height - new_mol_height) / 2;


        // do the actual moving
        vis.attr("transform",
                 "translate(" + [x_trans, y_trans] + ")" + " scale(" + min_ratio + ")");

                 // tell the zoomer what we did so that next we zoom, it uses the
                 // transformation we entered here
                 zoomer.translate([x_trans, y_trans ]);
                 zoomer.scale(min_ratio);

    };


    nodeGraph.links.forEach(function(d) {
        d.source = nodeGraph.nodes[d.source];
        d.target = nodeGraph.nodes[d.target];
    });

    


    var force = d3.layout.force()
    .charge(-120)
    //.chargeDistance(200)
    .linkDistance(function(d) {
        return d.attr.weight * 100;
    })
    //.friction(0.5)
    //.gravity(0.1)
    .nodes(nodeGraph.nodes)
    .links(nodeGraph.links)
    .size([width, height])
    .start();

    //force.stop();

    function dragstarted(d) {
        d3.event.sourceEvent.stopPropagation();
        if (!d.selected && !shiftKey) {
            // if this node isn't selected, then we have to unselect every other node
            node.classed("selected", function(p) { return p.selected =  p.previouslySelected = false; });
        }

        d3.select(this).classed("selected", function(p) { d.previouslySelected = d.selected; return d.selected = true; });

        node.filter(function(d) { return d.selected; })
            .each(function(d) { d.fixed |= 2; })

        d3.select(this).classed("fixed", d.fixed = true);

        console.log("drag start")
        force.resume()
    }

    function dragged(d) {
        node.filter(function(d) { return d.selected; })
        .each(function(d) { 
            d.x += d3.event.dx;
            d.y += d3.event.dy;

            d.px += d3.event.dx;
            d.py += d3.event.dy;

            d3.select(this)
            .attr("cx", d.x)
            .attr("cy", d.y)
        })

      link.attr("x1", function(d) { return d.source.x; })
      .attr("y1", function(d) { return d.source.y; })
      .attr("x2", function(d) { return d.target.x; })
      .attr("y2", function(d) { return d.target.y; });

      //force.stop();
    }

    function dragended(d) {
        //d3.event.sourceEvent.stopPropagation();
        //d3.select(self).classed("dragging", false);

        console.log("dragended")

    }

    node = node.data(nodeGraph.nodes).enter().append("circle")
    .attr("r", 4)
    .attr("cx", function(d) { return d.x; })
    .attr("cy", function(d) { return d.y; })
    .on("dblclick", function(d) { 
        d3.event.stopPropagation(); 

        d3.select(this).classed("fixed", d.fixed = false);
        force.resume()
        console.log("dbclick event")
    })
    .on("click", function(d) {
        //d3.event.stopPropagation();
        console.log("click event")
        if (d3.event.defaultPrevented) return;

        if (!shiftKey) {
            //if the shift key isn't down, unselect everything
            node.classed("selected", function(p) { return p.selected =  p.previouslySelected = false; })
        }

        // always select this node
        d3.select(this).classed("selected", d.selected = !d.previouslySelected);
    })
    .on("mousedown", function() {
        //d3.event.stopPropagation();
        console.log("node mouse down")
    })
    .on("mousedown.logaaaa", function() {
        
        console.log("node mouse down log")
    })
    .on("mousedown.asdasdaaa", function() {
        console.log("node mouse down asdasdaaa")
    })
    .on("mouseup", function(d) {
        if (d.selected && shiftKey) d3.select(this).classed("selected", d.selected = false);
        console.log("mouseup event")
    })
    .on("mouseover", function(){
        console.log("mouseover a node")
    })
    .on("mouseout", function() {
        console.log("mouseout a node")
    })
    .call(d3.behavior.drag()
          .on("dragstart", dragstarted)
          .on("drag", dragged)
          .on("dragend", dragended));

link = link.data(nodeGraph.links).enter().append("line")
    .attr("x1", function(d) { return d.source.x; })
    .attr("y1", function(d) { return d.source.y; })
    .attr("x2", function(d) { return d.target.x; })
    .attr("y2", function(d) { return d.target.y; })
    .on("dblclick", function(d) { 
        d3.event.stopPropagation(); 
        console.log("Link dbclick event")

        force.resume()
    })
    .on("click", function(d) {
        //d3.event.stopPropagation();
        console.log("link click event")
        if (d3.event.defaultPrevented) return;

        if (!shiftKey) {
            //if the shift key isn't down, unselect everything
            link.classed("selected", function(p) { return p.selected =  p.previouslySelected = false; })
        }

        // always select this link
        d3.select(this).classed("selected", d.selected = !d.previouslySelected);
    })
    .on("mousedown", function() {
        d3.event.stopPropagation();
        console.log("link mouse down")
    })
    .on("mousedown.logaaaa", function() {
        
        console.log("link mouse down log")
    })
    .on("mousedown.asdasdaaa", function() {
        console.log("link mouse down asdasdaaa")
    })
    .on("mouseup", function(d) {
        if (d.selected && shiftKey) d3.select(this).classed("selected", d.selected = false);
        console.log("link mouseup event")
    })
    .on("mouseover", function(){
        console.log("mouseover a link")
    })
    .on("mouseout", function() {
        console.log("mouseout a link")
    });
  function tick() {
      link.attr("x1", function(d) { return d.source.x; })
      .attr("y1", function(d) { return d.source.y; })
      .attr("x2", function(d) { return d.target.x; })
      .attr("y2", function(d) { return d.target.y; });

      node.attr('cx', function(d) { return d.x; })
      .attr('cy', function(d) { return d.y; });
  };

  force.on("tick", tick);

      //setTimeout(function(){force.stop();console.log("force stop")}, 4000);
          /*
          node.call(force.drag().origin(function() {
            var x = d3.select(this).attr("cx");
            var y = d3.select(this).attr("cy");
            return {x: x, y: y};
          }))
            */
    //});


    function keydown() {
        shiftKey = d3.event.shiftKey || d3.event.metaKey;
        metaKey = d3.event.metaKey;
        ctrlKey = d3.event.ctrlKey;

        console.log('d3.event', d3.event)

        if (d3.event.keyCode == 67) {   //the 'c' key
            center_view();
        }

        if (d3.event.keyCode == 88) { // the 'x' key
            force.resume();
            //force.alpha(.1);
        }

        if (d3.event.keyCode == 90) { // the 'x' key
            force.stop();
            //force.alpha(.1);
        }

        if (shiftKey) {
            svg_graph.call(zoomer)
            .on("mousedown.zoom", null)
            .on("touchstart.zoom", null)                                                                      
            .on("touchmove.zoom", null)                                                                       
            .on("touchend.zoom", null);                                                                       

            //svg_graph.on('zoom', null);                                                                     
            vis.selectAll('g.gnode')
            .on('mousedown.drag', null);

            brush.select('.background').style('cursor', 'crosshair')
            brush.call(brusher);
        }
    }

    function keyup() {
        shiftKey = d3.event.shiftKey || d3.event.metaKey;
        ctrlKey = d3.event.ctrlKey;

        brush.call(brusher)
        .on("mousedown.brush", null)
        .on("touchstart.brush", null)                                                                      
        .on("touchmove.brush", null)                                                                       
        .on("touchend.brush", null);                                                                       

        brush.select('.background').style('cursor', 'auto')
        svg_graph.call(zoomer);
    }
}