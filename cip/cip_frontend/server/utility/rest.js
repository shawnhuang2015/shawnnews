/**
 * This contains the REST functions to contact with the engine.
 * Includes: GET request.
 *           POST request.
 */

'use strict'

import Ontology from '../api/ontology/ontology.model';
import SingleCrowd from '../api/crowd/crowd.model';
import request from 'request-promise';
import others from './others';
import config from '../config/environment';

/**
 * Convert string to byte.
 * @param  {string} string [the string to convert to byte]
 * @return {int}           [the byte length of the string]
 */
function getBytes(string) {
  return Buffer.byteLength(string, 'utf8');
}

/**
 * Create the URL from end point and query string.
 * @param  {string} endpoint [the end point on the engine]
 * @param  {string} query    [the query string]
 * @return {string}          [the complete URL]
 *
 * Note: comment out console.log for production.
 */

function createURL(endpoint, query) {
  var url = 'http://' + config.remoteServer.crowdServer.host + ':' + 
                        config.remoteServer.crowdServer.port + '/' + endpoint;
  if (query != '') {
    url += '?' + query;
  }
  // console.log('Request path:', url);

  return url;
}

/**
 * GET API for making GET request to the engine.
 * @param  {string}   endpoint [the end point on the engine]
 * @param  {string}   query    [the query string]
 *
 * Note: comment out console.log for production.
 */
function get(endpoint, query) {
  // Default response.
  var result = { error: true, message: ''};

  // Make request to the engine.
  return request({
    url: createURL(endpoint, query),
    method: 'GET',
    timeout: 60000,
  })
    .then(res => {
      // console.log('Resonponse from server:\n', res);
      res = JSON.parse(res);
      if (res.error === true) {
        // If there is error from the engine return the error message.
        result.message = res.message;
      } else {
        // Otherwise return the result.
        result.error = false;
        result.message = res.results;
      }
      return result;
    })
    .catch(err => {
      // If there is other error, return it.
      result.message = err;
      return result;
    })
}

/**
 * POST API for making POST request to the engine.
 * @param  {string}   endpoint [the end point on the engine]
 * @param  {string}   query    [the query string]
 * @param  {string}   data     [the data to past over to the engine]
 *
 * Note: comment out console.log for production.
 */
function post(endpoint, query, data) {
  // Default response.
  var result = { error: true, message: ''};
  
  // Make request to the engine.
  return request({
    url: createURL(endpoint, query),
    method: 'POST',
    timeout: 60000,
    headers: { 
      'Content-Type': 'application/json',
      'Content-Length': getBytes(data)
    },
    body: data,
  })
    .then(res => {
      // console.log('Resonponse from server:\n', res);
      res = JSON.parse(res);
      if (res.error === true) {
        // If there is error from the engine return the error message.
        result.message = res.message;
      } else {
        // Otherwise return the result.
        result.error = false;
        result.message = res.results;
      }
      return result;
    })
    .catch(err => {
      // If there is other error, return it.
      result.message = err;
      return result;
    })
}

/**
 * Get a new Ontology from the engine
 * @return {[type]} [description]
 */
exports.getOntology = function() {
  return get('get_profile', 'threshold=' + config.ontoLimit)
}

/**
 * Create the crowd on the engine.
 * @param  {string} format [the format of the crowd: single or group]
 * @return {JSON}          [the crowd object]
 */
exports.createAtRemoteServer = function(format) {
  return function(crowd) {
    if (crowd) {
      var updates = { 
        crowdName: crowd.crowdName,
        tagAdded: 0,
        file: ''
      }

      // Check the crowd format to determine prefix and suffix.
      var prefix, suffix;
      if (format === 'single') {
        prefix = config.crowdPrefix.single;
        suffix = config.crowdFileSuffix.single;
      } else {
        prefix = config.crowdPrefix.group;
        suffix = config.crowdFileSuffix.group;
      }

      if (crowd.type === 'dynamic') {
        // If the crowd type is dynamic then set the tag to 1
        // and return the crowd.
        updates.tagAdded = 1;
        return updates;
      }
      else if (crowd.type === 'static') {
        // If the crowd type is static, create it on the engine.
        return Ontology.findOne({name: 'MetaData'}).exec()
          .then(onto => {
            // Generate the request body.
            return JSON.stringify({
              crowdIndex: onto.profile.crowdIndex,
              searchCond: {
                target: onto.profile.target,
                selector: others.createCondition(crowd.selector, onto.profile)
              }
            });
          })
          .then(body => {
            console.log(body);
            // Make request to the engine.
            var crowdName = prefix + crowd.crowdName;
            return post('crowd/v1/create', 'name=' + crowdName + '&limit=' + config.userLimit, body);
          })
          .then(response => {
            console.log(response);
            if (response.error === true) {
              // If there is error from the engine,
              // set the tag to -1
              updates.tagAdded = -1;
            }
            else {
              // Otherwise set the tag to 1,
              updates.tagAdded = 1;
              // set the file name,
              updates.file = crowd.crowdName + suffix;
              // and write the response from the engine to file.
              others.writeToFile(response.message, config.dataPath, updates.file);
            }
            return updates;
          })
          .catch(() => {
            // If there is other error in the process,
            // set the tag to -1.
            updates.tagAdded = -1;
            return updates;
          })
      }
    }
  }
}

