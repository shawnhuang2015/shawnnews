'use strict';

angular.module('cipApp')
.controller('crowdCtrl', function ($scope, $state, $translate, crowdFactory) {
  $scope.message = 'Crowd CRUD controller.';
  $scope.valid = false;

  var temp_data = [];
  
  // config of uib-pagination elements.
  $scope.page = {
    page_size : 10,
    current_page : 1,
    max_size : 6,
    total_items : 0
  }

  // config of crowd view.
  $scope.crowd = {
    list : [],
    type : [
      'static',
      'dynamic'
    ],
    factors : {}
  }

  // config of group view.
  $scope.group = {
    logic : [
      'or', 
      // 'and' // not support yet.
    ]
  }

  // config of ontology.
  $scope.ontology = {
    type : [],
    factor : [],
    operator : [
    '>=', 
    '<=', 
    ">", 
    "<", 
    "<>", 
    "=", 
    // not support yet.
    //"like", 
    //"dislike"
    ],
    behavior_operator : [
      '>=', '<=', ">", "<", "<>", "="
    ],
    action : [],
    object : {
      type : [
        {name: '在分类...中', id: 'Category'},
        {name: 'Item ID=', id: 'Item'},
        {name: '包含字段', id: 'Contains'},
      ],
      id : []
    },
    date_type : [
      {name:'在日期N-M之间',id:'absolute'},
      {name:'过去N天',id:'day'},
      {name:'过去N小时',id:'hour'},
    ]
  }

  $scope.init_create = function() {
    //$scope.crowd.list = [];
  }


  // crowd.view.html related functions are defined in this function.
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
      var v = Math.random();
      temp_data.push({
        crowdName: 'AAAA',
        type: 'static',
        count:v,
        created: '2016-04-03',
        tagAdded : function() {
          if (v < 0.7) return 1;
          if (v >= 0.7 && v <0.8) return -1;
          if (v > 0.8) return 0;
        }(),
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
      $scope.crowd.list = temp_data.slice(pageId*$scope.page.page_size, (pageId+1)*$scope.page.page_size)
    };

    $scope.crowdPageChanged();
    $scope.page.total_items += temp_data.length;

    // remove a crowd by name.
    $scope.remove = function(one_crowd) {
      if (confirm($translate.instant('Crowd_view.CrowdDeleteComfirmMessage'))) {
        console.log('Delete the crowd from server.')
      }
    };

    $scope.goToCreate = function() {
      $state.go('crowd.crowd_create');
    };
  }

  $scope.init_save = function() {
    console.log('init save');
  }

  $scope.init_user = function() {
    console.log('init user');
  }

});
