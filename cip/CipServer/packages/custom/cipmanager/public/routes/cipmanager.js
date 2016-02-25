'use strict';

angular.module('mean.cipmanager').config(['$stateProvider',
  function($stateProvider) {
    $stateProvider
        .state('all crowds', {
          url: '/crowd/main',
          templateUrl: 'cipmanager/views/index.html',
          requiredCircles: {
            circles: ['admin']
          }
        })
        .state('create crowd', {
          url: '/crowd/create',
          templateUrl: '/cipmanager/views/edit.html',
          requiredCircles : {
            circles: ['admin']
          }
        })
        .state('edit crowd', {
          url: '/crowd/:crowdName/edit',
          templateUrl: '/cipmanager/views/edit.html',
          requiredCircles : {
            circles: ['admin']
          }
        })
        .state('user list', {
            url: '/crowd/:crowdName/userlist',
            templateUrl: '/cipmanager/views/userlist.html',
            requiredCircles : {
                circles: ['admin']
            }
        })
        .state('create group', {
            url: '/crowd/group/',
            templateUrl: '/cipmanager/views/createGroup.html',
            requiredCircles : {
                circles: ['admin']
            }
        })
        .state('edit group', {
            url: '/crowd/group/:groupName/edit',
            templateUrl: '/cipmanager/views/createGroup.html',
            requiredCircles : {
                circles: ['admin']
            }
        })
        .state('group user list', {
            url: '/crowd/group/:groupName/userlist',
            templateUrl: '/cipmanager/views/groupUserlist.html',
            requiredCircles : {
                circles: ['admin']
            }
        })
  }
]);