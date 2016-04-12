'use strict';

var app = require('../..');
import request from 'supertest';

var newSingleCrowd1;
var newSingleCrowd2;
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
          crowdName: 'TestSingleCrowd1',
          description: 'This is the brand new singleCrowd number 1!!!',
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
          newSingleCrowd1 = res.body;
          done();
        });
    });

    it('should respond with the newly created singleCrowd', function() {
      newSingleCrowd1.crowdName.should.equal('TestSingleCrowd1');
      newSingleCrowd1.description.should.equal('This is the brand new singleCrowd number 1!!!');
    });

  });


  describe('POST /api/crowds', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/crowds')
        .send({
          crowdName: 'TestSingleCrowd2',
          description: 'This is the brand new singleCrowd number 2!!!',
          type: 'dynamic',
          count: 3,
          selector: {
            tag: [
              { 
                factor: 'gender.male',
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
          newSingleCrowd2 = res.body;
          done();
        });
    });

    it('should respond with the newly created singleCrowd', function() {
      newSingleCrowd2.crowdName.should.equal('TestSingleCrowd2');
      newSingleCrowd2.description.should.equal('This is the brand new singleCrowd number 2!!!');
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
          selector: [newSingleCrowd1._id, newSingleCrowd2._id]
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
    var groupCount;

    beforeEach(function(done) {
      request(app)
        .get('/api/groups/count')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err,res) => {
          if (err) {
            return done(err);
          }
          groupCount = res.body;
          done();
        });
    });

    afterEach(function() {
      groupCount = 0;
    });

    it('should response with the singleCrowd count', function() {
      groupCount.should.be.equal(1);
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


  describe('GET /api/groups/:id/sample', function() {
    var groupSample;

    beforeEach(function(done) {
      request(app)
        .get('/api/groups/' + newGroupCrowd._id + '/sample?count=10')
        .expect(200)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          groupSample = res.body;
          done();
        });
    });

    afterEach(function() {
      groupSample = {};
    });

    it('should respond with JSON object', function() {
      groupSample.should.be.instanceOf(Object);
    });

  });

  describe('POST /api/groups/user_count', function() {
    var userCount;

    beforeEach(function(done) {
      request(app)
        .post('/api/groups/user_count?rid=44441112233')    
        .expect(200)
        .send({
          selector: {
            tag: [
              { 
                condition: 'tag',
                factor: 'gender.female',
                operator: '=',
                weight: 1,
                name: 'gender'
              }
            ],
            ontology: [],
            behavior: []
          }
        })
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          userCount = res.body;
          done();
        });
    });

    afterEach(function() {
      userCount = {};
    });

    it('should respond with JSON object', function() {
      userCount.should.be.instanceOf(Object);
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
        .delete('/api/crowds/' + newSingleCrowd1._id)
        .expect(204)
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
        .delete('/api/crowds/' + newSingleCrowd2._id)
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
