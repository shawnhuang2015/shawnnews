/**
 * Created by chenzhuwei on 2/19/16.
 */
'use strict';
angular.module('mean.cipmanager').factory('CrowdManager', ['$resource',
    function($resource) {
        return $resource('api/db/crowd/:crowdName', {
            crowdName: '@crowdName'
        }, {
            update: {
                method: 'PUT'
            }
        });
    }
]);

angular.module('mean.cipmanager').factory('CrowdService', ['$http',
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
            //getGroupCount: getGroupCount,
            //createGroup: createGroup,
            //getGroupUserCount: getGroupUserCount,
        };
    }
]);
