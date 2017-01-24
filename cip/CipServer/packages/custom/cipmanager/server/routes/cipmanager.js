'use strict';

var crowdsingle = require('../controllers/crowd.single.server.controller.js');
var crowdgroup = require('../controllers/crowd.group.server.controller.js');
var rest = require('../controllers/rest.server.controller');

/* jshint -W098 */
// The Package is past automatically as first parameter
module.exports = function(Cipmanager, app, auth, database) {

  //call remote
  app.route('/api/ontology').get(rest.readMetadata);

  app.route('/api/rest/crowd/detail').post(rest.getCrowdDetailByPost);

  app.route('/api/rest/crowd/count').post(rest.getCrowdCountByPost);

  app.route('/api/rest/group/crowd/detail').post(rest.getGroupCrowdDetailByPost);

  app.route('/api/rest/group/crowd/count').post(rest.getGroupCrowdCountByPost);

  app.route('/api/rest/crowd/sample').get(rest.crowdSampleByGet);

  //db single crowd
  app.route('/api/db/crowd').get(crowdsingle.list).post(crowdsingle.create);

  app.route('/api/db/crowdcount').get(crowdsingle.count);

  app.route('/api/db/crowd/:crowdName').get(crowdsingle.read).put(crowdsingle.update).delete(crowdsingle.delete);

  app.param('crowdName', crowdsingle.crowdByName);

  //db crowd group
  app.route('/api/db/group/crowd').get(crowdgroup.list).post(crowdgroup.create);

  app.route('/api/db/group/crowdcount').get(crowdgroup.count);

  app.route('/api/db/group/crowd/:crowdGroupName').get(crowdgroup.read).put(crowdgroup.update).delete(crowdgroup.delete);

  app.param('crowdGroupName', crowdgroup.crowdByName);

  //download
  app.route('/api/download').get(rest.download);

  app.all('/', function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "X-Requested-With");
    next()
  });
  /*
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
  */
};
