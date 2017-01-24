'use strict';

var app = require('../..');
import request from 'supertest';

var newRest;

describe('Rest API:', function() {

  describe('GET /api/rests', function() {
    var rests;

    beforeEach(function(done) {
      request(app)
        .get('/api/rests')
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          rests = res.body;
          done();
        });
    });

    it('should respond with JSON array', function() {
      rests.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/rests', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/rests')
        .send({
          name: 'New Rest',
          info: 'This is the brand new rest!!!'
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newRest = res.body;
          done();
        });
    });

    it('should respond with the newly created rest', function() {
      newRest.name.should.equal('New Rest');
      newRest.info.should.equal('This is the brand new rest!!!');
    });

  });

  describe('GET /api/rests/:id', function() {
    var rest;

    beforeEach(function(done) {
      request(app)
        .get('/api/rests/' + newRest._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          rest = res.body;
          done();
        });
    });

    afterEach(function() {
      rest = {};
    });

    it('should respond with the requested rest', function() {
      rest.name.should.equal('New Rest');
      rest.info.should.equal('This is the brand new rest!!!');
    });

  });

  describe('PUT /api/rests/:id', function() {
    var updatedRest;

    beforeEach(function(done) {
      request(app)
        .put('/api/rests/' + newRest._id)
        .send({
          name: 'Updated Rest',
          info: 'This is the updated rest!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedRest = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedRest = {};
    });

    it('should respond with the updated rest', function() {
      updatedRest.name.should.equal('Updated Rest');
      updatedRest.info.should.equal('This is the updated rest!!!');
    });

  });

  describe('DELETE /api/rests/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/rests/' + newRest._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

    it('should respond with 404 when rest does not exist', function(done) {
      request(app)
        .delete('/api/rests/' + newRest._id)
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
