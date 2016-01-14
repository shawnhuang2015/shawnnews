var config = 
[ 
{
        "tabname": "Grid State Network",
        "index": 0,
        "elements": [ 
            { "label": { "name": "ID" } }, 
            { "textbox": {"name" : "id", "length" : 20, "placeholder": "input id"} },
            { "button": {"name" : "level", "label" : "Color Level"}},
            { "button": {"name" : "percentage", "label" : "Color Max Percentage"}},
        ],
        "attributes": {
          "depth" : 1,
          "max" : 1,
          "type" : "BUS"
        },
        "setting" : {
          "customized" : true,
          "layout" : "map",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
          ],
          "urlTemplate": "/engine/query/subgraph?minLevel={{minlevel}}&latMin={{latmin}}&lonMin={{lonmin}}&latMax={{latmax}}&lonMax={{lonmax}}"
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
                {"attr":"depth", "usage":"attributes", "name":"depth"},
                {"attr":"max", "usage":"attributes", "name":"max"}
              ] 
            }
          }
        } 
      },
      {
        "tabname": "Bus Lookup",
        "index": 1,
        "elements": [ 
            { "label": { "name": "ID" } }, { "textbox": {"name" : "id", "length" : 20, "placeholder": "input id"} },
            // { "label": { "name" : "Type"} }, { "textbox": {"name" : "type", "length" : 20, "placeholder": "input type" } },
            { "label": { "name": "Depth" } }, { "textbox": {"name" : "depth", "length" : 10, "placeholder": "1" } },
            { "label": { "name": "Max" } }, { "textbox": {"name" : "max", "length" : 10, "placeholder": "50" } }
        ],
        "attributes": {
          "depth" : 1,
          "max" : 35,
          "type" : "BUS"
        },
        "setting" : {
          "customized" : false,
          "layout" : "map",
          "edgeDirectedType" : {}
        },
        "initialization":{
          "coloring" : [
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