'use strict';
angular.module("mean.cipmanager").controller('CipmanagerController',
    ['$scope', '$stateParams', '$location', '$state', 'CrowdManager', 'GroupManager', 'CrowdService', 'GroupService','MeanUser',
    function ($scope, $stateParams, $location, $state, CrowdManager, GroupManager, CrowdService, GroupService, MeanUser) {

        //Initial variables
        $scope.valid = false;
        $scope.page_size = 10;
        $scope.totalItems = 1;
        $scope.totalGroupItems = 1;
        $scope.currentPage = 1;
        $scope.currentCrowdPage = 1;
        $scope.currentGroupPage = 1;
        $scope.crowdTodoList = [];
        $scope.factors = {};
        $scope.package = {
            name: 'cipmanager'
        };

        $scope.crowd_type = [
            'static',
            'dynamic'
        ];

        $scope.group_logic = [
            //'and',
            'or'
        ];

        ////////////////////////For Factors ///////////////////
        $scope.condition_type = [
            {name: '用户标签', id: 'tag'},
            {name: '兴趣偏好/意图', id: 'ontology'},
            {name: '行为', id: 'behavior'}
        ];

        //Tag
        $scope.tag_name = [];
        $scope.tag_factor = [];
        $scope.tag_operator = [
            '='//, '<>'
        ];

        //Ontology
        $scope.ontology_type = [];

        $scope.ontology_factor = [];

        $scope.operator = [
            '>=', '<=', ">", "<", "<>", "="//, "like", "dislike"
        ];

        $scope.behavior_operator = [
            '>=', '<=', ">", "<", "<>", "="
        ];

        $scope.action = [];

        $scope.object_type = [
            {name: '在分类...中', id: 'Category'},
            {name: 'Item ID=', id: 'Item'},
            {name: '包含字段', id: 'Contains'},
        ];

        $scope.object_category = [];

        $scope.object_id = [];

        $scope.date_type = [
            {name:'在日期N-M之间',id:'absolute'},
            {name:'过去N天',id:'day'},
            {name:'过去N小时',id:'hour'},
        ];

        //////////////////////Widget///////////////////////////////
        $scope.popup1 = {
            opened: false
        };

        $scope.open1 = function () {
            $scope.popup1.opened = true;
        };

        $scope.popup2 = {
            opened: false
        };

        $scope.open2 = function () {
            $scope.popup2.opened = true;
        };

        $scope.dateOptions = {
            formatYear: 'yy',
            startingDay: 1
        };

        //////////////////////Widget///////////////////////////////


        //////////////////////Local function///////////////////////
        $scope.addFactor = function (condition_type) {
            //Check valid
            if(condition_type == 'behavior' && (!$scope.factors.objectId || $scope.factors.objectId == '')) {
                alert("对象值不能为空");
                return;
            }
            if(condition_type == 'ontology' && (!$scope.factors.factor || $scope.factors.factor == '')) {
                alert("标签不能为空");
                return;
            }
            if(condition_type == 'behavior' && ($scope.factors.timeType == 'day' || $scope.factors.timeType == 'hour') && !$scope.factors.startTime) {
                alert("时间不能为空");
                return;
            }
            if(condition_type == 'behavior' && $scope.factors.timeType == 'absolute' && (!$scope.factors.startTime || !$scope.factors.endTime)) {
                alert("时间不能为空");
                return;
            }
            if(condition_type == 'behavior' && $scope.factors.timeType == 'absolute' && ($scope.factors.startTime > $scope.factors.endTime)) {
                alert("开始时间不能大于结束时间");
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
            $scope.st = '';
            $scope.et = '';

            CrowdService.getUserCountByFactor(param, function (param, data) {
                if(data.success) {
                    param.selector[condition_type][0].count = data.length;
                } else {
                    param.selector[condition_type][0].count = -2;
                }
            });
            $scope.crowdDetail.count = -1;
            CrowdService.getUserCountByFactor($scope.crowdDetail, function (param, data) {
                if(data.success) {
                    $scope.crowdDetail.count = data.length;
                } else {
                    $scope.crowdDetail.count = -2;
                }
            });
        };

        $scope.deleteFactor = function (factor, index) {
            factor.splice(index, 1);
            $scope.crowdDetail.count = -1;
            CrowdService.getUserCountByFactor($scope.crowdDetail, function (param, data) {
                if(data.success) {
                    $scope.crowdDetail.count = data.length;
                } else {
                    $scope.crowdDetail.count = -2;
                }
            });
        };

        $scope.resetFactor = function () {
            $scope.factors = {};
        };

        $scope.goToCreate = function () {
            $state.go('create crowd')
        };

        $scope.goToSaveCrowd = function () {
            for (var index in $scope.crowdDetail.selector.tag) {
                if($scope.crowdDetail.selector.tag[index].count == -1) {
                    alert("人群数量还未获取完成,请完成后再保存");
                    return;
                }
            }
            for (index in $scope.crowdDetail.selector.behavior) {
                if($scope.crowdDetail.selector.behavior[index].count == -1) {
                    alert("人群数量还未获取完成,请完成后再保存");
                    return;
                }
            }
            for (index in $scope.crowdDetail.selector.ontology) {
                if($scope.crowdDetail.selector.ontology[index].count == -1) {
                    alert("人群数量还未获取完成,请完成后再保存");
                    return;
                }
            }
            $state.go('save crowd', {crowdDetail: $scope.crowdDetail});
        };

        $scope.goToCreateGroup = function () {
            $state.go('create group')
        };

        $scope.goToSaveGroup = function () {
            $state.go('save group')
        };

        $scope.addToList = function (crowd) {
            for (var index in $scope.groupDetail.selector) {
                if (crowd == $scope.groupDetail.selector[index]) {
                    return;
                }
            }
            $scope.groupDetail.selector.push(crowd)
            $scope.getGroupUserCount();
        };

        $scope.removeFromList = function (index) {
            $scope.groupDetail.selector.splice(index, 1);
            $scope.getGroupUserCount();
        };

        $scope.initSaveCrowd = function () {
            checkLogin();

            $scope.crowdDetail = $stateParams.crowdDetail;
            $scope.crowdDetail.type = 'static';
        };
        //////////////////////Local function///////////////////////


        ////////////////////////Listener////////////////////////////
        $scope.pageChanged = function () {
            $scope.getCrowdsByPageId($scope.currentPage - 1);
        };

        $scope.pageCrowdChanged = function () {
            $scope.getCrowdsByPageId($scope.currentCrowdPage - 1);
        };

        $scope.pageGroupChanged = function () {
            $scope.getGroupsByPageId($scope.currentGroupPage - 1);
        };

        //Ontology related
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
                $scope.factors.name = $scope.ontology_data.tag[0].name;
                $scope.factors.operator = $scope.tag_operator[0];
                $scope.factors.weight = 1.0;
            } else if (nowSelected == 'ontology') {
                $scope.factors.name = $scope.ontology_data.interest_intent[0].ontology;
                $scope.factors.operator = $scope.operator[0];
                $scope.factors.weight = 0.0;
            } else if (nowSelected == 'behavior') {
                $scope.factors.action = $scope.ontology_data.behaviour[0].name;
                $scope.factors.operator = $scope.behavior_operator[0];
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
                angular.forEach($scope.ontology_data.tag, function (val) {
                    if (val.name == nowSelected) {
                        $scope.factors.factor = val.element[0];
                        $scope.tag_factor = val.element;
                    }
                });
            } else if ($scope.factors.condition == 'ontology') {
                angular.forEach($scope.ontology_data.ontology, function (val) {
                    if (val.name == nowSelected) {
                        $scope.traverseTree(val.tree, function (itemList) {
                            $scope.ontology_factor = itemList;
                            $scope.factors.factor = itemList[0];
                            $scope.factors.operator = $scope.operator[0];
                        });
                    }
                });
            }
        });

        $scope.$watch('factors.action', function (nowSelected) {
            if (!nowSelected) {
                return;
            }
            $scope.object_category = [];
            $scope.factors.objectType = '';

            angular.forEach($scope.ontology_data.behaviour, function (val) {
                if (val.name == nowSelected) {
                    angular.forEach(val.object, function (object) {
                        $scope.object_category.push(object.name);
                        if(!$scope.object_category || $scope.object_category.length <= 0) {
                            $scope.factors.objectType = "Behavior";  // For login and other actions who doesn't have object
                        } else {
                            $scope.factors.objectType = $scope.object_type[0].id;
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
            $scope.behavior_ontology_type = [];
            angular.forEach($scope.ontology_data.object_ontology, function (val) {
                if (val.object == nowSelected) {
                    angular.forEach(val.ontology, function (ontology) {
                        $scope.behavior_ontology_type.push(ontology.name);
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
            $scope.object_id = [];
            if($scope.factors.objectType == 'Category') {
                angular.forEach($scope.ontology_data.ontology, function (val) {
                    if (val.name == nowSelected) {
                        $scope.traverseTree(val.tree, function (itemList) {
                            $scope.object_id = itemList;
                            $scope.factors.objectId = itemList[0];
                        });
                    }
                });
            }
        });

        $scope.$watch('st', function (time) {
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

        $scope.$watch('et', function (time) {
            if (!time) {
                return;
            }
            $scope.factors.endTime = time.valueOf();
        });

        ////////////////////////Listener////////////////////////////


        ///////////////////////Network query////////////////////////
        //Init the crowds, query first page and total count
        $scope.init = function () {
            checkLogin();

            CrowdManager.query({pageId: 0, pageSz: $scope.page_size}, function (crowds) {
                $scope.crowds = crowds;
            });

            GroupManager.query({pageId: 0, pageSz: $scope.page_size}, function (groups) {
                $scope.grouplist = groups;
            });

            CrowdService.getCount(function (data) {
                if (data.success) {
                    $scope.totalItems = data.length;
                } else {
                    $scope.totalItems = 1;
                }
            });

            GroupService.getGroupCount(function (data) {
                if (data.success) {
                    $scope.totalGroupItems = data.length;
                } else {
                    $scope.totalGroupItems = 1;
                }
            });
        };

        $scope.initUserList = function () {
            $scope.findOne();
            CrowdService.getUserList($stateParams.crowdName, 'single', function (data) {
                if (data.success) {
                    $scope.userList = data.userList;
                } else {
                    $scope.userList = [];
                }
            });

            CrowdService.getUserCount($stateParams.crowdName, function (data) {
                if (data.success) {
                    $scope.userCount = data.length;
                } else {
                    $scope.userCount = 1;
                }
            });
        };

        $scope.initGroupUserList = function () {
            $scope.findGroup();
            CrowdService.getUserList($stateParams.groupName, 'multi', function (data) {
                if (data.success) {
                    $scope.userList = data.userList;
                } else {
                    $scope.userList = [];
                }
            });
        };

        //Get crowds by page id
        $scope.getCrowdsByPageId = function (pageId) {
            CrowdManager.query({pageId: pageId, pageSz: $scope.page_size}, function (crowds) {
                $scope.crowds = crowds;
            });
        };

        //Get groups by page id
        $scope.getGroupsByPageId = function (pageId) {
            GroupManager.query({pageId: pageId, pageSz: $scope.page_size}, function (grouplist) {
                $scope.grouplist = grouplist;
            });
        };

        //Remove crowd from server
        $scope.remove = function (crowd) {
            if(confirm("确定要删除这个人群吗？")) {
                if (crowd) {
                    crowd.$remove(function (response) {
                        for (var i in $scope.crowds) {
                            if ($scope.crowds[i] === crowd) {
                                $scope.crowds.splice(i, 1);
                            }
                        }
                    });
                }
            }
        };

        //Get crowd details from server
        $scope.findOne = function () {
            checkLogin();
            if (!$stateParams.crowdName) {
                $scope.crowdDetail = {};
                $scope.crowdDetail.selector = {};
                $scope.crowdDetail.selector.ontology = [];
                $scope.crowdDetail.selector.behavior = [];
                $scope.crowdDetail.selector.tag = [];
                $scope.initOntology();
            } else {
                CrowdManager.get({
                    crowdName: $stateParams.crowdName
                }, function (crowdDetail) {
                    $scope.crowdDetail = crowdDetail;
                });
                $scope.initOntology();
            }
        };

        //Update crowd info to server
        $scope.update = function (isValid) {
            if (isValid) {
                if ($stateParams.crowdName) {
                    var crowd = $scope.crowdDetail;
                    crowd.$update(function () {
                        alert("保存成功!");
                        $location.path('crowd/main');
                    });
                } else {
                    CrowdService.createCrowd($scope.crowdDetail, function (data) {
                        if (data.success) {
                            alert("创建成功!");
                            $location.path('/crowd/' + $scope.crowdDetail.crowdName + '/userlist');
                        } else {
                            alert("创建失败!");
                        }
                    });
                }
            } else {
                alert("数据存在问题");
                $scope.submitted = true;
            }
        };

        //Create crowd to server
        $scope.create = function (isValid) {
            if (isValid) {
                CrowdService.createCrowd($scope.crowdDetail, function (data) {
                    if (data.success) {
                        alert("创建成功!");
                        $location.path('/crowd/' + $scope.crowdDetail.crowdName + '/userlist');
                    } else {
                        alert("创建失败!");
                    }
                });
            } else {
                alert("数据存在问题");
                $scope.submitted = true;
            }
        };

        //Remove group from server
        $scope.removeGroup = function (group) {
            if(confirm("确定要删除这个人群组合吗？")) {
                if (group) {
                    group.$remove(function (response) {
                        for (var i in $scope.grouplist) {
                            if ($scope.grouplist[i] === group) {
                                $scope.grouplist.splice(i, 1);
                            }
                        }
                    });
                }
            }
        };

        //Get group details from server
        $scope.findGroup = function () {
            checkLogin();

            if (!$stateParams.groupName) {
                $scope.groupDetail = {};
                $scope.groupDetail.selector = [];
                $scope.groupDetail.logic = 'or';
            } else {
                GroupManager.get({
                    crowdName: $stateParams.groupName
                }, function (groupDetail) {
                    $scope.groupDetail = groupDetail;
                });
            }

            CrowdManager.query({pageId: 0, pageSz: $scope.page_size}, function (crowds) {
                $scope.crowds = crowds;
            });

            CrowdService.getCount(function (data) {
                if (data.success) {
                    $scope.totalItems = data.length;
                } else {
                    $scope.totalItems = 1;
                }
            });
        };

        //Update group info to server
        $scope.updateGroup = function (isValid) {
            if (isValid) {
                var selectorlist = $scope.groupDetail.selector;
                $scope.groupDetail.selector = [];
                $scope.groupDetail.type = 'static';
                for (var index in selectorlist) {
                    $scope.groupDetail.selector.push(selectorlist[index].crowdName);
                    if (selectorlist[index].type == 'dynamic') {
                        $scope.groupDetail.type = 'dynamic';
                    }
                }

                if ($stateParams.groupName) {
                    var group = $scope.groupDetail;
                    group.$update(function () {
                        alert("保存成功!");
                        $location.path('crowd/main');
                    });
                } else {
                    GroupService.createGroup($scope.groupDetail, function (data) {
                        if (data.success) {
                            alert("创建成功!");
                            $location.path('/crowd/group/' + $scope.groupDetail.crowdName + '/userlist');
                        } else {
                            alert("创建失败!");
                        }
                    });
                }
            } else {
                alert("数据存在问题");
                $scope.submitted = true;
            }
        };

        $scope.getGroupUserCount = function () {
            var param = {};
            param.selector = [];
            for (var index in $scope.groupDetail.selector) {
                param.selector.push($scope.groupDetail.selector[index].selector);
            }
            $scope.groupDetail.count = -1;
            GroupService.getGroupUserCount(param, function (data) {
                if (data.success) {
                    $scope.groupDetail.count = data.length;
                } else {
                    $scope.groupDetail.count = -2;
                }
            });
        };

        //Query ontology
        $scope.initOntology = function () {
            CrowdService.getOntology(function (data) {
                if (data.success) {
                    //Convert
                    $scope.ontology_data = data.content;

                    $scope.tag_name = [];
                    $scope.ontology_type = [];
                    $scope.action = [];
                    for (var index in data.content.tag) {
                        $scope.tag_name.push(data.content.tag[index].name);
                    }
                    for (index in data.content.interest_intent) {
                        $scope.ontology_type.push(data.content.interest_intent[index].ontology);
                    }
                    for (index in data.content.behaviour) {
                        $scope.action.push(data.content.behaviour[index].name);
                    }

                    $scope.factors.condition = 'tag';
                    $scope.factors.name = data.content.tag[0].name;
                    $scope.factors.action = data.content.behaviour[0].name;
                }
            });
        };

        ///////////////////////Network query////////////////////////

        var checkLogin = function() {
            if(!MeanUser.isAdmin) {
                $state.go('auth.login')
            }
        }
    }
]);
