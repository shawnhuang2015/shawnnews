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
          templateUrl: '/cipmanager/views/create.html',
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
        });
  }
]);