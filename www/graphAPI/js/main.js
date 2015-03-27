!function() {
	console.log("<== where the code starts.")

	window.v_width = $(window).width() - 20//1024;
	window.v_height = $(window).height()-255//660;

	var mygv = new gsqlv("mygv");
	var setting = {'divID':'prototype1', 'width':v_width, 'height': v_height};

	window.selectionBoxLabels = {"node":{}, "edge":{}}	// each node type, selected

	mygv
	.setting(setting)
	.init()
 

    var nodeLabelFiltering = {
        onChange: function(option, checked, select) {
            //if (checked)
            //  alert('Changed option ' + $(option).val() + '. is checked');
            //else
            // alert('Changed option ' + $(option).val() + '. is unchecked');
        },
        maxHeight: 200,
        buttonWidth: '100%',
        buttonText: function(options, select) {
			var labels = [];
			for (var key in selectionBoxLabels.node){
				selectionBoxLabels.node[key].selected = false;
			}

			options.each(function() {
			 if ($(this).attr('label') !== undefined) {
			     labels.push($(this).attr('label'));
			     selectionBoxLabels.node[$(this).attr('label')].selected = true;
			 }
			 else {
			     labels.push($(this).html());
			     selectionBoxLabels.node[$(this).html()].selected = true;
			 }
			});

			mygv.nodeLabel(labels);
			mygv.redraw();



            if (options.length === 0) {
                return 'No option selected ...';
            }
            else if (options.length > 5) {
                return 'More than 5 options selected!';
            }
             else {
                 return labels.join(', ') + '';
             }
        },
        //includeSelectAllOption: true,
        //selectAllText: 'Check all!'

    }

    var edgeLabelFiltering = {
        maxHeight: 200,
        buttonWidth: '100%',
        buttonText: function(options, select) {
        	var labels = [];

			for (var key in selectionBoxLabels.edge){
				selectionBoxLabels.edge[key].selected = false;
			}

			options.each(function() {
			 if ($(this).attr('label') !== undefined) {
			     labels.push($(this).attr('label'));
			     selectionBoxLabels.edge[$(this).attr('label')].selected = true;
			 }
			 else {
			     labels.push($(this).html());
			     selectionBoxLabels.edge[$(this).html()].selected = true;
			 }
			});

			mygv.edgeLabel(labels);
			mygv.redraw();

            if (options.length === 0) {
                return 'No option selected ...';
            }
            else if (options.length > 5) {
                return 'More than 5 options selected!';
            }
             else {
      
                 return labels.join(', ') + '';
             }
        },
        //includeSelectAllOption: true,
        //selectAllText: 'Check all!'

    }
 	// Initialize main menu
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
				window.pages_obj[removedSpaces] = '<div>' + generateForm(window.pages_obj[i]) + '</div>';
				//console.log(window.pages_obj[removedSpaces]);
				//console.log("Registering event for " + "#" + removedSpaces);

				$(document).on('click', '#'+removedSpaces, function(event){ 
					//console.log("click" + event.target.id);
					//console.log("form data is " + window.pages_obj[event.target.id]);
					$('#content-page').empty();
					$('#content-page').append(window.pages_obj[event.target.id]);

					UIObject = window.pages_obj[event.target.name.split("_")[1]]
					
					mygv.clean()
					mygv.layout(UIObject.setting.layout)
					mygv.preDefinition(UIObject.initialization)

					$('#layoutType').val(UIObject.setting.layout);

					window.selectionBoxLabels = {"node":{}, "edge":{}}

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

				if (i == 0) {
					//Index 0 is activated
					$('#content-page').empty();
					$('#content-page').append(window.pages_obj[removedSpaces]);

					UIObject = window.pages_obj[i]
					
					mygv.clean()
					mygv.layout(UIObject.setting.layout)
					mygv.preDefinition(UIObject.initialization)

					$('#layoutType').val(UIObject.setting.layout);
				}

				//console.log(tabline);
				$("#navbar ul").append(tabline); 				
			}



			$('#content-page').append(window.pages_obj["ExploreNeighborhood"]);

		});
		
		//*******************
		//initilize graph visualization menu

        //var bodyStyle = $('body')[0].style;
        $('#colorpicker').colorpicker({
            color: "#ff0000"  //bodyStyle.backgroundColor
        }).on('changeColor', function(ev) {
            //bodyStyle.backgroundColor = ev.color.toHex();
            mygv.coloringNodes(ev.color.toHex())
        });


        $('#node_label_filtering').multiselect(nodeLabelFiltering);
        $('#edge_label_filtering').multiselect(edgeLabelFiltering);
    });


	//.layout("force")	

	this.removeSpaces = function(s) {
		return s.replace(/\s/g, '');
	}

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

	this.labelFilter = function() {
		var node_label_string = document.getElementById("node_label").value;
		var edge_label_string = document.getElementById("edge_label").value;

		var node_label_array = node_label_string.split(';').map(function(d){return d.trim();})
		var edge_label_array = edge_label_string.split(';').map(function(d){return d.trim();})

		mygv.nodeLabel(node_label_array);
		mygv.edgeLabel(edge_label_array);
		mygv.redraw();
	}	

	this.selectingNodesByConditions = function() {
		var conditionString = $("#temp_node_conditions").val();
		mygv.selectingNodesByConditions(conditionString);
	}

	this.selectingEdgesByConditions = function() {
		var conditionString = $("#temp_edge_conditions").val();
		mygv.selectingEdgesByConditions(conditionString);
	}

	this.selectingByConditions = function() {
		var conditionString = $("#selectionCriterion").val();
		var conditionType = $("#selectionType").val();

		mygv.unselectingEverything();

		if (conditionType == "nodes") {
			mygv.selectingNodesByConditions(conditionString);
		}
		else if (conditionType == "edges") {
			mygv.selectingEdgesByConditions(conditionString);
		}
	}

	this.layoutChanged = function() {
		var layoutType = $("#layoutType").val();
		//mygv.layout(layoutType).run().center_view();
		mygv
		.layout(layoutType)
		.startLayout()
		.refreshAnimation(500)

		setTimeout(
		function() {
			mygv.center_view();
		}, 500)
	}

	this.highlightingNodes = function() {
		mygv.highlightingNodes();
	}

	this.hidingNodes = function() {
		mygv.hidingNodes();
	}

	this.coloringNodes = function() {		
		var highlightingColor = document.getElementById("highlightingColorPicker").value.toLowerCase()
		mygv.coloringNodes(highlightingColor);
		mygv.unselectingEverything();
	}

	this.coloringNodesOnChange = function() {
		var highlightingColor = document.getElementById("highlightingColorPicker").value.toLowerCase()
		mygv.coloringNodes(highlightingColor);
	}

	this.updateLabelFilteringListBox = function(d) {
		//console.log("updatelabelFilteringListBox.")

		// generate selectionBoxLabels
		nodelabels = selectionBoxLabels.node;
		selectionBoxLabels.node = {};

		d.vertices.forEach(function(n){

			if ('type' in n) {
				if ('type' in nodelabels) {
					selectionBoxLabels.node.type = nodelabels.type;
				}
				else {
					selectionBoxLabels.node.type = {"type":"key", "selected":true}
				}
			}
			if ('id' in n) {
				if ('id' in nodelabels) {
					selectionBoxLabels.node.id = nodelabels.id;
				}
				else {
					selectionBoxLabels.node.id = {"type":"key", "selected":true}
				}
			}

			for (var key in n.attr) {
				if (key in nodelabels) {
					selectionBoxLabels.node[key] = nodelabels[key];
				}
				else {
					selectionBoxLabels.node[key] = {"type":"attr", "selected":false}
				}
			}
		})

		edgelabels = selectionBoxLabels.edge;
		selectionBoxLabels.edge = {};

		d.Edges.forEach(function(n){

			if ('type' in n) {
				if ('type' in edgelabels) {
					selectionBoxLabels.edge.type = edgelabels.type;
				}
				else {
					selectionBoxLabels.edge.type = {"type":"key", "selected":true}
				}
			}
			if ('id' in n) {
				if ('id' in edgelabels) {
					selectionBoxLabels.edge.id = edgelabels.id;
				}
				else {
					selectionBoxLabels.edge.id = {"type":"key", "selected":true}
				}
			}

			for (var key in n.attr) {
				if (key in edgelabels) {
					selectionBoxLabels.edge[key] = edgelabels[key];
				}
				else {
					selectionBoxLabels.edge[key] = {"type":"attr", "selected":true}
				}
			}
		})


		// update node label list box.
		parent = $('#node_label_filtering').parent();
		$('#node_label_filtering').next().remove();
		$('#node_label_filtering').remove();

		parent.append('<select id="node_label_filtering" multiple="multiple">')	

		for (var key in selectionBoxLabels.node) {
			var tempOptionHTML = '<option value="' + key + '"';
			if (selectionBoxLabels.node[key].selected) {
				tempOptionHTML += 'selected="selected">'
			}
			else {
				tempOptionHTML += '>'
			}

			tempOptionHTML += key + '</option>'

			$('#node_label_filtering').append(tempOptionHTML)
		}

		/*
		$('#node_label_filtering').append('<option value="type" selected="selected">type</option>')
		$('#node_label_filtering').append('<option value="id" selected="selected">id</option>')
		$('#node_label_filtering').append('<option value="some_int" selected="selected">some_int</option>')
		$('#node_label_filtering').append('<option value="some_float" selected="selected">some_float</option>')
		*/

        $('#node_label_filtering').multiselect(nodeLabelFiltering);

        //Edge labels

		parent = $('#edge_label_filtering').parent();
		$('#edge_label_filtering').next().remove();
		$('#edge_label_filtering').remove();

		parent.append('<select id="edge_label_filtering" multiple="multiple">')	

		for (var key in selectionBoxLabels.edge) {
			var tempOptionHTML = '<option value="' + key + '"';
			if (selectionBoxLabels.edge[key].selected) {
				tempOptionHTML += 'selected="selected">'
			}
			else {
				tempOptionHTML += '>'
			}

			tempOptionHTML += key + '</option>'

			$('#edge_label_filtering').append(tempOptionHTML)
		}

        $('#edge_label_filtering').multiselect(edgeLabelFiltering);
	}

	this.onclick_submit = function(index) {
		var myObject = window.pages_obj[index]

		if (index == 0) {
			//http://uitest.graphsql.com:8080/engine/kneighborhood_full_type?id*primeryKey&type*type&depth@depth

			for (var key in myObject.events) {
				if (key == "submit") {
					temp_event = myObject.events[key]; 
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
					}

					var initRootNode = //document.getElementsByName(URL_attrs.type.name)[0].value +
							 "0&" + document.getElementsByName(URL_attrs.id.name)[0].value;
					mygv.rootNode(initRootNode);
				}
				else {
					mygv.setURL(myObject, key);
				}
			}

			//URL += URL_attrs[0] + "=" + document.getElementsByName(myObject.elements[3]["textbox"]["name"])[0].value +"&";
			//URL += URL_attrs[1] + "=" + document.getElementsByName(myObject.elements[1]["textbox"]["name"])[0].value +"&";
			//URL += URL_attrs[2] + "=1";


			$.get(submit_URL, function(message) {
				message = JSON.parse(message);
				if (!message.error) {
					newData = message.results;

					updateLabelFilteringListBox(newData);

					mygv
					.data(newData)
					.run()

				}
				else {
					mygv.clean();
				}
			})
		}
		else if (index == 1){
			for (var key in myObject.events) {
				if (key == "submit") {
					temp_event = myObject.events[key]; 
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

						
					}

					var initRootNode = document.getElementsByName(URL_attrs.type.name)[0].value +
									 "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
					mygv.rootNode(initRootNode);
				}
				else {
					mygv.setURL(myObject, key);
				}
			}

			$.get(submit_URL, function(message, initRootNode) {
				message = JSON.parse(message);
				if (!message.error) {
					newData = message.results;

					updateLabelFilteringListBox(newData);

					mygv
					.data(newData)
					.run()

				}
				else {
					mygv.clean();
				}
			})
		}
		else if (index == 2){
			for (var key in myObject.events) {
				if (key == "submit") {
					temp_event = myObject.events[key]; 
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
					}

					var initRootNode = document.getElementsByName(URL_attrs.type.name)[0].value +
									 "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
					mygv.rootNode(initRootNode);
				}
				else {
					mygv.setURL(myObject, key);
				}
			}

			$.get(submit_URL, function(message, initRootNode) {
				message = JSON.parse(message);
				if (!message.error) {
					newData = message.results;

					updateLabelFilteringListBox(newData);

					mygv
					.data(newData)
					.run()

				}
				else {
					mygv.clean();
				}
			})
		}
		else if (index == 3) {
			for (var key in myObject.events) {
				if (key == "submit") {
					temp_event = myObject.events[key]; 
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

						
					}

					var initRootNode = document.getElementsByName(URL_attrs.type.name)[0].value +
									 "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
					mygv.rootNode(initRootNode);
				}
				else {
					mygv.setURL(myObject, key);
				}
			}

			$.get(submit_URL, function(message, initRootNode) {
				message = JSON.parse(message);
				if (!message.error) {
					newData = message.results;

					updateLabelFilteringListBox(newData);

					mygv
					.data(newData)
					.run()

				}
				else {
					mygv.clean();
				}
			})
		}
		else {
			;
		}

		//console.log(index)
	}

	function old_example() {
		var width = 1366;
		var height = 768;

		var mygv = new gsqlv();
		var setting = {'divID':'prototype1', 'width':width, 'height': height};

		mygv
		.setting(setting)
		.data(data)

		var newNode1 = {"type":"newType1","key":"newKey1","attr":{"weight":"0.2","name":"newNode1newNode1newNode1"}};
		var newNode2 = {"type":"newType2","key":"newKey2","attr":{"weight":"0.2","name":"newNode1newNode1newNode1"}};
		var newLink1 = {"source":{"type":"newType1","key":"newKey1"},"target":{"type":"newType2","key":"newKey2"},"attr":{"weight":"1.0","name":"name0.7"}};
		var newLink2 = {"source":{"type":"newType1","key":"newKey1"},"target":{"type":"type0","key":"key45"},"attr":{"weight":"0.01","name":"name0.7"}};

		mygv.addNode(newNode1);
		mygv.addNode(newNode2);
		mygv.addLink(newLink1);
		mygv.addLink(newLink2);

		mygv
		.layout("force")
		.init()
		.run();

		setTimeout(mygv.center_view, 5000);
		window.mygv = mygv;
	}

	this.test = function() {
		console.log("test event handler.")
	}

	window.mygv = mygv;
	console.log("<== where the code ends.")
}()
