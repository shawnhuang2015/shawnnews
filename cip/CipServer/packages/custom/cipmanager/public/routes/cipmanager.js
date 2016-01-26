'use strict';

angular.module('mean.cipmanager').config(['$stateProvider',
  function($stateProvider) {
    $stateProvider.state('cipmanager example page', {
      url: '/cipmanager/example',
      templateUrl: 'cipmanager/views/index.html'
    });
  }
]);
