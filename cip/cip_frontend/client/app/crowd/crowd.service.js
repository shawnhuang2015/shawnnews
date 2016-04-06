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
      getCrowdSample : function(crowdID, user_count, type, callback) {
        $http.get('/api/crowds/' + crowdID + '/sample', {params:{count:user_count, type:type}})
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
      getUserList : function(crowdName, type, callback) {
        $http.get('/api/crowds/'+123+'/sample',{params:{cname:crowdName,count:10,type:type}}).
        success(function(data, status, headers, config) {
            callback({
                success: !data.error,
                userList: data.error ? [] : data.results.userIds
            });
        }).
        error(function(data, status, headers, config) {
            callback({
                success: false
            });
        });
      }
    }
  }
]);
