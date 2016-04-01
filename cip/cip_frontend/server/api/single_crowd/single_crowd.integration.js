'use strict';

var app = require('../..');
import request from 'supertest';

var newSingleCrowd;

describe('SingleCrowd API:', function() {

  describe('GET /api/single_crowd', function() {
    var singleCrowds;

    beforeEach(function(done) {
      request(app)
        .get('/api/single_crowd')
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

    it('should respond with JSON array', function() {
      singleCrowds.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/single_crowd', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/single_crowd')
        .send({
          name: 'New SingleCrowd',
          info: 'This is the brand new singleCrowd!!!'
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
      newSingleCrowd.name.should.equal('New SingleCrowd');
      newSingleCrowd.info.should.equal('This is the brand new singleCrowd!!!');
    });

  });

  describe('GET /api/single_crowd/:id', function() {
    var singleCrowd;

    beforeEach(function(done) {
      request(app)
        .get('/api/single_crowd/' + newSingleCrowd._id)
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
      singleCrowd.name.should.equal('New SingleCrowd');
      singleCrowd.info.should.equal('This is the brand new singleCrowd!!!');
    });

  });

  describe('PUT /api/single_crowd/:id', function() {
    var updatedSingleCrowd;

    beforeEach(function(done) {
      request(app)
        .put('/api/single_crowd/' + newSingleCrowd._id)
        .send({
          name: 'Updated SingleCrowd',
          info: 'This is the updated singleCrowd!!!'
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
      updatedSingleCrowd.name.should.equal('Updated SingleCrowd');
      updatedSingleCrowd.info.should.equal('This is the updated singleCrowd!!!');
    });

  });

  describe('DELETE /api/single_crowd/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/single_crowd/' + newSingleCrowd._id)
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
        .delete('/api/single_crowd/' + newSingleCrowd._id)
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
