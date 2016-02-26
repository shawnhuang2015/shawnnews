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

angular.module('mean.cipmanager').factory('Groupmanager', ['$resource',
  function($resource) {
    return $resource('api/db/group/crowd/:crowdName', {
      crowdName: '@crowdName'
    }, {
      update: {
        method: 'PUT'
      },
    });
  }
]);