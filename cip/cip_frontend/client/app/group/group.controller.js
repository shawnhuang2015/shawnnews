'use strict';

angular.module('cipApp')
  .controller('groupCtrl', function ($rootScope, $scope, $state, $translate, $stateParams, $location, groupFactory, crowdFactory) {
    $rootScope.group = $scope;
    $scope.message = 'Hello';

    // config of uib-pagination elements.
    $scope.page = {
      page_size : 10,
      current_page : 1, // For UI components, starts from 1.
      max_size : 6, // Page index max size
      total_items : 0
    }

    $scope.crowd_page = {
      page_size : 10,
      current_page : 1, // For UI components, starts from 1.
      max_size : 6, // Page index max size
      total_items : 0
    }

    $scope.crowd = {
      list : [],
      length: 0
    };

    $scope.group = {
      list : [],
      logic : [
        'or', 
        // 'and' // not support yet.
      ]
    }

    $scope.init_view = function() {

      crowdFactory
      .getCrowdsCount(function(data) {
        if (data.success)
          $scope.crowd.length = data.count;
        else 
          $scope.crowd.length = 0;
      })

      groupFactory
      .viewGroups(0, $scope.page.page_size, function (data) {
        if (data.success) {
          $scope.group.list = data.list;
        }
        else {
          $scope.group.list = [];
        }
      });


      $scope.remove = function(groupID) {
        console.log("Delete a group by ID");
        if (confirm($translate.instant('Group_view.GroupDeleteComfirmMessage'))) {
          groupFactory
          .deleteGroup(groupID, function(data) {
            for (var i in $scope.group.list) {
              if ($scope.group.list[i]._id === data._id) {
                  $scope.group.list.splice(i, 1);
              }
            } 
          })
        }
      }

      $scope.groupPageChanged = function() {
        $scope.getGroupsByPageId($scope.page.current_page - 1);
      }

      //Get groups by page id
      $scope.getGroupsByPageId = function (pageId) {
        groupFactory
        .viewGroups(pageId, $scope.page.page_size, function (data) {
          if (data.success) {
            $scope.group.list = data.list;
          }
          else {
            $scope.group.list = [];
          }
        });
      };

      $scope.goToCreateGroup = function() {
        $state.go('crowd.group_create');
      }
    }

    $scope.init_create = function() {
      console.log ("get crowd list")
      console.log ("get group amount of people.")

      $scope.groupDetail = {};
      $scope.groupDetail.selector = [];
      $scope.groupDetail.logic = 'or';

      crowdFactory
      .getCrowdsCount(function(data) {
        if (data.success) {
          $scope.crowd_page.total_items = data.count;
        }
        else {
          $scope.crowd_page.total_items = 0;
        }
      })

      crowdFactory
      .viewCrowds(0, $scope.crowd_page.page_size, function (data) {
        if (data.success) {
          $scope.crowd.list = data.list;
        }
        else {
          $scope.crowd.list = [];
        }
      });

      $scope.crowdPageChanged = function () {
        $scope.getCrowdsByPageId($scope.crowd_page.current_page - 1);
      };

      $scope.getCrowdsByPageId = function (pageId) {
        crowdFactory
        .viewCrowds(pageId, $scope.crowd_page.page_size, function(data) {
          $scope.crowd.list = data.list;
        })
      };

      $scope.addToList = function(crowd) {
        for (var index in $scope.groupDetail.selector) {
          if (crowd == $scope.groupDetail.selector[index]) {
              return;
          }
        }
        $scope.groupDetail.selector.push(crowd)
        $scope.getGroupUserCount();
      }

      $scope.removeFromList = function (index) {
        $scope.groupDetail.selector.splice(index, 1);
        $scope.getGroupUserCount();
      };

      $scope.getGroupUserCount = function () {
        var param = {};
        param.selector = [];
        for (var index in $scope.groupDetail.selector) {
            param.selector.push($scope.groupDetail.selector[index].selector);
        }
        $scope.groupDetail.count = -1;
        $scope.lastRid = Date.parse(new Date());

        $scope.groupDetail.count = 10;
        // groupFactory.getGroupUserCount($scope.lastRid, param, function (data) {
        //   if (data.success) {
        //       if($scope.lastRid == data.requestId) {
        //           $scope.groupDetail.count = data.length;
        //       }
        //   } else {
        //       $scope.groupDetail.count = -2;
        //   }
        // });
      };

      $scope.createGroup = function(isValid) {
        if (isValid) {
            var selectorlist = $scope.groupDetail.selector;
            $scope.groupDetail.selector = [];
            $scope.groupDetail.type = 'static';
            for (var index in selectorlist) {
                $scope.groupDetail.selector_id.push(selectorlist[index]._id);
                if (selectorlist[index].type == 'dynamic') {
                    $scope.groupDetail.type = 'dynamic';
                }
            }

            if ($stateParams.groupID !== undefined) {
                var group = $scope.groupDetail;
                group.$update(function () {
                    alert($translate.instant('Group_create.UpdatedSuccessfully'));
                    $location.path('/crowd/group_view');
                });
            } else {
                groupFactory.createGroup($scope.groupDetail, function (data) {
                    if (data.success) {
                        alert($translate.instant('Group_create.CreatedSuccessfully'));
                        $location.path('/crowd/' + $scope.groupDetail._id + '/group_user');
                    } else {
                        alert($translate.instant('Group_create.CreatedFailed'));
                    }
                });
            }
        } else {
            alert("Date Error");
            $scope.submitted = true;
        }
      };
    }

    $scope.init_user = function() {
      console.log("init user.")

      console.log($stateParams);
      console.log($scope);
    }

  });
