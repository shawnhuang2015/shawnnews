'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var miscCtrlStub = {
  download: 'miscCtrl.download',
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var miscIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './misc.controller': miscCtrlStub
});

describe('Misc API Router:', function() {

  it('should return an express router instance', function() {
    miscIndex.should.equal(routerStub);
  });

  describe('GET /api/miscs/download', function() {

    it('should route to misc.controller.download', function() {
      routerStub.get
        .withArgs('/download', 'miscCtrl.download')
        .should.have.been.calledOnce;
    });

  });

});
