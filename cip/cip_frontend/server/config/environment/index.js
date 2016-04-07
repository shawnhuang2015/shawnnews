'use strict';

var path = require('path');
var rootPath = path.normalize(__dirname + '/../..');
var _ = require('lodash');

function requiredProcessEnv(name) {
  if (!process.env[name]) {
    throw new Error('You must set the ' + name + ' environment variable');
  }
  return process.env[name];
}

// All configurations will extend these options
// ============================================
var all = {
  env: process.env.NODE_ENV,

  // Root path of server
  root: path.normalize(__dirname + '/../../..'),

  // Server port
  port: process.env.PORT || 3001,

  // Server IP
  ip: process.env.IP || '0.0.0.0',

  // Should we populate the DB with sample data?
  seedDB: false,

  // Secret for session, you will want to change this and make it an environment variable
  secrets: {
    session: 'cip-secret'
  },

  // MongoDB connection options
  mongo: {
    options: {
      db: {
        safe: true
      }
    }
  },

  // Remote server connection options
  remoteServer: {
    ruleEngine: {
    },
    crowdServer: {
      host: '192.168.22.51',
      port: 9001
    }
  },

  // Crowd prefix configuration for parameter in querying
  CrowdPrefix: {
    single: "single_",
    multi: "multi_"
  },

  // Crowd prefix configuration for file name
  CrowdFileSuffix: {
    single: ".user.single",
    multi: '.user.multi'
  },

  // The persist time of cache data from engine
  semanticSyncTime: 1800,

  // The limit for the number of ontology to retrieve
  ontoLimit: 1024,
  
  // The limit for the number of user to restrieve
  userLimit: 3000000,

  // The data path to save the response from server
  dataPath: rootPath + "/data/",
};

// Export the config object based on the NODE_ENV
// ==============================================
module.exports = _.merge(
  all,
  require('./shared'),
  require('./' + process.env.NODE_ENV + '.js') || {});
