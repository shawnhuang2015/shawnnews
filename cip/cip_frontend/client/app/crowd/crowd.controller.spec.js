'use strict';

describe('Controller: CrowdCtrl', function () {

  // load the controller's module
  beforeEach(module('cipApp.crowd'));

  var CrowdCtrl, scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($controller, $rootScope) {
    scope = $rootScope.$new();
    CrowdCtrl = $controller('CrowdCtrl', {
      $scope: scope
    });
  }));

  it('should ...', function () {
    expect(1).toEqual(1);
  });
});
