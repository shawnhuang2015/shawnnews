'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var ontologyCtrlStub = {
  index: 'ontologyCtrl.index',
};

var routerStub = {
  get: sinon.spy(),
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

});
