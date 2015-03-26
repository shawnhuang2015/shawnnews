!function() {
var gsqlv = function(x) {
	var gsqlv = {
		version : '1.1',
	};
	var setting; //var setting = {'divID':''divid', 'width':width, 'height':height};
	var data = {"nodes":[], "links":[]};
	var node_links = {}; // hash table for node and its linked node.
	var link_nodes = {}; // hash tabel for links and its source object and target object in node_links.

	var backup_data = {};
	var rawData;
	var event_handlers = {}; 

	var styleSheet;


	var zoomer_scale = [0.2, 15];

	var shiftKey, ctrlKey, altKey;
	var xScale;
	var yScale;

	var svg;
	var svg_zoomer_group, zoomer;
	var svg_background_rect;
	var svg_brusher_group, brusher, brusherExtent;
	var svg_vis_group;
	var svg_vis_nodes_group;
	var svg_vis_links_group;
	var svg_vis_nodes_labels_group;
	var svg_vis_links_labels_group;

	var layout = "force"; //layout type : force; randoml; hierarchical
	var layouts = {};

	var rootNode = 0;	// for hierarchical layout

	var dragger;

	var tickPreRender = 2;
	var tickNumber = 0;

	var temp_nodes;
	var temp_links;
	var temp_nodes_labels;
	var temp_links_labels;

	var node_label_key_array = []
	var node_label_attr_array = ["type", "id", "some_int"]

	var edge_label_key_array = []
	var edge_label_attr_array = ["some_int"]

	var expand_depth = 1;

	var preDefinition = {};

	var timer;

	window.__$$global_boolean = false;
	window.__$$global_string = x;
	window.__$$global_real = 0.0;

	var initNodeSetting = {
		'stroke': '#777',
		'stroke-width': '1',
		'fill': '#aaa',
		'opacity':1,
		'selected' : false,
		'previouslySelected' : false,
		'hierarchied' : false, // for hierarchical layout
		'x':window.v_width / 2,
		'y':window.v_height / 2,
	}

	var initLinkSetting = {
		'stroke': '#888',
		'stroke-width': '1',
		'fill': '#888',
		'opacity':0.8,
		'selected': false
	}

	gsqlv.data = function(newData) {
		if (!arguments.length) {
			return data;
		}
		else {
			newData = refineDataName(newData)
			rawData = clone(newData)

			data.nodes = []
			data.links = []

			rawData.nodes.forEach(function(d) {
				gsqlv.initNode(d);
				data.nodes.push(d);

				//nodeKey = d.type+ "&" +d.id
				//node_links[nodeKey] = {};
				//node_links[nodeKey].node = d;
				//node_links[nodeKey].links = [];
			})

			rawData.edges.forEach(function(d) {
				var sourceID = d.source;
				var targetID = d.target;

				rawData.nodes.forEach(function(n, index) {
					if (d.target.id == n.id && d.target.type == n.type) targetID = n;
					if (d.source.id == n.id && d.source.type == n.type) sourceID = n;
				}) 

				var newLink = {};
				newLink.attr = d.attr;
				newLink.target = targetID;
				newLink.source = sourceID;

				newLink.source = sourceID;
				newLink.target = targetID;

				/*
				nodeKey = targetID.type + "&" + targetID.id

				if (node_links[nodeKey].links.indexOf(sourceID) == -1) {
					node_links[nodeKey].links.push(sourceID);
				}

				nodeKey = sourceID.type + "&" + sourceID.id

				if (node_links[nodeKey].links.indexOf(targetID) == -1) {
					node_links[nodeKey].links.push(targetID);
				}
				*/

				gsqlv.initLink(newLink);
				data.links.push(newLink);
			})

			backup_data = clone(data)

			gsqlv.initNodeLinks();

			gsqlv.runPreDefinition(data.nodes, data.links)
			
			//gsqlv.nodes(data.nodes);
			//gsqlv.links(data.links);

			return gsqlv;
		}
	}

	gsqlv.initNodeLinks = function() {
		node_links = {};
		link_nodes = {};

		data.nodes.forEach(function(d) {
				nodeKey = d.type+ "&" +d.id
				node_links[nodeKey] = {};
				node_links[nodeKey].node = d;
				node_links[nodeKey].links = [];
			})

		data.links.forEach(function(d) {
			var sourceID = d.source;
			var targetID = d.target;

			targetKey = targetID.type + "&" + targetID.id
			sourceKey = sourceID.type + "&" + sourceID.id

			if (node_links[targetKey].links.indexOf(sourceID) == -1) {
				node_links[targetKey].links.push(sourceID);
			}

			if (node_links[sourceKey].links.indexOf(targetID) == -1) {
				node_links[sourceKey].links.push(targetID);
			}

			linkKey = targetKey + "$" + sourceKey
			link_nodes[linkKey] = {};
			link_nodes[linkKey].link = d;
			link_nodes[linkKey].target = node_links[targetKey];
			link_nodes[linkKey].source = node_links[sourceKey];
		})
	}

	gsqlv.clean = function() {
		gsqlv.data().nodes = [];
		gsqlv.data().links = [];
		node_links = {};
		link_nodes = {};
		gsqlv.redraw()

		return gsqlv;
	}

	gsqlv.initNode = function(x) {
		for (var key in initNodeSetting) {
			x[key] = initNodeSetting[key];
		}

		return x;
	}

	gsqlv.initLink = function(x) {
		for (var key in initLinkSetting) {
			x[key] = initLinkSetting[key];
		}
		return x;
	}

	gsqlv.addSubGraph = function(newGraph) {
		if (!arguments.length) return gsqlv;

		newGraph = refineDataName(newGraph)

		newGraph.nodes.forEach(function(n) {
			newNode = gsqlv.addNode(n);

			if (newNode) {
				gsqlv.runPreDefinition([newNode],[])
			}
		})

		newGraph.edges.forEach(function(e) {
			newLink = gsqlv.addLink(e);

			if (newLink) {
				gsqlv.runPreDefinition([],[newLink])
			}
		})	

		return gsqlv;
	}

	gsqlv.nodes = function(x) {
		if (!arguments.length) {
			return data.nodes;
		}
		else {
			//data.nodes = x;
			return gsqlv;
		}
	}

	gsqlv.addNode = function(x) {
		if (!arguments.length) return;

		var node = data.nodes.filter(function(n) {
			return (n.type === x.type && n.id === x.id);
		})[0]

		if (node) {
			return false;
		}
		else {
			gsqlv.initNode(x);
			data.nodes.push(x);

			nodeKey = x.type+"&"+x.id;
			node_links[nodeKey] = {}
			node_links[nodeKey].node = x;
			node_links[nodeKey].links = [];
			return x;
		}
	}

	gsqlv.links = function(x) {
		if(!arguments.length) {
			return data.links;
		}
		else {
			//data.links = x;
			return gsqlv;
		}
	}

	gsqlv.addLink = function(x) {
		if (!arguments.length) return;

		if (x.target.type === x.source.type && x.target.id === x.source.id) return gsqlv;

		var link = data.links.filter(function(l) {
			return (l.source.type === x.source.type && l.target.type === x.target.type
				&& l.source.id === x.source.id && l.target.id === x.target.id);
		})[0]

		if (link) {
			return false;
		}
		else {
			var sourceID = x.source; // index of source
			var targetID = x.target; // index of target

			data.nodes.forEach(function(n, index) {
				if (x.target.id == n.id && x.target.type == n.type) targetID = n;
				if (x.source.id == n.id && x.source.type == n.type) sourceID = n;
			}) 

			var newLink = {};
			newLink.attr = x.attr;
			newLink.target = targetID;	// object of target
			newLink.source = sourceID;	// object of source

			targetKey = targetID.type + "&" + targetID.id

			if (node_links[targetKey].links.indexOf(sourceID) == -1) {
				node_links[targetKey].links.push(sourceID);
			}

			sourceKey = sourceID.type + "&" + sourceID.id

			if (node_links[sourceKey].links.indexOf(targetID) == -1) {
				node_links[sourceKey].links.push(targetID);
			}

			linkKey = targetKey + "$" + sourceKey
			link_nodes[linkKey] = {};
			link_nodes[linkKey].link = newLink;
			link_nodes[linkKey].target = node_links[targetKey];
			link_nodes[linkKey].source = node_links[sourceKey];

			gsqlv.initLink(newLink);
			data.links.push(newLink);
			return newLink;
		}
	}

	gsqlv.node_links = function(x) {
		if (!arguments.length) {
			return node_links;
		}
		else {
			;
		}
	}

	gsqlv.link_nodes = function(x) {
		if (!arguments.length) {
			return link_nodes;
		}
		else {
			;
		}
	}

	gsqlv.setting = function(x) {
		if (!arguments.length) {
			return setting;
		}
		else {
			setting = x;
			return gsqlv;
		}
	}

	gsqlv.init = function(x) {
		if (arguments.length) {
			setting = x;
		}

		if (setting == undefined) {
			return console.log("setting is missing.");
		}

		d3.select("body").append("style");
		styleSheet = document.styleSheets[document.styleSheets.length - 1];
		gsqlv.initStyleSheet()

		var width = setting.width;
		var height = setting.height;

		svg = d3.select("#" + setting.divID)
		.attr("tabindex", 0)
		.style("width", width+"px")
		.style("height", height+"px")
		.on("keydown.brush", event_handlers.keydown_brush)
		.on("keyup.brush", event_handlers.keyup_brush)
		.each(function() {
			this.focus();
		})
		.append("svg")
		.attr("width", "100%")
		.attr("height", "100%")
		//.attr("width", width)
		//.attr("height", height)
		.attr("user-select", 'none')

		xScale = d3.scale.linear()
		.domain([0, width])
		.range([0, width])

		yScale = d3.scale.linear()
		.domain([0, height])
		.range([0, height])

		zoomer = d3.behavior.zoom()
		.scaleExtent(zoomer_scale)
		.x(xScale)
		.y(yScale)
		.on("zoomstart", event_handlers.zoomstart)
		.on("zoom", event_handlers.zooming)
		.on("zoomend", event_handlers.zoomend)

		svg_zoomer_group = svg.append('svg:g')
		.on("mousedown", event_handlers.zoomer_mousedown)
		.call(zoomer)

		svg_background_rect = svg_zoomer_group.append("rect")
		.attr("width", width)
		.attr("height", height)
		.attr("fill", "#99ccff")
		.attr("opacity", 0.2)
		.attr("stroke", "transparent")
		.attr("id", "svg_background_rect")

		brusher = d3.svg.brush()
		.x(xScale)
		.y(yScale)
		.on("brushstart", event_handlers.brushstart)
		.on("brush", event_handlers.brush)
		.on("brushend", event_handlers.brushend)

		svg_brusher_group = svg_zoomer_group.append("g")
		.datum(function() {return {selected: false, previouslySelected: false}})
		.attr("class", "brush");

		svg_vis_group = svg_zoomer_group.append("svg:g")
		.attr("id", "svg_vis_group")

		svg_brusher_group
		.call(brusher)
		.on("mousedown.brush", null)
		.on("touchstart.brush", null)
		.on("touchmove.brush", null)
		.on("touchend.brush", null);

		svg_brusher_group.select('.background').style('cursor', 'auto');


		svg_vis_links_group = svg_vis_group.append("g")
		.attr("class", "link")
		.attr("id", "svg_vis_links_group")

		svg_vis_links_labels_group = svg_vis_group.append("g")
		.attr("class", "link_label")
		.attr("id", "svg_vis_links_labels_group")

		svg_vis_nodes_group = svg_vis_group.append("g")
		.attr("class", "node")
		.attr("id", "svg_vis_nodes_group")

		svg_vis_nodes_labels_group = svg_vis_group.append("g")
		.attr("class", "node_label")
		.attr("id", "svg_vis_nodes_labels_group")


		layouts["force"] = d3.layout.force()
		.charge(-100)
		.friction(0.8)
		.linkDistance(30)
		.size([width, height])

		layouts["force"].on("tick", event_handlers.force_tick)

		dragger = d3.behavior.drag()
		.on("dragstart", event_handlers.node_dragstarted)
		.on("drag", event_handlers.node_dragging)
		.on("dragend", event_handlers.node_dragended)
			
		return gsqlv;
	}

	gsqlv.insertCSSRule = function (d) {

		styleSheet.insertRule(d, styleSheet.cssRules.length);

		return gsqlv;
	}

	gsqlv.initStyleSheet = function () {
		/*
		styleSheet.insertRule(".node {\
			stroke: #ccc;\
			stroke-width: 1px;\
			fill: #aaa;\
			}" , styleSheet.cssRules.length)

		styleSheet.insertRule(".node :not(.selected){\
			stroke: #666;\
			stroke-width: 1px;\
			fill: #aaa;\
			opacity: 0.9;\
			}" , styleSheet.cssRules.length)
		*/
		styleSheet.insertRule(".node .selected {\
			stroke: #e11;\
			stroke-width: 1.8px;\
			//fill: #f00;\
			}", styleSheet.cssRules.length)
		/*
		styleSheet.insertRule(".node_label {\
			-webkit-user-select: none;\
			cursor:default;\
			fill: #000;\
			stroke: transparent;\
			opacity: 0.9;\
			}", styleSheet.cssRules.length)
		*/

		styleSheet.insertRule("svg text {\
			-webkit-user-select: none;\
			cursor:default;\
			}", styleSheet.cssRules.length)

		styleSheet.insertRule(".node_label .selected {\
			fill: #000;\
			stroke: #000;\
			}", styleSheet.cssRules.length)

		/*
		styleSheet.insertRule(".link {\
			stroke: #999;\
			stroke-width: 1px;\
			}", styleSheet.cssRules.length)


		styleSheet.insertRule(".link :not(.selected){\
			stroke: #999;\
			stroke-width: 1px;\
			opacity:0.5;\
			}", styleSheet.cssRules.length)
		*/

		styleSheet.insertRule(".link .selected {\
			stroke: #d00;\
			stroke-width: 1.8px;\
			}", styleSheet.cssRules.length)

		styleSheet.insertRule(".brush .extent {\
			fill-opacity: .1;\
			stroke: #fff;\
			shape-rendering: crispEdges;\
			}", styleSheet.cssRules.length)

		/*
		styleSheet.insertRule(".link_label {\
			-webkit-user-select: none;\
			cursor:default;\
			fill:#0a0;\
			stroke:#0a0;\
			opacity: 0.8;\
			}", styleSheet.cssRules.length)
		*/

		styleSheet.insertRule(".link_label .selected{\
			fill: #f00;\
			stroke: #f00;\
			}", styleSheet.cssRules.length)
	}

	gsqlv.center_view = function () {
		// Center the view on the molecule(s) and scale it so that everything
		// fits in the window

		if (data === null)
		return;

		var nodes = data.nodes;
		var width = setting.width;
		var height = setting.height;

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
		min_ratio = Math.min(width_ratio, height_ratio) * 0.6;
		if (min_ratio > zoomer_scale[1]) min_ratio = zoomer_scale[1];
		if (min_ratio < zoomer_scale[0]) min_ratio = zoomer_scale[0];

		// the new dimensions of the molecule
		new_mol_width = mol_width * min_ratio;
		new_mol_height = mol_height * min_ratio;

		// translate so that it's in the center of the window
		x_trans = -(min_x) * min_ratio + (width - new_mol_width) / 2;
		y_trans = -(min_y) * min_ratio + (height - new_mol_height) / 2;

		if (svg_vis_group == undefined) return;
		if (zoomer == undefined) return;



		// do the actual moving
		svg_vis_group.attr("transform",
		"translate(" + [x_trans, y_trans] + ")" + " scale(" + min_ratio + ")");

		// tell the zoomer what we did so that next we zoom, it uses the
		// transformation we entered here
		zoomer.translate([x_trans, y_trans ]);
		zoomer.scale(min_ratio);

		gsqlv.refresh();
		clearTimeout(timer);
	}

	gsqlv.redraw = function() {
		svg_vis_links_group.selectAll("*").remove();
		svg_vis_nodes_group.selectAll("*").remove();
		svg_vis_nodes_labels_group.selectAll("*").remove();
		svg_vis_links_labels_group.selectAll("*").remove();

		temp_links = svg_vis_links_group
		.selectAll("line")
		.data(data.links)
		.enter().append("line")
		.each(function(d) {
			d3.select(this)
			.attr("id", "l_"+d.source.id+d.source.type+d.target.id+d.target.type)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("x1", function(d) { return d.source.x; })
			.attr("y1", function(d) { return d.source.y; })
			.attr("x2", function(d) { return d.target.x; })
			.attr("y2", function(d) { return d.target.y; })
		})

		temp_nodes = svg_vis_nodes_group
		.selectAll("circle")
		.data(data.nodes)
		.enter().append("circle")
		.each(function(d) {
			d3.select(this)
			.attr("id", "n_"+d.id+d.type)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("cx", function(d) { return d.x; })
			.attr("cy", function(d) { return d.y; })
		})
		.attr("r", 5)
        .on("dblclick", event_handlers.node_dblclick)
		.on("click", event_handlers.node_click)
		.on("mouseup", event_handlers.node_mouseup)
		.on("mouseover", event_handlers.node_mouseover)
		.on("mouseout", event_handlers.node_mouseout)
		.call(dragger)

		temp_nodes_labels = svg_vis_nodes_labels_group
		.selectAll(".node_label")
		.data(data.nodes)
		.enter().append("text")
		.each(function(d) {
			var darkerColor = d3.rgb(d.fill).darker(4)
			d3.select(this)
			.attr("id", "nl_"+d.id+d.type)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)

			nodeLabelGenerator(d3.select(this), node_label_attr_array, d);
		})
		.on("dblclick", event_handlers.node_label_dblclick)
		.on("click", event_handlers.node_label_click)
		.on("mouseup", event_handlers.node_label_mouseup)
		.on("mouseover", event_handlers.node_label_mouseover)
		.on("mouseout", event_handlers.node_label_mouseout)
		.call(dragger)

		temp_links_labels = svg_vis_links_labels_group
		.selectAll(".link_label")
		.data(data.links)
		.enter().append("text")
		.each(function(d) {
			var darkerColor = d3.rgb(d.stroke).darker()
			d3.select(this)
			.attr("id", "ll_"+d.source.id+d.source.type+d.target.id+d.target.type)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)

			linkLabelGenerator(d3.select(this), edge_label_attr_array, d);
		})

		return gsqlv;
	}

	gsqlv.refresh = function() {
		temp_nodes
		.each(function(d) {
			d3.select(this)
			//.attr("id", "n_"+d.id+d.type)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("cx", function(d) { return d.x; })
			.attr("cy", function(d) { return d.y; })
		})

		temp_links
		.each(function(d) {
			d3.select(this)
			//.attr("id", "l_"+d.source.id+d.source.type+d.target.id+d.target.type)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("x1", function(d) { return d.source.x; })
			.attr("y1", function(d) { return d.source.y; })
			.attr("x2", function(d) { return d.target.x; })
			.attr("y2", function(d) { return d.target.y; })
		})

		temp_nodes_labels
		.each(function(d) {
			var darkerColor = d3.rgb(d.fill).darker(4)

			d3.select(this)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)

			refreshNodeLabelPosition(d3.select(this), node_label_attr_array, d);
		})
		//.attr('x', function(d) { return d.x; })
		//.attr('y', function(d) { return d.y-1.5*(node_label_key_array.length+node_label_attr_array.length-1); })
		//.each(function(d) {
		//	d3.select(this).selectAll("tspan")
		//	.attr("x", d.x)
		//});

		temp_links_labels
		.each(function(d) {
			var darkerColor = d3.rgb(d.stroke).darker()

			d3.select(this)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)
			.attr("x", function(d) {return 0.5*d.source.x+ 0.5*d.target.x})

			refreshLinkLabelPosition(d3.select(this), edge_label_attr_array, d);
		})
		//.attr("x", function(d) {return 0.5*d.source.x+ 0.5*d.target.x})
		//.attr("y", function(d) {return 0.5*d.source.y+ 0.5*d.target.y - 1.5*(edge_label_key_array.length+edge_label_attr_array.length-1)}) 
		//.each(function(d) {
		//	d3.select(this).selectAll("tspan")
		//	.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
		//});

		return gsqlv
	}


	gsqlv.refreshAnimation = function(milliseconds) {
		temp_nodes
		.each(function(d) {
			d3.select(this)
			//.attr("id", "n_"+d.id+d.type)
			.transition()
			.duration(milliseconds)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("cx", function(d) { return d.x; })
			.attr("cy", function(d) { return d.y; })
		})

		temp_links
		.each(function(d) {
			d3.select(this)
			//.attr("id", "l_"+d.source.id+d.source.type+d.target.id+d.target.type)
			.transition()
			.duration(milliseconds)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("x1", function(d) { return d.source.x; })
			.attr("y1", function(d) { return d.source.y; })
			.attr("x2", function(d) { return d.target.x; })
			.attr("y2", function(d) { return d.target.y; })
		})

		temp_nodes_labels
		.each(function(d) {
			var darkerColor = d3.rgb(d.fill).darker(4)

			d3.select(this)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)

			refreshNodeLabelPosition(d3.select(this), node_label_attr_array, d);
		})
		//.attr('x', function(d) { return d.x; })
		//.attr('y', function(d) { return d.y-1.5*(node_label_key_array.length+node_label_attr_array.length-1); })
		//.each(function(d) {
		//	d3.select(this).selectAll("tspan")
		//	.attr("x", d.x)
		//});

		temp_links_labels
		.each(function(d) {
			var darkerColor = d3.rgb(d.stroke).darker()

			d3.select(this)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)
			.attr("x", function(d) {return 0.5*d.source.x+ 0.5*d.target.x})

			refreshLinkLabelPosition(d3.select(this), edge_label_attr_array, d);
		})
		//.attr("x", function(d) {return 0.5*d.source.x+ 0.5*d.target.x})
		//.attr("y", function(d) {return 0.5*d.source.y+ 0.5*d.target.y - 1.5*(edge_label_key_array.length+edge_label_attr_array.length-1)}) 
		//.each(function(d) {
		//	d3.select(this).selectAll("tspan")
		//	.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
		//});

		return gsqlv
	}

	gsqlv.layout = function(x) {
		if (!arguments.length) {
			return layout;
		}
		else {
			layout = x;

			return gsqlv
		}
	}

	gsqlv.preDefinition = function(x) {
		if (!arguments.length) {
			return preDefinition;
		}
		else {
			preDefinition = x;
			return gsqlv
		}
	}

	gsqlv.runPreDefinition = function(nodes, links) {

		// Have to run after gsqlv.initNodeLinks();

		if ('coloring' in preDefinition) {
			//[{"selection":"some_int < 3","selectionType":"nodes","color":"#00ff00"}]
			preDefinition.coloring.forEach(function(c) {
				if (c.selectionType == "nodes") {
					nodes.forEach(function(n){
						if (gsqlv.checkConditionForNode(n, c.selection)) {
							n.fill = c.color
						}
					})
				}
				else if (c.selectionType == "edges") {
					links.forEach(function(l){
						if (gsqlv.checkConditionForEdge(l, c.selection)) {
							l.stroke = c.color
						}
					})
				}
				else {

				}
			}) 
		}
		else if ('other predefinition attributes' in preDefinition) {

		}
		else {

		}
	}

	gsqlv.stylesheet = function(x) {
		if (!arguments.length) {
			return styleSheet;
		}
		else {
			styleSheet = x;
			return gsqlv
		}
	}

	gsqlv.nodeLabel = function(x) {
		if (!arguments.length) {
			return node_label_attr_array;
		}
		else {

			node_label_attr_array = x

			return gsqlv
		}
	}

	gsqlv.edgeLabel = function(x) {
		if (!arguments.length) {
			return edge_label_attr_array
		}
		else {

			edge_label_attr_array = x

			return gsqlv
		}
	}

	gsqlv.rootNode = function(x) {
		if (!arguments.length) {
			return rootNode;
		}
		else {
			rootNode = x;
			return gsqlv;
		}
	}

	gsqlv.highlightingNodes = function() {
		if (data.nodes.filter(function(d) { 
				return d.selected; 
			}).length == 0) {
			data.nodes.forEach(function(d) {
				d.opacity = initNodeSetting.opacity;
			})
			data.links.forEach(function(d) {
				d.opacity = initLinkSetting.opacity;
			})
		}
		else {
			data.nodes.forEach(function(d) {
				if (d.selected) {
					d.opacity = 1;
				}
				else {
					d.opacity = 0.3;
				}
			})

			data.links.forEach(function(d) {
				if (d.selected) {
					d.opacity = 1;
				}
				else {
					d.opacity = 0.3;
				}
			})

			// unselect everything after changing the color.
			data.nodes.forEach(function(d) {
				d.selected =  d.previouslySelected = false; 
			})
		}

		gsqlv.updateSelectedElements()
		gsqlv.refresh();
	}

	gsqlv.coloringNodes = function(x) {
		if (!arguments.length) {
			return;
		}
		else {
			if (data.nodes.filter(function(d) { 
					return d.selected; 
				}).length == 0) {
				//No selected nodes, do nothing.
			}
			else {
				data.nodes.forEach(function(d) {
					if (d.selected) {
						d.fill = x;
					}
					else {
						//d.opacity = 0.3;
					}
				})

				data.links.forEach(function(d) {
					if (d.selected) {
						d.stroke = x;
					}
					else {
						//d.opacity = 0.3;
					}
				})

				
			}

			gsqlv.refresh();
		}
		
	}

	gsqlv.unselectingEverything = function() {
		data.nodes.forEach(function(d) {
			d.selected =  d.previouslySelected = false; 
		})

		data.links.forEach(function(d) {
			d.selected = false; 
		})

		gsqlv.updateSelectedElements()
		gsqlv.refresh();
	}

	gsqlv.hidingNodes = function() {
		backup_data = clone(data);

		data.links = data.links.filter(function(d) {
			return !(d.source.selected || d.target.selected);
		})		

		data.nodes = data.nodes.filter(function(d) {
			return !d.selected;
		})

		gsqlv.initNodeLinks();

		gsqlv.redraw();
	}

	gsqlv.selectingNodesByConditions = function(x) {
		if (!arguments.length){
		 	return;
		}
		else {
			$("#selectingNodesByConditions").remove();

			y = __$$global_string;

			x = x == "" ? 'false':x;

			if (typeof data.nodes[0] == 'undefined')
				return;

			var jsString = "";

			gsqlv.data().nodes.forEach(function(n, i) {
				var jsItem = "!function(){"

				var jsVariable = "";
				jsVariable += "var node =" + y + '.data().nodes[' +i+'];'
				for (var key in n.attr) {
					jsVariable += "var "+key+"=node.attr."+key+";";
				}

				jsItem += jsVariable;
				jsItem += 'try{ if ('+x+') {node.selected=true;}\
						else{node.selected=false;}}catch(err){node.selected=false;}'

				jsItem += 'try{ if (node.'+x+') {node.selected=true;}\
						else{;}}catch(err){;}'
				/*
				jsItem += 'try{\
								if ('+x+') {\
									node.selected = true;\
								}\
								else {\
									node.selected = false;\
								}\
							}\
							catch(err){console.log(err.message);\
								console.log(node);\
								node.selected = false;\
							}\
							finally{}\
							});'
				*/
				jsItem += '}();'

				jsString += jsItem;
			})

			jsString += y+'.updateSelectedElements();'

			/*

			var jsVariable = "";
			for (var key in data.nodes[0].attr) {
				jsVariable += "var "+key+"=node.attr."+key+";";
			}

			var jsString = y + '.data().nodes.forEach(function(node) {\
					' + jsVariable + 'try{\
						if ('+x+') {\
							node.selected = true;\
						}\
						else {\
							node.selected = false;\
						}\
					}\
					catch(err){console.log(err.message);\
						console.log(node);\
						node.selected = false;\
					}\
					finally{}\
				});\
				'+ y+'.updateSelectedElements()'

			*/

			var code = "<script id='selectingNodesByConditions'>"+ jsString+ ";<"+"/script>";
			$('body').append($(code)[0]);
		}
	} 

	// nodes
	gsqlv.checkConditionForNode = function(n, condition) {
		//console.log("check condition for node.")

		$("#checkConditionForNode").remove();

		condition = condition == "" ? 'false':condition;

		var tempKey = n.type + "&" + n.id; //n == node_links[n.type + "&" + n.id].node

		var jsString = "";

		var jsItem = "!function(){"

		var jsVariable = "";
		jsVariable += "var node =" + __$$global_string + '.node_links()["' +tempKey+'"].node;'

		jsVariable += "var id = node.id;var type = node.type;"
		for (var key in n.attr) {
			jsVariable += "var "+key+"=node.attr."+key+";";
		}

		jsItem += jsVariable;
		jsItem += 'try{ if ('+condition+') {__$$global_boolean=true;}\
				else{__$$global_boolean=false;}}catch(err){__$$global_boolean=false;}'

		jsItem += '}();'

		jsString += jsItem;

		var code = "<script id='checkConditionForNode'>"+ jsString+ ";<"+"/script>";
		$('body').append($(code)[0]);

		return __$$global_boolean;
	}


	// links
	gsqlv.checkConditionForEdge = function(l, condition) {
		//console.log("check condition for link.")

		$("#checkConditionForEdge").remove();

		condition = condition == "" ? 'false':condition;

		var sourceID = l.source;
		var targetID = l.target;

		targetKey = targetID.type + "&" + targetID.id
		sourceKey = sourceID.type + "&" + sourceID.id

		linkKey = targetKey + "$" + sourceKey
		//link = link_nodes[linkKey].link

		var jsString = "";

		var jsItem = "!function(){"

		var jsVariable = "";
		jsVariable += "var link =" + __$$global_string + '.link_nodes()["' +linkKey+'"].link;'

		//jsVariable += "var id = link.id;var type = link.type;"
		for (var key in l.attr) {
			jsVariable += "var "+key+"=link.attr."+key+";";
		}

		jsItem += jsVariable;
		jsItem += 'try{ if ('+condition+') {__$$global_boolean=true;}\
				else{__$$global_boolean=false;}}catch(err){__$$global_boolean=false;}'

		jsItem += '}();'

		jsString += jsItem;

		var code = "<script id='checkConditionForEdge'>"+ jsString+ ";<"+"/script>";
		$('body').append($(code)[0]);
		
		return __$$global_boolean;
	}

	gsqlv.selectingEdgesByConditions = function(x) {
		if (!arguments.length){
		 	return;
		}
		else {
			gsqlv.unselectingEverything();
			$("#selectingEdgesByConditions").remove();

			y = __$$global_string;

			x = x == "" ? 'false':x;

			if (typeof data.links[0] == 'undefined')
				return;

			var jsString = "";

			gsqlv.data().links.forEach(function(l, i) {
				var jsItem = "!function(){"

				var jsVariable = "";
				jsVariable += "var link =" + y + '.data().links[' +i+'];'
				for (var key in l.attr) {
					jsVariable += "var "+key+"=link.attr."+key+";";
				}

				jsItem += jsVariable;
				jsItem += 'try{ if ('+x+') {link.selected=true;link.source.selected=true;link.target.selected=true}\
						else{;}}catch(err){;}'
				jsItem += 'try{ if (link.'+x+') {link.selected=true;link.source.selected=true;link.target.selected=true}\
						else{;}}catch(err){;}'
				/*
				jsItem += 'try{\
								if ('+x+') {\
									node.selected = true;\
								}\
								else {\
									node.selected = false;\
								}\
							}\
							catch(err){console.log(err.message);\
								console.log(node);\
								node.selected = false;\
							}\
							finally{}\
							});'
				*/
				jsItem += '}();'

				jsString += jsItem;
			})

			jsString += y+'.updateSelectedEdges();'

			/*

			var jsVariable = "";
			for (var key in data.nodes[0].attr) {
				jsVariable += "var "+key+"=node.attr."+key+";";
			}

			var jsString = y + '.data().nodes.forEach(function(node) {\
					' + jsVariable + 'try{\
						if ('+x+') {\
							node.selected = true;\
						}\
						else {\
							node.selected = false;\
						}\
					}\
					catch(err){console.log(err.message);\
						console.log(node);\
						node.selected = false;\
					}\
					finally{}\
				});\
				'+ y+'.updateSelectedElements()'

			*/

			var code = "<script id='selectingEdgesByConditions'>"+ jsString+ ";<"+"/script>";
			$('body').append($(code)[0]);
		}
	} 

	gsqlv.updateSelectedElements = function() {
		//console.log("test sucessful.")

		temp_nodes.classed("selected", function(d) {
			d.previouslySelected = !d.selected
			return d.selected;
		})

		temp_nodes_labels.classed("selected", function(d) {
			return d.selected;
		})

		temp_links.classed("selected", function(d) {
			return d.selected //= d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected //= d.target.selected || d.source.selected;
		});
	}

	gsqlv.updateSelectedEdges = function() {
		//console.log("test sucessful.")

		temp_nodes.classed("selected", function(d) {
			d.previouslySelected = !d.selected
			return d.selected;
		})

		temp_nodes_labels.classed("selected", function(d) {
			return d.selected;
		})

		temp_links.classed("selected", function(d) {
			return d.selected //= d.target.selected && d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected //= d.target.selected && d.source.selected;
		});
	}

	gsqlv.setURL = function(myObject, event) {

		if (event == "node_dblclick") {
			event_handlers.node_dblclick = function(d) {
				console.log(d)

				temp_event = myObject.events[event];
				submit_URL = temp_event.URL_head + "?"

				URL_attrs =  temp_event.URL_attrs

				for (var attr in URL_attrs) {
					name = attr;
					attr = URL_attrs[attr];

					if (attr.usage == "input") {
						submit_URL += name + "=" + document.getElementsByName(attr.name)[0].value +"&";
					}
					else if (attr.usage == "attributes") {
						submit_URL += name+ "=" + myObject.attributes[attr.name] + "&";
					}
					else if (attr.usage == "select") {
						submit_URL += name + "=" + d[attr.name] +"&";
					}
				}

				/*
				URL_attrs.forEach(function(a){
					index_txtbox = a.indexOf('*')
					if (index_txtbox != -1) {
						attr_name = a.split("*");
						node_dblclick_URL += attr_name[0] + "=" + d[attr_name[0]] +"&";
					}

					index_attr = a.indexOf('@')
					if (index_attr != -1) {
						attr_name = a.split("@");
						node_dblclick_URL += attr_name[0] + "=" + myObject.attributes[attr_name[1]] + "&";
					}
				})
				*/

				$.get(submit_URL, function(message) {
					message = JSON.parse(message);
					if (!message.error) {
						newData = message.results;

						gsqlv
						.addSubGraph(newData)
						.run()				

					}
					else {
						;
					}
				})

				d3.event.stopPropagation(); 
			}

			event_handlers.node_label_dblclick = event_handlers.node_dblclick;
		}
		else {
			// More URL and Events;
		}



		//console.log(URL_format + events);
	}

	gsqlv.run = function() {
		gsqlv.startLayout();
		gsqlv.redraw();

		clearTimeout(timer);
		timer = setTimeout(gsqlv.center_view, 5000);

		return gsqlv;
	}

	gsqlv.startLayout = function() {
		layouts["force"].stop();

		if (layout == "force") {
			layouts["force"]
			.nodes(data.nodes)
			.links(data.links)
			.start();
		}
		else if (layout == "hierarchical") {
			//console.log(layout +" layout")
			// do something for the herachical layout.

			separation_x = 40;
			separation_y = 25;

			data.nodes.forEach(function(d) {
				d.hierarchied = false;
			})

			node_links[rootNode].node.hierarchied = true; // fix the root node.
			node_links[rootNode].node.x = setting.width / 2;	
			node_links[rootNode].node.y = setting.height / 6;

			var setHierarchicalPosition = function (depth, links) {
				nextDepthLinks = [];

				var localSpace = 0;

				links.forEach(function(node, i) {
					if (!node.hierarchied) {
						node.hierarchied = true;
						node.x = (i-localSpace) * separation_x + node_links[rootNode].node.x;
						node.y = separation_y * depth + node_links[rootNode].node.y;
					}
					else {
						localSpace += 1;
					}
				})

				links.forEach(function(node, i) {
					nodeKey = node.type + "&" + node.id;
					node_links[nodeKey].links.forEach(function(n2, i2) {
						if (!n2.hierarchied) {
							nextDepthLinks.push(n2);
						}
						else {
							;
						}
					})
				})

				if (!nextDepthLinks.length) {
					return;
				}
				else {
					setHierarchicalPosition(depth + 1, nextDepthLinks);
				}
			}

			setHierarchicalPosition(1, node_links[rootNode].links);
		}
		else if (layout == "circle") {
			console.log(layout +" layout")

			layouts["circle"] = d3.layout.tree()
								.size([360, Math.min(setting.width, setting.height) / 2 -120])
								.separation(function(a, b){
									return (a.parent == b.parent ? 1 : 2) / a.depth / a.depth;
								})

			data.nodes.forEach(function(d) {
				d.hierarchied = false;
			})

			node_links[rootNode].node.hierarchied = true; // fix the root node.
			node_links[rootNode].node.x = setting.width / 2;	
			node_links[rootNode].node.y = setting.height / 2;

			//rootNode is a string as "0&1"

			root = {"children":[], "node":node_links[rootNode]}

			var setCircleTreeData = function (_this) {
				_this.node.links.forEach(function(node, i) {
					if (!node.hierarchied) {
						nodeKey = node.type + "&" + node.id;
						node.hierarchied = true;

						tempTreeNode = {"children":[], "node":node_links[nodeKey]}
						_this.children.push(tempTreeNode)
					}
				})
				
				_this.children.forEach(function(treeNode, i) {
					setCircleTreeData(treeNode)
				})
			}

			setCircleTreeData(root);

			circledNodes = layouts["circle"].nodes(root);

			function rotate(cx, cy, x, y, angle) {
				//X' = X cosB - Y sinB
				//Y' = X sinB + Y cosB
				var radians = (Math.PI / 180) * angle,
				cos = Math.cos(radians),
				sin = Math.sin(radians),
				nx = (cos * (x - cx)) - (sin * (y - cy)) + cx,
				ny = (sin * (x - cx)) + (cos * (y - cy)) + cy;
				return [nx, ny];
			}

			circledNodes.forEach(function(n) {
				//"transform" : "rotate(n.x - 90);translate(n.y)"

				position = rotate(0, 0, n.y, 0, n.x-Math.PI/2)

				//n.node.node.x = n.y * Math.cos(n.x) + 0 * Math.sin(n.x) + node_links[rootNode].node.x ;
				//n.node.node.y = n.y * Math.sin(n.x) + 0 * Math.cos(n.x) + node_links[rootNode].node.y ;
				n.node.node.x = position[0] + setting.width / 2
				n.node.node.y = position[1] + setting.height / 2
			})
		}
		else if (layout == "tree") {
			console.log(layout +" layout")

			separation_x = 40;
			separation_y = 25;

			number_separation_x = 1;
			number_separation_y = 1;

			layouts["tree"] = d3.layout.tree()
								//.size([360, Math.min(setting.width, setting.height) / 2 -120])
								.size([separation_x, separation_y])
								.separation(function(a, b){
									//return (a.parent == b.parent ? 1 : 2) / a.depth;
									return (a.parent == b.parent ? 1 : 2)
								})

			data.nodes.forEach(function(d) {
				d.hierarchied = false;
			})

			node_links[rootNode].node.hierarchied = true; // fix the root node.
			node_links[rootNode].node.x = setting.width / 2;	
			node_links[rootNode].node.y = setting.height / 6;

			//rootNode is a string as "0&1"

			root = {"children":[], "node":node_links[rootNode]}

			var setTreeData = function (_this, depth) {

				if (depth > number_separation_y) number_separation_y = depth

				tempNumber = 0;
				_this.node.links.forEach(function(node, i) {
					if (!node.hierarchied) {
						nodeKey = node.type + "&" + node.id;
						node.hierarchied = true;

						tempTreeNode = {"children":[], "node":node_links[nodeKey]}
						_this.children.push(tempTreeNode)
						tempNumber += 1;
					}
				})

				if (tempNumber > number_separation_x) number_separation_x = tempNumber;
				
				_this.children.forEach(function(treeNode, i) {
					setTreeData(treeNode, depth+1)
				})
			}

			setTreeData(root, 1);

			treeNodes = layouts["tree"]
						.size([number_separation_x*separation_x, number_separation_y*separation_y])
						.nodes(root);

			treeNodes.forEach(function(n) {
				n.node.node.x = n.x + setting.width / 2;
				n.node.node.y = n.y + setting.height / 6;
			})
		}
		else {

		}

		return gsqlv
	}

	gsqlv.event_handlers = function() {
		return event_handlers;
	}

	event_handlers.keydown_brush = function() {
		shiftKey = d3.event.shiftKey;
		ctrlKey = d3.event.ctrlKey;
		altKey = d3.event.altKey;

		console.log('d3.event.keyCode : ', d3.event.keyCode);

		if (d3.event.keyCode == 67) {   //the 'c' key
			gsqlv.center_view();
			layouts["force"].stop();
		}

		if (d3.event.keyCode == 68) {  // the 'd' key
			gsqlv.run();	
		}

		if (shiftKey) {
			svg_zoomer_group.call(zoomer)
			.on("mousedown.zoom", null)
			.on("touchstart.zoom", null)
			.on("touchmove.zoom", null)
			.on("touchend.zoom", null);

			svg_vis_group.selectAll('g.gnode')
			.on('mousedown.drag', null);

			svg_brusher_group.select('.background').style('cursor', 'crosshair')
			svg_brusher_group.call(brusher);
		}
	}

	event_handlers.keyup_brush = function() {
		shiftKey = d3.event.shiftKey;
		ctrlKey = d3.event.ctrlKey;
		altKey = d3.event.altKey;

		svg_brusher_group
		.call(brusher)
		.on("mousedown.brush", null)
		.on("touchstart.brush", null)
		.on("touchmove.brush", null)
		.on("touchend.brush", null);

		svg_brusher_group.select('.background').style('cursor', 'auto');
		svg_zoomer_group.call(zoomer);
	}

	event_handlers.zoomer_mousedown = function() {
		if (typeof temp_nodes == "undefined") return;

		temp_nodes.each(function(d) {
			d.selected = false;
			d.previouslySelected = false;
		})

		temp_nodes.classed("selected", false);

		temp_nodes_labels.classed("selected", false);

		temp_links.each(function(d) {
			d.selected = false;
			d.previouslySelected = false;
		})

		temp_links.classed("selected", false);

		temp_links_labels.each(function(d) {
			d.selected = false;
			d.previouslySelected = false;
		})

		temp_links_labels.classed("selected", false);
	}

	event_handlers.zoomstart = function() {
		//console.log("zoomstart")
	}

	event_handlers.zooming = function() {
		svg_vis_group.attr("transform", "translate(" + d3.event.translate + ")" + " scale(" + d3.event.scale + ")");
	}

	event_handlers.zoomend = function() {
		//console.log("zoom end")
	}

	event_handlers.brushstart = function() {
		if (typeof temp_nodes == "undefined") return;
		//console.log("brush start");
		temp_nodes.each(function(d) {
			d.previouslySelected = shiftKey && d.selected;
		})
	}

	event_handlers.brush = function() {
		if (typeof temp_nodes == "undefined") return;

		brusherExtent = d3.event.target.extent();
		temp_nodes.classed("selected", function(d) {
			return d.selected = d.previouslySelected ^
								(brusherExtent[0][0] <= d.x && d.x < brusherExtent[1][0]
								&& brusherExtent[0][1] <= d.y && d.y < brusherExtent[1][1]);
		});

		temp_nodes_labels.classed("selected", function(d) {
			return d.selected;
		})

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

	}

	event_handlers.brushend = function() {
		d3.event.target.clear();
		d3.select(this).call(d3.event.target);

		//console.log("brush end " + brusherExtent);
	}

	event_handlers.node_dragstarted = function(d) {
		d3.event.sourceEvent.stopPropagation();
		//console.log("node dragstarted")

		if (!d.selected && !shiftKey) {
			// unselect everything.
			temp_nodes.classed("selected", function(p) { 
				return p.selected =  p.previouslySelected = false; 
			});
		}

		//d3.select(this).classed("selected", function() {
		//	d.previouslySelected = d.selected; 
		//	return d.selected = true; 
		//})

		d3.select("#n_"+d.id+d.type).classed("selected", function() {
			d.previouslySelected = d.selected; 
			return d.selected = true; 
		})

		d3.select("#nl_"+d.id+d.type).classed("selected", function() {
			d.previouslySelected = d.selected; 
			return d.selected = true; 
		})


		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});
	}

	event_handlers.node_dragging = function(d) {
		temp_nodes.filter(function(d) { 
			return d.selected; 
		})
		.each(function(d) { 
			d.x += d3.event.dx;
			d.y += d3.event.dy;

			d.px += d3.event.dx;
			d.py += d3.event.dy;

			//d3.select(this)
			//.attr("cx", d.x)
			//.attr("cy", d.y)
        })

        gsqlv.refresh();
	}

	event_handlers.node_dragended = function(d) {
		//console.log("node dragended")
	}

	event_handlers.force_tick = function() {
		tickNumber += 1;
		if (!(tickNumber % tickPreRender)) {
			gsqlv.refresh();
		}
	}

	event_handlers.node_dblclick = function(d) {
		console.log("DB click on node" + d.id + " " + d.type);
		d3.event.stopPropagation(); 
	}

	event_handlers.node_click = function(d) {
		//d3.event.stopPropagation();
		if (d3.event.defaultPrevented) return;

		if (!shiftKey) {
			//if the shift key isn't down, unselect everything
			temp_nodes.classed("selected", function(p) { return p.selected =  p.previouslySelected = false; })
		}

		// always select this node
		d3.select(this).classed("selected", d.selected = !d.previouslySelected);

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});
	}

	event_handlers.node_mouseup = function(d) {
		if (d.selected && shiftKey) {
			d3.select(this).classed("selected", d.selected = false)
		}

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});
	}

	event_handlers.node_mouseover = function(d) {
		d3.select(this)
		.classed("selected", true);
	}

	event_handlers.node_mouseout = function(d) {
		d3.select(this)
		.classed("selected", d.selected);
	}

	event_handlers.node_label_dblclick = function(d) {
		console.log("dblclick of node " + d.id + " " + d.type)
		d3.event.stopPropagation(); 
	} 

	event_handlers.node_label_click = function(d) {
		if (d3.event.defaultPrevented) return;

		if (!shiftKey) {
			//if the shift key isn't down, unselect everything
			temp_nodes.classed("selected", function(p) { return p.selected =  p.previouslySelected = false; })
		}

		// always select this node
		d.selected = !d.previouslySelected

		temp_nodes.classed("selected", function(d) {
			return d.selected;
		});

		temp_nodes_labels.classed("selected", function(d) {
			return d.selected;
		});

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});			
	}

	event_handlers.node_label_mouseup = function(d) {
		if (d.selected && shiftKey) {
			d3.select(this).classed("selected", d.selected = false)
		}

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});			
	}

	event_handlers.node_label_mouseover = function(d) {
		d3.select("#n_"+d.id+d.type)
		.classed("selected", true);
	}

	event_handlers.node_label_mouseout = function(d) {
		d3.select("#n_"+d.id+d.type)
		.classed("selected", d.selected);
	}

	function nodeLabelGenerator(target, attributes, d) {
		var lines = -1;

		keys = attributes;
	
		if (keys.length > 0) {
			for (var i=0; i<keys.length; ++i) {
				if (typeof d[keys[i]] == "undefined"){
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", d.x)
					.text(keys[i]+":"+d[keys[i]])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", d.x)
					.attr("dy", 3)
					.text(keys[i]+":"+d[keys[i]])

					lines += 1;
				}
			}
		}

		if (attributes.length > 0) {
			for (var i=0; i<attributes.length; ++i) {
				if (typeof d.attr[attributes[i]] == "undefined"){
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", d.x)
					.text(attributes[i]+":"+d.attr[attributes[i]])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", d.x)
					.attr("dy", 3)
					.text(attributes[i]+":"+d.attr[attributes[i]])

					lines += 1;
				}
			}
		}

		target
		.attr("x", d.x)
		.attr("y", d.y-1.5*lines)

	}


	function refreshNodeLabelPosition(target, attributes, d) {
		var lines = -1;

		keys = attributes;
	
		if (keys.length > 0) {
			for (var i=0; i<keys.length; ++i) {
				if (typeof d[keys[i]] == "undefined"){
					continue;
				}
				else {
					lines += 1;
				}
			}
		}

		if (attributes.length > 0) {
			for (var i=0; i<attributes.length; ++i) {
				if (typeof d.attr[attributes[i]] == "undefined"){
					continue;
				}
				else {
					lines += 1;
				}
			}
		}

		target
		.attr("y", d.y-1.5*lines)

		target.selectAll("tspan")
		.attr("x", d.x)
	}

	function linkLabelGenerator(target, attributes, d) {

		var lines = -1;
		//.attr("x", function(d) {return 0.5*d.source.x+ 0.5*d.target.x})
		//.attr("y", function(d) {return 0.5*d.source.y+ 0.5*d.target.y - 1.5*(edge_label_key_array.length+edge_label_attr_array.length-1)}) 

		if (attributes.length > 0) {
			for (var i=0; i<attributes.length; ++i) {
				if (typeof d.attr[attributes[i]] == "undefined"){
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
					.text(attributes[i]+":"+d.attr[attributes[i]])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
					.attr("dy", 3)
					.text(attributes[i]+":"+d.attr[attributes[i]])

					lines += 1;
				}
			}
		}

		target
		.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
		.attr("y", 0.5*d.source.y+ 0.5*d.target.y- 1.5*lines)
	}

	function refreshLinkLabelPosition(target, attributes, d) {

		var lines = -1;
		if (attributes.length > 0) {
			for (var i=0; i<attributes.length; ++i) {
				if (typeof d.attr[attributes[i]] == "undefined"){
					continue;
				}
				else {
					lines += 1;
				}
			}
		}

		target
		.attr("y", 0.5*d.source.y+ 0.5*d.target.y- 1.5*lines)

		target.selectAll("tspan")
		.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
	}

	function refineDataName(newData) {
		result = {"nodes":[], "edges":[]};

		newData.vertices.forEach(function(n) {
			//{"type":"newType1","key":"newKey1","attr":{"weight":"0.2","name":"newNode1newNode1newNode1"}};
			tempNode = {"type":"","id":"","attr":""};
			tempNode.type = n.type
			tempNode.id = n.id
			tempNode.attr = clone(n.attr)

			result.nodes.push(tempNode);
		}) 

		newData.Edges.forEach(function(e) {
			//{"source":{"type":"newType1","key":"newKey1"},"target":{"type":"type0","key":"key45"},"attr":{"weight":"0.01","name":"name0.7"}};
			tempEdge = {"source":{"type":"","id":""},"target":{"type":"","id":""},"attr":""};
			tempEdge.source.type = e.src.type
			tempEdge.source.id = e.src.id
			tempEdge.target.type = e.tgt.type
			tempEdge.target.id = e.tgt.id
			tempEdge.attr = clone(e.attr)

			result.edges.push(tempEdge);
		})

		
		//console.log(result)

		return result
	}

	return gsqlv;
}

this.gsqlv = gsqlv;
}()






