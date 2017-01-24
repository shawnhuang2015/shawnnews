'use strict';

describe('Component: CipComponent', function () {

  // load the controller's module
  beforeEach(module('cipApp'));

  var CipComponent, scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($componentController, $rootScope) {
    scope = $rootScope.$new();
    CipComponent = $componentController('CipComponent', {
      $scope: scope
    });
  }));

  it('should ...', function () {
    expect(1).toEqual(1);
  });
});
