'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var restCtrlStub = {
  index: 'restCtrl.index',
  show: 'restCtrl.show',
  create: 'restCtrl.create',
  update: 'restCtrl.update',
  destroy: 'restCtrl.destroy'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var restIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './rest.controller': restCtrlStub
});

describe('Rest API Router:', function() {

  it('should return an express router instance', function() {
    restIndex.should.equal(routerStub);
  });

  describe('GET /api/rests', function() {

    it('should route to rest.controller.index', function() {
      routerStub.get
        .withArgs('/', 'restCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/rests/:id', function() {

    it('should route to rest.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'restCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/rests', function() {

    it('should route to rest.controller.create', function() {
      routerStub.post
        .withArgs('/', 'restCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/rests/:id', function() {

    it('should route to rest.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'restCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/rests/:id', function() {

    it('should route to rest.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'restCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/rests/:id', function() {

    it('should route to rest.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'restCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

});
