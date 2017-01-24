'use strict';

describe('Service: crowdService', function () {

  // load the service's module
  beforeEach(module('cipApp'));

  // instantiate service
  var crowdService;
  beforeEach(inject(function (_crowdService_) {
    crowdService = _crowdService_;
  }));

  it('should do something', function () {
    expect(!!crowdService).toBe(true);
  });

});
