/**
 * MainCtrl - controller
 */
function mainCtrl($scope, $state, $interval) {

    this.userName = 'Example user';
    this.userRole = 'Manager';
    this.helloText = 'Welcome in SeedProject';
    this.descriptionText = 'It is an application skeleton for a typical AngularJS web app. You can use it to quickly bootstrap your angular webapp projects and dev environment for these project.';

};

angular
  .module('cipApp')
  .controller('mainCtrl', mainCtrl)




