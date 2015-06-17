!function() {
console.log("======> ui main start");
  var app = angular.module('visualization.ui', ['ui.bootstrap']).controller('UIpages', function ($scope, $window) {      
    $scope.tabs = UIpages_JSON
    $scope.index = 0;
    $scope.object = $scope.tabs[0];
    $scope.input = {};
    $scope.graphMenu = {};

    $scope.graphMenu.node = {labels:["a", "b", "c"]};
    $scope.graphMenu.edge = {labels:["a", "b", "c"]};
    $scope.graphMenu.selections = ["Highlight", "Coloring", "Deleting", "Hiding"];
    $scope.graphMenu.layouts = ["Force Layout", "Hierarchical Layout", "Tree Layout", "Circle Layout"];
    $scope.graphMenu.visualization = ["Graph", "JSON", "Summary", "More"];

    $scope.initializeUI = function(tab, index) {
      $scope.input = {};
      $scope.index = index;
      $scope.object = $scope.tabs[$scope.index];
    }

    $scope.on_submit = function($http) {
      var object = $scope.object;
      var input = $scope.input;
      
      for (var key in object.events) {
        if (key == "submit") {
          temp_event = object.events[key]; 
          submit_URL = temp_event.URL_head;
          URL_attrs =  temp_event.URL_attrs;

          for (var attr in URL_attrs.vars) {
            submit_URL += "/" + input[URL_attrs.vars[attr].name];
          }

          submit_URL += "?";
          
          for (var attr in URL_attrs.maps) {
            name = URL_attrs.maps[attr].attr;
            attr = URL_attrs.maps[attr];

            //if (name == "id") continue;

            if (attr.usage == "input") {
              if (input[attr.name]==undefined) {
                ; 
              }
              else{
                if (name == "type") {
                  submit_URL += name + "=" + input[attr.name].toLowerCase() +"&";
                }
                else {
                  submit_URL += name + "=" + input[attr.name]/*.toLowerCase()*/ +"&";
                }                        
              }               
            }
            else if (attr.usage == "attributes") {
              submit_URL += name+ "=" + myObject.attributes[attr.name] + "&";
            }
          }

          $.get(submit_URL, function(data) {
              console.log(data + "success");
          });
        }
      }
    }
  });

console.log("======> ui main end ");
}()