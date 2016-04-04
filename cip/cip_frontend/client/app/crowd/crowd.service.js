'use strict';

angular.module('cipApp')
.factory('crowdFactory', ['$resource', function ($resource) {
  // Service logic
  // ...


  // Public API here
  // return $resource('api/db/crowd/:crowdName', {
  //     crowdName: '@crowdName'
  // }, 
  // {
  //     update: {
  //         method: 'PUT'
  //     }
  // });
  return $resource('api/db/crowd/:crowdID'); // Note the full endpoint address
  // Example factory('Entry')
  // var entry = Entry.get({ id: $scope.id }, function() {
  //   console.log(entry);
  // }); // get() returns a single entry

  // var entries = Entry.query(function() {
  //   console.log(entries);
  // }); //query() returns all the entries

  // $scope.entry = new Entry(); //You can instantiate resource class

  // $scope.entry.data = 'some data';

  // Entry.save($scope.entry, function() {
  //   //data saved. do something here.
  // }); //saves an entry. Assuming $scope.entry is the Entry object  
  }
])
.factory('CrowdService', ['$http',
  function($http) {
    var createCrowd = function(crowd, callback) {
      $http.post('/api/db/crowd',crowd).
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
    }

    var getCount = function(callback) {
      // Temporary - probably it should to be resource based.
      $http.get('/api/db/crowdcount').
      success(function(data, status, headers, config) {
          callback({
              success: !data.error,
              length: data.error ? 0 : data.results.count
          });
      }).
      error(function(data, status, headers, config) {
          callback({
              success: false
          });
      });
    };

    var getUserList = function(crowdName, type, callback) {
      $http.get('/api/rest/crowd/sample',{params:{cname:crowdName,count:10,type:type}}).
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

    var getUserCount = function(crowdName, callback) {
      // Temporary - probably it should to be resource based.
      $http.get('/api/rest/crowd/count',{params:{cname:crowdName}}).
      success(function(data, status, headers, config) {
          callback({
              success: !data.error,
              length: data.error ? 0 : data.results.count
          });
      }).
      error(function(data, status, headers, config) {
          callback({
              success: false
          });
      });
    }

    var getUserCountByFactor = function(rid, factor, callback) {
      // Temporary - probably it should to be resource based.
      $http.post('/api/rest/crowd/count?rid='+rid, factor).
      success(function(data, status, headers, config) {
          callback(factor, {
              success: !data.error,
              requestId: data.results.requestId,
              length: data.error ? 0 : data.results.count
          });
      }).
      error(function(data, status, headers, config) {
          callback({
              success: false
          });
      });
    }

    var getOntology = function (callback) {
      $http.get('/api/ontology').
      success(function(data, status, headers, config) {
          callback({
              success: !data.error,
              content: data
          });
      }).
      error(function(data, status, headers, config) {
          callback({
              success: false
          });
      });
    }

    return {
      getCount: getCount,
      getUserCount: getUserCount,
      getUserList: getUserList,
      getOntology: getOntology,
      getUserCountByFactor: getUserCountByFactor,
      createCrowd: createCrowd,
    };
  }
]);
