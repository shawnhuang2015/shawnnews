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

    $scope.crowd = {
      length: 0
    };

    $scope.group = {
      list : []
    }

    $scope.init_view = function() {
      console.log('init view of group.');

      $scope.group.list = [1,2,3,4,5];

      crowdFactory
      .getCrowdsCount(function(data) {
        if (data.success)
          $scope.crowd.length = data.count;
        else 
          $scope.crowd.length = 0;
      })

      // groupFactory
      // .viewGroups(0, $scope.page.page_size, function (data) {
      //   if (data.success) {
      //     $scope.group.list = data.list;
      //   }
      //   else {
      //     $scope.group.list = [];
      //   }
      // });


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

    $scope.init_user = function() {
      console.log("init user.")

      console.log($stateParams);
      console.log($scope);
    }
  });
