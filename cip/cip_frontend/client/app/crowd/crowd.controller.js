'use strict';

angular.module('cipApp')
.controller('crowdCtrl', function ($scope, $state, $translate, crowdFactory) {
  $scope.message = 'Crowd CRUD controller.';
  $scope.valid = false;

  var temp_data = [];
  
  // config of uib-pagination elements.
  $scope.page = {
    page_size : 10,
    current_page : 1, // For UI components, starts from 1.
    max_size : 6, // Page index max size
    total_items : 0
  }

  // config of crowd view.
  $scope.crowd = {
    list : [],
    type : [
      'static',
      'dynamic'
    ]
  }



  // config of group view.
  $scope.group = {
    logic : [
      'or', 
      // 'and' // not support yet.
    ]
  }

  // config of factors.
  $scope.factors = {};

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
    behavior_type : [],
    action : [],
    object : {
      type : [
        {name: $translate.instant('Ontology.Category'), id: 'Category'},
        {name: $translate.instant('Ontology.Item'), id: 'Item'},
        {name: $translate.instant('Ontology.Contains'), id: 'Contains'},
      ],
      id : [],
      category : []
    },
    date_type : [
      {name:$translate.instant('Ontology.AbsoluteDate'),id:'absolute'},
      {name:$translate.instant('Ontology.PassedDays'),id:'day'},
      {name:$translate.instant('Ontology.PassedHours'),id:'hour'},
    ]
  }

  // For Widget
  $scope.widget = {
    startTime : "",
    endTime : "",
    date_start : function() {
      this.popup_start = true;
    },
    date_end : function() {
      this.popup_end = true;
    },
    popup_start : false,
    popup_end : false,
    dateOptions : {
      formatYear: 'yy',
      startingDay: 1
    }
  }

  // Initialize the ontology object from backend.
  $scope.init_ontology = function() {
    crowdFactory.getOntology(function(data) {
      console.log(data);
      if (data.success) {
        //Convert
        var profile = data.content.profile;
        $scope.ontology.data = profile;

        $scope.tag.name = [];
        $scope.ontology.type = [];
        $scope.ontology.action = [];
        for (var index in profile.tag) {
            $scope.tag.name.push(profile.tag[index].name);
        }
        for (index in profile.interest_intent) {
            $scope.ontology.type.push(profile.interest_intent[index].ontology);
        }
        for (index in profile.behaviour) {
            $scope.ontology.action.push(profile.behaviour[index].name);
        }

        // init conditions for UI of crowd create.
        $scope.factors.condition = 'tag';
        $scope.factors.name = $scope.ontology.data.tag[0].name;
        $scope.factors.action = $scope.ontology.data.behaviour[0].name;
      }
    })
  }


  // crowd.view.html related functions are defined in this function.
  $scope.init_view = function() {
    // crowdFactory
    // .query({pageId: 0, pageSz: $scope.page_size}, function (crowds) {
    //     $scope.crowds = crowds;
    // });

    temp_data = [{
        _id: -3,
        crowdName: 'AAAA',
        type: 'static',
        count:3,
        created: '2016-04-03',
        tagAdded : 0,
    },
    {
        _id: -2,
        crowdName: 'BBB',
        type: 'static',
        count:4,
        created: '2016-04-03',
        tagAdded : 1
    },
    {
        _id: -1,
        crowdName: 'CCC',
        type: 'static',
        count:6,
        created: '2016-04-03',
        tagAdded : -1
    }]

    for (var i=0; i<110; ++i) {
      var v = Math.random();
      temp_data.push({
        _id: i,
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
      // .viewCrowds(pageId, $scope.page.page_size, function(data) {
      //   $scope.crowd.list = data.list;
      // })

      $scope.crowd.list = temp_data.slice(pageId*$scope.page.page_size, (pageId+1)*$scope.page.page_size)
    };

    //Testing
    $scope.crowdPageChanged();
    $scope.page.total_items += temp_data.length;
    //End Testing

    // remove a crowd by name.
    $scope.remove = function(crowdID) {
      if (confirm($translate.instant('Crowd_view.CrowdDeleteComfirmMessage'))) {
        console.log('Delete the crowd from server.')
        crowdFactory
        .deleteCrowd(crowdID, function(data) {
          console.log(data);
        })
      }
    };

    $scope.goCrowdCreate = function() {
      $state.go('crowd.crowd_create');
    };
  }

  // crowd.create.html related functions are defined in this function.
  $scope.init_create = function() {
    //$scope.crowd.list = [];
    // Testing.
    $scope.init_ontology();

    $scope.crowdDetail = {};
    $scope.crowdDetail.selector = {};
    $scope.crowdDetail.selector.ontology = [];
    $scope.crowdDetail.selector.behavior = [];
    $scope.crowdDetail.selector.tag = [];

    $scope.$watch('factors.condition', function (nowSelected) {
        if (!nowSelected) {
            // here we've initialized selected already
            // but sometimes that's not the case
            // then we get null or undefined
            return;
        }

        $scope.factors = {};
        $scope.factors.condition = nowSelected;

        if (nowSelected == 'tag') {
            $scope.factors.name = $scope.ontology.data.tag[0].name;
            $scope.factors.operator = $scope.tag.operator[0];
            $scope.factors.weight = 1.0;
        } else if (nowSelected == 'ontology') {
            $scope.factors.name = $scope.ontology.data.interest_intent[0].ontology;
            $scope.factors.operator = $scope.ontology.operator[0];
            $scope.factors.weight = 0.0;
        } else if (nowSelected == 'behavior') {
            $scope.factors.action = $scope.ontology.data.behaviour[0].name;
            $scope.factors.operator = $scope.ontology.behavior_operator[0];
            $scope.factors.value = 0;
            $scope.factors.timeType = 'absolute';
        }
    });

    $scope.$watch('factors.name', function (nowSelected) {
        if (!nowSelected) {
            // here we've initialized selected already
            // but sometimes that's not the case
            // then we get null or undefined
            return;
        }

        if ($scope.factors.condition == 'tag') {
            angular.forEach($scope.ontology.data.tag, function (val) {
                if (val.name == nowSelected) {
                    $scope.factors.factor = val.element[0];
                    $scope.tag.factor = val.element;
                }
            });
        } else if ($scope.factors.condition == 'ontology') {
            angular.forEach($scope.ontology.data.ontology, function (val) {
                if (val.name == nowSelected) {
                    $scope.traverseTree(val.tree, function (itemList) {
                        $scope.ontology.factor = itemList;
                        $scope.factors.factor = itemList[0];
                        $scope.factors.operator = $scope.ontology.operator[0];
                    });
                }
            });
        }
    });

    $scope.$watch('factors.action', function (nowSelected) {
      if (!nowSelected) {
          return;
      }
      $scope.ontology.object.category = [];
      $scope.factors.objectType = '';

      angular.forEach($scope.ontology.data.behaviour, function (val) {
          if (val.name == nowSelected) {
              angular.forEach(val.object, function (object) {
                  $scope.ontology.object.category.push(object.name);
                  if(!$scope.ontology.object.category || $scope.ontology.object.category.length <= 0) {
                      $scope.factors.objectType = "Behavior";  // For login and other actions who doesn't have object
                  } else {
                      $scope.factors.objectType = $scope.ontology.object.type[0].id;
                      $scope.factors.objectCategory = val.object[0].name;
                  }
              });
          }
      });
    });

    $scope.$watch('factors.objectCategory', function (nowSelected) {
        if (!nowSelected) {
            return;
        }
        $scope.ontology.behavior_type = [];
        angular.forEach($scope.ontology.data.object_ontology, function (val) {
            if (val.object == nowSelected) {
                angular.forEach(val.ontology, function (ontology) {
                    $scope.ontology.behavior_type.push(ontology.name);
                });
                $scope.factors.ontologyType = val.ontology[0].name;
            }
        });
    });

    $scope.$watch('factors.objectType', function (nowSelected) {
        if (!nowSelected) {
            return;
        }
        $scope.factors.objectId = '';
    });

    $scope.$watch('factors.ontologyType', function (nowSelected) {
        if (!nowSelected) {
            return;
        }
        $scope.factors.objectId = '';
        $scope.ontology.object.id = [];
        if($scope.factors.objectType == 'Category') {
            angular.forEach($scope.ontology.data.ontology, function (val) {
                if (val.name == nowSelected) {
                    $scope.traverseTree(val.tree, function (itemList) {
                        $scope.ontology.object.id = itemList;
                        $scope.factors.objectId = itemList[0];
                    });
                }
            });
        }
    });

    $scope.$watch('widget.startTime', function (time) {
        if (!time) {
            return;
        }
        if ($scope.factors.timeType == 'day') {
            $scope.factors.startTime = time * 86400000;
            $scope.factors.endTime = 0;
        } else if($scope.factors.timeType == 'hour') {
            $scope.factors.startTime = time * 3600000;
            $scope.factors.endTime = 0;
        } else {
            $scope.factors.startTime = time.valueOf();
        }
    });

    $scope.$watch('widget.endTime', function (time) {
        if (!time) {
            return;
        }
        $scope.factors.endTime = time.valueOf();
    });

    $scope.traverseTree = function (tree, callback) {
        var list = [];
        var queue = new Array();
        for (var i in tree) {
            var prefix = '';
            //if (queue.length > 0) {
            //    prefix = queue.pop() + '/';
            //} else {
            //    prefix = '';
            //}
            for (var j in tree[i].children) {
                list.push(prefix + tree[i].children[j]);
                queue.unshift(prefix + tree[i].children[j]);
            }
        }
        callback(list);
    };

    $scope.addFactor = function (condition_type) {
      //Check valid
      if(condition_type == 'behavior' && (!$scope.factors.objectId || $scope.factors.objectId == '')) {
          alert($translate.instant('Warning.ObjectValueNull'));
          return;
      }
      if(condition_type == 'ontology' && (!$scope.factors.factor || $scope.factors.factor == '')) {
          alert($translate.instant('Warning.TagNull'));
          return;
      }
      if(condition_type == 'behavior' && ($scope.factors.timeType == 'day' || $scope.factors.timeType == 'hour') && ($scope.factors.startTime <= 0.1 || $scope.factors.startTime == null)) {
          alert($translate.instant('Warning.DateNull'));
          return;
      }
      if(condition_type == 'behavior' && $scope.factors.timeType == 'absolute' && ($scope.factors.startTime == null || $scope.factors.endTime == null || $scope.factors.startTime <= 0.1 || $scope.factors.endTime <= 0.1)) {
          alert($translate.instant('Warning.DateNull'));
          return;
      }
      if(condition_type == 'behavior' && $scope.factors.timeType == 'absolute' && ($scope.factors.startTime > $scope.factors.endTime)) {
          alert($translate.instant('Warning.DateStartEndNull'));
          return;
      }
      if(condition_type == 'behavior' && ($scope.factors.value == null || $scope.factors.value < 0 || $scope.factors.value > 2147483647)) {
          alert($translate.instant('Warning.FrequencyNull'));
          return;
      }
      //Start add
      if (!$scope.crowdDetail) {
          $scope.crowdDetail = {};
      }
      if (!$scope.crowdDetail.selector[condition_type]) {
          $scope.crowdDetail.selector[condition_type] = [];
      }

      var param = {};
      param.selector = {};
      param.selector.ontology = [];
      param.selector.behavior = [];
      param.selector.tag = [];
      $scope.factors.count = -1;
      param.selector[condition_type].push($scope.factors);
      $scope.crowdDetail.selector[condition_type].push($scope.factors);

      $scope.factors = {};
      $scope.widget.startTime = '';
      $scope.widget.endTime = '';
      $scope.lastRid = Date.parse(new Date());
      crowdFactory.getCrowdUserCountByFactor($scope.lastRid, param, function (param, data) {
          if(data.success) {
              param.selector[condition_type][0].count = data.length;
          } else {
              param.selector[condition_type][0].count = -2;
          }
      });
      $scope.crowdDetail.count = -1;
      crowdFactory.getCrowdUserCountByFactor($scope.lastRid, $scope.crowdDetail, function (param, data) {
          if(data.success) {
              if($scope.lastRid == data.requestId) {
                  $scope.crowdDetail.count = data.length;
              }
          } else {
              $scope.crowdDetail.count = -2;
          }
      });

      // init conditions for UI of crowd create.
      $scope.factors.condition = 'tag';
      $scope.factors.name = $scope.tag.name[0]
      $scope.factors.factor = $scope.tag.factor[0];
      $scope.factors.operator = $scope.tag.operator[0];
      $scope.factors.weight = 1.0
    };

    $scope.deleteFactor = function (factor, index) {
      factor.splice(index, 1);
      $scope.crowdDetail.count = -1;
      $scope.lastRid = Date.parse(new Date());
      crowdFactory.getCrowdUserCountByFactor($scope.lastRid, $scope.crowdDetail, function (param, data) {
          if(data.success) {
              if($scope.lastRid == data.requestId) {
                  $scope.crowdDetail.count = data.length;
              }
          } else {
              $scope.crowdDetail.count = -2;
          }
      });
    };

    $scope.resetFactor = function () {
        $scope.factors = {};
    };

    $scope.goCrowdSave = function() {
      $state.go('crowd.crowd_save');
    };
  }

  $scope.init_save = function() {
    console.log('init save');
  }

  $scope.init_user = function() {
    console.log('init user');
  }

});
