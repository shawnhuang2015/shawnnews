/**
 * Created by chenzhuwei on 3/9/16.
 */
'use strict';
angular.module('mean.cipmanager').factory('GroupManager', ['$resource',
    function($resource) {
        return $resource('api/db/group/crowd/:crowdName', {
            crowdName: '@crowdName'
        }, {
            update: {
                method: 'PUT'
            }
        });
    }
]);

angular.module('mean.cipmanager').factory('GroupService', ['$http',
    function($http) {
        var getGroupCount = function (callback) {
            $http.get('/api/db/group/crowdcount').
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

        var createGroup = function(group, callback) {
            $http.post('/api/db/group/crowd',group).
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
        };

        var getGroupUserCount = function(rid, factor, callback) {
            $http.post('/api/rest/group/crowd/count?rid='+rid,factor).
            success(function(data, status, headers, config) {
                callback({
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
        };

        return {
            getGroupCount: getGroupCount,
            createGroup: createGroup,
            getGroupUserCount: getGroupUserCount,
        };
    }
]);
