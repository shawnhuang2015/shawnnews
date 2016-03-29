'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var crowdSingleCtrlStub = {
  index: 'crowdSingleCtrl.index',
  show: 'crowdSingleCtrl.show',
  create: 'crowdSingleCtrl.create',
  update: 'crowdSingleCtrl.update',
  destroy: 'crowdSingleCtrl.destroy'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var crowdSingleIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './crowdSingle.controller': crowdSingleCtrlStub
});

describe('CrowdSingle API Router:', function() {

  it('should return an express router instance', function() {
    crowdSingleIndex.should.equal(routerStub);
  });

  describe('GET /api/crowdSingles', function() {

    it('should route to crowdSingle.controller.index', function() {
      routerStub.get
        .withArgs('/', 'crowdSingleCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/crowdSingles/:id', function() {

    it('should route to crowdSingle.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'crowdSingleCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/crowdSingles', function() {

    it('should route to crowdSingle.controller.create', function() {
      routerStub.post
        .withArgs('/', 'crowdSingleCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/crowdSingles/:id', function() {

    it('should route to crowdSingle.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'crowdSingleCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/crowdSingles/:id', function() {

    it('should route to crowdSingle.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'crowdSingleCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/crowdSingles/:id', function() {

    it('should route to crowdSingle.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'crowdSingleCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

});
