'use strict';

angular.module('cipApp')
  .factory('groupFactory', ['$http',
    function($http) {
      return {
        test : 'test group factory',
        createGroup : function(group, callback) {
          $http.post('/api/groups', group)
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
        deleteGroup : function(groupID, callback) {
          $http.delete('/api/groups/' + groupID)
          .success(function(data, status, headers, config) {
              console.log("delete group ", status)
              var success = status == 204;
              callback({
                  success: success,
                  _id: groupID
              });
          })
          .error(function(data, status, headers, config) {
              callback({
                  success: false
              });
          });
        },
        getGroupUserCount : function(rid, factor, callback) {
          $http.post('/api/groups/user_count?rid='+rid,factor)
          .success(function(data, status, headers, config) {
              callback({
                  success: !data.error,
                  requestId: data.results.requestId,
                  length: data.error ? 0 : data.results.count
              });
          })
          .error(function(data, status, headers, config) {
              callback({
                  success: false
              });
          });
        },
        getGroupSample : function(groupID, sample_number, type, callback) {
          $http.get('/api/groups/' + groupID + '/sample', {params:{count:sample_number, type:type}})
          .success(function(data, status, headers, config) {
              var success = status == 200;
              
              callback({
                  success: success,
                  data: data,
              });
          })
          .error(function(data, status, headers, config) {
              callback({
                  success: false
              });
          });
        },
      }
    }
  ]);
