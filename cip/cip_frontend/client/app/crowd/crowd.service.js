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
]);
