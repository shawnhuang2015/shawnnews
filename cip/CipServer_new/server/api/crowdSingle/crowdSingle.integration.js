'use strict';

var app = require('../..');
import request from 'supertest';

var newCrowdSingle;

describe('CrowdSingle API:', function() {

  describe('GET /api/crowdSingles', function() {
    var crowdSingles;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowdSingles')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          crowdSingles = res.body;
          done();
        });
    });

    it('should respond with JSON array', function() {
      crowdSingles.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/crowdSingles', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/crowdSingles')
        .send({
          name: 'New CrowdSingle',
          info: 'This is the brand new crowdSingle!!!'
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newCrowdSingle = res.body;
          done();
        });
    });

    it('should respond with the newly created crowdSingle', function() {
      newCrowdSingle.name.should.equal('New CrowdSingle');
      newCrowdSingle.info.should.equal('This is the brand new crowdSingle!!!');
    });

  });

  describe('GET /api/crowdSingles/:id', function() {
    var crowdSingle;

    beforeEach(function(done) {
      request(app)
        .get('/api/crowdSingles/' + newCrowdSingle._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          crowdSingle = res.body;
          done();
        });
    });

    afterEach(function() {
      crowdSingle = {};
    });

    it('should respond with the requested crowdSingle', function() {
      crowdSingle.name.should.equal('New CrowdSingle');
      crowdSingle.info.should.equal('This is the brand new crowdSingle!!!');
    });

  });

  describe('PUT /api/crowdSingles/:id', function() {
    var updatedCrowdSingle;

    beforeEach(function(done) {
      request(app)
        .put('/api/crowdSingles/' + newCrowdSingle._id)
        .send({
          name: 'Updated CrowdSingle',
          info: 'This is the updated crowdSingle!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedCrowdSingle = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedCrowdSingle = {};
    });

    it('should respond with the updated crowdSingle', function() {
      updatedCrowdSingle.name.should.equal('Updated CrowdSingle');
      updatedCrowdSingle.info.should.equal('This is the updated crowdSingle!!!');
    });

  });

  describe('DELETE /api/crowdSingles/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/crowdSingles/' + newCrowdSingle._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

    it('should respond with 404 when crowdSingle does not exist', function(done) {
      request(app)
        .delete('/api/crowdSingles/' + newCrowdSingle._id)
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
