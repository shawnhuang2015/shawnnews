var UIpages = 
[ 
      {
        "tabname": "Explore One Step Neighborhood",
        "index": 0,
        "elements": [ 
            { "label": { "name": "ID" } }, { "textbox": {"name" : "id", "length" : 20 } },
            { "label": { "name" : "Type"} }, { "textbox": {"name" : "type", "length" : 10 } }, 
            /*
            { "label": { "name" : "Type"} }, { "dropdown": {"name": "type", "length" : 10, 
              "value":["transaction", "phone", "user", "bankcard", "merchantdate", "merchanthour", "merchantminute"]}
            },
            */
            { "label": { "name": "Depth" } }, { "textbox": {"name" : "depth", "length" : 10 } },
            { "label": { "name": "Max" } }, { "textbox": {"name" : "max", "length" : 10 } }
        ],
        "attributes": {
          "depth" : 1,
          "max" : 25
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
             /*
             {"selection":"type == 'transaction_0'", "type":"fill", "selectionType":"nodes", "color":"#3182bd"},
             {"selection":"type == 'client1'", "type":"fill", "selectionType":"nodes", "color":"#ff7f0e"},
             {"selection":"type == 'user2'", "type":"fill", "selectionType":"nodes", "color":"#c5b0d5"},
             {"selection":"type == 'bankcard3'", "type":"fill", "selectionType":"nodes", "color":"#c49c94"},
             {"selection":"type == 'phone4'", "type":"fill", "selectionType":"nodes", "color":"#f7b6d2"},
             {"selection":"type == 'merchantminute5'", "type":"fill", "selectionType":"nodes", "color":"#c7e9c0"},
             {"selection":"type == 'merchanthour6'", "type":"fill", "selectionType":"nodes", "color":"#a1d99b"},
             {"selection":"type == 'merchantdate7'", "type":"fill", "selectionType":"nodes", "color":"#31a354"}
             */
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/subgraph1step",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"input", "name":"type"},
                {"attr":"id", "usage":"input", "name":"id"},
                {"attr":"depth", "usage":"input", "name":"depth"},
                {"attr":"max", "usage":"input", "name":"max"}
              ]  
            }
          },
          "node_dblclick" : {
            "URL_head" : "/engine/subgraph1step",
            "URL_attrs" : {
              "vars" : [
              ],
              "maps" : [
                {"attr":"type", "usage":"select", "name":"type"},
                {"attr":"id", "usage":"select", "name":"id"},
               {"attr":"depth", "usage":"attributes", "name":"depth"},
                {"attr":"max", "usage":"attributes", "name":"max"}
              ] 
            }
          }
        } 
      }
    ]