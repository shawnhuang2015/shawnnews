var UIpages = 
[ 
      {
        "tabname": "Customer 360", //用户全景
        "index": 0,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} }
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 35,
          "type": "phone_no",
          "call_times":2,
          "bypass_number":3, //bypass_number表示保留多少个越级投诉
          "case_number":3 //表示对于每个srtype，保留多少个complaint
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             {"selection":"is_warning == 'Yes'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             {"selection":"is_warning == 'Yes'", "type":"r", "selectionType":"nodes", "value":"10"},
             {"selection":"id == 'Complaint_Type_A'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             {"selection":"id == 'Complaint_Type_A'", "type":"r", "selectionType":"nodes", "value":"8"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/client_complaint_v2",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"pid_id", "usage":"input", "name":"id"},
                {"attr":"call_times", "usage":"attributes", "name":"call_times"},
                {"attr":"bypass_number", "usage":"attributes", "name":"bypass_number"},
                {"attr":"case_number", "usage":"attributes", "name":"case_number"}
              ],
              "payload" : []
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
        }
      },
      {
        "tabname": "Phone Fraud",
        "index": 1,
        "elements": [ 
            { "label": { "name": "Source Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} },
            { "label": { "name": "Target Phone Number" } }, 
            { "textbox": {"name" : "tid", "length" : 20, "placeholder": "input phone number"} }
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 35,
          "type": "phone_no",
          "depth":2,
          "min_call_numbers":2,
          "min_time_stamp":0,
          "max_time_stamp":2147483647,
          "max_output_size":50,

        },
        "setting" : {
          "layout" : "circle", //"hierarchical",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             {"selection":"has_friend != undefined && has_friend == 0", "type":"fill", "selectionType":"nodes", "value":"#e34a33"},
             {"selection":"has_friend != undefined && has_friend == 1", "type":"fill", "selectionType":"nodes", "value":"#31a354"},
             {"selection":"type == 'no_shortest_path'", "type":"stroke", "selectionType":"edges", "value":"#000000"},
             {"selection":"type == 'no_shortest_path'", "type":"initialized", "selectionType":"edges", "value":false},
             // {"selection":"id == 'Complaint_Type_A'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             // {"selection":"id == 'Complaint_Type_A'", "type":"r", "selectionType":"nodes", "value":"8"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/phone_fraud",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"pid_id", "usage":"input", "name":"id"},
                {"attr":"tgt_pid", "usage":"input", "name":"tid"},
                {"attr":"min_call_numbers", "usage":"attributes", "name":"min_call_numbers"},
                {"attr":"depth", "usage":"attributes", "name":"depth"},
                {"attr":"min_time_stamp", "usage":"attributes", "name":"min_time_stamp"},
                {"attr":"max_time_stamp", "usage":"attributes", "name":"max_time_stamp"},
                {"attr":"max_output_size", "usage":"attributes", "name":"max_output_size"}
              ],
              "payload" : []
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
        }
      },
      {
        "tabname": "Recommandation",
        "index": 2,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} }
            /*,
            { "slider": {
                        "name" : "time", 
                        "length" : 130, 
                        "value": {
                                    'ticks': [0, 8, 24, 24*7, 24*14],
                                    'ticks_labels': ['0', '8h', '1d', '1w', '1m'],
                                    'ticks_positions': [0, 20, 50, 90, 100],
                                    'ticks_snap_bounds': 2,
                                    'value': 24
                                  }
                        } 
            }
            */
        ],
        "attributes": {
          "type": "phone_no",
          "recommandation":"contacts",
          "click_depth" : 1,
          "click_max" : 35,
          "limit": 3,
          "cached": false,
          "refresh": true
        },
        "setting" : {
          "layout" : "tree",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
            {"selection":"score > 8", "type":"r", "selectionType":"nodes", "value":"11"},
            {"selection":"score > 8", "type":"fill", "selectionType":"nodes", "value":"#e31a1c"},
            {"selection":"score > 6 && score <= 8", "type":"r", "selectionType":"nodes", "value":"10"},
            {"selection":"score > 6 && score <= 8", "type":"fill", "selectionType":"nodes", "value":"#fc4e2a"},
            {"selection":"score > 4 && score <= 6", "type":"r", "selectionType":"nodes", "value":"9"},
            {"selection":"score > 4 && score <= 6", "type":"fill", "selectionType":"nodes", "value":"#fd8d3c"},
            {"selection":"score > 2 && score <= 4", "type":"r", "selectionType":"nodes", "value":"8"},
            {"selection":"score > 2 && score <= 4", "type":"fill", "selectionType":"nodes", "value":"#feb24c"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/builder_multiple",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"cached", "usage":"attributes", "name":"cached"},
                {"attr":"refresh", "usage":"attributes", "name":"refresh"}
              ],
              "payload" : [
                {
                  "attr" : [
                    {"attr":"type", "usage":"attributes", "name":"type"},
                    {"attr":"limit", "usage":"attributes", "name":"limit"},
                    {"attr":"id", "usage":"input", "name":"id"}
                  ],
                  "usage" : "post",
                  "template" : '{"query_group":[{"select_attributes":["name"],"alg_control":[ {"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_device","weight":1.0}], "filters":{"hub_sample":200}}, {"start_nodes":[], "edge_types":[{"type":"device_app", "weight":1.0}], "filters":{"hub_sample":200,"limit":10}}, {"start_nodes":[], "edge_types":[{"type":"device_app", "weight": 1.0}], "filters":{"hub_sample":200,"limit":10, "not-connected":{"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_device"}]}}}, {"start_nodes":[], "edge_types":[{"type":"device_app", "weight": 1.0}], "filters":{"hub_sample":200,"limit":10}}], "query_name": "CF"}, {"select_attributes":["name"],"alg_control":[ {"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_phone_no","weight":1.0}], "filters":{"hub_sample":200}}, {"start_nodes":[], "edge_types":[{"type":"phone_no_device", "weight":1.0}], "filters":{"hub_sample":200,"limit":10, "not-connected":{"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_device"}]}}}, {"start_nodes":[], "edge_types":[{"type":"device_app", "weight": 1.0}], "filters":{"hub_sample":200,"limit":10}}], "query_name": "contact"}, {"select_attributes":["name"],"alg_control":[ {"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_device","weight":1.0}], "filters":{"hub_sample":200}}, {"start_nodes":[], "edge_types":[{"type":"device_base_station", "weight":1.0}], "filters":{"hub_sample":200,"limit":10}}, {"start_nodes":[], "edge_types":[{"type":"device_base_station", "weight": 1.0}], "filters":{"hub_sample":200,"limit":10, "not-connected":{"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_device"}]}}}, {"start_nodes":[], "edge_types":[{"type":"device_app", "weight": 1.0}], "filters":{"hub_sample":200,"limit":10}}], "query_name": "location"}], "combine_array": [0, 1, 2]}',
                  "template_location" : '{"alg_control":[ {"start_nodes":[{"id":"{{id}}", "type":"{{type}}"}], "edge_types":[{"type":"phone_no_phone_no","weight":1.0}], "filters":{"hub_sample":200}}, {"start_nodes":[], "edge_types":[{"type":"phone_no_device", "weight":1.0}], "filters":{"hub_sample":200,"limit":10, "not-connected":{"start_nodes":[{"id":"phone_no_1", "type":"phone_no"}], "edge_types":[{"type":"phone_no_device"}]}}}, {"start_nodes":[], "edge_types":[{"type":"device_app", "weight": 1.0}], "filters":{"hub_sample":200,"limit":{{limit}}}}]}'  
                }
                //{"attr":["type","id"], "usage":"array_input", "name":"input"}
              ]
            }
          }
          /*,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
          */
        }
      },
      {
        "tabname": "Complaint",
        "index": 3,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} }
            /*,
            { "slider": {
                        "name" : "time", 
                        "length" : 130, 
                        "value": {
                                    'ticks': [0, 8, 24, 24*7, 24*14],
                                    'ticks_labels': ['0', '8h', '1d', '1w', '1m'],
                                    'ticks_positions': [0, 20, 50, 90, 100],
                                    'ticks_snap_bounds': 2,
                                    'value': 24
                                  }
                        } 
            }
            */
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 35,
          "type": "phone_no",
          "call_times":2
        },
        "setting" : {
          "layout" : "hierarchical",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             {"selection":"is_warning == 'Yes'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             {"selection":"is_warning == 'Yes'", "type":"r", "selectionType":"nodes", "value":"10"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/client_complaint",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"pid_id", "usage":"input", "name":"id"},
                {"attr":"call_times", "usage":"attributes", "name":"call_times"}
              ],
              "payload" : []
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
        }
      },
      {
        "tabname": "Potential",
        "index": 4,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} }
            /*,
            { "slider": {
                        "name" : "time", 
                        "length" : 130, 
                        "value": {
                                    'ticks': [0, 8, 24, 24*7, 24*14],
                                    'ticks_labels': ['0', '8h', '1d', '1w', '1m'],
                                    'ticks_positions': [0, 20, 50, 90, 100],
                                    'ticks_snap_bounds': 2,
                                    'value': 24
                                  }
                        } 
            }
            */
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 35,
          "type": "phone_no",
          "call_times": 3, //call_times表示对于某个UDF朋友的条件，call_times＝3，则表示两个人之间通话次数大于等于3的为好友
          "threshold":4 //threshold=3表示，对于潜在集团的UDF，某一个集团下含有的输入用户的好友的个数大于等于3，则显示这个潜在集团
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/potential_group",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"pid_id", "usage":"input", "name":"id"},
                {"attr":"call_times", "usage":"attributes", "name":"call_times"},
                {"attr":"threshold", "usage":"attributes", "name":"threshold"}
              ],
              "payload" : []
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
        }
      },
      {
        "tabname": "Enterprise",
        "index": 5,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} }
            /*,
            { "slider": {
                        "name" : "time", 
                        "length" : 130, 
                        "value": {
                                    'ticks': [0, 8, 24, 24*7, 24*14],
                                    'ticks_labels': ['0', '8h', '1d', '1w', '1m'],
                                    'ticks_positions': [0, 20, 50, 90, 100],
                                    'ticks_snap_bounds': 2,
                                    'value': 24
                                  }
                        } 
            }
            */
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 35,
          "type": "phone_no",
          "max_member": 15, //在find_group_info这个UDF中，某一个集团下可能含有1000个人，如果让N＝100的话，则只会显示这个集团里的重要的前100个人，其他900个不显示。
          "call_times": 3, //call_times表示对于某个UDF朋友的条件，call_times＝3，则表示两个人之间通话次数大于等于3的为好友
        },
        "setting" : {
          "layout" : "tree",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             // {"selection":"enkey_mark == '1'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             // {"selection":"enkey_mark == '1'", "type":"r", "selectionType":"nodes", "value":"10"},
             {"selection":"importance == '1'", "type":"fill", "selectionType":"nodes", "value":"#ff1100"},
             {"selection":"importance == '1'", "type":"r", "selectionType":"nodes", "value":"7"},
             {"selection":"importance == '3'", "type":"fill", "selectionType":"nodes", "value":"#11ff00"},
             {"selection":"importance == '3'", "type":"r", "selectionType":"nodes", "value":"6"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/find_group_info",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"pid_id", "usage":"input", "name":"id"},
                {"attr":"N", "usage":"attributes", "name":"max_member"},
                {"attr":"call_times", "usage":"attributes", "name":"call_times"}
              ],
              "payload" : []
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
        }
      },
      {
        "tabname": "Graph",
        "index": 6,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} },
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 50,
          "type": "phone_no",
          "query_depth" : 1,
          "query_max" : 500
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             /*
             {"selection":"type == 'transaction_0'", "type":"fill", "selectionType":"nodes", "value":"#3182bd"},
             {"selection":"type == 'client1'", "type":"fill", "selectionType":"nodes", "value":"#ff7f0e"},
             {"selection":"type == 'user2'", "type":"fill", "selectionType":"nodes", "value":"#c5b0d5"},
             {"selection":"type == 'bankcard3'", "type":"fill", "selectionType":"nodes", "value":"#c49c94"},
             {"selection":"type == 'phone4'", "type":"fill", "selectionType":"nodes", "value":"#f7b6d2"},
             {"selection":"type == 'merchantminute5'", "type":"fill", "selectionType":"nodes", "value":"#c7e9c0"},
             {"selection":"type == 'merchanthour6'", "type":"fill", "selectionType":"nodes", "value":"#a1d99b"},
             {"selection":"type == 'merchantdate7'", "type":"fill", "selectionType":"nodes", "value":"#31a354"}
             */
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"attributes", "name":"type"},
                {"attr":"id", "usage":"input", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"query_depth"},
                {"attr":"max", "usage":"attributes", "name":"query_max"}
              ]  
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }  
        } 
      },
      {
        "tabname": "Realtime", //实时用户全景
        "index": 7,
        "elements": [ 
            { "label": { "name": "Phone Number" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input phone number"} }
        ],
        "attributes": {
          "click_depth" : 1,
          "click_max" : 35,
          "type": "phone_no",
          "call_times":2,
          "bypass_number":3, //bypass_number表示保留多少个越级投诉
          "case_number":3 //表示对于每个srtype，保留多少个complaint
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             {"selection":"is_warning == 'Yes'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             {"selection":"is_warning == 'Yes'", "type":"r", "selectionType":"nodes", "value":"10"},
             {"selection":"id == 'Complaint_Type_A'", "type":"fill", "selectionType":"nodes", "value":"#ff0000"},
             {"selection":"id == 'Complaint_Type_A'", "type":"r", "selectionType":"nodes", "value":"8"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/client_complaint_v2",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"pid_id", "usage":"input", "name":"id"},
                {"attr":"call_times", "usage":"attributes", "name":"call_times"},
                {"attr":"bypass_number", "usage":"attributes", "name":"bypass_number"},
                {"attr":"case_number", "usage":"attributes", "name":"case_number"}
              ],
              "payload" : []
            }
          }
          ,
          "node_dblclick" : {
            "URL_head" : "/engine/kstepsubgraph",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
                {"attr":"depth", "usage":"attributes", "name":"click_depth"},
                {"attr":"max", "usage":"attributes", "name":"click_max"}
              ] 
            }
          }
        }
      }
    ]

