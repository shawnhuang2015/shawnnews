var UIpages = 
[ 
      {
        "tabname": "Graph Stucture Lookup",
        "index": 0,
        "elements": [ 
            { "label": { "name": "ID" } }, { "textbox": {"name" : "id", "length" : 20, "placeholder": "input id"} },
            { "label": { "name" : "Type"} }, { "textbox": {"name" : "type", "length" : 20, "placeholder": "input type" } },
            { "label": { "name": "Depth" } }, { "textbox": {"name" : "depth", "length" : 10, "placeholder": "1" } },
            { "label": { "name": "Max" } }, { "textbox": {"name" : "max", "length" : 10, "placeholder": "500" } }
        ],
        "attributes": {
          "depth" : 1,
          "max" : 35
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
              // {"selection":"isBlacklisted == 'true'", "type":"stroke", "selectionType":"nodes", "value":"#000000"},
              // {"selection":"isBlacklisted == 'true'", "type":"stroke-width", "selectionType":"nodes", "value":"2"},
              // {"selection":"isBlacklisted == 'true'", "type":"background_color", "selectionType":"nodes", "value":"#000"},
              // {"selection":"isBlacklisted == 'true'", "type":"font-color", "selectionType":"nodes", "value":"#fff"}
          ]
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/kstepsubgraph", 
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
            "URL_head" : "/engine/kstepsubgraph", 
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