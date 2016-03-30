'use strict';

angular.module('cipApp', [
  'cipApp.auth',
  'cipApp.admin',
  'cipApp.constants',
  'ngCookies',
  'ngResource',
  'ngSanitize',
  'ngRoute',
  'btford.socket-io',
  'ui.bootstrap',
  'validation.match'
])
  .config(function($routeProvider, $locationProvider) {
    $routeProvider
      // .when('/test', {
      //   redirectTo : '/cip'
      // })
      .otherwise({
        redirectTo: '/'
      });

    $locationProvider.html5Mode(true);
  });
