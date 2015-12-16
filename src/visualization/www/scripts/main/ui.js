/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
require([], function(){
  //test();

  var button_height = 230;

  this.messageArray = [];  

  // remove space function.
  this.removeSpaces = function(s) {
    return s.replace(/\s/g, '');
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
        // mygv.rootNode(initRootNode);
        // mygv.targetNode(initTargetNode);
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

          // update the multi-selection lable box base on the graph data.
          //updateLabelFilteringListBox(mygv.data());

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

  // The configure object of the node label multiselecting list box.
  var nodeLabelFiltering = {
    maxHeight: 600,
    buttonWidth: '100%',
    enableClickableOptGroups: true,
    // Call back function once the selection changed.
    buttonText: function(options, select) {
      var labels = [];

      // if (mygv.data().nodes.length != 0)
      //   for (var key in selectionBoxLabels.node){
      //     for (var item in selectionBoxLabels.node[key]) {
      //       selectionBoxLabels.node[key][item] = false;
      //     }   
      //   } 

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

      // mygv.label(selectionBoxLabels);
      // mygv.redraw();

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

      // if (mygv.data().links.length != 0)
      //   for (var key in selectionBoxLabels.edge){
      //     for (var item in selectionBoxLabels.edge[key]) {
      //       selectionBoxLabels.edge[key][item] = false;
      //     }   
      //   }

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

      // mygv.label(selectionBoxLabels);
      // mygv.redraw();

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