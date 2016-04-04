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
  'validation.match',
  'pascalprecht.translate'
])

angular
.module('cipApp')
.config(config)
.run(function($rootScope, $state) {
    $rootScope.$state = $state;
});

function config($stateProvider, $urlRouterProvider, $locationProvider) {
  $urlRouterProvider.otherwise("/index/crowd_view");

  $stateProvider
  .state('index', {
      abstract: true,
      url: "/index",
      templateUrl: "template/views/common/content.html",
      controller: 'mainCtrl',
      controllerAs : 'main'
  })
  .state('index.main', {
      url: "/main",
      templateUrl: "template/views/main.html"
  })
  .state('index.crowd_view', {
      url: "/crowd_view",
      templateUrl: "app/crowd/crowd.view.html",
      controller: 'crowdCtrl',
      controllerAs : 'vm'
  })
  .state('index.crowd_create', {
      url: "/crowd_create",
      templateUrl: "app/crowd/crowd.create.html",
      controller: 'crowdCtrl',
      controllerAs : 'vm'
  })
  .state('index.group_view', {
      url: "/group_view",
      templateUrl: "app/group/group.view.html",
      controller: 'groupCtrl',
      controllerAs : 'vm'
  })
  .state('index.group_create', {
      url: "/group_create",
      templateUrl: "app/group/group.create.html",
      controller: 'groupCtrl',
      controllerAs : 'vm'
  })

  $locationProvider.html5Mode(true);
}





