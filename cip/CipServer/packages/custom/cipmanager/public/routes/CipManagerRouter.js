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
        .state('all groups', {
            url: '/crowd/group',
            templateUrl: 'cipmanager/views/groupIndex.html',
            requiredCircles: {
                circles: ['admin']
            }
        })
        .state('create crowd', {
          url: '/crowd/create',
          templateUrl: '/cipmanager/views/createCrowd.html',
          requiredCircles : {
            circles: ['admin']
          }
        })
        .state('save crowd', {
            url: '/crowd/save',
            templateUrl: '/cipmanager/views/saveCrowd.html',
            params: {
                crowdDetail: null
            },
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
        .state('group user list', {
            url: '/crowd/group/:groupName/userlist',
            templateUrl: '/cipmanager/views/groupUserlist.html',
            requiredCircles : {
                circles: ['admin']
            }
        })
  }
]);