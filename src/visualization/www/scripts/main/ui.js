/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
require([], function(){
  //test();

  d3.select('#prototype1').style('height', ($(window).height()-220)+'px');

  this.visualization = new gvis({
    configure: config,
    container: 'prototype1',
    render_type: 'map', // 'svg'
  });


  visualization.setNodeToolTipFormat(function(type, id, attrs) {
    var template = '<span style="color:{{color}}">{{key}}{{value}}</span><br />'; 

    var result = '<span style="color:#ff0000">Node</span><br />';

    result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'', value:type + '-' + id});

    for (var key in attrs) {
      result += gvis.utils.applyTemplate(template, {color:'#99d8c9', key:key+':', value:attrs[key]})
    }
    
    return result;
  })

  visualization.setLinkToolTipFormat(function(type, attrs) {
    var template = '<span style="color:{{color}}">{{key}}{{value}}</span><br />'; 

    var result = '<span style="color:#ff0000">Link</span><br />';

    result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'', value:type});

    for (var key in attrs) {
      result += gvis.utils.applyTemplate(template, {color:'#99d8c9', key:key+':', value:attrs[key]})
    }
    
    return result;
  })


  visualization
  .on('nodeClick', function(d, renderer) {
    //console.log('customized click', d, renderer)
    d3.select("#tableBox").selectAll("*").remove();

    d3.select("#tableBox")
    .html(outputTableforSelectedElements([d], []))
  })
  .on('nodeDblClick', function(d, renderer) {
    //console.log('customized dblclick', d)
    d3.select("#tableBox").selectAll("*").remove();

    d3.select("#tableBox")
    .html(outputTableforSelectedElements([d], []))
  })
  .on('linkClick', function(d, renderer) {
    //console.log('customized click', d)
    d3.select("#tableBox").selectAll("*").remove();

    d3.select("#tableBox")
    .html(outputTableforSelectedElements([], [d]))
  })
  .on('linkDblClick', function(d, renderer) {
    //console.log('customized dblclick', d)
    d3.select("#tableBox").selectAll("*").remove();

    d3.select("#tableBox")
    .html(outputTableforSelectedElements([], [d]))
  })
  .on('multiSelect', function(nodes, links, renderer) {
    //console.log('multiSelect : ', nodes.length, ',', links.length);
    d3.select("#tableBox").selectAll("*").remove();

    d3.select("#tableBox")
    .html(outputTableforSelectedElements(nodes, links))
  })

  var rootNodeType = '';
  var rootNodeID = '';
  var targetNodeType = '';
  var targetNodeID = '';

  var button_height = 230;
  var filteringStatus = {
    node:{
        transaction: {type:true, id:true}, 
        phone : {type:true, id:true},
        client : {type:true, id:true},
        user : {type:true, id:true},
        bankcard : {type:true, id:true, name:true, idCard:true}
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
    edge:{
        transactionphone:{type:true},
        //phone_no_phone_no:{call_times:true}
    }
  };

  window.selectionBoxLabels = filteringStatus

  this.messageArray = [];  

  // The configure object of the node label multiselecting list box.
  var nodeLabelFiltering = {
    maxHeight: 600,
    buttonWidth: '100%',
    enableClickableOptGroups: true,
    // Call back function once the selection changed.
    buttonText: function(options, select) {
      var labels = [];

      if (visualization.data().nodes().length != 0) {
        for (var key in selectionBoxLabels.node){
          for (var item in selectionBoxLabels.node[key]) {
            selectionBoxLabels.node[key][item] = false;
          }   
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

      Object.keys(selectionBoxLabels.node).forEach(function(type) {
        var labels = Object.keys(selectionBoxLabels.node[type]).filter(function(key) {
          return selectionBoxLabels.node[type][key];
        })

        visualization.showNodeLabelByType(type, labels);
      })

      visualization.update(0, 0);

      // test.showNodeLabel('tag', '1', ['type', 'a1', 'a2', 'asdf']);
      // test.showNodeLabelByType('usr', ['id', 'a1', 'a2']);

      // test.showLinkLabel('tag', '4', 'movie', '11', 'bbs', ['type', 'aa1', 'aa2']);
      // test.showLinkLabelByType('bbc', ['aa1', 'aa2', '11123']);

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

      if (visualization.data().links().length != 0) {
        for (var key in selectionBoxLabels.edge){
          for (var item in selectionBoxLabels.edge[key]) {
            selectionBoxLabels.edge[key][item] = false;
          }   
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

      Object.keys(selectionBoxLabels.edge).forEach(function(type) {
        var labels = Object.keys(selectionBoxLabels.edge[type]).filter(function(key) {
          return selectionBoxLabels.edge[type][key];
        })

        visualization.showLinkLabelByType(type, labels);
      })

      visualization.update(0, 0);

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


  function topology4VertexAttribute_to_json(vattr) {
    var result = {};
    var reggie = /(.*)\((\d*)\)$/;  // for degree(0) => degree, 0

    if (vattr == "" || vattr == undefined) {
      return result;
    }
    else if (typeof(vattr) == 'object') {
      return clone(vattr);
    }
    else {
      var attrs = vattr.split('|');
      attrs.forEach(function(attr, i) {
        //var items = attr.split(':');
        var items = attr.split(':', 1);
        items[1] = attr.substring(attr.indexOf(":")+1);

        if (items.length > 1 && items[0] != "" && items[0] != items[1]) {
          if (items[0] == 'type') { //|| items[0].indexOf("od_et") > -1 || items[0] == 'outdegree') {
            //items[0] = 'type_index';
            //console.log(items[0])
            return; // ignore the type attributes.
          }

          var tempAttr = reggie.exec(items[0]); // "degree(0)" => "degree(0)", "degree", "0";
          if (tempAttr == null) {
            result[items[0]] = items[1];
          }
          else {
            result[tempAttr[1]] = items[1];
          }
        }
      })
      return result; 
    }
  }

  function refineDataName(newData) {

    var color = d3.scale.category10();
    
    // the result should be as nodes and edges.
    var result = {"nodes":[], "links":[]};
    var arrResult = []
    newData.vertices.forEach(function(n) {
      // Example of a node :
      // {"type":"newType1","id":"newKey1","attr":{"weight":"0.2","name":"newNode1newNode1newNode1"}};
      tempNode = {};
      tempNode.type = n.type
      tempNode.id = n.id
      tempNode[gvis.settings.attrs] = topology4VertexAttribute_to_json(n.attr);
      tempNode[gvis.settings.styles] = {fill:color(n.type)};
      tempNode.other = gvis.utils.clone(n.other)

      arrResult.push(tempNode);
    }) 

    var tempResult = {};
    arrResult.forEach(function(n, i){
      tempResult[n.id + '-'+ n.type] = n; 
    })

    for (var key in tempResult) {
      result.nodes.push(tempResult[key]);
    }

    newData.edges.forEach(function(e) {
      // Example of a link :
      // {"source":{"type":"newType1","id":"newKey1"},"target":{"type":"type0","id":"key45"},"attr":{"weight":"0.01","name":"name0.7"}, "type":0};
      tempEdge = {"source":{"type":"","id":""}, "target":{"type":"","id":""}, directed:true};
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
      tempEdge[gvis.settings.attrs] = topology4VertexAttribute_to_json(e.attr) //The way to parse edge and vertex attrs are same.
      tempEdge[gvis.settings.styles] = {fill:color(e.type)};
      tempEdge.other = gvis.utils.clone(e.other)

      result.links.push(tempEdge);
    })
    
    return result
  }

  // remove space function.
  this.removeSpaces = function(s) {
    return s.replace(/\s/g, '');
  }

  this.deleteNodes = function() {
    visualization.getSelectedNodes().forEach(function(n) {
      visualization.dropNode(n);
    })

    visualization.update(500, 500);
  }

  this.coloringNodes = function(color) {
    visualization.getSelectedNodes().forEach(function(n) {
      n[gvis.settings.styles].fill = color;
    })

    visualization.update(0, 0);
  }

  this.JSONMessageOutput = function() {


    // remove the previous json message 
    d3.select("#messageBox").selectAll("*").remove();

    d3.select("#messageBox")
    .style("width", $('#viewport').width() + "px")
    .style("height", $(window).height()-button_height+"px")

    d3.select("#messageBox")
    .html("<span> JSON Message " + messageArray.length + " :</span> \n" + syntaxHighlight(messageArray[messageArray.length-1]))
  }

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
        s = s + '&nbsp;&nbsp;<input onkeypress="onkeypress_submit()" type ="text" class="form-control" placeholder="' + toLanguage(elements[i]["textbox"]["placeholder"], "ui_component", "label") + '" name="' + elements[i]["textbox"]["name"] + '" size=' + elements[i]["textbox"]["length"] + '>&nbsp;&nbsp;';
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

  this.updateLabelFilteringListBox = function(graph) {
    // if graph data is empty return.
    if (graph.nodes().length === 0) {
      return;
    }

    // store the previous status of the selectionBoxLabels. The status is like whether a label is selected or not.
    nodelabels = selectionBoxLabels.node;

    // initialize the selectionBoxLabels of node.
    // "__key" is the type for "type" and "id", which is the key of a node and edge.
    selectionBoxLabels.node = {};

    // for each nodes 'type','id' are default for every nodes.
    // Then get what attribtues each type of node has.
    graph.nodes().forEach(function(n){
      if (n.type in selectionBoxLabels.node) {
        ;
      }
      else {
        selectionBoxLabels.node[n.type] = {};
      }

      ['type', 'id'].forEach(function(d, i) {
        if (d in n) {
          if (typeof nodelabels[n.type] == 'object' && d in nodelabels[n.type]) {
            selectionBoxLabels.node[n.type][d] = nodelabels[n.type][d];
          }
          else {
            selectionBoxLabels.node[n.type][d] = false;
          }
        }
        else {
          ;
        }
      })

      for (var key in n[gvis.settings.attrs]) {
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
    if (graph.links().length === 0) {
      ;
    }

    // store the previous status of the selectionBoxLabels of edge. The status is like whether a label is selected or not.
    edgelabels = selectionBoxLabels.edge;
    selectionBoxLabels.edge = {};

    // initialize the selectionBoxLabels of edge.
    // "__key" is the type for "type" and "id", which is the key of a edge.
    // for each nodes 'type','id' are default for every edges.
    // Then get what attribtues each type of edge has.
    graph.links().forEach(function(n){
      if (n.type in selectionBoxLabels.edge) {
        ;
      }
      else {
        selectionBoxLabels.edge[n.type] = {};
      }

      ['type', 'id'].forEach(function(d, i) {
        if (d in n) {
          if (edgelabels[n.type] !== undefined && d in edgelabels[n.type]) {
            selectionBoxLabels.edge[n.type][d] = edgelabels[n.type][d];
          }
          else {
            selectionBoxLabels.edge[n.type][d] = false;
          }
        }
        else {
          ;
        }
      })

      for (var key in n.attr) {
        try {
          if (edgelabels[n.type] !== undefined && key in edgelabels[n.type]) {
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

  //onkeypress="onkeypress_submit()"
  this.onkeypress_submit = function() {

    //enter key
    if (event.keyCode == 13) {
      onclick_submit(page_index);
    }
  }

  this.onclick_submit = function(index) {


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
    // window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
    // edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }


    // for each events create coresponding URL and Payload.
    var submit_URL = "";
    var submit_payload = {};

    for (var key in myObject.events) {
      if (key == "submit") {
        // setting URL for submit button.
        temp_event = myObject.events[key]; 
        submit_URL = temp_event.URL_head;

        URL_attrs =  temp_event.URL_attrs

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
      }
      else {

        // setting URL for other events, such as double left click on nodes.
        // mygv.setURL(myObject, key);
      }
    }

    $.ajax({
      url : submit_URL,
      type : "post",
      data : submit_payload,//JSON.stringify(submit_payload),//'{"function":"vattr", "translate_ids": ["1", "25", "27"]}',
      dataType   : 'json',
      error: function(jqXHR, textStatus, errorThrown){
        // will fire when timeout is reached
        // mygv.clean();
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
          // mygv
          // .data(newData)
          // .run()

          newData = refineDataName(newData);

          visualization
          .read(newData)
          .layout(UIObject.setting.layout)
          .setRootNode(rootNodeType, rootNodeID)
          .render()

          // update the multi-selection lable box base on the graph data.
          updateLabelFilteringListBox(visualization.data());

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

    $('#tableBox').height($('#left_side').height() - $('#layoutport').height() - $('#queryInputInfoBox').height() - 20)
  }

  this.layoutChanged = function(newLayout) {
    visualization
    .layout(newLayout)
    .render(1000, 500)
  }


  window.page_index = 0;
  window.pages_obj = config; 
  //sort on index
  window.pages_obj.sort(function(a,b) {return (a.index - b.index);});

  for (var i=0; i < window.pages_obj.length; i++) {
    var tabname = window.pages_obj[i].tabname;
    var removedSpaces = removeSpaces(tabname);
    tabline = '<li><a href="#" id="' + removedSpaces  + '" name="'+removedSpaces+'_'+ i + '">' + toLanguage(tabname, 'tab', 'tab name') + '</a></li>';
    
    window.pages_obj[removedSpaces] = '' + generateForm(window.pages_obj[i]) + '';

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

      var UIObject = window.pages_obj[window.page_index];

      initializeUIComponent(UIObject);
    })

    if (i == 0) {
      //Index 0 is activated by default
      $('#content-page').empty();
      $('#content-page').append(window.pages_obj[removedSpaces]);

      // initialize the multi selection list box for displaying labels of nodes and edges.
      $('#node_label_filtering').multiselect(nodeLabelFiltering);
      $('#edge_label_filtering').multiselect(edgeLabelFiltering);

      UIObject = window.pages_obj[i]
      
      // mygv.clean()
      // layoutChanged(UIObject.setting.layout)
      // mygv.edgeDirectedType(UIObject.setting.edgeDirectedType)
      // mygv.preDefinition(UIObject.initialization)

      //window.selectionBoxLabels = mygv.label();
      //window.selectionBoxLabels = {node:{"__key":{'type':filteringStatus.node.type, 'id':filteringStatus.node.id}}, edge:{"__key":{'type':filteringStatus.edge.type, 'id':filteringStatus.edge.id}} }
      // window.selectionBoxLabels = {node:{"__key":{'type':checkUndefinedForBool(filteringStatus.node.type), 'id':checkUndefinedForBool(filteringStatus.node.id)}}, 
      //               edge:{"__key":{'type':checkUndefinedForBool(filteringStatus.edge.type), 'id':checkUndefinedForBool(filteringStatus.edge.id)}} }
      
      $('#layoutType').val(UIObject.setting.layout);
      initializeUIComponent(UIObject);

      updateQueryInputInfoBox({}, page_index);
    }

    $("#navbar ul").append(tabline);  
  }

  this.outputTableforSelectedElements = function(nodes, links) {
    var selectedNodes = nodes;
    var selectedEdges = links;

    var result = ''

    if (selectedNodes.length == 0 && selectedEdges.length == 0) {
      result += '<span style="color:#636363;font-size:15px;font-family: Times">'+toLanguage('No Selected Node', "ui_component", "label")+'</span>\n';
      return result;
    }

    for (var node in selectedNodes) {
      node = selectedNodes[node];

      var nodeItem = '<table class="table table-hover table-bordered" style="table-layout: fixed;word-wrap: break-word">';
      nodeItem += '<tr name="element_name" onclick="onClick_table_head(this,\'node\',\''
        +node.id+'\',\''+node.type
        +'\')"><td style="color: #fff;background-color: #337ab7;border-color: #2e6da4;width:25%;line-height:1.0;padding:4px">' 
        +toLanguage('Node Name', "ui_component", "label")+'</td><td style="width:75%;line-height:1.0;padding:4px">'
        +toLanguage(node.id)+'</td></tr>';
      //nodeItem += '<tr><th>Attribute Name</th><th>Value</th></tr>'; //style="color: #fff;background-color: #337ab7;border-color: #2e6da4;"

      nodeItem += '<tr name="attribute_name" onclick="onClick_table(this,\'node\')"><td style="line-height:1.0;padding:4px">'
        + toLanguage('type', "ui_component", "label") +'</td><td style="line-height:1.0;padding:4px">' 
        + toLanguage(node.type) + '</td></tr>'
      for (var key in Object.keys(node[gvis.settings.attrs])) {
        key = Object.keys(node[gvis.settings.attrs])[key];
        nodeItem += '<tr name="attribute_name" onclick="onClick_table(this,\'node\')"><td style="line-height:1.0;padding:4px">'
        + toLanguage(key, node.type) +'</td><td style="line-height:1.0;padding:4px">' 
        + toLanguage(node[gvis.settings.attrs][key], node.type, key) + '</td></tr>'
      }

      nodeItem += '</table>';
      result += nodeItem;
      
    }

    for (var link in selectedEdges) {
      link = selectedEdges[link]

      var linkItem = '<table class="table table-hover table-bordered" style="table-layout: fixed;word-wrap: break-word">';
      linkItem += '<tr name="element_name" onclick="onClick_table_head(this,\'link\',\''
        +link.source.id+'\',\''+link.source.type+'\',\''+link.target.id+'\',\''+link.target.type+'\',\''+link.type+
        '\')"><td style="color: #fff;background-color: #5bc0de;border-color: #46b8da;width:25%;line-height:1.0;padding:4px">'+toLanguage('Link Name', "ui_component", "label")+'</td><td style="width:75%;line-height:1.0;padding:4px">'+link.source.id+'=>'+link.target.id+'</td></tr>';
      //linkItem += '<tr><th>Attribute Name</th><th>Value</th></tr>';
      linkItem += '<tr name="attribute_name" onclick="onClick_table(this,\'link\')"><td style="line-height:1.0;padding:4px">'+toLanguage('type', "ui_component", "label")+'</td><td style="line-height:1.0;padding:4px">' + toLanguage(link.type) + '</td></tr>';

      for (var key in Object.keys(link[gvis.settings.attrs])) {
        key = Object.keys(link[gvis.settings.attrs])[key];

        if (link[gvis.settings.attrs][key] instanceof Array) {
          link[gvis.settings.attrs][key] = link[gvis.settings.attrs][key].join(",<br>");
        }

        linkItem += '<tr name="attribute_name" onclick="onClick_table(this,\'link\')"><td style="line-height:1.0;padding:4px">'+toLanguage(key, link.type)+'</td><td style="line-height:1.0;padding:4px">' + toLanguage(link[gvis.settings.attrs][key],link.type, key) + '</td></tr>'
      }

      linkItem += "</table>"
      result += linkItem;
    }

    return result;
  }

  //initialize the color picker.
  // set the colorchange event for changing the color of selected nodes and edges.
  $('#colorpicker').colorpicker({
      color: "#1f77b4"  //bodyStyle.backgroundColor
  }).on('changeColor', function(ev) {
      //bodyStyle.backgroundColor = ev.color.toHex();
      coloringNodes(ev.color.toHex())
  });

  // window.addEventListener('resize', function() {
  //   console.log('resizing')
  //   d3.select('#prototype1').style('height', $(window).height()-220+'px');
  //   visualization.update(0, 0);
  // });
});


function test() {
    var data = {
    "nodes": [
      {"id": "0", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 100, "shape": "square", "fill": "#ff5500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "1", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 100, "shape": "circle", "fill": "#005500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.8}},
      {"id": "2", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "cross", "fill": "#0055ff", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.7}},
      {"id": "3", "type": "movie", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "circle", "fill": "#ff5500", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.6}},
      {"id": "4", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "diamond", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "5", "type": "movie", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "circle", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}},
      {"id": "6", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10,"shape": "cross", "fill": "#E377C2", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.5}},
      {"id": "7", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 20, "shape": "circle", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.4}},
      {"id": "8", "type": "usr", "_attrs": {"a1": 1, "a2": 2, "a3": 4, "a4": 5}, "_styles": {"size": 60, "shape": "square", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}},
      {"id": "9", "type": "tag", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "diamond", "fill": "#1F77B4", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.9}},
      {"id": "10", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "square", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.8}},
      {"id": "11", "type": "movie", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 10, "shape": "diamond", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.7}},
      {"id": "12", "type": "usr", "_attrs": {"a1": 1, "a2": 2}, "_styles": {"size": 60, "shape": "cross", "fill": "#2CA02C", "stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.6}}],
    "links": [
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "aaa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "bbb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "1", "type": "tag"}, "type": "ccc", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "2", "type": "usr"}, "type": "aaa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "2", "type": "usr"}, "type": "bbb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "3", "type": "movie"}, "type": "dds", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": true},
      {"source": {"id": "4", "type": "tag"}, "target": {"id": "0", "type": "usr"}, "type": "adx", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "0", "type": "usr"}, "type": "fdd", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": false},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "6", "type": "usr"}, "type": "eew", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.5}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "3", "type": "movie"}, "type": "eer", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.4}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "4", "type": "tag"}, "type": "dff", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.3}, "directed": false},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "1", "type": "tag"}, "type": "sdf", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": true},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "6", "type": "usr"}, "type": "sdd", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "4", "type": "tag"}, "type": "eeq", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": false},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "5", "type": "movie"}, "type": "cca", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "6", "type": "usr"}, "type": "dfa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": true},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "5", "type": "movie"}, "type": "ccq", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.5}, "directed": false},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "6", "type": "usr"}, "type": "ccb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": true, "opacity": 0.4}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "6", "type": "usr"}, "type": "ccr", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.3}, "directed": true},
      {"source": {"id": "0", "type": "usr"}, "target": {"id": "7", "type": "tag"}, "type": "ccw", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#2CA02C", "strokeWidth": 1, "dashed": true, "opacity": 1}, "directed": false},
      {"source": {"id": "1", "type": "tag"}, "target": {"id": "8", "type": "usr"}, "type": "ssa", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.9}, "directed": true},
      {"source": {"id": "2", "type": "usr"}, "target": {"id": "9", "type": "tag"}, "type": "ssb", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#E377C2", "strokeWidth": 1, "dashed": true, "opacity": 0.8}, "directed": true},
      {"source": {"id": "3", "type": "movie"}, "target": {"id": "10", "type": "usr"}, "type": "bbc", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": false, "opacity": 0.7}, "directed": false},
      {"source": {"id": "4", "type": "tag"}, "target": {"id": "11", "type": "movie"}, "type": "bbs", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#1F77B4", "strokeWidth": 1, "dashed": true, "opacity": 0.6}, "directed": true},
      {"source": {"id": "5", "type": "movie"}, "target": {"id": "12", "type": "usr"}, "type": "bbc", "_attrs": {"aa1": 3, "aa2": 4}, "_styles": {"stroke": "#ccc", "strokeWidth": 1, "dashed": true, "opacity": 0.5}, "directed": true}]
  };


  d3.select('#prototype1').style('height', ($(window).height()-220)+'px');

  this.test = new gvis({
    configure: config,
    container: 'prototype1',
    //render_type: 'svg',
  });


  // for (var node in data.nodes) {
  //   test.addNode(data.nodes[node]);
  // }

  // for (var link in data.links) {
  //   test.addLink(data.links[link]);
  // }

  test
  .read(data)
  .layout('circle')
  //.layout('tree')
  //.layout('force')
  //.render()

  test.addLayout('cluster', function() {
    var size = this.createTreeBFS();

    var rootNode = this._graph.nodes(this._rootNodeKey);
    rootNode.x = gvis.settings.domain_width / 2.0;
    rootNode.y = gvis.settings.domain_height / 2.0;

    var layoutNodes = d3.layout.cluster()
    .size([size[0] * gvis.settings.domain_width / 4.0, size[1] * gvis.settings.domain_height / 4.0])
    .separation(function(a, b) {
      return (a.parent == b.parent ? 1 : 2) / a.depth / a.depth;
    })
    .nodes(this._tree)

    layoutNodes.forEach(function(node) {
      node.node.x = node.x
      node.node.y = node.y
    })
  })


  test.showNodeLabel('tag', '1', ['type', 'a1', 'a2', 'asdf']);
  test.showNodeLabelByType('usr', ['id', 'a1', 'a2']);

  test.showLinkLabel('tag', '4', 'movie', '11', 'bbs', ['type', 'aa1', 'aa2']);
  test.showLinkLabelByType('bbc', ['aa1', 'aa2', '11123']);

  test.setNodeToolTipFormat(function(type, id, attrs) {
    var template = '<span style="color:{{color}}">{{key}}{{value}}</span><br />'; 

    var result = '<span style="color:#ff0000">Node</span><br />';

    result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'', value:type + '-' + id});

    for (var key in attrs) {
      result += gvis.utils.applyTemplate(template, {color:'#99d8c9', key:key+':', value:attrs[key]})
    }
    
    return result;
  })

  test.setLinkToolTipFormat(function(type, attrs) {
    var template = '<span style="color:{{color}}">{{key}}{{value}}</span><br />'; 

    var result = '<span style="color:#ff0000">Link</span><br />';

    result += gvis.utils.applyTemplate(template, {color:'#fec44f', key:'', value:type});

    for (var key in attrs) {
      result += gvis.utils.applyTemplate(template, {color:'#99d8c9', key:key+':', value:attrs[key]})
    }
    
    return result;
  })

  test.render(1000, 500);

  test
  .on('nodeClick', function(d, renderer) {
    console.log('customized click', d, renderer)
  })
  .on('nodeDblClick', function(d, renderer) {
    console.log('customized dblclick', d)
  })
  .on('linkClick', function(d, renderer) {
    console.log('customized click', d)
  })
  .on('linkDblClick', function(d, renderer) {
    console.log('customized dblclick', d)
  })
  .on('multiSelect', function(nodes, links, renderer) {
    console.log('multiSelect : ', nodes.length, ',', links.length);
  })

  window.addEventListener('resize', function() {
    console.log('resizing')
    d3.select('#prototype1').style('height', $(window).height()-220+'px');
    test.update(0);
  });
}