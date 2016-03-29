'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var crowdGroupCtrlStub = {
  index: 'crowdGroupCtrl.index',
  show: 'crowdGroupCtrl.show',
  create: 'crowdGroupCtrl.create',
  update: 'crowdGroupCtrl.update',
  destroy: 'crowdGroupCtrl.destroy'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var crowdGroupIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './crowdGroup.controller': crowdGroupCtrlStub
});

describe('CrowdGroup API Router:', function() {

  it('should return an express router instance', function() {
    crowdGroupIndex.should.equal(routerStub);
  });

  describe('GET /api/crowdGroups', function() {

    it('should route to crowdGroup.controller.index', function() {
      routerStub.get
        .withArgs('/', 'crowdGroupCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowdGroups/:id', function() {

    it('should route to crowdGroup.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'crowdGroupCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/crowdGroups', function() {

    it('should route to crowdGroup.controller.create', function() {
      routerStub.post
        .withArgs('/', 'crowdGroupCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/crowdGroups/:id', function() {

    it('should route to crowdGroup.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'crowdGroupCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/crowdGroups/:id', function() {

    it('should route to crowdGroup.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'crowdGroupCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/crowdGroups/:id', function() {

    it('should route to crowdGroup.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'crowdGroupCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

});
