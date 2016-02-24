/**
 * Created by chenzhuwei on 2/19/16.
 */
'use strict';

angular.module('mean.cipmanager').factory('CrowdService', ['$http',
    function($http) {
        var createCrowd = function(crowd, callback) {
            $http.post('/api/db/crowd',crowd).
            success(function(data, status, headers, config) {
                callback({
                    success: true
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
                    success: true,
                    length: data.results.count
                });
            }).
            error(function(data, status, headers, config) {
                callback({
                    success: false
                });
            });
        };

        var getUserList = function(crowdName, pageId, pageSize, callback) {
            $http.get('/api/rest/crowd/detail',{params:{cname:crowdName, pageId:pageId, pageSz:pageSize}}).
            success(function(data, status, headers, config) {
                callback({
                    success: true,
                    userList: data.results.userIds
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
                    success: true,
                    length: data.results.count
                });
            }).
            error(function(data, status, headers, config) {
                callback({
                    success: false
                });
            });
        }

        var getUserCountByFactor = function(factor, callback) {
            // Temporary - probably it should to be resource based.
            $http.post('/api/rest/crowd/count',factor).
            success(function(data, status, headers, config) {
                callback({
                    success: true,
                    length: data.results.count
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
                    success: true,
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