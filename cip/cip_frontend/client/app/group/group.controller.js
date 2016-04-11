'use strict';

angular.module('cipApp')
  .controller('groupCtrl', function ($rootScope, $scope, groupFactory, crowdFactory) {
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
    };

    $scope.group = {
      list : []
    }

    $scope.init_view = function() {
      console.log('init view of group.');

      $scope.crowds = [];
      $scope.group.list = [1,2,3,4,5];

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
    }


  });
