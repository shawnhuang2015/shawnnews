'use strict';

angular.module('cipApp')
  .config(['$routeProvider', function($routeProvider) {
    $routeProvider
        .when('/cip', {
          templateUrl: 'app/cip/cip.html'
        })
        .when('/cip/crowd/group', {
            templateUrl: 'app/cip/groupIndex.html'
        })
        .when('/cip/crowd/create', {
          templateUrl: 'app/cip/createCrowd.html'
        })
        .when('/cip/crowd/save', {
            templateUrl: 'app/cip/saveCrowd.html',
            params: {
                crowdDetail: null
            }
        })
        .when('/cip/crowd/:crowdName/userlist', {
            templateUrl: 'app/cip/userlist.html'
        })
        .when('/cip/crowd/group', {
            templateUrl: 'app/cip/createGroup.html'
        })
        .when('/cip/crowd/group/:groupName/userlist', {
            templateUrl: 'app/cip/groupUserlist.html'
        })
  }
]);
