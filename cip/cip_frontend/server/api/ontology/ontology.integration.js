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

    it('should respond with JSON object', function() {
      ontologys.should.be.instanceOf(Object);
    });

  });

});
