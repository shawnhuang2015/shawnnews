!function() {
	console.log("<== where the code starts.")
	this.messageArray = [];  // Record message from backend.
	this.scaleAbilityData = [];  // Record the result for scalability testing.

	window.v_width = $(window).width() / 1.1; // Setting the width of the visualization.
	window.v_height = $(window).height()- 320;// Setting the height of the visualization.


	//<div class="container-fluid" width = v_width>
	$('#tapView').width(v_width*1.05);  // Setting the width of the tap for graph, JSON, summary, and more;

	// Create the graph visualization object.
	var mygv = new gsqlv("mygv");

	// The graph visualization initialization setting.
	var setting = {'divID':'prototype1', 'width':v_width, 'height': v_height};

	// LabelFiltering initialization status;
	var filteringStatus = {node:{type:true, id:false, score:true}, edge:{type:false}}; // node:{type:boolean, attr1:boolean, attr2:boolean ...}

	window.selectionBoxLabels = mygv.label();	// Initialized the multiselection list box data object.

	// Setting the setting.
	// Then initialize the graph visualization.
	mygv
	.setting(setting)
	.init()
 
	// The configure object of the node label multiselecting list box.
    var nodeLabelFiltering = {
        maxHeight: 600,
        buttonWidth: '100%',
        enableClickableOptGroups: true,
        // Call back function once the selection changed.
        buttonText: function(options, select) {
			var labels = [];

			if (mygv.data().nodes.length != 0)
				for (var key in selectionBoxLabels.node){
					for (var item in selectionBoxLabels.node[key]) {
						selectionBoxLabels.node[key][item] = false;
					}		
				}	

			options.each(function() {			 
				var key = $(this).val();
				var type = "";

				 if ($(this).parent().is('select')) {
				 	type = "__key"
				 	key = key;
				}
				else if ($(this).parent().is('optgroup')){
					type = $(this).parent().attr("value");
					key = key.split('&')[0]
				}
				else {
					type = "";
				}

				try {
					selectionBoxLabels.node[type][key] = true;
				}
				catch (err) {
					console.log(err);
				}

				labels.push(key);			 
			});

			mygv.label(selectionBoxLabels);
			mygv.redraw();

			if (options.length === 0) {
				return 'No option selected ...';
			}
			else if (options.length > 4) {
				return 'More than 4 options selected!';
			}
			else {
				return labels.join(', ') + '';
			}
		},
			//includeSelectAllOption: true,
			//selectAllText: 'Check all!'

	}


    // The configure object of the edge label multiselecting list box.
    var edgeLabelFiltering = {
        maxHeight: 600,
        buttonWidth: '100%',
        enableClickableOptGroups: true,
        // Call back function once the selection changed
        buttonText: function(options, select) {
        	var labels = [];

			if (mygv.data().links.length != 0)
				for (var key in selectionBoxLabels.edge){
					for (var item in selectionBoxLabels.edge[key]) {
						selectionBoxLabels.edge[key][item] = false;
					}		
				}

			options.each(function() {			 
				var key = $(this).val();
				var type = "";

				 if ($(this).parent().is('select')) {
				 	type = "__key"
				 	key = key;
				}
				else if ($(this).parent().is('optgroup')){
					type = $(this).parent().attr("value");
					key = key.split('&')[0]
				}
				else {
					type = "";
				}

				try {
					selectionBoxLabels.edge[type][key] = true;
				}
				catch (err) {
					console.log(err);
				}

				labels.push(key);			 
			});

			mygv.label(selectionBoxLabels);
			mygv.redraw();

			if (options.length === 0) {
				return 'No option selected ...';
			}
			else if (options.length > 4) {
				return 'More than 4 options selected!';
			}
			else {	
				return labels.join(', ') + '';
			}
		},
		//includeSelectAllOption: true,
		//selectAllText: 'Check all!'

	}

 	// Initialize main menu, base on the message from endpoint.
	$(document).ready(function() {
		//Query the engine to get tab layout information
		$.get( "engine/UIpages", function( data ) {
			//console.log("Received engine/UIpages " + data);


			window.pages_obj = JSON.parse(data); 
			//sort on index
			window.pages_obj.sort(function(a,b) {return (a.index - b.index);});
			//insert pages
			for (var i=0; i < window.pages_obj.length; i++) {
				var tabname = window.pages_obj[i].tabname;
				var removedSpaces = removeSpaces(tabname);
				tabline = '<li><a href="#" id="' + removedSpaces  + '" name="'+removedSpaces+'_'+ i + '">' + tabname + '</a></li>';
				// changed from generateForm to generateDropdown by Chong
				window.pages_obj[removedSpaces] = '<div>' + generateDropdown(window.pages_obj[i]) + '</div>';
				//console.log(window.pages_obj[removedSpaces]);
				//console.log("Registering event for " + "#" + removedSpaces);


				// onclick on main menu will clean everything.
				$(document).on('click', '#'+removedSpaces, function(event){ 
					//console.log("click" + event.target.id);
					//console.log("form data is " + window.pages_obj[event.target.id]);
					$('#content-page').empty();
					$('#content-page').append(window.pages_obj[event.target.id]);

					UIObject = window.pages_obj[event.target.name.split("_")[1]]
					
					// Clean graph visualization object.
					// initialize layout
					// setting pre definition of the graph visualization.
					mygv.clean()
					mygv.layout(UIObject.setting.layout)
					mygv.graphType(UIObject.setting.graphType)
					mygv.preDefinition(UIObject.initialization)

					$('#layoutType').val(UIObject.setting.layout);

					//window.selectionBoxLabels = {node:{"__key":{'type':filteringStatus.node.type, 'id':filteringStatus.node.id}}, edge:{"__key":{'type':filteringStatus.edge.type, 'id':filteringStatus.edge.id}} }
					window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
												edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }
					
					parent = $('#node_label_filtering').parent();
					$('#node_label_filtering').next().remove();
					$('#node_label_filtering').remove();

					parent.append('<select id="node_label_filtering" multiple="multiple">')	
					$('#node_label_filtering').multiselect(nodeLabelFiltering);

					parent = $('#edge_label_filtering').parent();
					$('#edge_label_filtering').next().remove();
					$('#edge_label_filtering').remove();

					parent.append('<select id="edge_label_filtering" multiple="multiple">')	
					$('#edge_label_filtering').multiselect(edgeLabelFiltering);
				});

				// the first item of main menu is activated by default.
				if (i == 0) {
					//Index 0 is activated
					$('#content-page').empty();
					$('#content-page').append(window.pages_obj[removedSpaces]);

					UIObject = window.pages_obj[i]
					
					mygv.clean()
					mygv.layout(UIObject.setting.layout)
					mygv.graphType(UIObject.setting.graphType)
					mygv.preDefinition(UIObject.initialization)

					//window.selectionBoxLabels = mygv.label();
					//window.selectionBoxLabels = {node:{"__key":{'type':filteringStatus.node.type, 'id':filteringStatus.node.id}}, edge:{"__key":{'type':filteringStatus.edge.type, 'id':filteringStatus.edge.id}} }
					window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
												edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }
					
					$('#layoutType').val(UIObject.setting.layout);
				}

				//console.log(tabline);
				$("#navbar ul").append(tabline); 				
			}

			
			// added the key press event of the selection condition.
			$('#selectionCondition').keypress(function(e){
				// enter key is pressed
				if(e.keyCode==13) {
					selectingByConditions();
				}
			});
			

			$('#content-page').append(window.pages_obj["ExploreNeighborhood"]);

			// stroe the JSON message.
			// out the json message in the JSON tab.
			// out the summary information of graph, and selected sub graph in the summary tab.
			messageArray.push(window.pages_obj)
			JSONMessageOutput();
			summaryInformationOutput();

			// added by Chong
			$(function () {
				var select = $('#idSel, #typeSel');
				select.change(function () {
					select.not(this).val(this.value);
				});
			});

		});
		
		//*******************
		//initilize graph visualization menu

        //initilize the color picker.
        // set the colorchange event for changing the color of selected nodes and edges.
        $('#colorpicker').colorpicker({
            color: "#1f77b4"  //bodyStyle.backgroundColor
        }).on('changeColor', function(ev) {
            //bodyStyle.backgroundColor = ev.color.toHex();
            mygv.coloringNodes(ev.color.toHex())
            mygv.coloringEdges(ev.color.toHex())
        });

        // initilize the multi selection list box for displaying labels of nodes and edges.
        $('#node_label_filtering').multiselect(nodeLabelFiltering);
        $('#edge_label_filtering').multiselect(edgeLabelFiltering);


    });

	// remove space function.
	this.removeSpaces = function(s) {
		return s.replace(/\s/g, '');
	}

	// Generate the data object for creating the main menu.
	// Specifiy the onclick event for submit button as onclick_submit(index).
	this.generateForm = function(pagesObject){

		elements = pagesObject.elements;
		URL = pagesObject.URL

		var s = "";
		for (var i=0; i < elements.length; i++){
			if (elements[i]["label"]){
				s = s + elements[i]["label"]["name"];
			}
			if (elements[i]["textbox"]){
				s = s + '    <input type ="text" name="' + elements[i]["textbox"]["name"] + '" size=' + elements[i]["textbox"]["length"] + '>    ';
			}
		}
		s += '   <input type = "submit" value = "Submit"'

		s += 'onclick = onclick_submit('+ pagesObject.index + ')';
		s +='>  '; 
		return s; 
	}

	// generate Select tag
	// NOTE: This is just a temporary function for fitting the js
	// I would not generate HTML representation with JavaScript

	// @author Chong
	var tmpSelItm = [['2015031032321674', 'TXN'], ['2015031096207853', 'USERID'], ['2015031032327031', 'TXN'], ['2015031032326233', 'TXN']];
	this.generateDropdown = function(pagesObject){

		elements = pagesObject.elements;
		URL = pagesObject.URL

		var tmp = "<label for='idSel'>ID</label>" +
					"<select id='idSel'>" +
					  "<option value='TXN'>" + tmpSelItm[0][0] + "</option> " +
					  "<option value='USERID'>" + tmpSelItm[1][0] + "</option> " +
					  "<option value='TXN'>" + tmpSelItm[2][0] + "</option> " +
					  "<option value='TXN'>" + tmpSelItm[3][0] + "</option> " +
					"</select>" +
					" <label for='typeSel'>Type</label> " +
					" <select id='typeSel'> " +
					  "<option value='TXN'>TXN</option> " +
					  "<option value='USERID'>USERID</option> " +

					"</select>" ;
		tmp += '   <input type = "submit" value = "Submit"'

		tmp += 'onclick = onclick_submit_dropdown('+ pagesObject.index + ')';
		tmp +='>  ';
		return tmp;
	}

	// Not in use.
	this.labelFilter = function() {
		// get the label string from multiselection list box.
		var node_label_string = document.getElementById("node_label").value;
		var edge_label_string = document.getElementById("edge_label").value;

		// parse the label array from label string.
		var node_label_array = node_label_string.split(';').map(function(d){return d.trim();})
		var edge_label_array = edge_label_string.split(';').map(function(d){return d.trim();})

		// Setting the label array for node label displaying and edge label displaying.
		mygv.nodeLabel(node_label_array);
		mygv.edgeLabel(edge_label_array);

		// redraw everyting on the main stage
		mygv.redraw();
	}	

	// Not in use.
	this.selectingNodesByConditions = function() {
		//get the condition string from the input box for node.
		var conditionString = $("#temp_node_conditions").val();

		// setting the condition string to the gsqlv.
		// doing the selecting nodes by condition string.
		mygv.selectingNodesByConditions(conditionString);
	}

	// Not in use.
	this.selectingEdgesByConditions = function() {

		//get the condition string from the input box for edge
		var conditionString = $("#temp_edge_conditions").val();

		//setting the condition string to the gsqlv.
		// doing the selecting edges by condition string.
		mygv.selectingEdgesByConditions(conditionString);
	}

	// Newest version for selecting by conditions
	this.selectingByConditions = function() {

		// get the condition string from input box.
		// get the condition type from the list box.
		var conditionString = $("#selectionCondition").val();
		var conditionType = $("#selectionType").val();

		// Unselect everything in the graph visualization.
		mygv.unselectingEverything();

		// doing selecting base on condition type and condition string.
		if (conditionType == "nodes") {
			mygv.selectingNodesByConditions(conditionString);
		}
		else if (conditionType == "edges") {
			mygv.selectingEdgesByConditions(conditionString);
		}
		
	}

	// call back function for the layout list box.
	this.layoutChanged = function() {

		// get the new layout name.
		var layoutType = $("#layoutType").val();
		
		// set the new layout for gsqlv
		// start the layout process
		// refresh as animation in 500 milliseconds.

		mygv
		.layout(layoutType)
		.startLayout()
		.refreshAnimation(500)

		// set to center view after 500 milliseconds.
		setTimeout(
		function() {
			mygv.center_view();
		}, 500)
	}

	// call back function for set root node button.
	this.setTheRootNode = function() {

		// set the selected node as the root node.
		mygv.setSelectedNodeToBerootNode();

		// redo the start layout.
		layoutChanged();
	}

	// call back function for the highlight button
	this.highlightingNodes = function() {

		// check whether or not there is a node has been selected.
		// if it is true, change the label of the button as unhighlighting.
		// else change the label of the button as highlighting.
		if(mygv.highlightingNodes()) {
			$('#b_highlightingNodes').html('unhighlighting');	
		}
		else {
			$('#b_highlightingNodes').html('highlighting');
		}
	}

	// call back function for hiding nodes.
	// hide the selected node from the graph.
	this.hidingNodes = function() {
		mygv.hidingNodes();
	}

	// coloring the nodes.
	// get color from color picker
	// set color for selected nodes and corelated edges.
	// then unselected everything.
	this.coloringNodes = function() {		
		var highlightingColor = document.getElementById("highlightingColorPicker").value.toLowerCase()
		mygv.coloringNodes(highlightingColor);
		mygv.unselectingEverything();
	}

	// call back function for onchange event of the color picker.
	// change the color of the selected ndoes and edges on the fly.
	this.coloringNodesOnChange = function() {
		var highlightingColor = document.getElementById("highlightingColorPicker").value.toLowerCase()
		mygv.coloringNodes(highlightingColor);
	}

	// use for update the label of the multi selection list box for the labels displaying of nodes and edges.
	// Input : graph data.
	//		1. Store the previous status of the selection.
	//  	2. Parsing the graph data, get what attributes each type has.
	//		3. Recreate the mutli selection list box base on types, attribtues, and status.
	this.updateLabelFilteringListBox = function(d) {
		// if graph data is empty return.
		if (Object.keys(d).length ===0) {
			return;
		}


		// if there is not a node in graph return;
		if (d.nodes.length === 0) {
			return;
		}

		// store the previous status of the selectionBoxLabels. The status is like whether a label is selected or not.
		nodelabels = selectionBoxLabels.node;

		// initilize the selectionBoxLabels of node.
		// "__key" is the type for "type" and "id", which is the key of a node and edge.
		selectionBoxLabels.node = {"__key":{}};

		// for each nodes 'type','id' are default for every nodes.
		// Then get what attribtues each type of node has.
		d.nodes.forEach(function(n){
			['type', 'id'].forEach(function(d, i) {
				if (d in n) {
					if (d in nodelabels["__key"]) {
						selectionBoxLabels.node["__key"][d] = nodelabels["__key"][d];
					}
					else {
						selectionBoxLabels.node["__key"][d] = false;
					}
				}
				else {
					;
				}
			})

			if (n.type in selectionBoxLabels.node) {
				;
			}
			else {
				selectionBoxLabels.node[n.type] = {};
			}

			for (var key in n.attr) {
				try {
					if (key in nodelabels[n.type]) {
						selectionBoxLabels.node[n.type][key] = nodelabels[n.type][key];
					}
					else {
						selectionBoxLabels.node[n.type][key] = (typeof filteringStatus.node[key] == 'undefined') ? false : filteringStatus.node[key];
					}
				}
				catch (err) {
					//selectionBoxLabels.node[n.type][key] = false;
					selectionBoxLabels.node[n.type][key] = (typeof filteringStatus.node[key] == 'undefined') ? false : filteringStatus.node[key];
				}		
			}
		})

		// if there is not a links in the graph, skip the labels generation process for links
		if (d.links.length === 0) {
			;
		}

		// store the previous status of the selectionBoxLabels of edge. The status is like whether a label is selected or not.
		edgelabels = selectionBoxLabels.edge;
		selectionBoxLabels.edge = {"__key":{}};

		// initilize the selectionBoxLabels of edge.
		// "__key" is the type for "type" and "id", which is the key of a edge.
		// for each nodes 'type','id' are default for every edges.
		// Then get what attribtues each type of edge has.
		d.links.forEach(function(n){
			['type', 'id'].forEach(function(d, i) {
				if (d in n) {
					if (d in edgelabels["__key"]) {
						selectionBoxLabels.edge["__key"][d] = edgelabels["__key"][d];
					}
					else {
						selectionBoxLabels.edge["__key"][d] = false;
					}
				}
				else {
					;
				}
			})

			if (n.type in selectionBoxLabels.edge) {
				;
			}
			else {
				selectionBoxLabels.edge[n.type] = {};
			}

			for (var key in n.attr) {
				try {
					if (key in edgelabels[n.type]) {
						selectionBoxLabels.edge[n.type][key] = edgelabels[n.type][key];
					}
					else {
						selectionBoxLabels.edge[n.type][key] = false;
					}
				}
				catch (err) {
					//selectionBoxLabels.edge[n.type][key] = false;
					selectionBoxLabels.edge[n.type][key] = (typeof filteringStatus.edge[key] == 'undefined') ? false : filteringStatus.edge[key];
				}		
			}
		})

		// update node label list box.
		// 1. Get the multi-selection list box container (.parent());
		// 2. Remove the next sibling of multi-selection list box.
		// 3. Remove the multi-selection list box itself.
		// 4. Append a new multi-selection list box.
		// 5. Append keys in the begin of the list box.
		// 6. Append attribute for each type in the list box.
		// 7. call the multi-selection box initialization.

		//1.2.3
		parent = $('#node_label_filtering').parent();
		$('#node_label_filtering').next().remove();
		$('#node_label_filtering').remove();

		//4.
		parent.append('<select id="node_label_filtering" multiple="multiple">')


		//5.
		for (var key in selectionBoxLabels.node.__key) {
			var tempOptionHTML = '<option value="' + key + '"';
			if (selectionBoxLabels.node.__key[key]) {
				tempOptionHTML += 'selected="selected">'
			}
			else {
				tempOptionHTML += '>'
			}

			tempOptionHTML += key + '</option>'
			$('#node_label_filtering').append(tempOptionHTML)
		} 

		//6.
		for (var type in selectionBoxLabels.node) {

			var groupList;

			if (type == "__key") {
				continue;
			} 
			else {
				groupList = $('<optgroup value="'+type+'" label="Type ' + type + '"></optgroup>')
				$('#node_label_filtering').append(groupList);
			}

			for (var key in selectionBoxLabels.node[type]) {
				var tempOptionHTML = '<option value="' + key +'&'+type + '"';
				if (selectionBoxLabels.node[type][key]) {
					tempOptionHTML += 'selected="selected">'
				}
				else {
					tempOptionHTML += '>'
				}

				tempOptionHTML += key + '</option>'
				groupList.append(tempOptionHTML)
			}	


		}

		//7.
        $('#node_label_filtering').multiselect(nodeLabelFiltering);

		// update edge label list box.
		// 1. Get the multi-selection list box container (.parent());
		// 2. Remove the next sibling of multi-selection list box.
		// 3. Remove the multi-selection list box itself.
		// 4. Append a new multi-selection list box.
		// 5. Append keys in the begin of the list box.
		// 6. Append attribute for each type in the list box.
		// 7. call the multi-selection box initialization.

		//1.2.3
		parent = $('#edge_label_filtering').parent();
		$('#edge_label_filtering').next().remove();
		$('#edge_label_filtering').remove();

		//4.
		parent.append('<select id="edge_label_filtering" multiple="multiple">')	

		//5.
		for (var key in selectionBoxLabels.edge.__key) {
			var tempOptionHTML = '<option value="' + key + '"';
			if (selectionBoxLabels.edge.__key[key]) {
				tempOptionHTML += 'selected="selected">'
			}
			else {
				tempOptionHTML += '>'
			}

			tempOptionHTML += key + '</option>'
			$('#edge_label_filtering').append(tempOptionHTML)
		} 

		//6.
		for (var type in selectionBoxLabels.edge) {

			var groupList;

			if (type == "__key") {
				continue;
			} 
			else {
				groupList = $('<optgroup value="'+type+'" label="Type ' + type + '"></optgroup>')
				$('#edge_label_filtering').append(groupList);
			}

			for (var key in selectionBoxLabels.edge[type]) {
				var tempOptionHTML = '<option value="' + key +'&'+type + '"';
				if (selectionBoxLabels.edge[type][key]) {
					tempOptionHTML += 'selected="selected">'
				}
				else {
					tempOptionHTML += '>'
				}

				tempOptionHTML += key + '</option>'
				groupList.append(tempOptionHTML)
			}		
		}

		//7.
        $('#edge_label_filtering').multiselect(edgeLabelFiltering);
	}

	// call back function for the submit button. 
	// Input : index of the menu in of the main menu.
	//	1. Get the pages object base on the index.
	//	2. Parse the pages object, and generate the URL for submit button.
	//	3. Parse the pages object, and set the URL for visualization by calling setURL().
	//	4. Call back function for the rest request of submit button.
	this.onclick_submit = function(index) {
		var myObject = window.pages_obj[index]

		// Initilize the multi-selection box label for new coming data.
		window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
		edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }

		// for each events create coresponding URL.
		for (var key in myObject.events) {
			if (key == "submit") {
				// setting URL for submit button.
				temp_event = myObject.events[key]; 
				submit_URL = temp_event.URL_head + "?"
				URL_attrs =  temp_event.URL_attrs

				/*
				if ("id" in URL_attrs) {
					submit_URL += "/" + document.getElementsByName(URL_attrs.id.name)[0].value + "?";
				}
				else {
					submit_URL += "?";
				}
				*/
				
				for (var attr in URL_attrs) {
					name = attr;
					attr = URL_attrs[attr];

					//if (name == "id") continue;

					if (attr.usage == "input") {
						if (document.getElementsByName(attr.name)[0].value=="") {
							;	
						}
						else{
							if (name == "type") {
								submit_URL += name + "=" + document.getElementsByName(attr.name)[0].value.toLowerCase() +"&";
							}
							else {
								submit_URL += name + "=" + document.getElementsByName(attr.name)[0].value/*.toLowerCase()*/ +"&";
							}
							
						} 
						//submit_URL += name + "=" + (document.getElementsByName(attr.name)[0].value==""?1:document.getElementsByName(attr.name)[0].value) +"&";
					}
					else if (attr.usage == "attributes") {
						submit_URL += name+ "=" + myObject.attributes[attr.name] + "&";
					}
				}

				// initilize the root node as the query node.
				//	a. create rootNode id.
				//	b. set root node by id.
				//  c. rootNode is = Type + "&" + ID;
				var rootNodeType;
				var rootNodeID;

				// Get type from Input box. Sometimes, we don't use type as input for query.
				// But type is another key for the retrieve the node in graph. Default is '0';
				if ("type" in URL_attrs) {
					//rootNodeType = document.getElementsByName(URL_attrs.type.name)[0].value;

					// type default is txn;
					rootNodeType = document.getElementsByName(URL_attrs.type.name)[0].value==""?"txn":document.getElementsByName(URL_attrs.type.name)[0].value;
				}
				else {
					rootNodeType = "0"
				}

				// Get id from Inputbox. The 'id' usually is used as input for query.
				// Default is '0'
				if ("id" in URL_attrs) {
					rootNodeID = document.getElementsByName(URL_attrs.id.name)[0].value;
				}
				else {
					rootNodeID = "0";
				}

				var initRootNode = rootNodeType + "&" + rootNodeID;
				/*document.getElementsByName(URL_attrs.type.name)[0].value +
						 "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
						 */
				mygv.rootNode(initRootNode);
			}
			else {

				// setting URL for other events, such as double left click on nodes.
				mygv.setURL(myObject, key);
			}
		}

		// call back function for the rest query of the submit button.
		$.get(submit_URL, function(message) {
			// JSON parse the message string.
			try {
				message = JSON.parse(message);
			}
			catch (err){
				console.log("REST query result is not a vaild JSON string")
				return ;
			}
			
			if (!message.error) {
				// get the json object of the result.
				newData = message.results;

				// initilize the graph visualization by using the new data.
				// run the visualization.
				mygv
				.data(newData)
				.run()

				// update the multi-selection lable box base on the graph data.
				updateLabelFilteringListBox(mygv.data());

				// store the new message in the messageArray.
				messageArray.push(message);

				// out put the JSON message in the json tab.
				JSONMessageOutput()
			}
			else {

				// clean everything of the graph visualization.
				mygv.clean();
			}
		})
		//console.log(index)
	}

	// for Dropdown list
	// by Chong
	this.onclick_submit_dropdown = function(index) {
		var myObject = window.pages_obj[index]

		// Initilize the multi-selection box label for new coming data.
		window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}},
		edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }

		// for each events create coresponding URL.
		for (var key in myObject.events) {
			if (key == "submit") {
				// setting URL for submit button.
				temp_event = myObject.events[key];
				submit_URL = temp_event.URL_head + "?"
				URL_attrs =  temp_event.URL_attrs

				/*
				if ("id" in URL_attrs) {
					submit_URL += "/" + document.getElementsByName(URL_attrs.id.name)[0].value + "?";
				}
				else {
					submit_URL += "?";
				}
				*/

				for (var attr in URL_attrs) {
					name = attr;
					attr = URL_attrs[attr];

					// submit_URL += "id=" + $('#typeSel option:selected').text() + "&type" + $('#typeSel option:selected').text().toLowerCase();
					//if (name == "id") continue;

					if (attr.usage == "input") {

							if (name == "type") {
								submit_URL += name + "=" + $('#typeSel option:selected').text().toLowerCase() +"&";
							}
							else {
								submit_URL += name + "=" + $('#idSel option:selected').text()/*.toLowerCase()*/ +"&";
							}

						//submit_URL += name + "=" + (document.getElementsByName(attr.name)[0].value==""?1:document.getElementsByName(attr.name)[0].value) +"&";
					}

				}

				// initilize the root node as the query node.
				//	a. create rootNode id.
				//	b. set root node by id.
				//  c. rootNode is = Type + "&" + ID;
				var rootNodeType;
				var rootNodeID;

				// Get type from Input box. Sometimes, we don't use type as input for query.
				// But type is another key for the retrieve the node in grap              h. Default is '0';
				if ("type" in URL_attrs) {

					// type default is txn;
					//rootNodeType = document.getElementsByName(URL_attrs.type.name)[0].value==""?"txn":document.getElementsByName(URL_attrs.type.name)[0].value;
					rootNodeType = $('#typeSel option:selected').text();
				}
				else {
					rootNodeType = "0"
				}

				// Get id from Inputbox. The 'id' usually is used as input for query.
				// Default is '0'
				if ("id" in URL_attrs) {
					rootNodeID = $('#idSel option:selected').text();
				}
				else {
					rootNodeID = "0";
				}

				var initRootNode = rootNodeType + "&" + rootNodeID;
				/*document.getElementsByName(URL_attrs.type.name)[0].value +
						 "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
						 */
				mygv.rootNode(initRootNode);
			}
			else {

				// setting URL for other events, such as double left click on nodes.
				mygv.setURL(myObject, key);
			}
		}

		// call back function for the rest query of the submit button.
		$.get(submit_URL, function(message) {
			// JSON parse the message string.
			try {
				message = JSON.parse(message);
			}
			catch (err){
				console.log("REST query result is not a vaild JSON string")
				return ;
			}

			if (!message.error) {
				// get the json object of the result.
				newData = message.results;

				// initilize the graph visualization by using the new data.
				// run the visualization.
				mygv
				.data(newData)
				.run()

				// update the multi-selection lable box base on the graph data.
				updateLabelFilteringListBox(mygv.data());

				// store the new message in the messageArray.
				messageArray.push(message);

				// out put the JSON message in the json tab.
				JSONMessageOutput()
			}
			else {

				// clean everything of the graph visualization.
				mygv.clean();
			}
		})
		//console.log(index)
	}

	// use for syntax high lighting for the JSON object.
	// In the <pre> tage, <span> tage with class will be use for the syntax highlighting.
	this.syntaxHighlight = function (json) {
	    if (typeof json != 'string') {
	         json = JSON.stringify(json, undefined, 2);
	    }
	    json = json.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
	    return json.replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g, function (match) {
	        var cls = 'number';
	        if (/^"/.test(match)) {
	            if (/:$/.test(match)) {
	                cls = 'key';
	            } else {
	                cls = 'string';
	            }
	        } else if (/true|false/.test(match)) {
	            cls = 'boolean';
	        } else if (/null/.test(match)) {
	            cls = 'null';
	        }
	        return '<span class="' + cls + '">' + match + '</span>'; // style="font-family: Times"
	    });
	}

	// out put the json message in the json tab, of which id is 'messageBox'.
	this.JSONMessageOutput = function() {


		// remove the previous json message 
		d3.select("#messageBox").selectAll("*").remove();

		d3.select("#messageBox")
		.style("width", v_width + "px ")
		.style("height", v_height+"px")

		d3.select("#messageBox")
		.html("<span> JSON Message " + messageArray.length + " :</span> \n" + syntaxHighlight(messageArray[messageArray.length-1]))
	}

	// out out the summary information for both whole graph and selected sub graph.
	this.summaryInformationOutput = function () {
		// remove previous summary information display.
		d3.select("#summaryOfAllNodes").selectAll("*").remove();
		d3.select("#summaryOfSelectedNodes").selectAll("*").remove();

		// output the summary information for the whole graph.
		d3.select("#summaryOfAllNodes")
		.style("width", v_width + "px ")
		.style("height", v_height/2.02+"px")
		.html(mygv.summaryInformationForAllNodes())

		// output the summary information for the selected sub graph.
		d3.select("#summaryOfSelectedNodes")
		.style("width", v_width + "px ")
		.style("height", v_height/2.02+"px")
		.html(mygv.summaryInformationForSelectedNodes())
	}

	// Just output the summary information for the selected sub graph.
	this.updateSummaryInformationForSelectedNodes = function() {

		// remove previous summary information.
		d3.select("#summaryOfSelectedNodes").selectAll("*").remove();

		// out the new summary information.
		d3.select("#summaryOfSelectedNodes")
		.style("height", v_height/2.02+"px")
		.html(mygv.summaryInformationForSelectedNodes())
	}

	this.checkUndefinedForBool = function(x) {
		if (typeof x == 'undefined') {
			return false;
		}
		else {
			return x;
		}
	}

	//refer the graph visualization object.
	window.mygv = mygv;
	console.log("<== where the code ends.")
}()