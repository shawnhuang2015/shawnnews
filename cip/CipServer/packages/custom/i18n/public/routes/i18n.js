'use strict';

angular.module('mean.i18n').config(['$stateProvider',
  function($stateProvider) {
    $stateProvider.state('locales example page', {
      url: '/locales/example',
      templateUrl: 'locales/views/index.html'
    });
  }
]);
