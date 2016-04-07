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
  'ui.select',
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
  $urlRouterProvider.otherwise("/crowd/crowd_view");

  $stateProvider
  .state('crowd', {
      abstract: true,
      url: "/crowd",
      templateUrl: "template/views/common/content.html",
      controller: 'mainCtrl',
      controllerAs : 'main'
  })
  .state('crowd.main', {
      url: "/main",
      templateUrl: "template/views/main.html"
  })
  .state('crowd.crowd_view', {
      url: "/crowd_view",
      templateUrl: "app/crowd/crowd.view.html",
      controller: 'crowdCtrl',
      controllerAs : 'vm'
  })
  .state('crowd.crowd_create', {
      url: "/crowd_create",
      templateUrl: "app/crowd/crowd.create.html",
      controller: 'crowdCtrl',
      controllerAs : 'vm'
  })
  .state('crowd.crowd_save', {
      url: "/crowd_save",
      templateUrl: "app/crowd/crowd.save.html",
      controller: 'crowdCtrl',
      params: {
        crowdDetail: null
      },
      controllerAs : 'vm'
  })
  .state('crowd.crowd_user', {
      url: "/:crowdID/crowd_user",
      templateUrl: "app/crowd/crowd.user.html",
      controller: 'crowdCtrl',
      controllerAs : 'vm'
  })
  .state('crowd.group_view', {
      url: "/group_view",
      templateUrl: "app/group/group.view.html",
      controller: 'groupCtrl',
      controllerAs : 'vm'
  })
  .state('crowd.group_create', {
      url: "/group_create",
      templateUrl: "app/group/group.create.html",
      controller: 'groupCtrl',
      controllerAs : 'vm'
  })

  $locationProvider.html5Mode(true);
}





