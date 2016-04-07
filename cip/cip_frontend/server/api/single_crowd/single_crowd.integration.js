'use strict';

var app = require('../..');
import request from 'supertest';

var newSingleCrowd;

describe('SingleCrowd API:', function() {

  describe('GET /api/crowds', function() {
    var singleCrowds;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowds')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          singleCrowds = res.body;
          done();
        });
    });

    it('should respond with JSON object', function() {
      singleCrowds.should.be.instanceOf(Object);
    });

  });

  describe('POST /api/crowds', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/crowds')
        .send({
          crowdName: 'TestCrowd',
          description: 'This is the brand new singleCrowd!!!',
          type: 'static',
          count: 1,
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
      newSingleCrowd.crowdName.should.equal('TestCrowd');
      newSingleCrowd.description.should.equal('This is the brand new singleCrowd!!!');
    });

  });

  describe('GET /api/crowds/count', function() {
    var crowdCount;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowds/count')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err,res) => {
          if (err) {
            return done(err);
          }
          crowdCount = res.body.count;
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

  describe('GET /api/crowds/:id', function() {
    var singleCrowd;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowds/' + newSingleCrowd._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          singleCrowd = res.body;
          done();
        });
    });

    afterEach(function() {
      singleCrowd = {};
    });

    it('should respond with the requested singleCrowd', function() {
      singleCrowd.crowdName.should.equal('TestCrowd');
      singleCrowd.description.should.equal('This is the brand new singleCrowd!!!');
    });

  });

  describe('PUT /api/crowds/:id', function() {
    var updatedSingleCrowd;

    beforeEach(function(done) {
      request(app)
        .put('/api/crowds/' + newSingleCrowd._id)
        .send({
          crowdName: 'TestCrowd',
          description: 'This is the updated singleCrowd!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedSingleCrowd = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedSingleCrowd = {};
    });

    it('should respond with the updated singleCrowd', function() {
      updatedSingleCrowd.crowdName.should.equal('TestCrowd');
      updatedSingleCrowd.description.should.equal('This is the updated singleCrowd!!!');
    });

  });

  describe('GET /api/crowds/:id/sample', function() {
    var crowdSample;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowds/' + newSingleCrowd._id + '/sample')
        .expect(200)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          crowdSample = res.body;
          done();
        });
    });

    afterEach(function() {
      crowdSample = {};
    });

    it('should respond with JSON object', function() {
      crowdSample.should.be.instanceOf(Object);
    });

  });

  describe('POST /api/crowds/user_count', function() {
    var userCount;

    beforeEach(function(done) {
      request(app)
        .post('/api/crowds/user_count?rid=44441112233')    
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

    it('should respond with 404 when singleCrowd does not exist', function(done) {
      request(app)
        .delete('/api/crowds/' + newSingleCrowd._id)
        .expect(404)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

  });

});
