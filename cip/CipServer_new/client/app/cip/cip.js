'use strict';

angular.module('cipApp')
  .config(['$routeProvider', function($routeProvider) {
    $routeProvider
        .when('/cip', {
          templateUrl: 'cip.html'
        })
        .when('/cip/crowd/group', {
            templateUrl: 'groupIndex.html'
        })
        .when('/cip/crowd/create', {
          templateUrl: 'createCrowd.html'
        })
        .when('/cip/crowd/save', {
            templateUrl: 'saveCrowd.html',
            params: {
                crowdDetail: null
            }
        })
        .when('/cip/crowd/:crowdName/userlist', {
            templateUrl: 'userlist.html'
        })
        .when('/cip/crowd/group', {
            templateUrl: 'createGroup.html'
        })
        .when('/cip/crowd/group/:groupName/userlist', {
            templateUrl: 'groupUserlist.html'
        })
  }
]);
