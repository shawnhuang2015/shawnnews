'use strict';

angular.module('cipApp').factory('GroupManager', ['$resource',
    function($resource) {
        return $resource('/api/crowdGroup/crowd/:crowdName', {
            crowdName: '@crowdName'
        }, {
            update: {
                method: 'PUT'
            }
        });
    }
]);

angular.module('cipApp').factory('GroupService', ['$http',
    function($http) {
        var getGroupCount = function (callback) {
            $http.get('/api/crowdGroup/crowdcount').
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
            $http.post('/api/crowdGroup/crowd',group).
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
