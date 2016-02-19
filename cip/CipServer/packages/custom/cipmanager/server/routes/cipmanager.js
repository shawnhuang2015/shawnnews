'use strict';

var crowd = require('../controllers/crowd.server.controller');
var rest = require('../utility/restful');

/* jshint -W098 */
// The Package is past automatically as first parameter
module.exports = function(Cipmanager, app, auth, database) {

  app.route("/api/ontology").get(rest.readMetadata);

  app.route("/api/rest/crowd/detail").get(rest.getCrowdDetailByGet).post(rest.getCrowdDetailByPost);

  app.route("/api/rest/crowd/count").get(rest.getCrowdCountByGet).post(rest.getCrowdCountByPost);

  app.route("/api/rest/crowd/delete").get(rest.deleteCrowd);

  app.route("/api/rest/crowd/create").post(rest.createCrowd);

  app.route("/api/rest/crowd/sample").get(rest.crowdSampleByGet).post(rest.crowdSampleByPost);

  app.route("/api/db/crowd").get(crowd.list).post(crowd.create);

  app.route("/api/db/crowd/:crowdName").get(crowd.read).put(crowd.update).delete(crowd.delete);

  app.param('crowdName', crowd.crowdByName);

  app.get('/api/cipmanager/example/anyone', function(req, res, next) {
    res.send('Anyone can access this');
  });

  app.get('/api/cipmanager/example/auth', auth.requiresLogin, function(req, res, next) {
    res.send('Only authenticated users can access this');
  });

  app.get('/api/cipmanager/example/admin', auth.requiresAdmin, function(req, res, next) {
    res.send('Only users with Admin role can access this');
  });

  app.get('/api/cipmanager/example/render', function(req, res, next) {
    Cipmanager.render('index', {
      package: 'cipmanager'
    }, function(err, html) {
      //Rendering a view from the Package server/views
      res.send(html);
    });
  });
};
