require(['gsqlv'], function(){
  console.log("<== where the code starts.")

  // Test case for dropdown list.
  // ["Exampe name", "source account", "target account", "depth"]
  var demoInputExample = [
    ['Example 1', '112.237.21.102', '', '', ''], 
    ['Example 2', '183.67.101.55', '', '', '']
  ];

  this.messageArray = [];  // Record message from backend.
  this.scaleAbilityData = [];  // Record the result for scalability testing.

  var button_height = 50 + 50 + 30 + 34 + 19 + 62 + 10 //+ 290//$('#bodyRow_1').height() + $('#bodyRow_2').height() + $('#bodyRow_hr').height();
  //console.log(button_height);
  
  window.v_width = $('#viewport').width(); // Setting the width of the visualization.
  window.v_height = $(window).height() - button_height;// Setting the height of the visualization.

  //$('#layoutport').height(v_height + 62);
  var maxNumberOfNodes = 500;
  var outputTabsNumber = 0;

  //<div class="container-fluid" width = v_width>
  //$('#tapView').width(v_width*1.05);  // Setting the width of the tap for graph, JSON, summary, and more;

  // Create the graph visualization object.
  var mygv = new gsqlv("mygv");
  console.log("gsqlv version " + mygv.version);

  var log_view_width = 0.0
  var autoFresh = false;
  var autoFreshEngine;
  var autoFreshInterval = 1000 * 6;

  // The graph visualization initialization setting.
  var setting = {'divID':'prototype1', 'width':window.v_width*(1-log_view_width), 'height': v_height};

  // LabelFiltering initialization status;
  var filteringStatus = {node:{type:true, id:true, 
                              // app:{score:false, name:true}, 
                              // friend:{count:true},
                              // location:{count:true},
                              // enterprise:{group_count:true, count:true, name:true},
                              // warning:{is_warning:true},
                              // phone_no:{importance:true,has_friend:true,fraction_of_callees_has_friends:true},
                              // reason:{reason:true},
                              // level:{complaint_count:true},
                              // bypass_complaint:{bypass_complaint_count:true},
                              // srtype:{category:true, complaint_count:false},
                              // solution:{product_list:true}
                            }, 
                          edge:{type:false, id:false,
                              //phone_no_phone_no:{call_times:true}
                          }
                        };

  window.selectionBoxLabels = mygv.label(); // Initialized the multiselection list box data object.

  // Setting the setting.
  // Then initialize the graph visualization.
  mygv
  .setting(setting)
  .init()
 
  $('svg').mouseover(function() {
    $('#'+setting.divID).focus();
    //console.log('123')
  })//

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
        return toLanguage('No option selected ...', "ui_component", "label");
      }
      else if (options.length > 3) {
        return toLanguage('More than 3 options selected!', "ui_component", "label");
      }
      else {
        return labels.map(function(d){return toLanguage(d, "ui_component", "label")}).join(',') + '';
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
        return toLanguage('No option selected ...', "ui_component", "label");
      }
      else if (options.length > 4) {
        return toLanguage('More than 4 options selected!', "ui_component", "label");
      }
      else {
        return labels.map(function(d){return toLanguage(d, "ui_component", "label")}).join(', ') + '';
      }
    },
    //includeSelectAllOption: true,
    //selectAllText: 'Check all!'

  }

  // remove space function.
  this.removeSpaces = function(s) {
    return s.replace(/\s/g, '');
  }

  // Generate the data object for creating the main menu.
  // Specifiy the onclick event for submit button as onclick_submit(index).
  this.generateForm = function(pagesObject){

    elements = pagesObject.elements;
    URL = pagesObject.URL

    var s = '<div class="form-group" >';
    for (var i=0; i < elements.length; i++){
      if (elements[i]["label"]){
        s += '<label>'+toLanguage(elements[i]["label"]["name"], "ui_component", "label")+':</label>';
      }
      if (elements[i]["textbox"]){
        s = s + '&nbsp;&nbsp;<input type ="text" class="form-control" placeholder="' + toLanguage(elements[i]["textbox"]["placeholder"], "ui_component", "label") + '" name="' + elements[i]["textbox"]["name"] + '" size=' + elements[i]["textbox"]["length"] + '>&nbsp;&nbsp;';
      }
      if (elements[i]["selection"]) {
        s += '&nbsp;&nbsp;<select class="form-control" name='+ elements[i]["selection"]["name"] +'>'

        elements[i]["selection"].value.forEach(function(type_name, i) {
           s += "<option value='" + type_name + "'> " + type_name +"</option> ";
        });

        s += '</select>&nbsp;&nbsp;'
      }
      if (elements[i]["datetimebox"]) {
        var date_time_template = '<div class="input-group date form_datetime " data-date="2015-03-16 00:00:00" data-date-format="yyyy-mm-dd hh:ii:ss" data-link-field="{id}">\
          <input class="form-control" size="{size}" type="text" value="" name="{name}" placeholder="{placeholder}">\
          <span class="input-group-addon"><span class="glyphicon glyphicon-remove"></span></span>\
          <span class="input-group-addon"><span class="glyphicon glyphicon-th"></span></span>\
          </div>\
          <input type="hidden" id="{id}" value="" />'

        s += date_time_template
            .replace(/\{name\}/g, elements[i]["datetimebox"]["name"])
            .replace(/\{size\}/g, elements[i]["datetimebox"]["length"])
            .replace(/\{id\}/g, elements[i]["datetimebox"]["name"] + "_datetimebox")
            .replace(/\{placeholder\}/g, elements[i]["datetimebox"]["placeholder"])

      }

      if (elements[i]["dropdown"]) {
        s += generateDropDown(elements[i]["dropdown"]);
      }

      if (elements[i]["slider"]) {
        s += '&nbsp;&nbsp;&nbsp;&nbsp;'
        s += '<input id="slider_'+elements[i]["slider"]['name']+'" type="text"/>'
        s += '&nbsp;&nbsp;&nbsp;&nbsp;'
      }
    }
    s += '&nbsp;&nbsp;<button type = "submit" class="btn btn-primary"'

    s += 'onclick = onclick_submit('+ pagesObject.index + ')';
    s +='> '+toLanguage('Submit Query', "ui_component", "label")+'</button></div>'; 
    return s; 
  }

  this.generateDropDown = function(dropdown) {
    var result = '<div id="button_'+dropdown.name+'" class="dropdown form-group">\
                  <button class="btn btn-default dropdown-toggle" type="button" data-toggle="dropdown" style="width:'+dropdown.length*10+'px"><b>'+toLanguage(dropdown.placeholder, "ui_component", "label")+'</b>\
                  <span class="caret"></span></button>\
                  </button>\
                  <ul id="dropdown_'+dropdown.name+'" class="dropdown-menu" style="width:200%">';

    var content = dropdown.content;
    var elements = content.elements;

    /*
      <label class="col-sm-1 control-label" for="lg">Type</label>
      <div class="col-sm-3">
        <input class="form-control" type="text" id="lg">
      </div>
    */

    if (content.type == "dynamic") {
      result += generateDynamicDropdownItem(elements)

      result += '<div class="">'
      result += '&nbsp;&nbsp;&nbsp;&nbsp;'
      result += '<button name="dropdown_item_add" type="button" class="btn btn-success btn-xs" aria-label="Close">\
                  <span class="glyphicon glyphicon-plus" aria-hidden="true"></span>\
                  </button>'
      result += '</div>'
    }  
    else if (content.type == "static"){
      result += "Coming Soon."
    }  
    else {
      result += "Not Defined Yet."
    }

    result += '</ul></div>&nbsp;&nbsp;';
    return result;
  }

  this.generateDynamicDropdownItem = function(elements) {
      var result = '<div name="dropdown_item" class="form-group">';
      result += '&nbsp;'

      for (var i=0; i < elements.length; i++){
        if (elements[i]["label"]){
          result += '&nbsp;<label class="control-lable">'+toLanguage(elements[i]["label"]["name"], "ui_component", "label")+':</label>&nbsp;';
        }
        if (elements[i]["textbox"]){
          result += '<input class="form-control" type ="text" placeholder="' + toLanguage(elements[i]["textbox"]["placeholder"], "ui_component", "label") + '" name="' + elements[i]["textbox"]["name"] + '" size=' + elements[i]["textbox"]["length"] + '>&nbsp;&nbsp;';
        }
        if (elements[i]["selection"]) {
          result += '<select class="form-control" name='+ elements[i]["selection"]["name"] +'>'

          elements[i]["selection"].value.forEach(function(type_name, i) {
             result += "<option value='" + type_name + "'> " + type_name +"</option> ";
          });

          result += '</select>'
        }
      }

      result += '<button name="dropdown_item_remove" tabindex="-1" type="button" class="btn btn-danger btn-xs" aria-label="Close">\
                  <span class="glyphicon glyphicon-remove" aria-hidden="true"></span>\
                </button>'

      result += '</div>'

      return result;
  }

  this.generateFormForExample = function(pagesObject) {

    elements = pagesObject.elements;
    URL = pagesObject.URL

    var tmp = "<label for='example_list'>Example List : </label>" +
          "<select id='example_list' style='width:200px'>";

    for (var key in demoInputExample) {
      tmp += "<option value='" + key + "'> " + demoInputExample[key][0] +"</option> ";
    }
          /*
            "<option value='0'> Example Fraud Behavior</option> " +
            "<option value='1'> Example Normal Behavior</option> " +
            "<option value='2'> Example Another Fraud Behavior</option> " +
            */

    tmp += "</select>" +
          " <label for='sourceid'>Source Account : </label> " +
          " <input type = 'text' id='sourceid' value=" + demoInputExample[0][1] +" size = 10 readonly>" + 
          " <label for='targetid'>Target Account : </label> " +
          " <input type = 'text' id='targetid' value=" + demoInputExample[0][2] +" size = 10 readonly>" + 
          " <label for='depth'>Search Depth : </label> " +
          " <input type = 'text' id='depth' value=" + demoInputExample[0][3] +" size = 10 readonly>";

    tmp += '&nbsp; <input type = "submit" value = "Submit"';


    tmp += 'onclick = onclick_submit_dropdown('+ pagesObject.index + ')';
    tmp +='> <br />  ';
    tmp += '<label>REST Call : </label> '
    tmp += '<input type = "text" id="urlLab" value=" " readonly size = 80>';
    return tmp;
  }

  this.onclick_submit_dropdown = function(index) {
 

    // test for post
    /*
    var testData = {"function":"vattr", "translate_ids":["1", "25", "27"]};
    testData.translate_ids.push("30");
    
    $.ajax({
        url : "http://shawnhuang.graphsql.com:8080/engine/builtins",
        type: "POST",
        data : JSON.stringify(testData), //'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
        dataType   : 'json',
        beforeSend: function(xhr) {
                console.log("Before Send");
                    xhr.setRequestHeader("Content-Type","application/json");
        },
        success: function(data, textStatus, jqXHR)
        {
            console.log(data.results)//data - response from server
        },
        error: function (jqXHR, textStatus, errorThrown)
        {
            console.log(textStatus)
        }
    });
    */
    
    /*
    $.ajax({
        url : "http://shawnhuang.graphsql.com:8080/engine/list_nodes_attr/1/4",
        type: "GET",
        // data : {"function":"vattr", "translate_ids": ["32", "1", "3"]},
        crossDomain: true,
        dataType   : 'json',
        beforeSend: function(xhr) {
                console.log("Before Send");
                //xhr.setRequestHeader('Access-Control-Allow-Origin', 'false');
                    //xhrObj.setRequestHeader("Content-Type","application/json");
              //xhrObj.setRequestHeader("Accept","application/json");
        },
        success: function(data, textStatus, jqXHR)
        {
            console.log(data.results)//data - response from server
        },
        error: function (jqXHR, textStatus, errorThrown)
        {
            console.log(textStatus)
        }
    });
    */
    var myObject = window.pages_obj[index]

    // initialize the multi-selection box label for new coming data.
    window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
    edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }

    // for each events create coresponding URL.
    for (var key in myObject.events) {
      if (key == "submit") {
        // setting URL for submit button.
        temp_event = myObject.events[key]; 
        submit_URL = temp_event.URL_head;

        URL_attrs =  temp_event.URL_attrs

        /*
        if ("id" in URL_attrs) {
          submit_URL += "/" + document.getElementsByName(URL_attrs.id.name)[0].value + "?";
        }
        else {
          submit_URL += "?";
        }
        */

        for (var attr in URL_attrs.vars) {
          submit_URL += "/" + $('#' + URL_attrs.vars[attr].name)[0].value;
        }

        submit_URL += "?";
        
        for (var attr in URL_attrs.maps) {
          name = URL_attrs.maps[attr].attr;
          attr = URL_attrs.maps[attr];

          //if (name == "id") continue;

          if (attr.usage == "input") {
            if ($('#' + attr.name)[0].value=="") {
              ; 
            }
            else{
                //submit_URL += name + "=" + document.getElementsByName(attr.name)[0].value/*.toLowerCase()*/ +"&";   
                submit_URL += name + "=" + $('#' + attr.name)[0].value/*.toLowerCase()*/ +"&";  
            } 
            //submit_URL += name + "=" + (document.getElementsByName(attr.name)[0].value==""?1:document.getElementsByName(attr.name)[0].value) +"&";
          }
          else if (attr.usage == "attributes") {
            submit_URL += name+ "=" + myObject.attributes[attr.name] + "&";
          }
        }

        // initialize the root node as the query node.
        //  a. create rootNode id.
        //  b. set root node by id.
        //  c. rootNode is = Type + "&" + ID;
        var rootNodeType;
        var rootNodeID;

        var targetNodeType;
        var targetNodeID;

        // Get type from Input box. Sometimes, we don't use type as input for query.
        // But type is another key for the retrieve the node in graph. Default is '0';

        // Get id from Inputbox. The 'id' usually is used as input for query.
        // Default is '0'

        var isTypeExist = false;
        var tempArray = URL_attrs.vars.concat(URL_attrs.maps);
        var tempRootID = "src"  // "id"
        var tempTargetID = "tgt"

        for (var i in tempArray) {
          var item = tempArray[i];
          if (item.attr == "type") {
            isTypeExist = true;
            rootNodeType = 0;
            targetNodeType = 0;
          }

          // Only for one project.
          if (item.attr == tempRootID) {
            rootNodeID = $('#' + item.name)[0].value;//document.getElementsByName(item.name)[0].value;
          }

          if (item.attr == tempTargetID) {
            targetNodeID = $('#' + item.name)[0].value;//document.getElementsByName(item.name)[0].value;
          }
        }
        if (isTypeExist) {
          //rootNodeType = document.getElementsByName(type.name)[0].value;
          // type default is txn;
          ;
        }
        else {
          rootNodeType = "0";
          targetNodeType = "0";
        }

        var initRootNode = rootNodeType + "&" + rootNodeID;
        var initTargetNode = targetNodeType + "&" + targetNodeID;
        /*document.getElementsByName(URL_attrs.type.name)[0].value +
             "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
             */
        mygv.rootNode(initRootNode);
        mygv.targetNode(initTargetNode);
      }
      else {

        // setting URL for other events, such as double left click on nodes.
        mygv.setURL(myObject, key);
      }
    }

    $('#urlLab').val(submit_URL);

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

        // initialize the graph visualization by using the new data.
        // run the visualization.
        mygv
        .data(newData)
        .run()

        /*just for 360 
        var getAttr = {"function":"vattr", "translate_ids":[]};
        mygv.data().nodes.forEach(function(n) {
          getAttr.translate_ids.push(n.id);
        })
        
        $.ajax({
            url : "http://shawnhuang.graphsql.com:8080/engine/builtins",
            type: "POST",
            data : JSON.stringify(getAttr), //'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
            dataType   : 'json',
            beforeSend: function(xhr) {
                        xhr.setRequestHeader("Content-Type","application/json");
            },
            success: function(data, textStatus, jqXHR)
            {
                console.log(data.results)//data - response from server

                mygv.data().nodes.forEach(function(n, i) {
              n.attr = clone(data.results[i].attr)
            })

                mygv
                .runPreDefinition(mygv.data().nodes, mygv.data().links)
                .run();

                updateLabelFilteringListBox(mygv.data());
            },
            error: function (jqXHR, textStatus, errorThrown)
            {
                console.log(textStatus)
            }
        });
        */

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
  this.layoutChanged = function(newLayout) {

    // get the new layout name.
    // set the new layout for gsqlv
    // start the layout process
    // refresh as animation in 500 milliseconds.
    if (newLayout == undefined) {
      mygv
      .startLayout()
      .refreshAnimation(500)

      setTimeout(function() {
        mygv.center_view();
      }, 500)
    }
    else {

      if (newLayout == 'force') {
        mygv
        .layout(newLayout)
        .startLayout();

        setTimeout(function() {
          mygv.center_view();
        }, 4000);
      }
      else {
        mygv
        .layout(newLayout)
        .startLayout()
        .refreshAnimation(500)   

        setTimeout(function() {
          mygv.center_view();
        }, 500)
      }


      $('#layoutProperty').empty();
    
      if (newLayout == "force") {
        var label = '<label>'+toLanguage('Force Layout', "ui_component", "label")+'</label><br/>'
        var restartForce = '<button type="button" class="btn btn-default" onclick="setTheRootNode()">'+toLanguage('Restart Force', "ui_component", "label")+'</button>';
        var centerView = '<button type="button" class="btn btn-default" onclick="mygv.center_view();">'+toLanguage('Set Center View', "ui_component", "label")+'</button>';
        $('#layoutProperty').append(label + restartForce + centerView);
      }
      else if (newLayout == "tree") {
        var label = '<label>'+toLanguage('Tree Layout', "ui_component", "label")+'</label><br/>'
        var rootString = '<button type="button" class="btn btn-default" onclick="setTheRootNode()">'+toLanguage('Set Tree Root', "ui_component", "label")+'</button>';
        var centerView = '<button type="button" class="btn btn-default" onclick="mygv.center_view();">'+toLanguage('Set Center View', "ui_component", "label")+'</button>';
        $('#layoutProperty').append(label + rootString + centerView);
      }
      else if (newLayout == "hierarchical") {
        var label = '<label>'+toLanguage('Hierarchical Layout', "ui_component", "label")+'</label><br/>'
        var rootString = '<button type="button" class="btn btn-default" onclick="setTheRootNode()">'+toLanguage('Set Hierarchical Root', "ui_component", "label")+'</button>';
        var centerView = '<button type="button" class="btn btn-default" onclick="mygv.center_view();">'+toLanguage('Set Center View', "ui_component", "label")+'</button>';
        $('#layoutProperty').append(label + rootString + centerView);
      }
      else if (newLayout == "circle") {
        var label = '<label>'+toLanguage('Circle Layout', "ui_component", "label")+'</label><br/>'
        var rootString = '<button type="button" class="btn btn-default" onclick="setTheRootNode()">'+toLanguage('Set Center Node', "ui_component", "label")+'</button>';
        var centerView = '<button type="button" class="btn btn-default" onclick="mygv.center_view();">'+toLanguage('Set Center View', "ui_component", "label")+'</button>';
        $('#layoutProperty').append(label + rootString + centerView);
      }
    }

    updateTableBoxSize();
    //$('#tableBox').height($('#left_side').height() - $('#layoutport').height() - $('#queryInputInfoBox').height() - 20)
    // set to center view after 500 milliseconds.
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
      $('#b_highlightingNodes').html(toLanguage('Unhighlight', "ui_component", "label")); 
    }
    else {
      $('#b_highlightingNodes').html(toLanguage('Highlight', "ui_component", "label"));
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
  //    1. Store the previous status of the selection.
  //    2. Parsing the graph data, get what attributes each type has.
  //    3. Recreate the mutli selection list box base on types, attribtues, and status.
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

    // initialize the selectionBoxLabels of node.
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
          if (typeof nodelabels[n.type] == 'object' && key in nodelabels[n.type]) {
            selectionBoxLabels.node[n.type][key] = nodelabels[n.type][key];
          }
          else {
            //selectionBoxLabels.node[n.type][key] = (typeof filteringStatus.node[key] == 'undefined') ? false : filteringStatus.node[key];
            selectionBoxLabels.node[n.type][key] = (typeof filteringStatus.node[n.type][key] == 'undefined') ? false : filteringStatus.node[n.type][key];
          }
        }
        catch (err) {
          selectionBoxLabels.node[n.type][key] = false;
          //selectionBoxLabels.node[n.type][key] = (typeof filteringStatus.node[key] == 'undefined') ? false : filteringStatus.node[key];
          //selectionBoxLabels.node[n.type][key] = (typeof filteringStatus.node[n.type][key] == 'undefined') ? false : filteringStatus.node[n.type][key];
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

    // initialize the selectionBoxLabels of edge.
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

      if (key == 'id') {
        tempOptionHTML += toLanguage('Node id', "ui_component", "label") + '</option>'
      }
      else {
        tempOptionHTML += toLanguage(key, "ui_component", "label") + '</option>'
      }
      
      $('#node_label_filtering').append(tempOptionHTML)
    } 

    //6.
    for (var type in selectionBoxLabels.node) {

      if (Object.keys(selectionBoxLabels.node[type]).length == 0) {
        continue;
      }

      var groupList;

      if (type == "__key") {
        continue;
      } 
      else {
        groupList = $('<optgroup value="'+type+'" label="' + toLanguage('type', "ui_component", "label")+': ' + toLanguage(type, type, type) + '"></optgroup>')
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

        tempOptionHTML += toLanguage(key, type, key) + '</option>'
        groupList.append(tempOptionHTML)
      } 


    }

    // Call the multiselec function for node label filtering.
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

      tempOptionHTML += toLanguage(key, "ui_component", "label") + '</option>'
      $('#edge_label_filtering').append(tempOptionHTML)
    } 

    //6.
    for (var type in selectionBoxLabels.edge) {

      if (Object.keys(selectionBoxLabels.edge[type]).length == 0) {
        continue;
      }

      var groupList;

      if (type == "__key") {
        continue;
      } 
      else {
        groupList = $('<optgroup value="'+type+'" label="' + toLanguage('type', "ui_component", "label")+': ' + toLanguage(type, type, type) + '"></optgroup>')
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

        tempOptionHTML += toLanguage(key, type, key) + '</option>'
        groupList.append(tempOptionHTML)
      }   
    }

    //7.
    $('#edge_label_filtering').multiselect(edgeLabelFiltering);
  }

  // call back function for the submit button. 
  // Input : index of the menu in of the main menu.
  //  1. Get the pages object base on the index.
  //  2. Parse the pages object, and generate the URL for submit button.
  //  3. Parse the pages object, and set the URL for visualization by calling setURL().
  //  4. Call back function for the rest request of submit button.
  this.onclick_submit = function(index) {
    if (autoFresh) {
      if (autoFreshEngine == undefined) {
        this.onclick_submit_function(index);
      }
      else {
        clearTimeout(autoFreshEngine);
      }
      autoFreshEngine = setInterval(function(){ this.onclick_submit_function(index); }, autoFreshInterval);
    }
    else {
      this.onclick_submit_function(index);
    }
  }
  this.onclick_submit_function = function(index) {


    // test for post
    /*
    var testData = {"function":"vattr", "translate_ids":["1", "25", "27"]};
    testData.translate_ids.push("30");
    
    $.ajax({
        url : "http://shawnhuang.graphsql.com:8080/engine/builtins",
        type: "POST",
        data : JSON.stringify(testData), //'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
        dataType   : 'json',
        beforeSend: function(xhr) {
                console.log("Before Send");
                    xhr.setRequestHeader("Content-Type","application/json");
        },
        success: function(data, textStatus, jqXHR)
        {
            console.log(data.results)//data - response from server
        },
        error: function (jqXHR, textStatus, errorThrown)
        {
            console.log(textStatus)
        }
    });
    */
    
    var myObject = window.pages_obj[index]

    // initialize the multi-selection box label for new coming data.
    window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
    edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }


    // for each events create coresponding URL and Payload.
    var submit_URL = "";
    var submit_payload = {};

    for (var key in myObject.events) {
      if (key == "submit") {
        // setting URL for submit button.
        temp_event = myObject.events[key]; 
        submit_URL = temp_event.URL_head;

        URL_attrs =  temp_event.URL_attrs

        /*
        if ("id" in URL_attrs) {
          submit_URL += "/" + document.getElementsByName(URL_attrs.id.name)[0].value + "?";
        }
        else {
          submit_URL += "?";
        }
        */

        for (var attr in URL_attrs.vars) {
          submit_URL += "/" + document.getElementsByName(URL_attrs.vars[attr].name)[0].value.trim();
        }

        submit_URL += "?";
        
        for (var attr in URL_attrs.maps) {
          name = URL_attrs.maps[attr].attr;
          attr = URL_attrs.maps[attr];

          //if (name == "id") continue;

          if (attr.usage == "input") {
            if (document.getElementsByName(attr.name)[0].value=="") {
              ; 
              if (name == "max") {
                maxNumberOfNodes = 500; 
              }  
            }
            else{
              if (name == "type") {
                submit_URL += name + "=" + document.getElementsByName(attr.name)[0].value.trim() +"&";
              }
              else if (name == "endtime" || name == "starttime") {
                try {
                  var reggie = /(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})/;
                  var dateString = document.getElementsByName(attr.name)[0].value.trim();
                  var dateArray = reggie.exec(dateString); 
                  var dateObject = new Date(
                      (+dateArray[1]),
                      (+dateArray[2])-1, // Careful, month starts at 0!
                      (+dateArray[3]),
                      (+dateArray[4]),
                      (+dateArray[5]),
                      (+dateArray[6])
                  );

                  var epochSecond = dateObject.getTime() / 1000 - dateObject.getTimezoneOffset() * 60;

                  submit_URL += name + "=" + epochSecond +"&";
                }
                catch (err) {
                  ;
                }

              }
              else {
                submit_URL += name + "=" + document.getElementsByName(attr.name)[0].value.trim() +"&";
                if (name == "max") {
                  maxNumberOfNodes = parseInt(document.getElementsByName(attr.name)[0].value.trim());
                }  
              } 
            } 
            //submit_URL += name + "=" + (document.getElementsByName(attr.name)[0].value==""?1:document.getElementsByName(attr.name)[0].value) +"&";
          }
          else if (attr.usage == "attributes") {
            submit_URL += name+ "=" + myObject.attributes[attr.name] + "&";
          }
        }

        for (var payload in URL_attrs.payload) {
          payload = URL_attrs.payload[payload];

          switch (payload.usage) {
            case "array_input" :
              submit_payload[payload.name] = [];

              var dropdown_id = "#dropdown_" + payload.name;

              $(dropdown_id).find("[name='dropdown_item']").each(function(i, e){
                var payload_item = {}
                var isExist = true;

                payload.attr.forEach(function(a){
                  payload_item[a] = $(e).find('[name="'+a+'"]').val().trim();

                  if (payload_item[a] == "") {
                    isExist = false;
                  }
                })
                
                if (isExist) {
                  var isDuplicate = false;
                  submit_payload[payload.name].forEach(function(d, i) {
                    if (d.type == payload_item.type && d.id == payload_item.id) {
                      isDuplicate |= true;
                    }                
                  })

                  if (!isDuplicate) {
                    submit_payload[payload.name].push(payload_item);
                  }
                }
              })
              
              break;
            case "object_input" :
              submit_payload[payload.name] = {};
              break;
            case "post" :
              submit_payload = payload.template;
              for (var attr in payload.attr) {
                //tab_template.replace(/\{id\}/g, temp_id)
                attr = payload.attr[attr]
                if (attr.usage == "input") {
                  tempInput = document.getElementsByName(attr.name)[0].value.trim();
                  submit_payload = submit_payload.replace(new RegExp("{{"+attr.attr+"}}", "g"), tempInput)
                }
                else if (attr.usage == "attributes"){
                  tempInput = myObject.attributes[attr.name];
                  submit_payload = submit_payload.replace(new RegExp("{{"+attr.attr+"}}", "g"), tempInput)
                }
                else if (attr.usage == "select") {

                }
              }
            default:
              break;
          }
        }

        //remove the last "&" or "?"
        submit_URL = submit_URL.slice(0, -1);

        // initialize the root node as the query node.
        //  a. create rootNode id.
        //  b. set root node by id.
        //  c. rootNode is = Type + "&" + ID;
        var rootNodeType;
        var rootNodeID;

        var targetNodeType;
        var targetNodeID;

        // Get type from Input box. Sometimes, we don't use type as input for query.
        // But type is another key for the retrieve the node in graph. Default is '0';

        // Get id from Inputbox. The 'id' usually is used as input for query.
        // Default is '0'

        var isTypeExist = false;
        var tempArray = URL_attrs.vars.concat(URL_attrs.maps);
        var tempRootID = "id"  // "id"
        var tempTargetID = "tgt_pid"

        for (var i in tempArray) {
          var item = tempArray[i];
          if (item.attr == "type") {
            isTypeExist = true;

            if (item.usage == "input") {
              rootNodeType = document.getElementsByName(item.name)[0].value==""?"0":document.getElementsByName(item.name)[0].value;
              targetNodeType = document.getElementsByName(item.name)[0].value==""?"0":document.getElementsByName(item.name)[0].value;
            }
            else if (item.usage == "attributes") {
              rootNodeType = targetNodeType = myObject.attributes[item.attr]
            }
          }

          // Only for one project.
          if (item.attr == tempRootID || item.attr == "pid_id") {
            rootNodeID = document.getElementsByName(item.name)[0].value;
          }

          if (item.attr == tempTargetID) {
            targetNodeID = document.getElementsByName(item.name)[0].value;
          }
        }
        if (isTypeExist) {
          //rootNodeType = document.getElementsByName(type.name)[0].value;
          // type default is txn;
          ;
        }
        else {
          rootNodeType = "phone_no";
          targetNodeType = "phone_no";
        }

        if (!isTypeExist && rootNodeID == undefined) {
          var inputArray = $('#button_'+URL_attrs.payload[0].name).find("[name='type']");
          var inputIdArray = $('#button_'+URL_attrs.payload[0].name).find("[name='id']");

          for (var k=0; k<inputArray.length; ++k) {
            rootNodeType = inputArray[k].value;
            rootNodeID = inputIdArray[k].value;
            if (rootNodeType != "" && rootNodeID != "") {
              break;
            }
          }
        }

        var initRootNode = rootNodeType + "&" + rootNodeID;
        var initTargetNode = targetNodeType + "&" + targetNodeID;
        /*document.getElementsByName(URL_attrs.type.name)[0].value +
             "&" + document.getElementsByName(URL_attrs.id.name)[0].value;
             */
        mygv.rootNode(initRootNode);
        mygv.targetNode(initTargetNode);
      }
      else {

        // setting URL for other events, such as double left click on nodes.
        mygv.setURL(myObject, key);
      }
    }

    $.ajax({
      url : submit_URL,
      type : "post",
      data : submit_payload,//JSON.stringify(submit_payload),//'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
      dataType   : 'json',
      error: function(jqXHR, textStatus, errorThrown){
        // will fire when timeout is reached
        mygv.clean();
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
        //do something
        if (typeof message == 'object') {
          ;
        }
        else {
          try {
            message = JSON.parse(message);
          }
          catch (err){
            console.log("REST query result is not a vaild JSON format")
            return ;
          }
        }


        if (!message.error && message.error != undefined) {
          // get the json object of the result.
          newData = message.results;

          updateQueryInputInfoBox(submit_payload, page_index);

          if (newData.neighborhood_size > maxNumberOfNodes) {
            switch (language) {
              case "chinese":
              alert ("找到 " + newData.neighborhood_size + " 个相邻节点， 但是只有 " + maxNumberOfNodes + " 个节点被显示. 请提高最大节点个数。")
              break;
              case "english":
              alert ("Found " + newData.neighborhood_size + " nodes. But only " + maxNumberOfNodes + " nodes are shown in the visualization. Please increase the Max number.")
              break;
              default:
              alert ("Found " + newData.neighborhood_size + " nodes. But only " + maxNumberOfNodes + " nodes are shown in the visualization. Please increase the Max number.")
            }
          }

          // initialize the graph visualization by using the new data.
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
        else { // if is error message
          if (language == "chinese") {
            if (message.message.indexOf('error_: invalid / deleted') != -1) {
              alert("输入的查询节点不存在，请重新输入。");
            }
            else if (message.message.indexOf('Adding ID failed.') != -1) {
              alert("输入的查询节点类型不存在，请重新输入。");
            }
            else {
              alert(message.message);
            }
          }
          else if (language == "english") {
            alert(message.message);
          }
          else {
            alert(message.message);
          }
        }
      }
    })

    // call back function for the rest query of the submit button.
    // $.get(submit_URL, function(message) {
    //   // JSON parse the message string.

    //   if (typeof message == 'object') {
    //     ;
    //   }
    //   else {
    //     try {
    //       message = JSON.parse(message);
    //     }
    //     catch (err){
    //       console.log("REST query result is not a vaild JSON format")
    //       return ;
    //     }
    //   }

      
    //   if (!message.error) {
    //     // get the json object of the result.
    //     newData = message.results;

    //     // initialize the graph visualization by using the new data.
    //     // run the visualization.
    //     mygv
    //     .data(newData)
    //     .run()

    //     just for 360 
    //     var getAttr = {"function":"vattr", "translate_ids":[]};
    //     mygv.data().nodes.forEach(function(n) {
    //       getAttr.translate_ids.push(n.id);
    //     })
        
    //     $.ajax({
    //         url : "http://shawnhuang.graphsql.com:8080/engine/builtins",
    //         type: "POST",
    //         data : JSON.stringify(getAttr), //'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
    //         dataType   : 'json',
    //         beforeSend: function(xhr) {
    //                     xhr.setRequestHeader("Content-Type","application/json");
    //         },
    //         success: function(data, textStatus, jqXHR)
    //         {
    //             console.log(data.results)//data - response from server

    //             mygv.data().nodes.forEach(function(n, i) {
    //           n.attr = clone(data.results[i].attr)
    //         })

    //             mygv
    //             .runPreDefinition(mygv.data().nodes, mygv.data().links)
    //             .run();

    //             updateLabelFilteringListBox(mygv.data());
    //         },
    //         error: function (jqXHR, textStatus, errorThrown)
    //         {
    //             console.log(textStatus)
    //         }
    //     });
        

    //     // update the multi-selection lable box base on the graph data.
    //     updateLabelFilteringListBox(mygv.data());

    //     // store the new message in the messageArray.
    //     messageArray.push(message);

    //     // out put the JSON message in the json tab.
    //     JSONMessageOutput()
    //   }
    //   else {

    //     // clean everything of the graph visualization.
    //     mygv.clean();
    //     alert(JSON.stringify(message));
    //   }

    // })
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
    .style("width", v_width + "px")
    .style("height", v_height+"px")

    d3.select("#messageBox")
    .html("<span> JSON Message " + messageArray.length + " :</span> \n" + syntaxHighlight(messageArray[messageArray.length-1]))
  }

  // out out the summary information for both whole graph and selected sub graph.
  this.summaryInformationOutput = function () {
    // remove previous summary information display.
    d3.select("#summaryOfAllNodes").selectAll("*").remove();
    d3.select("#summaryOfSelectedNodes").selectAll("*").remove();
    d3.select("#tableBox").selectAll("*").remove();

    // output the summary information for the whole graph.
    d3.select("#summaryOfAllNodes")
    .style("width", v_width + "px")
    .style("height", v_height/2.02+"px")
    .html(mygv.summaryInformationForAllNodes())

    // output the summary information for the selected sub graph.
    d3.select("#summaryOfSelectedNodes")
    .style("width", v_width + "px")
    .style("height", v_height/2.02+"px")
    .html(mygv.summaryInformationForSelectedNodes())

    d3.select("#tableBox")
    //.style("height", v_height+"px")
    .html(mygv.outputTableforSelectedNodes())
  }

  // Just output the summary information for the selected sub graph.
  this.updateSummaryInformationForSelectedNodes = function() {

    // remove previous summary information.
    d3.select("#summaryOfSelectedNodes").selectAll("*").remove();

    // out the new summary information.
    d3.select("#summaryOfSelectedNodes")
    .style("height", v_height/2.02+"px")
    .html(mygv.summaryInformationForSelectedNodes())

    d3.select("#tableBox").selectAll("*").remove();

    // out the new log information.
    d3.select("#tableBox")
    .html(mygv.outputTableforSelectedNodes())
  }

  this.checkUndefinedForBool = function(x) {
    if (typeof x == 'undefined') {
      return false;
    }
    else {
      return x;
    }
  }

  this.onClick_table = function(d, type) {

    console.log(arguments);

    var attr = 'attr'
    var value = 'value'
    $(d)
    .find('td')
    .each(function(i, d) {
      var temp = $(d).html()
      switch(i) {
        case 0:
          //attr = fromLanguage(temp);
          break;
        case 1:
          //value = fromLanguage(temp);
        break;
          default:
        break;
      }
    })

    var conditionString = attr + "=='" + value + "'";

    // Unselect everything in the graph visualization.
    mygv.unselectingEverything();

    // doing selecting base on condition type and condition string.
    if (type == "node") {
      mygv.selectingNodesByConditions(conditionString);
    }
    else if (type == "link") {
      mygv.selectingEdgesByConditions(conditionString);
    }

    $('tr[name=element_name]').css("background-color","")
    $('tr[name=attribute_name]').css("background-color","")

    $(d).css("background-color","#a1d99b")
  }  

  this.onClick_table_head = function() {
    console.log(arguments);

    var conditionString = "";

    if (arguments[1] == 'node') {

      conditionString += 'id=="'+ arguments[2]+'"&&type=="'+arguments[3]+'"';

      mygv.unselectingEverything();
      mygv.selectingNodesByConditions(conditionString);
    }
    else if (arguments[1] == 'link') {

      conditionString += 'source.id=="'+ arguments[2]+'"&&source.type=="'+arguments[3]
        + '"&&target.id=="' + arguments[4] + '"&&target.type=="' + arguments[5]
        + '"&&type=="' + arguments[6] + '"';

      mygv.unselectingEverything();
      mygv.selectingEdgesByConditions(conditionString);
    }

    $('tr[name=element_name]').css("background-color","")
    $('tr[name=attribute_name]').css("background-color","")

    $(arguments[0]).css("background-color","#a1d99b")
  } 

  this.initializeUIComponent = function(d) {
    // initializing label filtering selection box.
    var parent = $('#node_label_filtering').parent();
    $('#node_label_filtering').next().remove();
    $('#node_label_filtering').remove();

    parent.append('<select id="node_label_filtering" multiple="multiple">') 
    $('#node_label_filtering').multiselect(nodeLabelFiltering);

    parent = $('#edge_label_filtering').parent();
    $('#edge_label_filtering').next().remove();
    $('#edge_label_filtering').remove();

    parent.append('<select id="edge_label_filtering" multiple="multiple">') 
    $('#edge_label_filtering').multiselect(edgeLabelFiltering);

    // initializing dropbown box for multi-condition, if it is exist.
    d.elements.forEach(function(e) {
      if(e.dropdown) {
        e = e.dropdown;
        var eID = "#dropdown_" + e.name;
    
        $(eID).click(function(event){
          event.stopPropagation();
        })

        $(eID).find("[name='dropdown_item_add']")
        .click(function(event) {
          event.stopPropagation();

          var name = this.parentElement.parentElement.parentElement.id.split("_")[1];
          var content = window.pages_obj[window.page_index].elements.filter(function(e){
            return e[Object.keys(e)[0]].name == name
          })[0]

          var item = generateDynamicDropdownItem(content.dropdown.content.elements);
          
          $(item).insertBefore(this.parentElement)
          .find("[name='dropdown_item_remove']")
          .click(function(event) {
            event.stopPropagation();
            this.parentElement.remove()
          })
        });

        $(eID).find("[name='dropdown_item_remove']")
        .click(function(event) {
          event.stopPropagation();
          this.parentElement.remove()
        })
      }
      else if (e.slider) {
        e = e.slider;
        $("#slider_"+e.name).slider(e.value);
      }
    })


    // Initializing the output table.
    this.findCommonTransaction();

    // Initializeing date time picker box.
    $('.form_datetime').datetimepicker({
        language:  language=='chinese'?'zh-CN':undefined,
        weekStart: 0,
        todayBtn:  0,
        autoclose: 1,
        todayHighlight: 1,
        startView: 'month',
        forceParse: 1,
        showMeridian: 0
    });
  }

  this.removeOutputTabs = function() {
    //console.log('remove :' + event.target);
    event.stopPropagation();

    var tabs = $(event.target).parent().parent().siblings();
    var tab = tabs[tabs.length-1];

    $(tab).find('a').tab('show');

    $(event.target).parent().parent().remove();
    $($(event.target).parent().attr("href")).remove();
  }
  
  this.findCommonTransaction = function() {

    /*
    bankCardNo: "62262301****5015"
    cardHolder: "*婷婷"
    clientIp: "3549902612"
    flowId: "316281258863313I"
    idCard: "3101091986****1068"
    isBlacklisted: "false"
    merchantNumber: "10002291354"
    orderEpochTime: "1427846402"
    orderTime: "2015-04-01 00:00:02"
    outdegree: "5"
    userPhone: "180****7208"
    */
    var BreakException= {};
    this.outputCSVType = 'findCommonTransaction';

    try {
      mygv.data().nodes.forEach(function(d, i) {
        if (d.selected && d.type == 'transaction') {
          throw BreakException;
        }
      })
    }
    catch(e) {
      if (e!==BreakException) throw e;

      switch (language) {
        case "chinese":
        alert("在寻找关联交易时，请不要选择交易节点.");
        break;
        case "english":
        default:
        alert("Please only select Entity Nodes for finding common transactions.");
      }
      
      return;
    }


    this.transactionEntity = ["id", "orderTime", "merchantNumber", "cardHolder", "idCard", "userPhone", "bankCardNo", "flowId", "clientIp", "isBlacklisted"];
    this.commonTransactionArray = [];
    var result = mygv.getCommonTransaction();

    if (result.length == 0) {
      return;
    }

    // Create Tab
    var tab_template = '<li role="presentation"><a href="#{id}" aria-controls="{id}" role="tab" data-toggle="tab">{id} <span style="background-color:#ddd;" class="glyphicon glyphicon-remove" aria-hidden="true" onclick="removeOutputTabs()"></span></a></li>'
    var content_template = '<div role="tabpanel" class="tab-pane" style="overflow:auto;height:170px;" id="{id}">{content}</div>'

    var temp_id = 'outputTab_'+outputTabsNumber;

    var li = $(tab_template.replace(/\{id\}/g, temp_id))
    
    // Create Table
    var tableHTML = '<table class="table table-bordered table-striped">'
    tableHTML += '<thead><tr>'

    this.transactionEntity.forEach(function(d, i) {
      tableHTML += '<th style="line-height:1.0">' + toLanguage(d, "ui_component", "label") + '</th>'
    })

    tableHTML += '</tr></thead>'

    tableHTML += '<tbody>'

    result.forEach(function(d, i) {
      var item = [];
      this.transactionEntity.forEach(function(a, j) {
        if (a == 'id') {
          item.push(d[a]);
        }
        else {
          item.push(d.attr[a]);
        }   
      })

      this.commonTransactionArray.push(item);

      tableHTML += '<tr>'

      item.forEach(function(item, j) {
        tableHTML += '<td style="line-height:1.0">' + item + '</td>'
      })
      tableHTML += '</tr>'
    })

    tableHTML += '</tbody>'
    tableHTML += '</table>'

    // Append Element
    var content = $(content_template
                    .replace(/\{id\}/g, 'outputTab_'+outputTabsNumber)
                    .replace(/\{content\}/g, tableHTML))

    outputTabsNumber++;

    li.data('table', clone(this.commonTransactionArray));

    $('#outputTabs').append(li);
    $('#outputContent').append(content);
    
    $('a[href="#'+temp_id+'"]').tab('show')
  }
  
  this.findCommonEntities = function() {
    var maxRiskLevel = 30;
    var BreakException= {};
    this.outputCSVType = 'findCommonEntities';

    try {
      mygv.data().nodes.forEach(function(d, i) {
        if (d.selected && d.type != 'transaction') {
          throw BreakException;
        }
      })
    }
    catch(e) {
      if (e!==BreakException) throw e;

      switch (language) {
        case "chinese":
        alert("在寻找关联信息时，请不要选择非交易节点.")
        break;
        case "english":
        default:
        alert("Please only select Transaction Nodes for finding common entities.")
      }
      
      return;
    }

    this.commonEntitiesArray = [];
    this.commonEntitiesTableHead = ['Risk Level'];

    var result = mygv.getCommonEntities();

    if (result.getCommonEntities_totleNumber == 0) {
      return;
    }

    for (var key in result) {
      if (key == 'getCommonEntities_totleNumber') continue;
      this.commonEntitiesTableHead.push(key);
    }

    this.commonEntitiesArray.push(this.commonEntitiesTableHead.map(function(d){return toLanguage(d, "ui_component", "label")}));

    // Create Tab
    var tab_template = '<li role="presentation"><a href="#{id}" aria-controls="{id}" role="tab" data-toggle="tab">{id} <span style="background-color:#ddd;" class="glyphicon glyphicon-remove" aria-hidden="true" onclick="removeOutputTabs()"></span></a></li>'
    var content_template = '<div role="tabpanel" class="tab-pane" style="overflow:auto;height:170px;" id="{id}">{content}</div>'

    var temp_id = 'outputTab_'+outputTabsNumber;

    var li = $(tab_template.replace(/\{id\}/g, temp_id))


    // Create Table
    var tableHTML = '<table class="table table-bordered table-striped">'
    tableHTML += '<thead><tr>'

    this.commonEntitiesTableHead.forEach(function(d, i) {
      tableHTML += '<th style="line-height:1.0">' + toLanguage(d, "ui_component", "label") + '</th>'
    })

    tableHTML += '</tr></thead>'

    tableHTML += '<tbody>'

    for (var index=0; index<result.getCommonEntities_totleNumber; ++index) {
      var item = [];
      var isMaxRiskLevel = true;
      
      this.commonEntitiesTableHead.forEach(function(d, i) {
        if (i == 0) {
          item.push(index+1);
        }
        else {
          if (typeof result[d][index] != 'undefined') {
            d = result[d][index];
            item.push(d.id + ' ['+d.commonEntityScore+'/'+result.getCommonEntities_totleNumber+']');
            isMaxRiskLevel &= false;
          }
          else {
            item.push(" ");
          }
        }
      })

      if (isMaxRiskLevel) {
        break
      }

      this.commonEntitiesArray.push(item);

      if (index < maxRiskLevel) {
        tableHTML += '<tr>'

        item.forEach(function(item, j) {
          tableHTML += '<td style="line-height:1.0">' + item + '</td>'
        })

        tableHTML += '</tr>'
      }
    }

    // for (var key in result) {
    //   if (key == 'getCommonEntities_totleNumber') continue;

      



    //   result[key].forEach(function(d, j) {
    //     if (j < maxRiskLevel) {
    //       item.push(d.id + ' ['+d.commonEntityScore+'/'+result.getCommonEntities_totleNumber+']'); 
    //     }
    //     else {
    //       return;
    //     }
        
    //   })

    //   this.commonEntitiesArray.push(item);

    //   tableHTML += '<tr>'

    //   item.forEach(function(item, j) {
    //     tableHTML += '<td style="line-height:1.0">' + item + '</td>'
    //   })
    //   tableHTML += '</tr>'
    // }

    tableHTML += '</tbody>'
    tableHTML += '</table>'

    // Append Element
    var content = $(content_template
                    .replace(/\{id\}/g, 'outputTab_'+outputTabsNumber)
                    .replace(/\{content\}/g, tableHTML))

    outputTabsNumber++;

    li.data('table', clone(this.commonEntitiesArray));

    $('#outputTabs').append(li);
    $('#outputContent').append(content);
    
    $('a[href="#'+temp_id+'"]').tab('show')
  }

  this.outputExcelFile = function() {
    try {
      if (outputCSVType == 'findCommonTransaction') {
        var data = clone($('#outputTabs').find('li[class=active]').data('table'))
        if (data.length == 0) {
          return;
        }


        /*
        var csvContent = "data:text/csv;charset=utf-8;filename=findCommonTransaction.csv,";
        csvContent += this.transactionEntity.join(",") + "\n";

        data.forEach(function(d, index){
            var dataString = d.join(",");
            csvContent += index < data.length ? dataString+ "\n" : dataString;
        });
        */

        data.unshift(this.transactionEntity.map(function(d){return toLanguage(d, "ui_component", "label");}));
        
        var exportFileName = "Common_Transaction_Output";
        export_data_to_excel(data ,exportFileName);
      }
      else if (outputCSVType == 'findCommonEntities') {
        var data = clone($('#outputTabs').find('li[class=active]').data('table'))

        if (data.length == 0) {
          return;
        }

        var exportFileName = "Common_Entities_Output";
        export_data_to_excel(data ,exportFileName);
      }
      else {
        return;
      }

      var encodedUri = encodeURI(csvContent);
      window.open(encodedUri);
    }
    catch (e) {
      return;
    }
    
  }

  this.outputCSVFile = function() {
    try {
      if (outputCSVType == 'findCommonTransaction') {
        var data = $('#outputTabs').find('li[class=active]').data('table')
        if (data.length == 0) {
          return;
        }


        /*
        var csvContent = "data:text/csv;charset=utf-8;filename=findCommonTransaction.csv,";
        csvContent += this.transactionEntity.join(",") + "\n";

        data.forEach(function(d, index){
            var dataString = d.join(",");
            csvContent += index < data.length ? dataString+ "\n" : dataString;
        });
        */

        var csvString = this.transactionEntity.map(function(d){return toLanguage(d, "ui_component", "label")}).join(",") + '%0A';

        data.forEach(function(d, i){
          csvString += d.join(",") + '%0A';
        })
        
        var exportFileName = "Common_Transaction_Output.csv";
        $('<a></a>', {
            "download": exportFileName,
            "href": "data:," + csvString,
            "id": "Common_Transaction_Output"
        }).appendTo("body")[0].click().remove();
      }
      else if (outputCSVType == 'findCommonEntities') {
        var data = $('#outputTabs').find('li[class=active]').data('table')

        if (data.length == 0) {
          return;
        }

        var csvString = "";
        data.forEach(function(d, i){
          csvString += d.join(",") + '%0A';
        })

        var exportFileName = "Common_Entities_Output.csv";
        $('<a></a>', {
            "download": exportFileName,
            "href": "data:," + csvString,
            "id": "exportDataID"
        }).appendTo("body")[0].click().remove();
      }
      else {
        return;
      }

      var encodedUri = encodeURI(csvContent);
      window.open(encodedUri);
    }
    catch (e) {
      return;
    } 
  }

  this.updateQueryInputInfoBox = function(payload, page_index) {
    var table = "<h3>"+toLanguage(window.pages_obj[page_index]['tabname'], 'tab', 'tab name')+"</h3>"
    if (typeof payload != 'object' || Object.keys(payload).length >= 0) {
      //return;
    }
    else {
      //$('#queryInputInfoBox').html(JSON.stringify(payload));
      table += '<table class="table table-condensed table-bordered">'
      table += '<tr><th>'+toLanguage('Query Type', "ui_component", "label")+'</th><th>'+toLanguage('Query ID', "ui_component", "label")+'</th></tr>';
      payload['input'].forEach(function(d){
        table += '<tr><td>'+d.type+'</td><td>'+d.id+'</td></tr>';
      })
      table += '</table>'
    }

    $('#queryInputInfoBox').html(table);

    updateTableBoxSize();
    //console.log("payload : " + payload);
  }

  this.updateTableBoxSize = function() {
    $('#tableBox').height($('#left_side').height() - $('#layoutport').height() - $('#queryInputInfoBox').height() - 20)
  }

  // Initialize main menu, base on the message from endpoint.
  //$(document).ready(function() {
    //Query the engine to get tab layout information
    // Start UIpages => End UIpages
    //$.get( "engine/UIpages", function( data ) {

      window.page_index = 0;
      //window.pages_obj = JSON.parse(data); 
      window.pages_obj = UIpages;   //data//UIpages;
      //sort on index
      window.pages_obj.sort(function(a,b) {return (a.index - b.index);});
      //insert pages
      for (var i=0; i < window.pages_obj.length; i++) {
        var tabname = window.pages_obj[i].tabname;
        var removedSpaces = removeSpaces(tabname);
        tabline = '<li><a href="#" id="' + removedSpaces  + '" name="'+removedSpaces+'_'+ i + '">' + toLanguage(tabname, 'tab', 'tab name') + '</a></li>';
        
        // dropdown list for demo example.
        //window.pages_obj[removedSpaces] = '<div>' + generateFormForExample(window.pages_obj[i]) + '</div>';

        // input box for input 
        window.pages_obj[removedSpaces] = '' + generateForm(window.pages_obj[i]) + '';

        //console.log(window.pages_obj[removedSpaces]);
        //console.log("Registering event for " + "#" + removedSpaces);


        // onclick on main menu will clean everything.
        $(document).on('click', '#'+removedSpaces, function(event){ 

          if (window.page_index == parseInt(event.target.name.split("_")[1])) {
            event.stopPropagation();
            return;
          }
          else {
            window.page_index = parseInt(event.target.name.split("_")[1]);
          }
          //console.log("click" + event.target.id);
          //console.log("form data is " + window.pages_obj[event.target.id]);
          $('#urlLab').val("");
          $('#content-page').empty();
          $('#content-page').append(window.pages_obj[event.target.id]);

          UIObject = window.pages_obj[window.page_index]
          
          // Clean graph visualization object.
          // initialize layout
          // setting pre definition of the graph visualization.
          mygv.clean()
          layoutChanged(UIObject.setting.layout)
          mygv.edgeDirectedType(UIObject.setting.edgeDirectedType)
          mygv.preDefinition(UIObject.initialization)

          $('#layoutType').val(UIObject.setting.layout);

          //window.selectionBoxLabels = {node:{"__key":{'type':filteringStatus.node.type, 'id':filteringStatus.node.id}}, edge:{"__key":{'type':filteringStatus.edge.type, 'id':filteringStatus.edge.id}} }
          window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
          edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }
          

          initializeUIComponent(UIObject);

          updateQueryInputInfoBox({}, page_index);


          if (window.pages_obj[page_index].tabname == "实时用户全景") {
            clearTimeout(autoFreshEngine);
            autoFresh = true;
          }
          else {
            clearTimeout(autoFreshEngine);
            autoFresh = false;
          }
        });

        // the first item of main menu is activated by default.
        if (i == 0) {
          //Index 0 is activated
          $('#content-page').empty();
          $('#content-page').append(window.pages_obj[removedSpaces]);

          // initialize the multi selection list box for displaying labels of nodes and edges.
          $('#node_label_filtering').multiselect(nodeLabelFiltering);
          $('#edge_label_filtering').multiselect(edgeLabelFiltering);

          UIObject = window.pages_obj[i]
          
          mygv.clean()
          layoutChanged(UIObject.setting.layout)
          mygv.edgeDirectedType(UIObject.setting.edgeDirectedType)
          mygv.preDefinition(UIObject.initialization)

          //window.selectionBoxLabels = mygv.label();
          //window.selectionBoxLabels = {node:{"__key":{'type':filteringStatus.node.type, 'id':filteringStatus.node.id}}, edge:{"__key":{'type':filteringStatus.edge.type, 'id':filteringStatus.edge.id}} }
          window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
                        edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }
          
          $('#layoutType').val(UIObject.setting.layout);
          initializeUIComponent(UIObject);

          updateQueryInputInfoBox({}, page_index);
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

      // stroe the JSON message.
      // out the json message in the JSON tab.
      // out the summary information of graph, and selected sub graph in the summary tab.
      messageArray.push(window.pages_obj)
      JSONMessageOutput();
      summaryInformationOutput();
    //});
    //End UIpages.
  
    //*******************
    //initialize graph visualization menu

    //initialize the color picker.
    // set the colorchange event for changing the color of selected nodes and edges.
    $('#colorpicker').colorpicker({
        color: "#1f77b4"  //bodyStyle.backgroundColor
    }).on('changeColor', function(ev) {
        //bodyStyle.backgroundColor = ev.color.toHex();
        mygv.coloringNodes(ev.color.toHex())
        mygv.coloringEdges(ev.color.toHex())
    });
  //});

  

  //refer the graph visualization object.
  window.mygv = mygv;
  console.log("<== where the code ends.")
  return "UI setted up."
});
