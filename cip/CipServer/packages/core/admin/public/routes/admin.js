'use strict';
angular.module('mean.admin').config(['$stateProvider', '$urlRouterProvider',
  function($stateProvider, $urlRouterProvider) {
    $stateProvider
      .state('crowd', {
        url: '/crowd/main',
        templateUrl: 'cipmanager/views/index.html',
        requiredCircles: {
          circles: ['admin']
        }
      }).state('brain', {
        url: '/crowd/main',
        templateUrl: 'cipmanager/views/index.html',
        requiredCircles: {
            circles: ['admin']
        }
      }).state('users', {
        url: '/admin/users',
        templateUrl: 'admin/views/users.html',
        requiredCircles: {
            circles: ['admin']
        }
      }).state('settings', {
        url: '/admin/settings',
        templateUrl: 'admin/views/settings.html',
        requiredCircles: {
          circles: ['admin']
        }
      }).state('modules', {
        url: '/admin/modules',
        templateUrl: 'admin/views/modules.html',
        requiredCircles: {
          circles: ['admin']
        }
      }).state('admin settings', {
        url: '/admin/_settings',
        templateUrl: 'admin/views/example.html',
        requiredCircles: {
          circles: ['admin']
        }
      });
  }
  ]).config(['ngClipProvider',
      function(ngClipProvider) {
          ngClipProvider.setPath('../admin/assets/lib/zeroclipboard/dist/ZeroClipboard.swf');
      }
  ]);