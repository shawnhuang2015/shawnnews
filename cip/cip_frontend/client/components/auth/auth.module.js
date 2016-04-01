'use strict';

angular.module('cipApp.auth', [
  'cipApp.constants',
  'cipApp.util',
  'ngCookies',
  'ui.router'
])
  .config(function($httpProvider) {
    $httpProvider.interceptors.push('authInterceptor');
  });
