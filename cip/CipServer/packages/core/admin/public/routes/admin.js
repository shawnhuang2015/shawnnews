'use strict';
angular.module('mean.admin').config(['$stateProvider', '$urlRouterProvider',
    function ($stateProvider, $urlRouterProvider) {
        $stateProvider
            .state('crowd', {
                url: '/crowd/main',
                templateUrl: 'cipmanager/views/index.html',
                requiredCircles: {
                    circles: ['admin']
                }
            })
            .state('group', {
                url: '/crowd/group',
                templateUrl: 'cipmanager/views/groupIndex.html',
                requiredCircles: {
                    circles: ['admin']
                }
            });
    }
]).config(['ngClipProvider',
    function (ngClipProvider) {
        ngClipProvider.setPath('../admin/assets/lib/zeroclipboard/dist/ZeroClipboard.swf');
    }
]);