/**
 * Delete the crowd on the engine.
 * @param  {string} format [the format of the crowd: single or group]
 * @return {JSON}          [the crowd object]
 *
 * Note: comment out console.log for production.
 */
exports.destroyAtRemoteServer = function(format) {
  return function(crowd){
    if (crowd) {
      // Check the crowd format to determine prefix and suffix.
      var prefix;
      if (format === 'single') {
        prefix = config.crowdPrefix.single;
      } else {
        prefix = config.crowdPrefix.group;
      }

      if (crowd.type === 'dynamic') {
        // If the crowd type is dynamic then do nothing.
        return crowd;
      } else if (crowd.type === 'static') {
        // If the crowd type is static, delete it on the engine.
        return Ontology.findOne({ name: 'MetaData' }).exec()
          .then(onto => {
            // Make request to the engine.
            var crowdName = prefix + crowd.crowdName;
            return get('crowd/v1/delete', 'name=' + crowdName + '&type=' + onto.profile.crowdIndex.vtype);
          })
          .then(response => {
            if (response.error === true) {
              // If there is error from the engine.
              // console.log('Error in deleting crowd at remote server\n', response.message);
            } else {
              // Otherwise successfully deleted the crowd.
              // console.log('Succesfully deleted crowd', crowd.crowdName);
            }
            return crowd;
          })
          .catch(err => {
            // If there is other error in the process.
            // console.log('Error in deleting crowd\n', err);
          })
      }
    }
  }
}

/**
 * Get a sample of users from the engine
 * @param  {string} format [the format of the crowd: single or group]
 * @param  req             [request object]
 * @param  res             [response object]
 * @return {JSON}          [response from the engine]
 */
exports.sample = function(format, req, res) {
  return function(crowd) {
    if (crowd) {
      // Check the crowd format to determine prefix and suffix.
      var prefix;
      if (format === 'single') {
        prefix = config.crowdPrefix.single;
      } else {
        prefix = config.crowdPrefix.group;
      }

      return Ontology.findOne({ name: 'MetaData' }).exec()
        .then(onto => {
          // Make request to the engine.
          var crowdName = prefix + crowd.crowdName;
          return get('crowd/v1/get', 'name=' + crowdName + '&type=' + 
            onto.profile.crowdIndex.vtype + '&limit=' + req.query.count);
        })
        .then(response => {
          if (response.error === true) {
            // If there is error from the engine,
            // send the error to the client.
            res.status(config.statusCode.ERROR).send(response.message);
            return null;
          } else {
            // Otherwise, return the result.
            return response.message;
          }
        })
        .catch(err => {
          // If there is other error in the process.
          res.status(config.statusCode.ERROR).send(err);
          return null
        });
    }
  }
}

/**
 * Get the number of user form the crowd from the engine.
 * @param  {string} format [the format of the crowd: single or group]
 * @param  req             [request object]
 * @param  res             [response object]
 * @return {JSON}          [response from the engine]
 */
exports.userCount = function(format, req, res) {
  return Ontology.findOne({ name: 'MetaData' })
    .then(onto => {
      // Generate the request body.
      return JSON.stringify({
        target: onto.profile.target,
        selector: others.createCondition(req.body.selector, onto.profile)
      });
    })
    .then(body => {
      // Make request to the engine.
      return post('crowd/v1/user_search', 'limit=0&rid=' + req.query.rid, body)
    })
    .then(response => {
      if (response.error === true) {
        // If there is error from the engine,
        // send the error to the client.
        res.status(config.statusCode.ERROR).send(response.message);
        return null;
      } else {
         // Otherwise, return the result.
        return response.message;
      }
    })
}
