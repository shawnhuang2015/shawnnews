'use strict';

angular.module('cipApp')
.factory('crowdFactory', ['$http',
  function($http) {
    return {
      getOntology : function (callback) {
        $http.get('/api/ontologys')
        .success(function(data, status, headers, config) {
            callback({
                success: !data.error,
                content: data
            });
        })
        .error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      getCrowdsCount : function(callback) {
        // Temporary - probably it should to be resource based.
        $http.get('/api/crowds/count')
        .success(function(data, status, headers, config) {
            callback({
                success: !data.error,
                length: data.error ? 0 : data.results.count
            });
        })
        .error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      viewCrowds : function(page_id, page_size, callback) {
        $http.get('/api/crowds/',{params:{page_id:page_id,page_size:page_size}})
        .success(function(data, status, headers, config) {
            console.log('success');
            callback({
                success: !data.error,
                list: data.results
            });
        })
        .error(function(data, status, headers, config) {
            console.log('err');
            callback({
                success: false
            });
        }); 
      },
      getCrowd : function(crowdID, callback) {
        $http.get('/api/crowds/' + crowdID)
        .success(function(data, status, headers, config) {
            callback({
                success: !data.error
            });
        })
        .error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      deleteCrowd : function(crowdID, callback) {
        $http.delete('/api/crowds/' + crowdID)
        .success(function(data, status, headers, config) {
            callback({
                success: !data.error
            });
        })
        .error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      updateCrowd : function(crowdID, callback) {
        $http.put('/api/crowds' + crowdID)
        .success(function(data, status, headers, config) {
            callback({
                success: !data.error
            });
        })
        .error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      createCrowd : function(crowd, callback) {
        $http.post('/api/crowds/',crowd).
        success(function(data, status, headers, config) {
            callback({
                success: !data.error
            });
        }).
        error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      getCrowdSample : function(crowdID, sample_number, type, callback) {
        $http.get('/api/crowds/' + crowdID + '/sample', {params:{count:sample_number, type:type}})
        .success(function(data, status, headers, config) {
            callback({
                success: !data.error
            });
        })
        .error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      },
      getCrowdUserCountByFactor : function(rid, factor, callback) {
        $http.post('/api/crowds/user_count?rid='+rid, factor)
        .success(function(data, status, headers, config) {
            callback(factor, {
                success: !data.error,
                requestId: data.results.requestId,
                length: data.error ? 0 : data.results.count
            });
        })
        .error(function(data, status, headers, config) {
            callback(factor, {
                success: false
            });
        });
      }
    }
  }
]);
