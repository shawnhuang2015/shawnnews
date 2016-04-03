/**
 * INSPINIA - Responsive Admin Theme
 *
 */

/**
 * MainCtrl - controller
 */
function mainCtrl($scope, $state, $interval) {

    this.userName = 'asdfExample user';
    this.helloText = 'Welcome in SeedProject';
    this.descriptionText = 'It is an application skeleton for a typical AngularJS web app. You can use it to quickly bootstrap your angular webapp projects and dev environment for these project.';

    // $scope.i = 0;
    // $interval(function() {
    //   $scope.i++;
    // }, 100);
};

function minorCtrl($scope) {
  this.userName = 'Example Userssss';
  this.helloText = 'Hello Test helloText';
  this.descriptionText = 'It dsfasfasddf sdfsadprojecsdfaasdfts anddsfasfasddf sdfsadprojecsdfaasdfts anddsfasfasddf sdfsadprojecsdfaasdfts andipsdsfasfasddf sdfsadprojecsdfaasdfts and dev environmsdfasdent for these project.';
}


angular
  .module('cipApp')
  .controller('mainCtrl', mainCtrl)
  .controller('minorCtrl', minorCtrl)