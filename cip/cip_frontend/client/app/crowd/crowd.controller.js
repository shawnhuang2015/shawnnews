'use strict';

angular.module('cipApp')
.controller('crowdCtrl', function ($scope, crowdFactory) {
  $scope.message = 'Crowd CRUD controller.';
  $scope.crowds = [];
  
  $scope.page = {
    page_size : 10,
    current_page : 1,
    max_size : 6,
    total_items : 0
  }

  $scope.init_create = function() {
    $scope.crowds = [];
  }

  var temp_data = []

  $scope.init_view = function() {
    // crowdFactory
    // .query({pageId: 0, pageSz: $scope.page_size}, function (crowds) {
    //     $scope.crowds = crowds;
    // });

    temp_data = [{
        crowdName: 'AAAA',
        type: 'static',
        count:3,
        created: '2016-04-03',
        tagAdded : 0,
    },
    {
        crowdName: 'BBB',
        type: 'static',
        count:4,
        created: '2016-04-03',
        tagAdded : 1
    },
    {
        crowdName: 'CCC',
        type: 'static',
        count:6,
        created: '2016-04-03',
        tagAdded : -1
    }]

    for (var i=0; i<110; ++i) {
      temp_data.push({
        crowdName: 'AAAA',
        type: 'static',
        count:3,
        created: '2016-04-03',
        tagAdded : 0,
      })
    }

    $scope.crowdPageChanged = function () {
      $scope.getCrowdsByPageId($scope.page.current_page - 1);
    };

    $scope.getCrowdsByPageId = function (pageId) {
      // crowdFactory
      // .query({pageId: pageId, pageSz: $scope.page_size}, function (crowds) {
      //     $scope.crowds = crowds;
      // });
      $scope.crowds = temp_data.slice(pageId*$scope.page.page_size, (pageId+1)*$scope.page.page_size)
    };

    $scope.crowdPageChanged();
    $scope.page.total_items += temp_data.length;
  }
});
