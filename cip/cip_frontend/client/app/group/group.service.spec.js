'use strict';

describe('Service: group', function () {

  // load the service's module
  beforeEach(module('cipApp'));

  // instantiate service
  var group;
  beforeEach(inject(function (_group_) {
    group = _group_;
  }));

  it('should do something', function () {
    expect(!!group).toBe(true);
  });

});
