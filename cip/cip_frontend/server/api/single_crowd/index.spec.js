'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var singleCrowdCtrlStub = {
  index: 'singleCrowdCtrl.index',
  show: 'singleCrowdCtrl.show',
  create: 'singleCrowdCtrl.create',
  update: 'singleCrowdCtrl.update',
  destroy: 'singleCrowdCtrl.destroy'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var singleCrowdIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './single_crowd.controller': singleCrowdCtrlStub
});

describe('SingleCrowd API Router:', function() {

  it('should return an express router instance', function() {
    singleCrowdIndex.should.equal(routerStub);
  });

  describe('GET /api/single_crowd', function() {

    it('should route to singleCrowd.controller.index', function() {
      routerStub.get
        .withArgs('/', 'singleCrowdCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/single_crowd/:id', function() {

    it('should route to singleCrowd.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'singleCrowdCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/single_crowd', function() {

    it('should route to singleCrowd.controller.create', function() {
      routerStub.post
        .withArgs('/', 'singleCrowdCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/single_crowd/:id', function() {

    it('should route to singleCrowd.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'singleCrowdCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/single_crowd/:id', function() {

    it('should route to singleCrowd.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'singleCrowdCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/single_crowd/:id', function() {

    it('should route to singleCrowd.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'singleCrowdCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

});
