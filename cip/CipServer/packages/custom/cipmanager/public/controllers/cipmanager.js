'use strict';

/* jshint -W098 */
angular.module('mean.cipmanager').controller('CipmanagerController', ['$scope','$stateParams', 'Global', '$state', 'Cipmanager',
  function($scope, $stateParams, Global, $state, Cipmanager) {
    $scope.global = Global;
    $scope.factors = {};
    $scope.package = {
      name: 'cipmanager'
    };

    $scope.crowd_type = [
      {name:'static'},
      {name:'dynamic'}
    ];
    $scope.type_selected='static';//id的值，区分类型

    $scope.ontology_type = [
      {name:'demographic',id:'1'},
      {name:'interest',id:'2'},
      {name:'behavior',id:'3'}
    ];
    //$scope.factors.ontology_selected.id='1';//id的值，区分类型

    $scope.profile = [
      'Age',
      'Sex',
      'Income'
    ];

    $scope.operator = [
      '>=', '=', '<='
    ];

    $scope.addGroup = function() {
      if(!$scope.crowdDetail) {
        $scope.crowdDetail = {};
      }
      if(!$scope.crowdDetail.selector) {
        $scope.crowdDetail.selector = new Array();
      }
      $scope.crowdDetail.selector.push(new Array());
    }

    $scope.deleteGroup = function(index) {
      $scope.crowdDetail.selector.splice(index, 1);
    }

    $scope.addFactor = function(group_index, ontology_type) {
      if(!$scope.crowdDetail.selector[group_index][ontology_type]) {
        $scope.crowdDetail.selector[group_index][ontology_type]=[];
      }
      if($scope.crowdDetail.selector.length > group_index) {
        $scope.crowdDetail.selector[group_index][ontology_type].push($scope.factors);
        $scope.factors = {};
      } else {
        alert("Please add a group first");
      }
    }

    $scope.deleteFactor = function(factor, index) {
      factor.splice(index,1);
    }

    $scope.resetFactor = function() {
      $scope.factors = {};
    }

    $scope.find = function() {
      Cipmanager.query(function(crowds) {
        $scope.crowds = crowds;
      });
    };

    $scope.findOne = function() {
      Cipmanager.get({
        crowdName: $stateParams.crowdName
      }, function(crowdDetail) {
        $scope.crowdDetail = crowdDetail;
      });
    };

    $scope.goToCreate = function () {
      $state.go('create crowd')
    };

    $scope.click = function () {
      alert('click');
    };

    $scope.remove = function(crowd) {
      if (crowd) {
        crowd.$remove(function(response) {
          for (var i in $scope.crowds) {
            if ($scope.crowds[i] === crowd) {
              $scope.crowds.splice(i, 1);
            }
          }
          $location.path('crowds');
        });
      }
    }

    $scope.update = function(isValid) {
      if (isValid) {
        var crowd = $scope.crowdDetail;

        crowd.$update(function() {
          alert("Save Success!");
          $location.path('crowd/' + crowd.crowdName);
        });
      } else {
        $scope.submitted = true;
      }
    };

    $scope.create = function(isValid) {
      if (isValid) {
        var crowd = $scope.crowdDetail;

        crowd.$create(function() {
          alert("Create Success!");
          $location.path('crowd/main');
        });
      } else {
        $scope.submitted = true;
      }
    };

  }
]);
