'use strict';

var proxyquire = require('proxyquire').noPreserveCache();

var groupCtrlStub = {
  index: 'groupCtrl.index',
  count: 'groupCtrl.count',
  show: 'groupCtrl.show',
  create: 'groupCtrl.create',
  update: 'groupCtrl.update',
  destroy: 'groupCtrl.destroy',
  sample: 'groupCtrl.sample',
  userCount: 'groupCtrl.userCount'
};

var routerStub = {
  get: sinon.spy(),
  put: sinon.spy(),
  patch: sinon.spy(),
  post: sinon.spy(),
  delete: sinon.spy()
};

// require the index with our stubbed out modules
var groupIndex = proxyquire('./index.js', {
  'express': {
    Router: function() {
      return routerStub;
    }
  },
  './group.controller': groupCtrlStub
});

describe('Group Crowd API Router:', function() {

  it('should return an express router instance', function() {
    groupIndex.should.equal(routerStub);
  });

  describe('GET /api/groups', function() {

    it('should route to group.controller.index', function() {
      routerStub.get
        .withArgs('/', 'groupCtrl.index')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/groups/count', function() {

    it('should route to group.controller.count', function() {
      routerStub.get
        .withArgs('/count', 'groupCtrl.count')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/groups/:id', function() {

    it('should route to group.controller.show', function() {
      routerStub.get
        .withArgs('/:id', 'groupCtrl.show')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/groups', function() {

    it('should route to group.controller.create', function() {
      routerStub.post
        .withArgs('/', 'groupCtrl.create')
        .should.have.been.calledOnce;
    });

  });

  describe('PUT /api/groups/:id', function() {

    it('should route to group.controller.update', function() {
      routerStub.put
        .withArgs('/:id', 'groupCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('PATCH /api/groups/:id', function() {

    it('should route to group.controller.update', function() {
      routerStub.patch
        .withArgs('/:id', 'groupCtrl.update')
        .should.have.been.calledOnce;
    });

  });

  describe('DELETE /api/groups/:id', function() {

    it('should route to group.controller.destroy', function() {
      routerStub.delete
        .withArgs('/:id', 'groupCtrl.destroy')
        .should.have.been.calledOnce;
    });

  });

  describe('GET /api/groups/:id/sample', function() {

    it('should route to group.controller.sample', function() {
      routerStub.get
        .withArgs('/:id/sample', 'groupCtrl.sample')
        .should.have.been.calledOnce;
    });

  });

  describe('POST /api/groups/user_count', function() {

    it('should route to group.controller.userCount', function() {
      routerStub.post
        .withArgs('/user_count', 'groupCtrl.userCount')
        .should.have.been.calledOnce;
    });

  });

});
