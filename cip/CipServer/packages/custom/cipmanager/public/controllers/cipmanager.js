'use strict';

/* jshint -W098 */
angular.module('mean.cipmanager').controller('CipmanagerController', ['$scope', 'Global', 'Cipmanager',
  function($scope, Global, Cipmanager) {
    $scope.global = Global;
    $scope.package = {
      name: 'cipmanager'
    };

    $scope.find = function() {
      Crowd.query(function(crowds) {
        $scope.crowds = crowds;
      });
    };

  }
]);
