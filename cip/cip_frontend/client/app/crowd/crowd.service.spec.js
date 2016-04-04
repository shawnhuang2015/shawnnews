'use strict';

describe('Service: crowd', function () {

  // load the service's module
  beforeEach(module('cipApp'));

  // instantiate service
  var crowd;
  beforeEach(inject(function (_crowd_) {
    crowd = _crowd_;
  }));

  it('should do something', function () {
    expect(!!crowd).toBe(true);
  });

});
