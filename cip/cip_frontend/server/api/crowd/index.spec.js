'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var crowdCtrlStub = {
  index: 'crowdCtrl.index',
  count: 'crowdCtrl.count',
  show: 'crowdCtrl.show',
  create: 'crowdCtrl.create',
  update: 'crowdCtrl.update',
  destroy: 'crowdCtrl.destroy',
  sample: 'crowdCtrl.sample',
  userCount: 'crowdCtrl.userCount'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var crowdIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './crowd.controller': crowdCtrlStub
});

describe('Single Crowd API Router:', function() {

  it('should return an express router instance', function() {
    crowdIndex.should.equal(routerStub);
  });

  describe('GET /api/crowds', function() {

    it('should route to crowd.controller.index', function() {
      routerStub.get
        .withArgs('/', 'crowdCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowds/count', function() {

    it('should route to crowd.controller.count', function() {
      routerStub.get
        .withArgs('/count', 'crowdCtrl.count')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowds/:id', function() {

    it('should route to crowd.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'crowdCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/crowds', function() {

    it('should route to crowd.controller.create', function() {
      routerStub.post
        .withArgs('/', 'crowdCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/crowds/:id', function() {

    it('should route to crowd.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'crowdCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/crowds/:id', function() {

    it('should route to crowd.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'crowdCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/crowds/:id', function() {

    it('should route to crowd.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'crowdCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowds/:id/sample', function() {

    it('should route to crowd.controller.sample', function() {
      routerStub.get
        .withArgs('/:id/sample', 'crowdCtrl.sample')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/crowds/user_count', function() {

    it('should route to crowd.controller.userCount', function() {
      routerStub.post
        .withArgs('/user_count', 'crowdCtrl.userCount')
        .should.have.been.calledOnce;
    });

  });

});
