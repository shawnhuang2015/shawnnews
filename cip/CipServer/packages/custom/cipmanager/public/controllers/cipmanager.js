'use strict';
angular.module("mean.cipmanager").controller('CipmanagerController', ['$scope','$stateParams','$location', 'Global', '$state', 'Cipmanager', 'CrowdService',
  function($scope, $stateParams,$location, Global, $state, Cipmanager, CrowdService) {

    //Initial variables
    $scope.page_size = 10;
    $scope.totalItems = 1;
    $scope.currentPage = 1;
    $scope.currentUserListPage = 1;

    $scope.global = Global;
    $scope.factors = {};
    $scope.package = {
      name: 'cipmanager'
    };

    $scope.crowd_type = [
      'static',
      'dynamic'
    ];

    ////////////////////////For temp test///////////////////
    $scope.condition_type = [
      'ontology',
      'behavior'
    ];

    $scope.ontology_type = [];

    $scope.factor_tag = [];

    $scope.operator = [
      '>=','<=', ">", "<", "<>", "=", "like", "dislike"
    ];

    $scope.behavior_operator = [
      '>=','<=', ">", "<", "<>", "="
    ];

    $scope.action = [];

    $scope.object_type = [
      'Category',
      'Item'
    ];

    $scope.object_category = [];

    $scope.object_id = [];

    $scope.date_type = [
      'absolute',
      'relative'
    ];

    //////////////////////Widget///////////////////////////////
    $scope.popup1 = {
      opened: false
    };

    $scope.open1 = function() {
      $scope.popup1.opened = true;
    };

    $scope.popup2 = {
      opened: false
    };

    $scope.open2 = function() {
      $scope.popup2.opened = true;
    };

    $scope.dateOptions = {
      formatYear: 'yy',
      startingDay: 1
    };

    //$scope.disabled = function(date, mode) {
    //  return mode === 'day' && date.valueOf() < $scope.startTime.valueOf();
    //};
    //////////////////////Widget///////////////////////////////


    //////////////////////Local function///////////////////////
    $scope.addFactor = function(condition_type) {
      if(!$scope.crowdDetail) {
        $scope.crowdDetail = {};
      }
      if(!$scope.crowdDetail.selector[condition_type]) {
        $scope.crowdDetail.selector[condition_type]=[];
      }
      var param = {};
      param.selector = {};
      param.selector.ontology = [];
      param.selector.behavior = [];
      param.selector[condition_type].push($scope.factors);
      CrowdService.getUserCountByFactor(param, function(data){
        $scope.factors.count = data.length;
        $scope.crowdDetail.selector[condition_type].push($scope.factors);
        $scope.factors = {};
      });
      CrowdService.getUserCountByFactor($scope.crowdDetail, function(data){
        $scope.crowdDetail.count = data.length;
      });
    }

    $scope.deleteFactor = function(factor, index) {
      factor.splice(index,1);
      CrowdService.getUserCountByFactor($scope.crowdDetail, function(data){
        $scope.crowdDetail.count = data.length;
      });
    }

    $scope.resetFactor = function() {
      $scope.factors = {};
    }

    $scope.goToCreate = function () {
      $state.go('create crowd')
    };

    //////////////////////Local function///////////////////////


    ////////////////////////Listener////////////////////////////
    $scope.pageChanged = function() {
      $scope.getCrowdsByPageId($scope.currentPage - 1);
    };

    $scope.pageUserListChanged = function() {
      $scope.getUserListByPageId($scope.currentUserListPage - 1);
    };


    //Ontology related
    $scope.traverseTree = function(tree, callback) {
      var list = [];
      var queue = new Array();
      for(var i in tree){
        var prefix;
        if(queue.length > 0) {
          prefix = queue.pop()+'/';
        } else {
          prefix = '';
        }
        for(var j in tree[i].children) {
          list.push(prefix + tree[i].children[j]);
          queue.unshift(prefix + tree[i].children[j]);
        }
      }
      callback(list);
    }

    $scope.$watch('factors.type', function(nowSelected){
      if(!nowSelected){
        // here we've initialized selected already
        // but sometimes that's not the case
        // then we get null or undefined
        return;
      }
      $scope.factors.factor = '';
      angular.forEach($scope.ontology_data.ontology, function(val){
        if(val.name == nowSelected) {
          $scope.traverseTree(val.tree, function(itemList) {
            $scope.factor_tag = itemList;
          });
        }
      });
    });

    $scope.$watch('factors.action', function(nowSelected){
      if(!nowSelected){
        return;
      }
      $scope.object_category = [];
      $scope.factors.objectCategory = '';
      angular.forEach($scope.ontology_data.behaviour, function(val){
        if(val.name == nowSelected) {
          angular.forEach(val.object, function(object) {
            $scope.object_category.push(object.vtype);
          });
        }
      });
    });

    $scope.$watch('factors.objectCategory', function(nowSelected){
      if(!nowSelected){
        return;
      }
      $scope.factors.object_id = '';
      angular.forEach($scope.ontology_data.object_ontology, function(val){
        if(val.object == nowSelected) {
          $scope.object_id = [];
          angular.forEach(val.ontologies, function(ontology) {
            angular.forEach($scope.ontology_data.ontology, function(val){
              if(ontology == nowSelected) {
                $scope.traverseTree(val.tree, function(itemList) {
                  $scope.object_id.push(itemList);
                });
              }
            });
          });
        }
      });
    });

    $scope.$watch('st', function(time){
      if(!time){
        return;
      }
      if($scope.factors.timeType == 'relative') {
        $scope.factors.startTime = time;
        $scope.factors.endTime = $scope.factors.startTime;
      } else {
        $scope.factors.startTime = time.valueOf();
      }
    });

    $scope.$watch('et', function(time){
      if(!time){
        return;
      }
      $scope.factors.endTime = time.valueOf();
    });

    ////////////////////////Listener////////////////////////////


    ///////////////////////Network query////////////////////////
    //Init the crowds, query first page and total count
    $scope.init = function() {
      Cipmanager.query({pageId:0,pageSz:$scope.page_size},function(crowds) {
        $scope.crowds = crowds;
      });
      CrowdService.getCount(function(data) {
        if (data.success) {
          $scope.totalItems = data.length;
        } else {
          $scope.totalItems = 1;
        }
      });
    };

    $scope.initCreate = function() {
      $scope.crowdDetail = {};
      $scope.crowdDetail.selector = {};
      $scope.crowdDetail.selector.ontology = [];
      $scope.crowdDetail.selector.behavior = [];
      $scope.initOntology();
    }

    $scope.initUserList = function() {
      $scope.findOne();
      CrowdService.getUserList($stateParams.crowdName, 0, $scope.page_size, function(data) {
        if (data.success) {
          $scope.userList = data.userList;
        } else {
          $scope.userList = [];
        }
      });
      CrowdService.getUserCount($stateParams.crowdName, function(data) {
        if (data.success) {
          $scope.userCount = data.length;
        } else {
          $scope.userCount = 1;
        }
      });
    };

    //Get crowds by page id
    $scope.getCrowdsByPageId = function(pageId) {
      Cipmanager.query({pageId:pageId,pageSz:$scope.page_size},function(crowds) {
        $scope.crowds = crowds;
      });
    }

    //Get userlist by page id
    $scope.getCrowdsByPageId = function(pageId) {
      CrowdService.getUserList($stateParams.crowdName, pageId, $scope.page_size, function(crowds) {
        $scope.crowds = crowds;
      });
    }

    //Remove crowd from server
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

    //Get crowd details from server
    $scope.findOne = function() {
      Cipmanager.get({
        crowdName: $stateParams.crowdName
      }, function(crowdDetail) {
        $scope.crowdDetail = crowdDetail;
      });
      $scope.initOntology();
    };

    //Update crowd info to server
    $scope.update = function(isValid) {
      if (isValid) {
        var crowd = $scope.crowdDetail;

        crowd.$update(function() {
          alert("保存成功!");
          $location.path('crowd/' + crowd.crowdName);
        });
      } else {
        alert("数据存在问题");
        $scope.submitted = true;
      }
    };

    //Create crowd to server
    $scope.create = function(isValid) {
      if(isValid) {
        CrowdService.createCrowd($scope.crowdDetail, function(data) {
          if(data.success) {
            alert("创建成功!");
            $location.path('/crowd/'+$scope.crowdDetail.crowdName+'/userlist');
          } else {
            alert("创建失败!");
          }
        });
      } else {
        alert("数据存在问题");
        $scope.submitted = true;
      }

    };

    //Query ontology
    $scope.initOntology = function() {
      CrowdService.getOntology(function (data) {
        if (data.success) {
          //Convert
          $scope.ontology_data = data.content;
          $scope.ontology_type = [];
          $scope.action = [];
          for(var index in data.content.ontology){
            $scope.ontology_type.push(data.content.ontology[index].name);
          }
          for(var index in data.content.behaviour){
            $scope.action.push(data.content.behaviour[index].name);
          }
        }
      });
    };

    ///////////////////////Network query////////////////////////

  }
]);
