!function() {
var gsqlv = function(x) {
	var gsqlv = {
		version : '2.0',
	};
	var setting; //var setting = {'divID':''divid', 'width':width, 'height':height};
	var graphType = 'undirected' // undirected or directed. 
	var data = {"nodes":[], "links":[]}; // graph data object.
	var node_links = {}; // hash table for node and its linked node.
	var link_nodes = {}; // hash table for links and its source object and target object in node_links.

	var backup_data = {};
	var rawData;
	var event_handlers = {};  // events handlers call back function object for visualization.

	var styleSheet; // css obeject in visualization lib.

	var colorDomain = []; 
	for (var ti=0; ti<10; ++ti) {
		colorDomain.push(ti);
	}

	// prepare color for using on demand.
	var color = d3.scale.category10().domain(colorDomain); 

	// Zoomer range [min value, max value];
	var zoomer_scale = [0.2, 15];

	// Record key events.
	var shiftKey, ctrlKey, altKey;

	// Visualization Scales for both x and y.
	var xScale;
	var yScale;

	// Visualization layers structure.
	// svg
	// |>svg_zoomer_group
	// |  |>svg_background_rect
	// |  |>svg_brush_group
	// |  |>svg_vis_group
	// |  |  |>svg_vis_links_group
	// |  |  |>svg_vis_links_labels_group
	// |  |  |>svg_vis_nodes_group
	// |  |  |>svg_vis_nodes_labels_group
	// |  |  |>svg_vis_Z_index_group
	var svg;
	var svg_zoomer_group, zoomer;
	var svg_background_rect;
	var svg_brusher_group, brusher, brusherExtent;
	var svg_vis_group;
	var svg_vis_nodes_group;
	var svg_vis_links_group;
	var svg_vis_nodes_labels_group;
	var svg_vis_links_labels_group;
	var svg_vis_Z_index_group;

	//layout type : force; tree; hierarchical; circle;
	var layout = "force"; 

	// layout object;
	var layouts = {};

	// root node of tree layout, hierarchical layout, circle layout. initialized as 0
	var rootNode = 0;

	// d3 drag event handler object.
	var dragger;

	// force directed layout refresh controler. Refresh once every tickPreRender times.
	var tickPreRender = 2;
	var tickNumber = 0;

	// svg elements group.
	// svg:circle elements array for nodes.
	var temp_nodes;

	// svg:path elements array for links.
	var temp_links;

	// svg:text elements array for node labels.
	var temp_nodes_labels;

	// svg:text elements array for link labels.
	var temp_links_labels;

	// line function as interpolate(basis)
	// line is d3.svg.line().interpolate("basis");
	var line;

	// type of node hash table. dict[type] = [nodes, ...]
	var typeArray;

	// not use any more.
	var node_label_attr_array = []//["type", "id", "some_int"]
	var edge_label_attr_array = []//["some_int"]

	// use for displaying labels of nodes and edges. __key is default as type and id.
	var node_label_dict_type = {"__key":{'type':false, 'id':true}} // dict[node_type] = [label1, label2, ...] 
	var edge_label_dict_type = {"__key":{'type':false, 'id':true}} // dict[edge_type] = [label1, label2, ...] 


	// may use in layout algorithm.
	var expand_depth = 1;

	// User defined pre defined coloring and highlighing condition for graph visualization.
	// get from Endpoint.
	var preDefinition = {};

	// settime out object.
	var timer;


	// global variables for selecting node and edge by string condition.
	window.__$$global_boolean = false;
	window.__$$global_string = x;
	window.__$$global_real = 0.0;

	var rootNodeSetting = {
		'stroke': '#ff0000',//'#777',
		'stroke-width': '1',
		//'fill': '#74c476',
		'opacity':1,
		'r':9,
		'selected' : false,
		'previouslySelected' : false,
		'hierarchied' : false, // for hierarchical layout
		'x':window.v_width / 2,
		'y':window.v_height / 2,
	}

	// initialization of node visualization, and node label visualization.
	var initNodeSetting = {
		'stroke': 'transparent',//'#777',
		'stroke-width': '1',
		'fill': '#aec7e8',
		'opacity':1,
		'r':6,
		'selected' : false,
		'previouslySelected' : false,
		'hierarchied' : false, // for hierarchical layout
		'x':window.v_width / 2,
		'y':window.v_height / 2,
	}

	// initialization of link visualization, and link lable visualization.
	var initLinkSetting = {
		'stroke': '#aec7e8',
		'stroke-width': '1',
		'fill': 'none',
		'opacity':0.8,
		'selected': false,
		'previouslySelected' : false,
		'background_rx' : 1, // background for labels of links
		'background_ry' : 1,
		'background_color': '#fff',
		'background_stroke' : '#ddd',
		'background_stroke_width' : 0.2,
		'background_opacity' : 0.8,
		'background_margin': .5
	}

	// Set graph data. Get graph data.
	gsqlv.data = function(newData) {
		if (!arguments.length) {
			return data;
		}
		else {
			// refine the data name, and get the raw graph data ready to use. 
			newData = refineDataName(newData)
			rawData = clone(newData)

			// begin to set the real graph data ready to use.
			data.nodes = []
			data.links = []

			rawData.nodes.forEach(function(d) {
				// initialize node, then add the node
				gsqlv.initNode(d);
				data.nodes.push(d);
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

				if ('type' in d) {
					newLink.type = d.type;
				}
				else {
					newLink.type = "Undefined";
				}

				//initialize link, then add the link
				gsqlv.initLink(newLink);

				if (graphType == "directed") {
					data.links.push(newLink);
				}
				else if (graphType == "undirected") {
					var temp = data.links.filter(function(l) {
						return (l.target == newLink.target && l.source == newLink.source && l.type == newLink.type) 
						|| (l.target == newLink.source && l.source == newLink.target && l.type == newLink.type)
					})

					if (temp.length) {
						;
					}
					else {
						data.links.push(newLink);
					}
				}
				else {
					console.log("unknown graph type.")
				}
				
			})

			// back up the graph data in case of needs
			backup_data = clone(data)

			// initialized the hash table of node, and link.
			gsqlv.initNodeLinks();

			// run the pre definition of highlighting, coloring for visualization.
			gsqlv.runPreDefinition(data.nodes, data.links)

			return gsqlv;
		}
	}

	gsqlv.initNodeLinks = function() {
		// hash table for node
		node_links = {};

		// hash table for link
		link_nodes = {};

		// initialize the node_links dict[type&id] = {node:{}, links:{}, degree:{in, out, ...}
		// 1. hash all node.
		// 2. append neigboring node in dict[type&id].links:{}
		data.nodes.forEach(function(d) {
				nodeKey = d.type+ "&" +d.id
				node_links[nodeKey] = {};
				node_links[nodeKey].node = d;
				node_links[nodeKey].links = [];
				node_links[nodeKey].degree = {"in":[], "out":[]};
			})

		// initialize the link_nodes dict[type&id$type&id] = { dict[link.type]={link, target, source, ...} }
		// 1. hash all links;
		// 2. handle hash conflict base on edge.type.
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

			node_links[targetKey].degree.in.push(sourceID);
			node_links[sourceKey].degree.out.push(targetID);

			linkKey = targetKey + "$" + sourceKey
			if (linkKey in link_nodes) {
				;
			}
			else {
				link_nodes[linkKey] = {};
			}

			if (d.type in link_nodes[linkKey]) {
				return;
			}
			else {
				link_nodes[linkKey][d.type] = {}
				link_nodes[linkKey][d.type].link = d;
				link_nodes[linkKey][d.type].target = node_links[targetKey];
				link_nodes[linkKey][d.type].source = node_links[sourceKey];
			}
		})

		// if root node is deleted in some case, set the first node of the node array as the root node.
		// check root node base on Uppercase.

		var tempRootNodeIndex = Object.keys(node_links).map(function(d) {return d.toUpperCase()}).indexOf(rootNode.toUpperCase());
		//if (rootNode in node_links) {
		if (tempRootNodeIndex != -1) {
			rootNode = Object.keys(node_links)[tempRootNodeIndex];
		}
		else {
			rootNode = Object.keys(node_links)[0];
		}
	}

	gsqlv.clean = function() {
		// clean every thing of the graph data.
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

		// refine name of the new sub graph
		newGraph = refineDataName(newGraph)

		newGraph.nodes.forEach(function(n) {
			//add the nodes of the new graph in the graph data.
			newNode = gsqlv.addNode(n);

			// if it is a newNode, setting the predefinition.
			// else do nothing.
			if (newNode) {
				gsqlv.runPreDefinition([newNode],[])
			}
		})

		newGraph.edges.forEach(function(e) {
			// add the links of the new graph in the graph data.
			newLink = gsqlv.addLink(e);

			// if it is a new link, setting the pre definition.
			// else keep everything the same.
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

		//check whether the new node is exist.
		var node = data.nodes.filter(function(n) {
			return (n.type === x.type && n.id === x.id);
		})[0]

		// if true, return false for setting pre definition.
		// else add the new nodes and return.
		if (node) {
			for (var attrname in x.attr) { node.attr[attrname] = x.attr[attrname]; }
			return false;
		}
		else {
			gsqlv.initNode(x);
			data.nodes.push(x);

			nodeKey = x.type+"&"+x.id;
			node_links[nodeKey] = {}
			node_links[nodeKey].node = x;
			node_links[nodeKey].links = [];
			node_links[nodeKey].degree = {"in":[], "out":[]};
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

		// if it is a self-circle, return;
		if (x.target.type === x.source.type && x.target.id === x.source.id) {
			return gsqlv;
		} 

		// check whether the new link is exist.
		// a. directed : find a link with same target, src, and type.
		// b. undirected : find a link with same type, and same a pair of nodes.
		// c. other : TBD.
		var link;

		if (graphType == "directed") {
			link = data.links.filter(function(l) {
				return (l.source.type == x.source.type && l.target.type == x.target.type
					&& l.source.id == x.source.id && l.target.id == x.target.id 
					&& l.type == x.type);
			})
		}
		else if (graphType == "undirected") {
			link = data.links.filter(function(l) {
				return (l.source.type == x.source.type && l.target.type == x.target.type
					&& l.source.id == x.source.id && l.target.id == x.target.id 
					&& l.type == x.type) 
				|| (l.source.type == x.target.type && l.target.type == x.source.type
					&& l.source.id == x.target.id && l.target.id == x.source.id 
					&& l.type == x.type)
			})
		}
		else {
			concole.log("unknown graph type.")
			return;
		}

		// if the link is exist, return false for setting pre definition.
		// else added the link in the graph data, and return.
		if (link.length) {
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

			if ("type" in x) {
				newLink.type = x.type;
			}
			else {
				newLink.type = "Undefined";
			}

			targetKey = targetID.type + "&" + targetID.id

			if (node_links[targetKey].links.indexOf(sourceID) == -1) {
				node_links[targetKey].links.push(sourceID);
			}

			sourceKey = sourceID.type + "&" + sourceID.id

			if (node_links[sourceKey].links.indexOf(targetID) == -1) {
				node_links[sourceKey].links.push(targetID);
			}

			node_links[targetKey].degree.in.push(sourceID);
			node_links[sourceKey].degree.out.push(targetID);

			linkKey = targetKey + "$" + sourceKey

			if (linkKey in link_nodes) {
				;
			}
			else {
				link_nodes[linkKey] = {};
			}

			link_nodes[linkKey][newLink.type] = {};
			link_nodes[linkKey][newLink.type].link = newLink;
			link_nodes[linkKey][newLink.type].target = node_links[targetKey];
			link_nodes[linkKey][newLink.type].source = node_links[sourceKey];

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

	gsqlv.graphType = function(x) {
		if (!arguments.length) {
			return graphType;
		}
		else {
			graphType = x==undefined?'undirected':x;
			return gsqlv;
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

	// Main initialization.
	gsqlv.init = function(x) {
		if (arguments.length) {
			setting = x;
		}

		if (setting == undefined) {
			return console.log("setting is missing.");
		}

		// appending the CSS style.
		d3.select("body").append("style");

		// Get the last CSS tag.
		styleSheet = document.styleSheets[document.styleSheets.length - 1];

		// Appending gsqlv CSS in the last CSS tag (newest one, in order to make sure the CSS is working).
		gsqlv.initStyleSheet()

		var width = setting.width;
		var height = setting.height;

		// SVG element
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

		// zooming and panning handler.
		// It is an d3.behavior.zoom() object.
		zoomer = d3.behavior.zoom()
		.scaleExtent(zoomer_scale)
		.x(xScale)
		.y(yScale)
		.on("zoomstart", event_handlers.zoomstart)
		.on("zoom", event_handlers.zooming)
		.on("zoomend", event_handlers.zoomend)

		// the svg element of the zoomer.
		// Everything that want to be zoomed and panned should be in this group.
		svg_zoomer_group = svg.append('svg:g')
		.on("mousedown", event_handlers.zoomer_mousedown)
		.call(zoomer)
		.on("dblclick.zoom", null)

		// background color of the main stage.
		svg_background_rect = svg_zoomer_group.append("rect")
		.attr("width", width)
		.attr("height", height)
		.attr("fill", "#dadaeb")
		.attr("opacity", 0.2)
		.attr("stroke", "transparent")
		.attr("id", "svg_background_rect")

		// brush for selecting on the main stage.
		// it is a d3.svg.brush() object.
		brusher = d3.svg.brush()
		.x(xScale)
		.y(yScale)
		.on("brushstart", event_handlers.brushstart)
		.on("brush", event_handlers.brush)
		.on("brushend", event_handlers.brushend)

		// the svg element of the brush elements.
		svg_brusher_group = svg_zoomer_group.append("g")
		.datum(function() {return {selected: false, previouslySelected: false}})
		.attr("class", "brush");

		// Visualization group for graph visualization.
		// it will be zoomed and panned by zoomer in the event_handlers.zooming
		svg_vis_group = svg_zoomer_group.append("svg:g")
		.attr("id", "svg_vis_group")

		// binding the brush event for the brush group.
		svg_brusher_group
		.call(brusher)
		.on("mousedown.brush", null)
		.on("touchstart.brush", null)
		.on("touchmove.brush", null)
		.on("touchend.brush", null);

		svg_brusher_group.select('.background').style('cursor', 'auto');

		// Appending visualization layers.
		// Visualization layers structure.
		// svg
		// |>svg_zoomer_group
		// |  |>svg_background_rect
		// |  |>svg_brush_group
		// |  |>svg_vis_group
		// |  |  |>svg_vis_links_group
		// |  |  |>svg_vis_links_labels_group
		// |  |  |>svg_vis_nodes_group
		// |  |  |>svg_vis_nodes_labels_group
		// |  |  |>svg_vis_Z_index_group
		svg_vis_links_group = svg_vis_group.append("g")
		//.attr("class", "link")
		.attr("id", "svg_vis_links_group")

		svg_vis_links_labels_group = svg_vis_group.append("g")
		//.attr("class", "link_label")
		.attr("id", "svg_vis_links_labels_group")

		svg_vis_nodes_group = svg_vis_group.append("g")
		//.attr("class", "node")
		.attr("id", "svg_vis_nodes_group")

		svg_vis_nodes_labels_group = svg_vis_group.append("g")
		//.attr("class", "node_label")
		.attr("id", "svg_vis_nodes_labels_group")

		svg_vis_Z_index_group = svg_vis_group.append("g")
		//.attr("class", "z_index_group")
		.attr("id", "svg_vis_Z_index_group")

		// force layout initailization.
		// It is a d3.layout.force() object.
		layouts["force"] = d3.layout.force()
		.charge(-100)
		.friction(0.8)
		.size([width, height])

		// Set the force tick call back function for animation of the graph while run the force layout.
		layouts["force"].on("tick", event_handlers.force_tick)

		// Dragger node events handler object.
		// It is an d3.behavior.drag() object.
		dragger = d3.behavior.drag()
		.on("dragstart", event_handlers.node_dragstarted)
		.on("drag", event_handlers.node_dragging)
		.on("dragend", event_handlers.node_dragended)

		// line object for create a basis curve.
		// it is a d3.svg.line() object.
		line = d3.svg.line().interpolate("basis");


			
		return gsqlv;
	}

	// insert a css rule for the graph visualization.
	gsqlv.insertCSSRule = function (d) {

		styleSheet.insertRule(d, styleSheet.cssRules.length);

		return gsqlv;
	}

	// initialize the style sheet of the graph visualization.
	// mainly for the style :
	// 1 .node.selected
	// 2 .link.selected
	// 3 .link_label.selected
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
		styleSheet.insertRule(".node.selected {\
			stroke: #d62728;\
			stroke-width: 1px;\
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

		/*
		styleSheet.insertRule(".node_label .selected {\
			fill: #000;\
			stroke: #000;\
			}", styleSheet.cssRules.length)
		*/
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

		styleSheet.insertRule(".link.selected {\
			stroke: #d62728;\
			stroke-width: 1px;\
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

		styleSheet.insertRule(".link_label.selected{\
			fill: #d62728;\
			stroke: #d62728;\
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
		min_ratio = Math.min(width_ratio, height_ratio) * 0.85;
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
		svg_vis_group
		.transition()
		.duration(500)
		.attr("transform", "translate(" + [x_trans, y_trans] + ")" + " scale(" + min_ratio + ")");

		// tell the zoomer what we did so that next we zoom, it uses the
		// transformation we entered here
		zoomer.translate([x_trans, y_trans ]);
		zoomer.scale(min_ratio);

		gsqlv.refresh();
		clearTimeout(timer);
	}

	// redraw everything.
	// 1. remove everything.
	// 2. append everything back.
	// 3. update the selection status of all elements.
	gsqlv.redraw = function() {

		// output the summary information of the graph in the summary view.
		window.summaryInformationOutput(); 

		// remove everything.
		svg_vis_links_group.selectAll("*").remove();
		svg_vis_nodes_group.selectAll("*").remove();
		svg_vis_nodes_labels_group.selectAll("*").remove();
		svg_vis_links_labels_group.selectAll("*").remove();
		svg_vis_Z_index_group.selectAll("*").remove();

		// draw link markers.
		svg_vis_links_group
		.append("defs")
		.selectAll("marker")
		.data(data.links)
		.enter().append("marker")
		.each(function(d) {
			d3.select(this)
			.attr("id", "lm_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type))
			.attr("markerWidth", "10")
			.attr("markerHeight", "10")
			.attr("refX", 5)
			.attr("refY", 3)
			.attr("orient", "auto")
			.attr("fill", d.stroke)
			.attr("markerUnits", "userSpaceOnUse") // User for "strokeWidth"
			.attr("stroke-width", "3px")
			.html('<path d="M0,0 L0,6 L7,3 L0,0"/>') // define the shape of the marker.
		})

		// draw links
		temp_links = svg_vis_links_group
		.selectAll(".edge")
		.data(data.links)
		.enter().append("path")
		.each(function(d) {
			d3.select(this)
			.attr("id", "l_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type))
			.attr("class", "link")
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("stroke-linecap", "round")
			.attr("stroke-dasharray", function(l) {
				if (d.type == "-1") {
					return "5 2"
				}
				else {
					return "5 0"
				}
			})
			.attr("opacity", d["opacity"])
			.attr("d", function(d) {
				return curveLinkPoints(d);//line([[d.source.x, d.source.y],[d.target.x, d.target.y]])
			})

			if (graphType == "directed") {
				d3.select(this)
				.attr("style", "marker-end:url(#" + "lm_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type) + ")")
			}
			else if (graphType == "undirected") {

			}
			
		})

		// draw nodes and binding events
		temp_nodes = svg_vis_nodes_group
		.selectAll("circle")
		.data(data.nodes)
		.enter().append("circle")
		.each(function(d) {
			d3.select(this)
			.attr("id", "n_"+refineIDString(d.id+d.type))
			.attr("class", "node")
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("cx", function(d) { return d.x; })
			.attr("cy", function(d) { return d.y; })
			.attr("r", d.r)
		})
        .on("dblclick", event_handlers.node_dblclick)
		.on("click", event_handlers.node_click)
		.on("mouseup", event_handlers.node_mouseup)
		.on("mouseover", event_handlers.node_mouseover)
		.on("mouseout", event_handlers.node_mouseout)
		.call(dragger)

		// draw node labes and binding events (since node label is likely on the top of nodes).
		temp_nodes_labels = svg_vis_nodes_labels_group
		.selectAll(".node_label")
		.data(data.nodes)
		.enter().append("text")
		.each(function(d) {
			var darkerColor = d3.rgb(d.fill).darker(2)
			d3.select(this)
			.attr("id", "nl_"+refineIDString(d.id+d.type))
			.attr("class", "node_label")
			.attr("font-size", 3)
			.attr("font-family", "sans-serif")
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor.toString())
			.attr("opacity", d["opacity"]*0.9)

			//nodeLabelGenerator(d3.select(this), node_label_attr_array, d);
			nodeLabelGenerator(d3.select(this), node_label_dict_type, d);
		})
		.on("dblclick", event_handlers.node_label_dblclick)
		.on("click", event_handlers.node_label_click)
		.on("mouseup", event_handlers.node_label_mouseup)
		.on("mouseover", event_handlers.node_label_mouseover)
		.on("mouseout", event_handlers.node_label_mouseout)
		.call(dragger)

		// draw link labels
		temp_links_labels = svg_vis_links_labels_group
		.selectAll(".link_label")
		.data(data.links)
		.enter().append("text")
		.each(function(d) {
			var darkerColor = d3.rgb(d.stroke).darker()
			d3.select(this)
			.attr("id", "ll_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type))
			.attr("class", "link_label")
			.attr("font-size", 3)
			.attr("font-family", "sans-serif")
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)
			.on("click", event_handlers.link_click)
			.on("mousedown", event_handlers.link_mousedown)
			.on("mouseup", event_handlers.link_mouseup)
			.on("mouseover", event_handlers.link_mouseover)
			.on("mouseout", event_handlers.link_mouseout)

			var lines = linkLabelGenerator(d3.select(this), edge_label_dict_type, d);

			// use for generating the background rect for link label
			linkLabelBackgroundGenerator(d3.select(this), d, lines);
		})

		// update the selection status of all elements.
		gsqlv.updateSelectedElements();
		return gsqlv;
	}

	// refresh nodes, links, node labels, link labels.
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
			.attr("r", function(d) { return d.r; })

			/*
				var initNodeSetting = {
					'stroke': 'transparent',//'#777',
					'stroke-width': '1',
					'fill': '#aec7e8',
					'opacity':1,
					'r':6,
					'selected' : false,
					'previouslySelected' : false,
					'hierarchied' : false, // for hierarchical layout
					'x':window.v_width / 2,
					'y':window.v_height / 2,
				}
			*/
		})
		/*
		temp_links
		.each(function(d) {
			d3.select(this)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("x1", function(d) { return d.source.x; })
			.attr("y1", function(d) { return d.source.y; })
			.attr("x2", function(d) { return d.target.x; })
			.attr("y2", function(d) { return d.target.y; })
		})
		*/	 
		temp_links
		.each(function(d) {
			d3.select(this)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("d", function(d) {
				return curveLinkPoints(d);
			})
			//.attr("x1", function(d) { return d.source.x; })
			//.attr("y1", function(d) { return d.source.y; })
			//.attr("x2", function(d) { return d.target.x; })
			//.attr("y2", function(d) { return d.target.y; })
			//.attr("marker-end", "url(#" + "lm_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type) + ")")
		})

		temp_nodes_labels
		.each(function(d) {
			var darkerColor = d3.rgb(d.fill).darker(2)

			d3.select(this)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)

			refreshNodeLabelPosition(d3.select(this), node_label_dict_type, d);
		})

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

			var lines = refreshLinkLabelPosition(d3.select(this), edge_label_dict_type, d);
			refreshLinkLabelBackground(d3.select(this), d, lines);
		})
		//.attr("x", function(d) {return 0.5*d.source.x+ 0.5*d.target.x})
		//.attr("y", function(d) {return 0.5*d.source.y+ 0.5*d.target.y - 1.5*(edge_label_key_array.length+edge_label_attr_array.length-1)}) 
		//.each(function(d) {
		//	d3.select(this).selectAll("tspan")
		//	.attr("x", 0.5*d.source.x+ 0.5*d.target.x)
		//});

		d3.selectAll("marker")
		.each(function(d) {
			d3.select(this)
			.attr("fill", d.stroke)
		})

		//window.updateSummaryInformationForSelectedNodes();

		return gsqlv
	}



	// refresh nodes, links, link labels, node labels by transition().duration()
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
			.attr("r", function(d) { return d.r; })
		})

		temp_links
		.each(function(d) {
			d3.select(this)
			.transition()
			.duration(milliseconds)
			.attr("fill", d.fill)
			.attr("stroke", d.stroke)
			.attr("stroke-width", d["stroke-width"])
			.attr("opacity", d["opacity"])
			.attr("d", function(d) {
				return curveLinkPoints(d);
			})
		})

		temp_nodes_labels
		.each(function(d) {
			var darkerColor = d3.rgb(d.stroke).darker(4)

			d3.select(this)
			.attr("font-size", 3)
			.attr("stroke", "transparent")
			.attr("stroke-width", 0.1)
			.attr("text-anchor", "middle")
			.attr("transform", "scale(1, 1)")
			.attr("fill", darkerColor)
			.attr("opacity", d["opacity"]*0.9)

			refreshNodeLabelPosition(d3.select(this), node_label_dict_type, d);
		})

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

			var lines = refreshLinkLabelPosition(d3.select(this), edge_label_dict_type, d);
			refreshLinkLabelBackground(d3.select(this), d, lines);
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

		//update rootNode preDefinition
		try {
			var tempNode = node_links[rootNode].node;

			for (var key in rootNodeSetting) {
				tempNode[key] = rootNodeSetting[key];
			}
		}
		catch (err) {
			console.log("Root Node is not found. in function runPreDefinition. " + err);
		}


		if (typeof preDefinition == 'undefined') {
			return;
		}
		// Have to run after gsqlv.initNodeLinks();
		// if there is a coloring pre definition, do the coloring for the node and edges, base on the selection condition.
		if ('coloring' in preDefinition) {
			//[{"selection":"some_int < 3","selectionType":"nodes","color":"#00ff00"}]
			preDefinition.coloring.forEach(function(c) {
				if (c.selectionType == "nodes") {
					nodes.forEach(function(n){
						if (gsqlv.checkConditionForNode(n, c.selection)) {
							n.fill = c.color;//color(c.color)
							
							if (n.type == "TXN") {
								n.stroke = "#101010";
							}
							else {
								n.stroke = "#636363";
							}
						}
					})
				}
				else if (c.selectionType == "edges") {
					links.forEach(function(l){
						if (gsqlv.checkConditionForEdge(l, c.selection)) {
							l.stroke = c.color;//color(c.color)
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

	gsqlv.label = function(x) {
		if (!arguments.length) {
			return {node:node_label_dict_type, edge:edge_label_dict_type}
		}
		else {
			node_label_dict_type = x.node;
			edge_label_dict_type = x.edge;

			return gsqlv;
		}
	}

	gsqlv.rootNode = function(x) {
		if (!arguments.length) {
			return rootNode;
		}
		else {
			rootNode = x;

			try {
				var tempNode = node_links[rootNode].node;

				for (var key in rootNodeSetting) {
					tempNode[key] = rootNodeSetting[key];
				}
			}
			catch (err) {
				console.log("Root Node is not found. in function rootNode. " + err);
			}
			return gsqlv;
		}
	}

	// Set a selected node to be root node.
	// root node is refered by d.type + & + d.id
	gsqlv.setSelectedNodeToBerootNode = function() {
		var selectedNode = data.nodes.filter(function(d) { 
				return d.selected; 
			})
		var tempRootNode = "";
		if (selectedNode.length == 1) {
			selectedNode.forEach(function(d) {
				tempRootNode = d.type + "&" + d.id
			})

			//tempRootNode = Object.keys(node_links)[Object.keys(node_links).map(function(d) {return d.toUpperCase()}).indexOf(tempRootNode.toUpperCase())];

			gsqlv.rootNode(tempRootNode);
		}
		else {
			;
		}
	}

	// highlighting selected node.
	gsqlv.highlightingNodes = function() {
		var unhighlighting = true;

		// if nothing is selected. unhighlighting every thing
		// else high lighting the selected nodes and edges.
		if (data.nodes.filter(function(d) { 
				return d.selected; 
			}).length == 0) {
			data.nodes.forEach(function(d) {
				d.opacity = initNodeSetting.opacity;
			})
			data.links.forEach(function(d) {
				d.opacity = initLinkSetting.opacity;
				d.background_opacity = d.opacity;
			})

			unhighlighting = false;

			svg_vis_Z_index_group.selectAll("*").remove();
			gsqlv.redraw();
		}
		else {
			//get the selected svg node elements which need to move forefront
			var temp_nodes = data.nodes.filter(function(d) {
				if (d.selected) {
					d.opacity = 1;
				}
				else {
					d.opacity = 0.3;
				}

				return d.selected;
			})

			//get the selected svg link elements which need to move forefront
			var temp_links = data.links.filter(function(d) {
				if (d.selected) {
					d.opacity = 1;
				}
				else {
					d.opacity = 0.3;
				}

				d.background_opacity = d.opacity;

				return d.selected
			})

			// remove everything in the forefont (svg_vis_Z_index_group)
			svg_vis_Z_index_group.selectAll("*").remove();

			// redraw everything
			gsqlv.redraw();

			// Move the svg elements to forefront. The elements include follows :
			// 1. Selected links
			// 2. label background of selected links
			// 3. labels of selected links
			// 4. selected nodes
			// 5. labels of selected nodes


			//1
			temp_links.forEach(function(d) {
				var tl = d3.select("#l_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type))[0][0]
				svg_vis_links_group[0][0].removeChild(tl)
				svg_vis_Z_index_group[0][0].appendChild(tl)
			})

			//2
			temp_links.forEach(function(d) {
				var tl = d3.select("#ll_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type)+"_background")[0][0];
				svg_vis_links_labels_group[0][0].removeChild(tl)
				svg_vis_Z_index_group[0][0].appendChild(tl)
			})

			//3
			temp_links.forEach(function(d) {
				var tl = d3.select("#ll_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type))[0][0];
				svg_vis_links_labels_group[0][0].removeChild(tl)
				svg_vis_Z_index_group[0][0].appendChild(tl)
			})

			//4
			temp_nodes.forEach(function(d) {
				var tn = d3.select("#n_"+refineIDString(d.id+d.type))[0][0];
				svg_vis_nodes_group[0][0].removeChild(tn)
				svg_vis_Z_index_group[0][0].appendChild(tn)
			})

			//5
			temp_nodes.forEach(function(d) {
				var tn = d3.select("#nl_"+refineIDString(d.id+d.type))[0][0];
				svg_vis_nodes_labels_group[0][0].removeChild(tn)
				svg_vis_Z_index_group[0][0].appendChild(tn)	
			})

			/*
			svg_vis_Z_index_group.selectAll("use").remove();

			svg_vis_Z_index_group.selectAll(".use_temp")
			.data(temp_links)
			.enter()
			.append("use")
			.attr("xlink:href", function(d){
				return "#l_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type);
			})

			svg_vis_Z_index_group.selectAll(".use_temp")
			.data(temp_links)
			.enter()
			.append("use")
			.attr("xlink:href", function(d){
				return "#ll_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type)+"_background";
			})

			svg_vis_Z_index_group.selectAll(".use_temp")
			.data(temp_links)
			.enter()
			.append("use")
			.attr("xlink:href", function(d){
				return "#ll_"+refineIDString(d.type+d.source.id+d.source.type+d.target.id+d.target.type);
			})

			svg_vis_Z_index_group.selectAll(".use_temp")
			.data(temp_nodes)
			.enter()
			.append("use")
			.attr("xlink:href", function(d){
				return "#n_"+refineIDString(d.id+d.type);
			})

			svg_vis_Z_index_group.selectAll(".use_temp")
			.data(temp_nodes)
			.enter()
			.append("use")
			.attr("xlink:href", function(d){
				return "#nl_"+refineIDString(d.id+d.type);
			})
			*/


			// unselect everything after changing the opacity.
			data.nodes.forEach(function(d) {
				d.selected =  d.previouslySelected = false; 
			})


			gsqlv.updateSelectedElements()
			gsqlv.refresh();
		}


		return unhighlighting;
	}

	// coloring the nodes
	// Input : color
	//	change the color of the selected nodes of the graph.
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
			}

			gsqlv.refresh();
		}	
	}

	// coloring Edges
	// Input : color
	// 	change the color of the selected edges of the graph.
	gsqlv.coloringEdges = function(x) {
						
		if (!arguments.length) {
			return;
		}
		else {
			if (data.links.filter(function(d) { 
					return d.selected; 
				}).length == 0) {
				//No selected nodes, do nothing.
			}
			else {
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

	// unselect everything.
	gsqlv.unselectingEverything = function() {
		// previouslySelected is use for unselect node by shiftKey.
		data.nodes.forEach(function(d) {
			d.selected =  d.previouslySelected = false; 
		})

		data.links.forEach(function(d) {
			d.selected = false; 
		})

		gsqlv.updateSelectedElements()
		gsqlv.refresh();
	}

	// hiding the selected nodes and edges.
	gsqlv.hidingNodes = function() {
		backup_data = clone(data);

		data.links = data.links.filter(function(d) {
			return !d.selected;//!(d.source.selected || d.target.selected);
		})		

		data.nodes = data.nodes.filter(function(d) {
			return !d.selected;
		})

		gsqlv.initNodeLinks();

		gsqlv.redraw();
	}

	// selecting node by conditions.
	// Input : Condition string.
	//	1. append a script tag on the fly to check the condition string for each node.
	// 	2. run updateSelected Elements()
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
				jsVariable += "var __node = " + y + '.data().nodes[' +i+'];'
				//jsVariable += "var node = jQuery.extend(true, {}, " + y + '.data().nodes[' +i+']);'
				jsVariable += "var id = __node.id;var type = __node.type;"

				for (var key in n.attr) {
					jsVariable += "var "+key+"=__node.attr."+key+";";
				}

				jsItem += jsVariable;
				jsItem += 'try{ if ('+x+') {__node.selected=true;}\
						else{__node.selected=false;}}catch(err){__node.selected=false;}'

				jsItem += '}();'

				jsString += jsItem;
			})

			jsString += y+'.updateSelectedElements();'

			var code = "<script id='selectingNodesByConditions'>"+ jsString+ ";<"+"/script>";
			$('body').append($(code)[0]);
		}
	} 

	// nodes
	// Input : the node object, condition string.
	//	1. append a script tag on the fly to check the condition string.
	gsqlv.checkConditionForNode = function(n, condition) {
		//console.log("check condition for node.")
		// append <script> tag on the fly for checking the condition. 
		$("#checkConditionForNode").remove();

		condition = condition == "" ? 'false':condition;

		var tempKey = n.type + "&" + n.id; //n == node_links[n.type + "&" + n.id].node

		var jsString = "";

		var jsItem = "!function(){"

		var jsVariable = "";

		//__$$global_string equals to the name of the graph visualization object.
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
	// Input : the link object, condition string.
	gsqlv.checkConditionForEdge = function(l, condition) {
		//console.log("check condition for link.")

		// remove the <script>
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
		jsVariable += "var link =" + __$$global_string + '.link_nodes()["' +linkKey+'"]["'+l.type+'"].link;'
		jsVariable += "var type = link.type;"

		for (var key in l.attr) {
			jsVariable += "var "+key+"=link.attr."+key+";";
		}

		jsItem += jsVariable;
		jsItem += 'try{ if ('+condition+') {__$$global_boolean=true;}\
				else{__$$global_boolean=false;}}catch(err){__$$global_boolean=false;}'

		jsItem += '}();'

		jsString += jsItem;

		var code = "<script id='checkConditionForEdge'>"+ jsString+ ";<"+"/script>";

		// append a script tag.
		$('body').append($(code)[0]);
		
		return __$$global_boolean;
	}


	// selecting edges by conditions.
	// Input : Condition string.
	//	1. append a script tag on the fly to check the condition string for each edges.
	// 	2. run updateSelectedEdges()
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
				jsVariable += "var __link =" + y + '.data().links[' +i+'];'
				jsVariable += "var type = __link.type;"

				for (var key in l.attr) {
					jsVariable += "var "+key+"=__link.attr."+key+";";
				}

				jsItem += jsVariable;
				jsItem += 'try{ if ('+x+') {__link.selected=true;"__link.source.selected=true;__link.target.selected=true"}\
						else{;}}catch(err){;}'

				jsItem += '}();'

				jsString += jsItem;
			})

			jsString += y+'.updateSelectedEdges();'

			var code = "<script id='selectingEdgesByConditions'>"+ jsString+ ";<"+"/script>";
			$('body').append($(code)[0]);
		}
	} 

	gsqlv.updateSelectedElements = function() {
		//console.log("test sucessful.")

		temp_nodes.classed("selected", function(d) {
			//d.previouslySelected = !d.selected
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
		// set the node_dblclick event as a specific call back function.
		if (event == "node_dblclick") {
			event_handlers.node_dblclick = function(d) {
				console.log(d)

				/*
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
				*/

				temp_event = myObject.events[event]; 
				submit_URL = temp_event.URL_head + "?"
				URL_attrs =  temp_event.URL_attrs

				// the URL will be the format as transactionfraud?id=370781****277X&type=ssn"
				/*
				if ("id" in URL_attrs) {
					submit_URL += "/" + d.id + "?";
				}
				else {
					submit_URL += "?";
				}
				*/
				
				for (var attr in URL_attrs) {
					name = attr;
					attr = URL_attrs[attr];

					// If the attr is 'id', don't do the same as other attribtues.
					// This part should be imporved as defined by the UIpage setting message.
					//if (name == "id") continue;

					if (attr.usage == "input") {
						if (document.getElementsByName(attr.name)[0].value=="") {
							;	
						}
						else{
							submit_URL += name + "=" + (document.getElementsByName(attr.name)[0].value==""?1:document.getElementsByName(attr.name)[0].value) +"&";
						}

					}
					else if (attr.usage == "attributes") {
						submit_URL += name+ "=" + myObject.attributes[attr.name] + "&";
					}
					else if (attr.usage == "select") {
						if (name == 'type')
							submit_URL += name + "=" + d[attr.name].toLowerCase() +"&";
						else
							submit_URL += name + "=" + d[attr.name] +"&";
					}
				}

				$.get(submit_URL, function(message) {
					message = JSON.parse(message);
					if (!message.error) {
						newData = message.results;

						// after get the rest query subgraph.
						// add sub graph to gsqlv.
						// run it.
						gsqlv
						.addSubGraph(newData)
						.run()	

						updateLabelFilteringListBox(mygv.data());

						// backup json message, and output the json message in JSON tabs, and output the summary information in the Summary tabs.
						window.messageArray.push(message);
						window.JSONMessageOutput();
						window.summaryInformationOutput();			

					}
					else {
						;
					}
				})

				// stop propagation of click.
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

		// doing the layout computation.
		gsqlv.startLayout();

		// redraw everything base on the selection status and newly generated layout.
		gsqlv.redraw();

		clearTimeout(timer);
		timer = setTimeout(gsqlv.center_view, 5000);

		return gsqlv;
	}

	gsqlv.startLayout = function() {
		layouts["force"].stop();

		// Get the right rootNode key;
		var tempRootNode = Object.keys(node_links).filter(function(d) {
			return d.toUpperCase() == rootNode.toUpperCase();
		})

		rootNode = tempRootNode[0] == undefined ? Object.keys(node_links)[0] : tempRootNode[0];

		if (layout == "force") {
			// deep copy nodes. Since we need to append a node between two node for a nice looking force layout.
			var tempNodes = data.nodes.slice();

			// If there are multiple links between two nodes, we just count as one links. Other wise the link strength may lead wierd problem.
			var tempLink = function(links) {
								var result = [];
								
								Object.keys(links).forEach(function(lKey){
									var s = links[lKey][Object.keys(links[lKey])[0]].link.source;
									var t = links[lKey][Object.keys(links[lKey])[0]].link.target;
									var ii = {}
									tempNodes.push(ii);
									result.push({source: s, target: ii}, {source: ii, target: t});
								})
								
								/*
								Object.keys(links).forEach(function(lKey){
									var s = links[lKey][Object.keys(links[lKey])[0]].link.source;
									var t = links[lKey][Object.keys(links[lKey])[0]].link.target;
									result.push({source: s, target: t});
								})
								*/
								return result;
							}(link_nodes)


			// define the configuration of the force direct layout.
			layouts["force"]
			.linkDistance(function(l) {
				var numberOfNeighbor = 0;
				var s = l.source;
				var t = l.target;

				if ("id" in s) {
					var skey = s.type + "&" + s.id;
					numberOfNeighbor += node_links[skey].links.length;
				}

				if ("id" in t) {
					var tkey = t.type + "&" + t.id;
				 	numberOfNeighbor += node_links[tkey].links.length;
				}		

				return 10 * numberOfNeighbor;
			})
			.linkStrength(2)
			.nodes(tempNodes)
			.links(tempLink)
			.start();
		}
		else if (layout == "hierarchical") {
			//console.log(layout +" layout")
			// do something for the herachical layout.

			separation_x = 55;
			separation_y = 45;

			data.nodes.forEach(function(d) {
				d.hierarchied = false;
			})

			node_links[rootNode].node.hierarchied = true; // fix the root node.
			node_links[rootNode].node.x = setting.width / 2;	
			node_links[rootNode].node.y = setting.height / 2;

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
			//console.log(layout +" layout")

			// circle layout only supports the graph with 3 or more nodes.
			// circle layout is generated by a tree layout algorith + rotation function.
			if (data.nodes.length <= 2) {
				if (typeof data.nodes[0] != 'undefined') {
					data.nodes[0].x = window.v_width / 2 - 50;
				}
				return gsqlv;
			}

			// circle layout x,y coordinate separations.
			separation_x = 360;
			separation_y = 35;

			data.nodes.forEach(function(d) {
				d.hierarchied = false;
			})

			node_links[rootNode].node.hierarchied = true; // fix the root node.
			node_links[rootNode].node.x = setting.width / 2;	
			node_links[rootNode].node.y = setting.height / 2;

			//rootNode is a string as "0&1"

			var tempMaxDepth = 0;

			root = {"children":[], "node":node_links[rootNode], "_depth":0}

			var setCircleTreeData = function (_this) {
				_this.node.links.forEach(function(node, i) {
					if (!node.hierarchied) {
						nodeKey = node.type + "&" + node.id;
						node.hierarchied = true;

						tempTreeNode = {"children":[], "node":node_links[nodeKey], "_depth":_this._depth+1}
						_this.children.push(tempTreeNode)

						if (tempMaxDepth < _this._depth+1){
							tempMaxDepth = _this._depth + 1;
						}
					}
				})
				
				_this.children.forEach(function(treeNode, i) {
					setCircleTreeData(treeNode)
				})
			}

			setCircleTreeData(root);

			layouts["circle"] = d3.layout.tree()
					.size([separation_x, separation_y*tempMaxDepth*2])
					.separation(function(a, b){
						return (a.parent == b.parent ? 1 : 2) / a.depth / a.depth;
					})

			circledNodes = layouts["circle"].nodes(root);

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

			// standard tree layout algorithm of d3.
			// there are several other layout algorithm for use if needed.
			// in case of needed.

			separation_x = 55;
			separation_y = 35;

			number_separation_x = 1;
			number_separation_y = 1;

			layouts["tree"] = d3.layout.tree()
								//.size([360, Math.min(setting.width, setting.height) / 2 -120])
								.size([separation_x, separation_y])
								.separation(function(a, b){
									//return (a.parent == b.parent ? 1 : 2) / a.depth;
									return (a.parent == b.parent ? 1 : 2);
								})

			data.nodes.forEach(function(d) {
				d.hierarchied = false;
			})

			node_links[rootNode].node.hierarchied = true; // fix the root node.
			node_links[rootNode].node.x = setting.width / 2;	
			node_links[rootNode].node.y = setting.height / 2;

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

			translate_x = 0;
			translate_y = 0;
			
			treeNodes.forEach(function(n) {
				if (n.depth == 0) {
					//n.node.node.x = n.x //+ setting.width / 2;
					//n.node.node.y = n.y //+ setting.height / 2;
					translate_x = n.node.node.x - n.x;
					translate_y = n.node.node.y - n.y
				}
				else {
					n.node.node.x = n.x + translate_x;
					n.node.node.y = n.y + translate_y;	
				}
			})
			
		}
		else {

		}

		return gsqlv
	}

	// Generate the summary information view for all nodes for each type.
	gsqlv.summaryInformationForAllNodes = function() {
		var numberOfNodes = data.nodes.length;
		var numberOfEdges = data.links.length;
		var result = '<span style="color:green;font-size:15px;font-family: Times">Summary Info of the Graph:</span>\n'

		result += '<span style="color:green;font-size:15px;font-family: Times">All Types : </span>\n'
		result += '<span style="color:red;font-size:15px;font-family: Times">Number of Nodes : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + numberOfNodes + '</span>\n' 
		result += '<span style="color:red;font-size:15px;font-family: Times">Number of Edges : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + numberOfEdges + '</span>\n'

		var averageInDegree = 0;
		var averageOutDegree = 0;

		// 1. parse the typeArray to know, what nodes each type has.
		// 2. use the type array to generate the summary information of each type and all types.
		//	|	mostly for the average in and out degree.
		typeArray = {};

		for (var node in node_links) {
			averageInDegree += node_links[node].degree.in.length;
			averageOutDegree += node_links[node].degree.out.length;

			if (node_links[node].node.type in typeArray) {
				typeArray[node_links[node].node.type].push(node_links[node]);
			}
			else {
				typeArray[node_links[node].node.type] = [];
				typeArray[node_links[node].node.type].push(node_links[node]);
			}
		}

		if (numberOfNodes == 0) {
			;
		}
		else {
			averageInDegree /= numberOfNodes;
			averageOutDegree /= numberOfNodes;
		}

		result += '<span style="color:red;font-size:15px;font-family: Times">Average In Degree : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageInDegree.toFixed(2) + '</span>\n' 
		result += '<span style="color:red;font-size:15px;font-family: Times">Average Out Degree : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageOutDegree.toFixed(2) + '</span>\n'

		for (var type in typeArray) {
			result += '<span style="color:green;font-size:15px;font-family: Times">Type ' + type + ' : </span>\n'
			result += '<span style="color:red;font-size:15px;font-family: Times">Number of Nodes : </span>' 
			result += '<span style="color:blue;font-size:15px;font-family: Times">' + typeArray[type].length + '</span>\n' 

			var averageInDegreeT = 0;
			var averageOutDegreeT = 0;

			for (var node in typeArray[type]) {
				averageInDegreeT += typeArray[type][node].degree.in.length;
				averageOutDegreeT += typeArray[type][node].degree.out.length;
			}

			if (typeArray[type].length == 0) {
				;
			}
			else {
				averageInDegreeT /= typeArray[type].length;
				averageOutDegreeT /= typeArray[type].length;
			}

			result += '<span style="color:red;font-size:15px;font-family: Times">Average In Degree : </span>' 
			result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageInDegreeT.toFixed(2) + '</span>\n' 
			result += '<span style="color:red;font-size:15px;font-family: Times">Average Out Degree : </span>' 
			result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageOutDegreeT.toFixed(2) + '</span>\n'
		}
		return result
	}

	// 
	gsqlv.summaryInformationForSelectedNodes = function() {
		var numberOfNodes = data.nodes.filter(function(n){return n.selected}).length;
		var numberOfEdges = data.links.filter(function(n){return n.selected}).length;

		if (numberOfNodes == 0) {
			return '<span style="color:green;font-size:15px;font-family: Times">No Selected Node</span>\n'
		}
		var result = '<span style="color:green;font-size:15px;font-family: Times">Summary Info of the Selected Nodes:</span>\n'

		result += '<span style="color:green;font-size:15px;font-family: Times">All Types : </span>\n'
		result += '<span style="color:red;font-size:15px;font-family: Times">Number of Selected Nodes : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + numberOfNodes + '</span>\n' 
		result += '<span style="color:red;font-size:15px;font-family: Times">Number of Selected Edges : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + numberOfEdges + '</span>\n'

		var averageInDegree = 0;
		var averageOutDegree = 0;


		// 1. parse the typeArray to know, what nodes each type has in the selected subgraph.
		// 2. use the type array to generate the summary information of each type and all types.
		//	|	mostly for the average in and out degree.

		var tempTypeArray = {};

		for (var node in node_links) {
			if (node_links[node].node.selected) {
				averageInDegree += node_links[node].degree.in.length;
				averageOutDegree += node_links[node].degree.out.length;

				if (node_links[node].node.type in tempTypeArray) {
					tempTypeArray[node_links[node].node.type].push(node_links[node]);
				}
				else {
					tempTypeArray[node_links[node].node.type] = [];
					tempTypeArray[node_links[node].node.type].push(node_links[node]);
				}
			}
			else {
				;
			}
		}

		if (numberOfNodes == 0) {
			;
		}
		else {
			averageInDegree /= numberOfNodes;
			averageOutDegree /= numberOfNodes;
		}

		result += '<span style="color:red;font-size:15px;font-family: Times">Average In Degree : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageInDegree.toFixed(2) + '</span>\n' 
		result += '<span style="color:red;font-size:15px;font-family: Times">Average Out Degree : </span>' 
		result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageOutDegree.toFixed(2) + '</span>\n'

		for (var type in tempTypeArray) {
			result += '<span style="color:green;font-size:15px;font-family: Times">Type ' + type + ' : </span>\n'
			result += '<span style="color:red;font-size:15px;font-family: Times">Number of Nodes : </span>' 
			result += '<span style="color:blue;font-size:15px;font-family: Times">' + tempTypeArray[type].length + '</span>\n' 

			var averageInDegreeT = 0;
			var averageOutDegreeT = 0;

			for (var node in tempTypeArray[type]) {
				averageInDegreeT += tempTypeArray[type][node].degree.in.length;
				averageOutDegreeT += tempTypeArray[type][node].degree.out.length;
			}

			if (tempTypeArray[type].length == 0) {
				;
			}
			else {
				averageInDegreeT /= tempTypeArray[type].length;
				averageOutDegreeT /= tempTypeArray[type].length;
			}

			result += '<span style="color:red;font-size:15px;font-family: Times">Average In Degree : </span>' 
			result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageInDegreeT.toFixed(2) + '</span>\n' 
			result += '<span style="color:red;font-size:15px;font-family: Times">Average Out Degree : </span>' 
			result += '<span style="color:blue;font-size:15px;font-family: Times">' + averageOutDegreeT.toFixed(2) + '</span>\n'
		}
		return result
	}

	gsqlv.typeArray = function() {
		return typeArray;
	}

	// doing the scaleAbility Test.
	// 1. append n nodes and 2 n edges randomly.
	gsqlv.scaleAbilityTest = function(n) {
		// add n new nodes and 2 * n new edges;
		console.log(n + "ScaleAbilityTest");

		var tNodeLength = data.nodes.length;

		for (var i=0; i<n; ++i) {
			var newNode = {};
			newNode.type = i%3;
			newNode.id = (data.nodes.length).toString();
			newNode.attr = {};
			newNode.attr.some_int = Math.floor(Math.random() * 100);
			newNode.attr.some_float = Math.random();
			newNode.attr.some_string = "string " + Math.floor(Math.random() * 100);

			gsqlv.addNode(newNode);
		}

		for (var i=0; i<n; ++i) {
			var targetID = i + tNodeLength - 1;
			var sourceID;
			if (tNodeLength > n) {
				sourceID = Math.floor(Math.random() * n) + tNodeLength - n;
			}
			else {
				sourceID = Math.floor(Math.random() * tNodeLength);
			}
			// add link1;
			var newLink = {};

			newLink.type = i%3;
			newLink.attr = {};
			newLink.attr.some_int = Math.floor(Math.random() * 100);
			newLink.attr.some_float = Math.random();
			newLink.attr.some_string = "string " + Math.floor(Math.random() * 100);

			newLink.source = {};
			newLink.target = {};

			newLink.source.id = data.nodes[sourceID].id;
			newLink.target.id = data.nodes[targetID].id;

			newLink.source.type = data.nodes[sourceID].type;
			newLink.target.type = data.nodes[targetID].type;

			gsqlv.addLink(newLink);

			// ad link2;
			var newLink = {};

			newLink.type = i%2;
			newLink.attr = {};
			newLink.attr.some_int = Math.floor(Math.random() * 100);
			newLink.attr.some_float = Math.random();
			newLink.attr.some_string = "string " + Math.floor(Math.random() * 100);

			newLink.source = {};
			newLink.target = {};

			newLink.source.id = data.nodes[targetID].id;
			newLink.target.id = data.nodes[sourceID].id;

			newLink.source.type = data.nodes[targetID].type;
			newLink.target.type = data.nodes[sourceID].type;

			gsqlv.addLink(newLink);
		}
	}	

	// get the event_handler.
	// it includes all event handlers of gsqlv.
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

		if (d3.event.keyCode == 69) {  // the 'e' key
			gsqlv.scaleAbilityTest(10); // Added n new random nodes and 2*n new random edges;
			window.updateLabelFilteringListBox(data);
			var tempTest = [0, 0, 0];

			var start = +new Date();  // log start timestamp
			gsqlv.startLayout();
			var end =  +new Date();  // log end timestamp
			var diff = end - start;
			tempTest[1] = diff;

			var start = +new Date();  // log start timestamp
			gsqlv.redraw();
			var end =  +new Date();  // log end timestamp
			var diff = end - start;
			tempTest[0] = diff;

			var start = +new Date();  // log start timestamp
			gsqlv.refresh();
			var end =  +new Date();  // log end timestamp
			var diff = end - start;
			tempTest[2] = diff;
			window.scaleAbilityData.push(tempTest);
			
			console.log(JSON.stringify(scaleAbilityData));
		}

		if (shiftKey) {
			svg_zoomer_group.call(zoomer)
			.on("dblclick.zoom", null)
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
		svg_zoomer_group.call(zoomer)
		.on("dblclick.zoom", null);
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
		window.updateSummaryInformationForSelectedNodes();
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

		window.updateSummaryInformationForSelectedNodes();
	}

	event_handlers.brushend = function() {
		d3.event.target.clear();
		d3.select(this).call(d3.event.target);

		temp_nodes.each(function(d) {
			d.previouslySelected = d.selected;
		})

		temp_links.each(function(d) {
			d.previouslySelected = d.selected;
		})
		//console.log("brush end " + brusherExtent);
	}

	event_handlers.node_dragstarted = function(d) {
		d3.event.sourceEvent.stopPropagation();
		//console.log("node dragstarted")

		if (!d.selected && !shiftKey) {
			// unselect everything.
			temp_nodes.classed("selected", function(p) { 
				return p.selected = p.previouslySelected = false; 
			});
		}

		if (shiftKey) {
			//if the shift key is down, triggle selection
			d3.select(this).classed("selected", d.selected = !d.previouslySelected);
		}
		else {
			// always select this node
			d3.select(this).classed("selected", d.selected = true);
		}

		//d3.select(this).classed("selected", function() {
		//	d.previouslySelected = d.selected; 
		//	return d.selected = true; 
		//})

		d3.select("#n_"+refineIDString(d.id+d.type)).classed("selected", function() {
			d.previouslySelected = d.selected; 
			return d.selected; 
		})

		d3.select("#nl_"+refineIDString(d.id+d.type)).classed("selected", function() {
			d.previouslySelected = d.selected; 
			return d.selected; 
		})


		temp_links.classed("selected", function(d) {
			return d.previouslySelected = d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.previouslySelected = d.selected = d.target.selected || d.source.selected;
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

		window.updateSummaryInformationForSelectedNodes();
        gsqlv.refresh();
	}

	event_handlers.node_dragended = function(d) {
		//console.log("node dragended")
		window.updateSummaryInformationForSelectedNodes();
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
		/*

		if (!shiftKey) {
			//if the shift key isn't down, unselect everything
			temp_nodes.classed("selected", function(p) { return p.selected =  p.previouslySelected = false; })
		}

		// always select this node
		d3.select(this).classed("selected", d.selected = !d.previouslySelected);

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		window.updateSummaryInformationForSelectedNodes();
		*/
	}

	event_handlers.node_mouseup = function(d) {
		/*
		if (d.selected && shiftKey) {
			d3.select(this).classed("selected", d.selected = false)
		}

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});
		*/
	}

	event_handlers.node_mouseover = function(d) {
		d3.select(this)
		.classed("selected", true);
	}

	event_handlers.node_mouseout = function(d) {
		d3.select(this)
		.classed("selected", d.selected);

		d3.event.stopPropagation();
	}

	event_handlers.node_label_dblclick = function(d) {
		console.log("dblclick of node " + d.id + " " + d.type)
		d3.event.stopPropagation(); 
	} 

	event_handlers.node_label_click = function(d) {
		if (d3.event.defaultPrevented) return;
		/*
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

		window.updateSummaryInformationForSelectedNodes();		
		*/	
	}

	event_handlers.node_label_mouseup = function(d) {
		/*
		if (d.selected && shiftKey) {
			d3.select(this).classed("selected", d.selected = false)
		}

		temp_links.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});

		temp_links_labels.classed("selected", function(d) {
			return d.selected = d.target.selected || d.source.selected;
		});			
		*/
	}

	event_handlers.node_label_mouseover = function(d) {
		d3.select("#n_"+refineIDString(d.id+d.type))
		.classed("selected", true);
	}

	event_handlers.node_label_mouseout = function(d) {
		d3.select("#n_"+refineIDString(d.id+d.type))
		.classed("selected", d.selected);
	}


	/*
	.on("click", event_handlers.link_click)
	.on("mouseup", event_handlers.link_mouseup)
	.on("mouseover", event_handlers.link_mouseover)
	.on("mouseout", event_handlers.link_mouseout)
	*/

	event_handlers.link_mousedown = function(d) {
		d3.event.stopPropagation();

		if (!shiftKey) {
			// unselect everything.
			temp_links.classed("selected", function(p) { 
				return p.selected = p.previouslySelected = false; 
			});
		}

		if (shiftKey) {
			//if the shift key is down, triggle selection
			d3.select(this).classed("selected", d.selected = !d.previouslySelected);
		}
		else {
			// always select this link
			d3.select(this).classed("selected", d.selected = true);
		}

		d.previouslySelected = d.selected;

		//d.target.selected = d.selected;
		//d.source.selected = d.selected;

		gsqlv.updateSelectedElements();
		window.updateSummaryInformationForSelectedNodes();
	}

	event_handlers.link_click = function(d) {

	}

	event_handlers.link_mouseup = function(d) {

	}

	event_handlers.link_mouseover = function(d) {

	}

	event_handlers.link_mouseout = function(d) {

	}	

	function nodeLabelGenerator(target, labels, d) {
		var lines = -1;

		var keys = labels['__key'];

		for (var key in keys) {
			if (keys[key]) {
				if (d[key] === undefined) {
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", d.x)
					.text(key+":"+d[key])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", d.x)
					.attr("dy", 3)
					.text(key+":"+d[key])

					lines += 1;
				}
			}
		}

		var attrs = labels[d.type]

		for (var attr in attrs) {
			if (attrs[attr]) {
				if (d.attr[attr] === undefined){
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", d.x)
					.text(attr+":"+d.attr[attr])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", d.x)
					.attr("dy", 3)
					.text(attr+":"+d.attr[attr])

					lines += 1;
				}
			}
		}

		target.selectAll("tspan")
		.style("word-wrap", "break-word")

		target
		.attr("x", d.x)
		.attr("y", d.y-1.5*lines+1)

		return lines;
	}


	function refreshNodeLabelPosition(target, labels, d) {
		var lines = -1;

		var keys = labels['__key'];

		for (var key in keys) {
			if (keys[key]) {
				if (d[key] === undefined) {
					continue;
				}
				else if (lines == -1) {
					lines += 1;
				}
				else {
					lines += 1;
				}
			}
		}

		var attrs = labels[d.type]

		for (var attr in attrs) {
			if (attrs[attr]) {
				if (d.attr[attr] === undefined){
					continue;
				}
				else if (lines == -1) {
					lines += 1;
				}
				else {
					lines += 1;
				}
			}
		}

		target
		.attr("x", d.x)
		.attr("y", d.y-1.5*lines+1)

		target.selectAll("tspan")
		.attr("x", d.x)

		return lines;
	}

	function linkLabelGenerator(target, labels, d) {

		var lines = -1;

		var keys = labels["__key"];

		for (var key in keys) {
			if (keys[key]) {
				if (d[key] === undefined){
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", 0.5*d._C0[0]+ 0.5*d._C1[0])
					.text(key+":"+d[key])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", 0.5*d._C0[0]+ 0.5*d._C1[0])
					.attr("dy", 3)
					.text(key+":"+d[key])

					lines += 1;
				}
			}
		}

		var attrs = labels[d.type];

		for (var attr in attrs) {
			if (attrs[attr]) {
				if (d.attr[attr] === undefined){
					continue;
				}
				else if (lines == -1) {
					target.append("tspan")
					.attr("x", 0.5*d._C0[0]+ 0.5*d._C1[0])
					.text(attr+":"+d.attr[attr])

					lines += 1;
				}
				else {
					target.append("tspan")
					.attr("x", 0.5*d._C0[0]+ 0.5*d._C1[0])
					.attr("dy", 3)
					.text(attr+":"+d.attr[attr])

					lines += 1;
				}
			}
		}

		target
		.attr("x", 0.5*d._C0[0]+ 0.5*d._C1[0])
		.attr("y", 0.5*d._C0[1]+ 0.5*d._C1[1]- 1.5*lines + 1)

		return lines;
	}

	function linkLabelBackgroundGenerator(target, d, lines) {
		// get the bounding box of the target element in svg.
		var __target = target[0][0].getBBox();

		// append a rect as background
		svg_vis_links_labels_group.insert("rect", "#"+target.attr("id"))
		.attr("rx", d.background_rx)
	    .attr("ry", d.background_ry)
	    .attr("x", __target.x - d.background_margin)
	    .attr("y", __target.y - 2 * d.background_margin)
	    .attr("id", target.attr("id")+"_background")
	    .attr("width", __target.width + 2 * d.background_margin)
	    .attr("height", __target.height + 3 * d.background_margin)
	    .style("fill", d.background_color)
	    .style("opacity", d.background_opacity)
	    .style("stroke", d.background_stroke)
	    .style('stroke-width', d.background_stroke_width);
	}

	function refreshLinkLabelBackground(target, d, lines) {
		// get the bounding box of the target element in svg
		var __target = target[0][0].getBBox();

		// refresh the link label back groubnd.
		d3.select("#"+target.attr("id")+"_background")
	    .attr("x", __target.x - d.background_margin)
	    .attr("y", __target.y - 2 * d.background_margin)
	    .attr("width", __target.width + 2 * d.background_margin)
	    .attr("height", __target.height + 3 * d.background_margin)
	    .style("fill", d.background_color)
	    .style("opacity", d.background_opacity)
	    .style("stroke", d.background_stroke)
	    .style('stroke-width', d.background_stroke_width);
	}

	function refreshLinkLabelPosition(target, labels, d) {

		var lines = -1;

		var keys = labels["__key"];

		for (var key in keys) {
			if (keys[key]) {
				if (d[key] === undefined){
					continue;
				}
				else if (lines == -1) {
					lines += 1;
				}
				else {
					lines += 1;
				}
			}
		}

		var attrs = labels[d.type];

		for (var attr in attrs) {
			if (attrs[attr]) {
				if (d.attr[attr] === undefined){
					continue;
				}
				else if (lines == -1) {
					lines += 1;
				}
				else {
					lines += 1;
				}
			}
		}

		// x coordinate should be defined for each tspan.
		// y coordinate is defined for text. tspans are using dy attribtues.
		target
		.attr("y", 0.5*d._C0[1]+ 0.5*d._C1[1]- 1.5*lines + 1)

		target.selectAll("tspan")
		.attr("x", 0.5*d._C0[0]+ 0.5*d._C1[0])

		return lines;
	}

	function refineDataName(newData) {
		// the result should be as nodes and edges.
		var result = {"nodes":[], "edges":[]};

		newData.vertices.forEach(function(n) {
			// Example of a node :
			// {"type":"newType1","id":"newKey1","attr":{"weight":"0.2","name":"newNode1newNode1newNode1"}};
			tempNode = {"type":"","id":"","attr":""};
			tempNode.type = n.type
			tempNode.id = n.id
			tempNode.attr = clone(n.attr)

			result.nodes.push(tempNode);
		}) 

		newData.edges.forEach(function(e) {
			// Example of a link :
			// {"source":{"type":"newType1","id":"newKey1"},"target":{"type":"type0","id":"key45"},"attr":{"weight":"0.01","name":"name0.7"}, "type":0};
			tempEdge = {"source":{"type":"","id":""},"target":{"type":"","id":""},"attr":""};
			tempEdge.source.type = e.src.type
			tempEdge.source.id = e.src.id
			tempEdge.target.type = e.tgt.type
			tempEdge.target.id = e.tgt.id

			// Check whether a edge includes 'type' attribute. 
			if ("type" in e) {
				tempEdge.type = e.type
			}
			else {
				tempEdge.type = "Undefined"
			}
			
			//deep copy the object of the attributes.
			tempEdge.attr = clone(e.attr)

			result.edges.push(tempEdge);
		})

		//return the refined raw graph data.
		return result
	}

	function refineIDString(name) {
		return name.replace(/ /g, '').replace(/[^a-zA-Z0-9]/g, '_')
	}

	// rotation (x,y) base on (cx,cy) for angle defee
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

	// Generate curve link points base on C0 C1.
	// update _C0 and _C1 here.
	function curveLinkPoints(link) {
		var a = 0.25;
		var b = 20; // initNodeSetting.r * 4;
		var edgeMargin = initNodeSetting.r + 2;
		var markMargin = 0;

		if (graphType == "undirected") {
			markMargin = 0;
		}
		else if (graphType == "directed") {
			markMargin = 2;
		}
		else {
			;
		}

		var C0 = [0, 0]
		var C1 = [0, 0]

		var x0 = link.source.x
		var y0 = link.source.y
		var x1 = link.target.x
		var y1 = link.target.y

		var n = 0;
		var i = -1;

		line.interpolate("basis");

		var targetKey = link.target.type + "&" + link.target.id
		var sourceKey = link.source.type + "&" + link.source.id

		var linkKeyST = targetKey + "$" + sourceKey;
		var linkKeyTS = sourceKey + "$" + targetKey;



		if (linkKeyST in link_nodes) {
			n += Object.keys(link_nodes[linkKeyST]).length;
			i = Object.keys(link_nodes[linkKeyST]).indexOf(link.type.toString())
		}

		if (linkKeyTS in link_nodes) {
			n += Object.keys(link_nodes[linkKeyTS]).length;
		} 

		if (n == 0 || i == -1) {
			return ;
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

		C0 = rotate(x0, y0, C0[0], C0[1], angle);
		C1 = rotate(x0, y0, C1[0], C1[1], angle);


		link._C0 = C0;
		link._C1 = C1;

		var tdc = Math.sqrt((C1[0]-x1)*(C1[0]-x1)+(C1[1]-y1)*(C1[1]-y1));


		var tx0 = x0 - edgeMargin * (x0 - C0[0]) / tdc
		var ty0 = y0 - edgeMargin * (y0 - C0[1]) / tdc;

		var tx1 = x1 - (edgeMargin+markMargin) * (x1 - C1[0]) / tdc
		var ty1 = y1 - (edgeMargin+markMargin) * (y1 - C1[1]) / tdc;

		if (tdc < edgeMargin) {

			if (td < edgeMargin) {
				tx0 = x0;
				ty0 = y1;
				tx1 = x1;
				ty1 = y1;
			}

			C1[0] = x1 * 0.5 + x0 *0.5;
			C1[1] = y1 * 0.5 + y0 * 0.5;
			C0 = C1;
		}

		return line([[tx0, ty0], C0, C1, [tx1, ty1]]);
	}

	return gsqlv;
}

this.gsqlv = gsqlv;
}()






