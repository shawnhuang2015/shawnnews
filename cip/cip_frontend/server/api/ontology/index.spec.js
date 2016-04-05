'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var ontologyCtrlStub = {
  index: 'ontologyCtrl.index',
  show: 'ontologyCtrl.show',
  create: 'ontologyCtrl.create',
  update: 'ontologyCtrl.update',
  destroy: 'ontologyCtrl.destroy'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var ontologyIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './ontology.controller': ontologyCtrlStub
});

describe('Ontology API Router:', function() {

  it('should return an express router instance', function() {
    ontologyIndex.should.equal(routerStub);
  });

  describe('GET /api/ontologys', function() {

    it('should route to ontology.controller.index', function() {
      routerStub.get
        .withArgs('/', 'ontologyCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/ontologys/:id', function() {

    it('should route to ontology.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'ontologyCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/ontologys', function() {

    it('should route to ontology.controller.create', function() {
      routerStub.post
        .withArgs('/', 'ontologyCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/ontologys/:id', function() {

    it('should route to ontology.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'ontologyCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/ontologys/:id', function() {

    it('should route to ontology.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'ontologyCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/ontologys/:id', function() {

    it('should route to ontology.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'ontologyCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

});
