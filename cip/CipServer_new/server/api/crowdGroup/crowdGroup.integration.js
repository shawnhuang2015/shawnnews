'use strict';

var app = require('../..');
import request from 'supertest';

var newCrowdGroup;

describe('CrowdGroup API:', function() {

  describe('GET /api/crowdGroups', function() {
    var crowdGroups;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowdGroups')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          crowdGroups = res.body;
          done();
        });
    });

    it('should respond with JSON array', function() {
      crowdGroups.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/crowdGroups', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/crowdGroups')
        .send({
          name: 'New CrowdGroup',
          info: 'This is the brand new crowdGroup!!!'
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newCrowdGroup = res.body;
          done();
        });
    });

    it('should respond with the newly created crowdGroup', function() {
      newCrowdGroup.name.should.equal('New CrowdGroup');
      newCrowdGroup.info.should.equal('This is the brand new crowdGroup!!!');
    });

  });

  describe('GET /api/crowdGroups/:id', function() {
    var crowdGroup;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowdGroups/' + newCrowdGroup._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          crowdGroup = res.body;
          done();
        });
    });

    afterEach(function() {
      crowdGroup = {};
    });

    it('should respond with the requested crowdGroup', function() {
      crowdGroup.name.should.equal('New CrowdGroup');
      crowdGroup.info.should.equal('This is the brand new crowdGroup!!!');
    });

  });

  describe('PUT /api/crowdGroups/:id', function() {
    var updatedCrowdGroup;

    beforeEach(function(done) {
      request(app)
        .put('/api/crowdGroups/' + newCrowdGroup._id)
        .send({
          name: 'Updated CrowdGroup',
          info: 'This is the updated crowdGroup!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedCrowdGroup = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedCrowdGroup = {};
    });

    it('should respond with the updated crowdGroup', function() {
      updatedCrowdGroup.name.should.equal('Updated CrowdGroup');
      updatedCrowdGroup.info.should.equal('This is the updated crowdGroup!!!');
    });

  });

  describe('DELETE /api/crowdGroups/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/crowdGroups/' + newCrowdGroup._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

    it('should respond with 404 when crowdGroup does not exist', function(done) {
      request(app)
        .delete('/api/crowdGroups/' + newCrowdGroup._id)
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
