'use strict';

var app = require('../..');
import request from 'supertest';

var newOntology;

describe('Ontology API:', function() {

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

    it('should respond with JSON array', function() {
      ontologys.should.be.instanceOf(Array);
    });

  });

  describe('POST /api/ontologys', function() {
    beforeEach(function(done) {
      request(app)
        .post('/api/ontologys')
        .send({
          name: 'New Ontology',
          info: 'This is the brand new ontology!!!'
        })
        .expect(201)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          newOntology = res.body;
          done();
        });
    });

    it('should respond with the newly created ontology', function() {
      newOntology.name.should.equal('New Ontology');
      newOntology.info.should.equal('This is the brand new ontology!!!');
    });

  });

  describe('GET /api/ontologys/:id', function() {
    var ontology;

    beforeEach(function(done) {
      request(app)
        .get('/api/ontologys/' + newOntology._id)
        .expect(200)
        .expect('Content-Type', /json/)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          ontology = res.body;
          done();
        });
    });

    afterEach(function() {
      ontology = {};
    });

    it('should respond with the requested ontology', function() {
      ontology.name.should.equal('New Ontology');
      ontology.info.should.equal('This is the brand new ontology!!!');
    });

  });

  describe('PUT /api/ontologys/:id', function() {
    var updatedOntology;

    beforeEach(function(done) {
      request(app)
        .put('/api/ontologys/' + newOntology._id)
        .send({
          name: 'Updated Ontology',
          info: 'This is the updated ontology!!!'
        })
        .expect(200)
        .expect('Content-Type', /json/)
        .end(function(err, res) {
          if (err) {
            return done(err);
          }
          updatedOntology = res.body;
          done();
        });
    });

    afterEach(function() {
      updatedOntology = {};
    });

    it('should respond with the updated ontology', function() {
      updatedOntology.name.should.equal('Updated Ontology');
      updatedOntology.info.should.equal('This is the updated ontology!!!');
    });

  });

  describe('DELETE /api/ontologys/:id', function() {

    it('should respond with 204 on successful removal', function(done) {
      request(app)
        .delete('/api/ontologys/' + newOntology._id)
        .expect(204)
        .end((err, res) => {
          if (err) {
            return done(err);
          }
          done();
        });
    });

    it('should respond with 404 when ontology does not exist', function(done) {
      request(app)
        .delete('/api/ontologys/' + newOntology._id)
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
