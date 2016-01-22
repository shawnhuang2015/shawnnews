'use strict';

/* jshint -W098 */
angular.module('mean.crowd').controller('CrowdController', ['$scope', 'Global', 'Crowd',
  function($scope, Global, Crowd) {
    $scope.global = Global;
    $scope.package = {
      name: 'crowd'
    };
  }
]);
