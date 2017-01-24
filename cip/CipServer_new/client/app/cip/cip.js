'use strict';

angular.module('cipApp')
  .config(['$routeProvider', function($routeProvider) {
    $routeProvider
        .when('/crowd/main', {
          templateUrl: 'app/cip/cip.html'
        })
        .when('/crowd/group', {
            templateUrl: 'app/cip/groupIndex.html'
        })
        .when('/crowd/create', {
          templateUrl: 'app/cip/createCrowd.html'
        })
        .when('/crowd/save', {
            templateUrl: 'app/cip/saveCrowd.html',
            params: {
                crowdDetail: null
            }
        })
        .when('/crowd/:crowdName/userlist', {
            templateUrl: 'app/cip/userlist.html'
        })
        .when('/crowd/group', {
            templateUrl: 'app/cip/createGroup.html'
        })
        .when('/crowd/group/:groupName/userlist', {
            templateUrl: 'app/cip/groupUserlist.html'
        })
  }
]);
