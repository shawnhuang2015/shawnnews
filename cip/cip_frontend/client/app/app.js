'use strict';

angular.module('cipApp', [
  // 'cipApp.auth',
  // 'cipApp.admin',
  // 'cipApp.constants',
  'ngCookies',
  'ngResource',
  'ngSanitize',
  'ui.router',
  'ui.bootstrap',
  'validation.match'
])

angular
.module('cipApp')
.config(config)
.run(function($rootScope, $state) {
    $rootScope.$state = $state;
});

function config($stateProvider, $urlRouterProvider, $locationProvider) {
  $urlRouterProvider.otherwise("/index/main");

  $stateProvider
  .state('index', {
      abstract: true,
      url: "/index",
      templateUrl: "template/views/common/content.html",
      controller: 'mainCtrl',
      controllerAs: 'main'
  })
  .state('index.main', {
      url: "/main",
      templateUrl: "template/views/main.html",
      data: { pageTitle: 'Example view index main' }
  })
  .state('index.create_single_crowd', {
      url: "/create_single_crowd",
      templateUrl: "template/views/main.html",
      data: { pageTitle: 'create_single_crowd' }
  })
  .state('index.minor', {
      url: "/minor",
      templateUrl: "template/views/minor.html",
      data: {},
      controller: 'minorCtrl',
      controllerAs : 'minor'
  })

  $locationProvider.html5Mode(true);
}