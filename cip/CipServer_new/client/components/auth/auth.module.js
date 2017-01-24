'use strict';

angular.module('cipApp.auth', [
  'cipApp.constants',
  'cipApp.util',
  'ngCookies',
  'ngRoute'
])
  .config(function($httpProvider) {
    $httpProvider.interceptors.push('authInterceptor');
  });
