'use strict';

var app = require('../..');
import request from 'supertest';

var newSingleCrowd;
var newGroupCrowd;

describe('Group Crowd API:', function() {

  describe('GET /api/ontologys', function() {
    var ontologys;

    beforeEach(function(done) {
      request(app)
        .get('/api/ontologys')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          ontologys = res.body;
          done();
        });
    });

    it('should respond with JSON object', function() {
      ontologys.should.be.instanceOf(Object);
    });

  });

  describe('GET /api/groups', function() {
    var groupCrowds;

    beforeEach(function(done) {
      request(app)
        .get('/api/groups')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          groupCrowds = res.body;
          done();
        });
    });

    it('should respond with JSON array', function() {
      groupCrowds.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/crowds', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/crowds')
        .send({
          crowdName: 'TestSingleCrowd',
          description: 'This is the brand new singleCrowd!!!',
          type: 'dynamic',
          count: 3,
          selector: {
            tag: [
              { 
                factor: 'gender.female',
                operator: '=',
                name: 'gender',
                count: '3'
              }
            ],
            ontology: [],
            behavior: []
          }
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newSingleCrowd = res.body;
          done();
        });
    });

    it('should respond with the newly created singleCrowd', function() {
      newSingleCrowd.crowdName.should.equal('TestSingleCrowd');
      newSingleCrowd.description.should.equal('This is the brand new singleCrowd!!!');
    });

  });

  describe('POST /api/groups', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/groups')
        .send({
          crowdName: 'TestGroupCrowd',
          description: 'This is the brand new groupCrowd!!!',
          type: 'static',
          count: 3,
          logic: 'or',
          selector: [newSingleCrowd._id]
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newGroupCrowd = res.body;
          done();
        });
    });

    it('should respond with the newly created group', function() {
      newGroupCrowd.crowdName.should.equal('TestGroupCrowd');
      newGroupCrowd.description.should.equal('This is the brand new groupCrowd!!!');
    });

  });

  describe('GET /api/groups/count', function() {
    var crowdCount;

    beforeEach(function(done) {
      request(app)
        .get('/api/groups/count')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err,res) => {
          if (err) {
            return done(err);
          }
          crowdCount = res.body;
          done();
        });
    });

    afterEach(function() {
      crowdCount = 0;
    });

    it('should response with the singleCrowd count', function() {
      crowdCount.should.be.equal(1);
    });

  });


  describe('GET /api/groups/:id', function() {
    var groupCrowd;

    beforeEach(function(done) {
      request(app)
        .get('/api/groups/' + newGroupCrowd._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          groupCrowd = res.body;
          done();
        });
    });

    afterEach(function() {
      groupCrowd = {};
    });

    it('should respond with the requested group', function() {
      groupCrowd.crowdName.should.equal('TestGroupCrowd');
      groupCrowd.description.should.equal('This is the brand new groupCrowd!!!');
    });

  });

  describe('PUT /api/groups/:id', function() {
    var updatedGroupCrowd;

    beforeEach(function(done) {
      request(app)
        .put('/api/groups/' + newGroupCrowd._id)
        .send({
          crowdName: 'TestGroupCrowd',
          description: 'This is the updated groupCrowd!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedGroupCrowd = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedGroupCrowd = {};
    });

    it('should respond with the updated group', function() {
      updatedGroupCrowd.crowdName.should.equal('TestGroupCrowd');
      updatedGroupCrowd.description.should.equal('This is the updated groupCrowd!!!');
    });

  });

  describe('DELETE /api/groups/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/groups/' + newGroupCrowd._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

    it('should respond with 404 when group does not exist', function(done) {
      request(app)
        .delete('/api/groups/' + newGroupCrowd._id)
        .expect(404)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

  });
  
  describe('DELETE /api/crowds/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/crowds/' + newSingleCrowd._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

  });

});
