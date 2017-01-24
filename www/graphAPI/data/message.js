var UIpages_JSON = [ 
      {
        "tabname": "YeePay Fraud",
        "index": 0,
        "elements": [ { "label": "IP", "name" : "id", "length" : 20},
            { "label": "Depth", "name": "depth", "length" : 20}
        ],
        "attributes": {
          "depth" : 1
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {"0":"undirected"}
        },
        "initialization":{
          "coloring" : []
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/check_ip",
            "URL_attrs" : {
              "vars" : [
                {"attr":"id", "usage":"input", "name":"id"}
              ],
              "maps" : [
                {"attr":"depth", "usage":"input", "name":"depth"},
                {"attr":"starttime", "usage":"input", "name":"starttime"},
                {"attr":"endtime", "usage":"input", "name":"endtime"}
              ]  
            }
          },
          "node_dblclick" : {
            "URL_head" : "/engine/check_ip",
            "URL_attrs" : {
              "vars" : [
                {"attr":"id", "usage":"select", "name":"id"}
              ],
              "maps" : [
                {"attr":"depth", "usage":"attributes", "name":"depth"},
                {"attr":"starttime", "usage":"input", "name":"starttime"},
                {"attr":"endtime", "usage":"input", "name":"endtime"}
              ] 
            }
          }
        } 
      }]
      /*
      {
        "tabname": "YeePay Securty Log",
        "index": 1,
        "elements": [ { "label": "IP", "name" : "id", "length" : 15},
            { "label": "Start Time", "name": "starttime", "length" : 25},
            { "label": "End Time", "name" : "endtime", "length" : 25}
        ],
        "attributes": {
          "depth" : 1
        },
        "setting" : {
          "layout" : "circle",
          "edgeDirectedType" : {"0":"undirected"}
        },
        "initialization":{
          "coloring" : []
        },
        "events" : {
          "submit" : {
            "URL_head" : "/engine/check_ip",
            "URL_attrs" : {
              "vars" : [
                {"attr":"id", "usage":"input", "name":"id"}
              ],
              "maps" : [
                {"attr":"depth", "usage":"input", "name":"depth"},
                {"attr":"starttime", "usage":"input", "name":"starttime"},
                {"attr":"endtime", "usage":"input", "name":"endtime"}
              ]  
            }
          },
          "node_dblclick" : {
            "URL_head" : "/engine/check_ip",
            "URL_attrs" : {
              "vars" : [
                {"attr":"id", "usage":"select", "name":"id"}
              ],
              "maps" : [
                {"attr":"depth", "usage":"attributes", "name":"depth"},
                {"attr":"starttime", "usage":"input", "name":"starttime"},
                {"attr":"endtime", "usage":"input", "name":"endtime"}
              ] 
            }
          }
        } 
      }
    ]
    */