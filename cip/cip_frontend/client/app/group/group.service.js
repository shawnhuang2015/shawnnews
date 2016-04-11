'use strict';

angular.module('cipApp')
  .factory('groupFactory', ['$http',
    function($http) {
      return {
        test : 'test group factory',
        viewGroups : function(page_id, page_size, callback) {
        $http.get('/api/groups/',{params:{page_id:page_id,page_size:page_size}})
        .success(function(data, status, headers, config) {
            var success = status == 200 || status == 200;
            callback({
                success: success,
                list: data
            });
        })
        .error(function(data, status, headers, config) {
            console.log('err');
            callback({
                success: false
            });
        }); 
      },
      }
    }
  ]);