var example_chart1 = '<svg width="300" height="150"><g class="rgroups" transform="translate(40,110)" style="fill: rgb(31, 119, 180);"><rect width="15" style="fill-opacity: 1;" height="50.778688524590166" y="-50.778688524590166" x="0"></rect><rect width="15" style="fill-opacity: 1;" height="38.72950819672131" y="-38.72950819672131" x="18"></rect><rect width="15" style="fill-opacity: 1;" height="50.49180327868852" y="-50.49180327868852" x="37"></rect><rect width="15" style="fill-opacity: 1;" height="24.959016393442624" y="-24.959016393442624" x="55"></rect><rect width="15" style="fill-opacity: 1;" height="1.1475409836065609" y="-1.147540983606575" x="73"></rect></g><g class="rgroups" transform="translate(40,110)" style="fill: rgb(44, 160, 44);"><rect width="15" style="fill-opacity: 1;" height="11.475409836065573" y="-62.25409836065573" x="0"></rect><rect width="15" style="fill-opacity: 1;" height="10.9016393442623" y="-49.63114754098362" x="18"></rect><rect width="15" style="fill-opacity: 1;" height="15.204918032786885" y="-65.69672131147541" x="37"></rect><rect width="15" style="fill-opacity: 1;" height="5.163934426229503" y="-30.122950819672127" x="55"></rect><rect width="15" style="fill-opacity: 1;" height="0.5737704918032733" y="-1.7213114754098342" x="73"></rect></g><g class="rgroups" transform="translate(40,110)" style="fill: rgb(255, 127, 14);"><rect width="15" style="fill-opacity: 1;" height="1.1475409836065609" y="-63.40163934426231" x="0"></rect><rect width="15" style="fill-opacity: 1;" height="2.5819672131147513" y="-52.213114754098356" x="18"></rect><rect width="15" style="fill-opacity: 1;" height="4.3032786885245855" y="-70" x="37"></rect><rect width="15" style="fill-opacity: 1;" height="1.4344262295081904" y="-31.557377049180317" x="55"></rect><rect width="15" style="fill-opacity: 1;" height="0.5737704918032733" y="-2.2950819672130933" x="73"></rect></g><g class="x axis" transform="translate(40,110)"><g class="tick" transform="translate(31,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">21</text></g><g class="tick" transform="translate(67,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">23</text></g><g class="tick" transform="translate(104,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">25</text></g><g class="tick" transform="translate(141,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">27</text></g><g class="tick" transform="translate(177,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">01</text></g><g class="tick" transform="translate(214,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">03</text></g><path class="domain" d="M0,6V0H220V6"></path></g><g class="y axis" transform="translate(40,40)"><g class="tick" transform="translate(0,70)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">0</text></g><g class="tick" transform="translate(0,55.65573770491803)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">50</text></g><g class="tick" transform="translate(0,41.31147540983606)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">100</text></g><g class="tick" transform="translate(0,26.9672131147541)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">150</text></g><g class="tick" transform="translate(0,12.62295081967213)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">200</text></g><path class="domain" d="M-6,0H0V70H-6"></path></g><g class="legend" x="195" y="25" height="100" width="100"><g><rect x="195" y="10" width="10" height="10" style="fill: rgb(31, 119, 180);"></rect><text x="210" y="20" height="30" width="100" style="fill: rgb(31, 119, 180);">APP</text></g><g><rect x="195" y="35" width="10" height="10" style="fill: rgb(44, 160, 44);"></rect><text x="210" y="45" height="30" width="100" style="fill: rgb(44, 160, 44);">网页</text></g><g><rect x="195" y="60" width="10" height="10" style="fill: rgb(255, 127, 14);"></rect><text x="210" y="70" height="30" width="100" style="fill: rgb(255, 127, 14);">彩信</text></g></g><text transform="rotate(-90)" y="-5" x="-75" dy="1em" style="font-size: 14px;">流量总数</text><text class="xtext" x="110" y="145" text-anchor="middle">天</text><text class="title" x="150" y="20" text-anchor="middle" style="font-size: 9px; text-decoration: underline;">每天使用的流量</text></svg>'
var example_chart2 = '<svg width="320" height="230"><g transform="translate(40,10)"><g class="x axis" transform="translate(0,200)"><g class="tick" transform="translate(13,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">A</text></g><g class="tick" transform="translate(24,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">B</text></g><g class="tick" transform="translate(35,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">C</text></g><g class="tick" transform="translate(46,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">D</text></g><g class="tick" transform="translate(57,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">E</text></g><g class="tick" transform="translate(68,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">F</text></g><g class="tick" transform="translate(79,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">G</text></g><g class="tick" transform="translate(90,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">H</text></g><g class="tick" transform="translate(101,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">I</text></g><g class="tick" transform="translate(112,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">J</text></g><g class="tick" transform="translate(123,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">K</text></g><g class="tick" transform="translate(134,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">L</text></g><g class="tick" transform="translate(145,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">M</text></g><g class="tick" transform="translate(156,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">N</text></g><g class="tick" transform="translate(167,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">O</text></g><g class="tick" transform="translate(178,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">P</text></g><g class="tick" transform="translate(189,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">Q</text></g><g class="tick" transform="translate(200,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">R</text></g><g class="tick" transform="translate(211,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">S</text></g><g class="tick" transform="translate(222,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">T</text></g><g class="tick" transform="translate(233,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">U</text></g><g class="tick" transform="translate(244,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">V</text></g><g class="tick" transform="translate(255,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">W</text></g><g class="tick" transform="translate(266,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">X</text></g><g class="tick" transform="translate(277,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">Y</text></g><g class="tick" transform="translate(288,0)" style="opacity: 1;"><line y2="6" x2="0"></line><text dy=".71em" y="9" x="0" style="text-anchor: middle;">Z</text></g><path class="domain" d="M0,6V0H300V6"></path></g><g class="y axis"><g class="tick" transform="translate(0,200)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">0%</text></g><g class="tick" transform="translate(0,184.25444811840654)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">1%</text></g><g class="tick" transform="translate(0,168.5088962368131)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">2%</text></g><g class="tick" transform="translate(0,152.76334435521966)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">3%</text></g><g class="tick" transform="translate(0,137.0177924736262)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">4%</text></g><g class="tick" transform="translate(0,121.27224059203274)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">5%</text></g><g class="tick" transform="translate(0,105.52668871043932)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">6%</text></g><g class="tick" transform="translate(0,89.78113682884583)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">7%</text></g><g class="tick" transform="translate(0,74.0355849472524)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">8%</text></g><g class="tick" transform="translate(0,58.29003306565894)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">9%</text></g><g class="tick" transform="translate(0,42.54448118406549)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">10%</text></g><g class="tick" transform="translate(0,26.798929302472054)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">11%</text></g><g class="tick" transform="translate(0,11.053377420878597)" style="opacity: 1;"><line x2="-6" y2="0"></line><text dy=".32em" x="-9" y="0" style="text-anchor: end;">12%</text></g><path class="domain" d="M-6,0H0V200H-6"></path><text transform="rotate(-90)" y="6" dy=".71em" style="text-anchor: end;">Frequency</text></g><rect class="bar" x="8" width="10" y="71.40607778302628" height="128.59392221697374"></rect><rect class="bar" x="19" width="10" y="176.50763659266258" height="23.49236340733742"></rect><rect class="bar" x="30" width="10" y="156.19587466540702" height="43.80412533459298"></rect><rect class="bar" x="41" width="10" y="133.03416784758306" height="66.96583215241694"></rect><rect class="bar" x="52" width="10" y="0" height="200"></rect><rect class="bar" x="63" width="10" y="163.9741772949142" height="36.0258227050858"></rect><rect class="bar" x="74" width="10" y="168.27271295858918" height="31.72728704141082"></rect><rect class="bar" x="85" width="10" y="104.04660683356953" height="95.95339316643047"></rect><rect class="bar" x="96" width="10" y="90.31648559282002" height="109.68351440717998"></rect><rect class="bar" x="107" width="10" y="197.5909305621162" height="2.409069437883801"></rect><rect class="bar" x="118" width="10" y="187.84443394740987" height="12.155566052590132"></rect><rect class="bar" x="129" width="10" y="136.62415367658636" height="63.37584632341364"></rect><rect class="bar" x="140" width="10" y="162.11620217288615" height="37.88379782711385"></rect><rect class="bar" x="151" width="10" y="93.73327035112582" height="106.26672964887418"></rect><rect class="bar" x="162" width="10" y="81.79814202487796" height="118.20185797512204"></rect><rect class="bar" x="173" width="10" y="169.62683042040624" height="30.373169579593764"></rect><rect class="bar" x="184" width="10" y="198.5041725712486" height="1.4958274287513973"></rect><rect class="bar" x="195" width="10" y="105.73138088490002" height="94.26861911509998"></rect><rect class="bar" x="206" width="10" y="100.37789324515822" height="99.62210675484178"></rect><rect class="bar" x="217" width="10" y="57.4082821602897" height="142.5917178397103"></rect><rect class="bar" x="228" width="10" y="156.57376791056527" height="43.426232089434734"></rect><rect class="bar" x="239" width="10" y="184.6008502598016" height="15.399149740198396"></rect><rect class="bar" x="250" width="10" y="162.84049755943946" height="37.159502440560544"></rect><rect class="bar" x="261" width="10" y="197.63816721776098" height="2.36183278223902"></rect><rect class="bar" x="272" width="10" y="168.91828058573452" height="31.08171941426548"></rect><rect class="bar" x="283" width="10" y="198.8348291607621" height="1.165170839237902"></rect></g></svg>'



