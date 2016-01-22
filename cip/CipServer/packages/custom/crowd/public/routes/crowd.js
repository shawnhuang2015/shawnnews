'use strict';

angular.module('mean.crowd').config(['$stateProvider',
  function($stateProvider) {
    $stateProvider.state('crowd example page', {
      url: '/crowd',
      templateUrl: 'crowd/views/index.html'
    });
  }
]);
