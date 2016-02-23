'use strict';

angular.module('mean.cipmanager').factory('Cipmanager', ['$resource',
  function($resource) {
    return $resource('api/db/crowd/:crowdName', {
      crowdName: '@crowdName'
    }, {
      update: {
        method: 'PUT'
      },
    });
  }
]);
