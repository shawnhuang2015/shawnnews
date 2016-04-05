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

  // config of factors.
  $scope.condition_type = [
    {name: $translate.instant('Ontology.Tag'), id: 'tag'},
    {name: $translate.instant('Ontology.Interest'), id: 'ontology'},
    {name: $translate.instant('Ontology.Behavior'), id: 'behavior'}
  ];

  $scope.tag = {
    name : [],
    factor : [],
    operator : [
      '=',
      // '<>'
    ]
  };


  // config of ontology.
  $scope.ontology = {
    data : {},
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
        {name: $translate.instant('Ontology.Category'), id: 'Category'},
        {name: $translate.instant('Ontology.Item'), id: 'Item'},
        {name: $translate.instant('Ontology.Contains'), id: 'Contains'},
      ],
      id : []
    },
    date_type : [
      {name:$translate.instant('Ontology.AbsoluteDate'),id:'absolute'},
      {name:$translate.instant('Ontology.PassedDays'),id:'day'},
      {name:$translate.instant('Ontology.PassedHours'),id:'hour'},
    ]
  }

  // Initialize the ontology object from backend.
  $scope.init_ontology = function() {
    crowdFactory.getOntology(function(data) {
      if (data.success) {
        //Convert
        $scope.ontology.data = data.content;

        $scope.tag.name = [];
        $scope.ontology.type = [];
        $scope.ontology.action = [];
        for (var index in data.content.tag) {
            $scope.tag.name.push(data.content.tag[index].name);
        }
        for (index in data.content.interest_intent) {
            $scope.ontology.type.push(data.content.interest_intent[index].ontology);
        }
        for (index in data.content.behaviour) {
            $scope.ontology.action.push(data.content.behaviour[index].name);
        }

        $scope.crowd.factors.condition = 'tag';
        $scope.crowd.factors.name = data.content.tag[0].name;
        $scope.crowd.factors.action = data.content.behaviour[0].name;
      }
    })
  }

  // crowd.create.html rlated functions are defined in this function.
  $scope.init_create = function() {
    //$scope.crowd.list = [];
  }


  // crowd.view.html related functions are defined in this function.
  $scope.init_view = function() {
    // crowdFactory
    // .query({pageId: 0, pageSz: $scope.page_size}, function (crowds) {
    //     $scope.crowds = crowds;
    // });

    // Testing.
    $scope.init_ontology();

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

    $scope.goCrowdCreate = function() {
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
