'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var singleCrowdCtrlStub = {
  index: 'singleCrowdCtrl.index',
  count: 'singleCrowdCtrl.count',
  show: 'singleCrowdCtrl.show',
  create: 'singleCrowdCtrl.create',
  update: 'singleCrowdCtrl.update',
  destroy: 'singleCrowdCtrl.destroy',
  sample: 'singleCrowdCtrl.sample',
  //userList: 'singleCrowdCtrl.userList',
  userCount: 'singleCrowdCtrl.userCount'
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

  describe('GET /api/crowds', function() {

    it('should route to singleCrowd.controller.index', function() {
      routerStub.get
        .withArgs('/', 'singleCrowdCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowds/count', function() {

    it('should route to singleCrowd.controller.count', function() {
      routerStub.get
        .withArgs('/count', 'singleCrowdCtrl.count')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowds/:id', function() {

    it('should route to singleCrowd.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'singleCrowdCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/crowds', function() {

    it('should route to singleCrowd.controller.create', function() {
      routerStub.post
        .withArgs('/', 'singleCrowdCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/crowds/:id', function() {

    it('should route to singleCrowd.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'singleCrowdCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/crowds/:id', function() {

    it('should route to singleCrowd.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'singleCrowdCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/crowds/:id', function() {

    it('should route to singleCrowd.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'singleCrowdCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowds/:id/sample', function() {

    it('should route to singleCrowd.controller.sample', function() {
      routerStub.get
        .withArgs('/:id/sample', 'singleCrowdCtrl.sample')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/crowds/user_count', function() {

    it('should route to singleCrowd.controller.userCount', function() {
      routerStub.post
        .withArgs('/user_count', 'singleCrowdCtrl.userCount')
        .should.have.been.calledOnce;
    });

  });

});
