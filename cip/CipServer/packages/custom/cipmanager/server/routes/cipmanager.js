'use strict';

var crowd = require('../controllers/crowd.server.controller');

/* jshint -W098 */
// The Package is past automatically as first parameter
module.exports = function(Cipmanager, app, auth, database) {

  app.route("/api/crowd").get(crowd.list).post(crowd.create);

  app.route("/api/crowd/:crowd_name").get(crowd.read).put(crowd.update).delete(crowd.delete);

  app.param('crowd_name', crowd.crowdByName);

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
