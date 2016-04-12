'use strict';

var app = require('../..');
import request from 'supertest';

var newGroupCrowd;

describe('Group Crowd API:', function() {

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

  // describe('POST /api/groups', function() {
  //   beforeEach(function(done) {
  //     request(app)
  //       .post('/api/groups')
  //       .send({
  //         name: 'New Group',
  //         info: 'This is the brand new group!!!'
  //       })
  //       .expect(201)
  //       .expect('Content-Type', /json/)
  //       .end((err, res) => {
  //         if (err) {
  //           return done(err);
  //         }
  //         newGroupCrowd = res.body;
  //         done();
  //       });
  //   });

  //   it('should respond with the newly created group', function() {
  //     newGroupCrowd.name.should.equal('New Group');
  //     newGroupCrowd.info.should.equal('This is the brand new group!!!');
  //   });

  // });

  // describe('GET /api/groups/:id', function() {
  //   var groupCrowd;

  //   beforeEach(function(done) {
  //     request(app)
  //       .get('/api/groups/' + newGroupCrowd._id)
  //       .expect(200)
  //       .expect('Content-Type', /json/)
  //       .end((err, res) => {
  //         if (err) {
  //           return done(err);
  //         }
  //         groupCrowd = res.body;
  //         done();
  //       });
  //   });

  //   afterEach(function() {
  //     groupCrowd = {};
  //   });

  //   it('should respond with the requested group', function() {
  //     groupCrowd.name.should.equal('New Group');
  //     groupCrowd.info.should.equal('This is the brand new group!!!');
  //   });

  // });

  // describe('PUT /api/groups/:id', function() {
  //   var updatedGroupCrowd;

  //   beforeEach(function(done) {
  //     request(app)
  //       .put('/api/groups/' + newGroupCrowd._id)
  //       .send({
  //         name: 'Updated Group',
  //         info: 'This is the updated group!!!'
  //       })
  //       .expect(200)
  //       .expect('Content-Type', /json/)
  //       .end(function(err, res) {
  //         if (err) {
  //           return done(err);
  //         }
  //         updatedGroupCrowd = res.body;
  //         done();
  //       });
  //   });

  //   afterEach(function() {
  //     updatedGroupCrowd = {};
  //   });

  //   it('should respond with the updated group', function() {
  //     updatedGroupCrowd.name.should.equal('Updated Group');
  //     updatedGroupCrowd.info.should.equal('This is the updated group!!!');
  //   });

  // });

  // describe('DELETE /api/groups/:id', function() {

  //   it('should respond with 204 on successful removal', function(done) {
  //     request(app)
  //       .delete('/api/groups/' + newGroupCrowd._id)
  //       .expect(204)
  //       .end((err, res) => {
  //         if (err) {
  //           return done(err);
  //         }
  //         done();
  //       });
  //   });

  //   it('should respond with 404 when group does not exist', function(done) {
  //     request(app)
  //       .delete('/api/groups/' + newGroupCrowd._id)
  //       .expect(404)
  //       .end((err, res) => {
  //         if (err) {
  //           return done(err);
  //         }
  //         done();
  //       });
  //   });

  // });

});
