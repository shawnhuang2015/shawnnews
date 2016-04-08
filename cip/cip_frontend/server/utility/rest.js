/**
 * This contains the REST functions to contact with the engine.
 * Includes: GET request.
 *           POST request.
 */

'use strict'

import Ontology from '../api/ontology/ontology.model';
import request from 'request-promise';
import others from './others';
import config from '../config/environment';

/**
 * Convert string to byte.
 * @param  {string} string [the string to convert to byte]
 * @return {byte}          [the processed string in byte]
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
 * @param  {Function} callback [calll back function (error, result)]
 *
 * Note: comment out console.log for production.
 */
exports.get = function(endpoint, query, callback) {
  request({
    url: createURL(endpoint, query),
		method: 'GET',
    timeout: 60000
	}, (err, res, body) => {
    if (err) {
      //console.log('Error:', err);
      callback(err, null);
    } else {
      //console.log('Response body:\n', body);
      var cbErr = null;
      var cbRes = null;
      try {
        // Try to parse the message to check if valid
        JSON.parse(body);
        cbRes = body;
      } catch (e) {
        cbErr = e;
      } finally {
        callback(cbErr, cbRes);
      }
    }
	});
}

/**
 * POST API for making POST request to the engine.
 * @param  {string}   endpoint [the end point on the engine]
 * @param  {string}   query    [the query string]
 * @param  {json}     data     [the data to past over to the engine]
 * @param  {Function} callback [call back function (error, result)]
 *
 * Note: comment out console.log for production.
 */
// exports.post = function(endpoint, query, data, callback) {
//   request({
//   	url: createURL(endpoint, query),
//     method: 'POST',
//     timeout: 60000,
// 		headers: { 
//       'Content-Type': 'application/json',
//       'Content-Length': getBytes(data)
//     },
//     body: data
//   }, (err, res, body) => {
//   	if (err) {
//       //console.log('Error:', err);
//       callback(err, null);
//     } else {
//       //console.log('Response body:\n', body);
//       var cbErr = null;
//       var cbRes = null;
//       try {
//         // Try to parse the message to check if valid
//         JSON.parse(body);
//         cbRes = body;
//       } catch (e) {
//         cbErr = e;
//       } finally {
//         callback(cbErr, cbRes);
//       }
//     }
//   });
// }

exports.post = function(endpoint, query, data) {
  var result = { error: true, message: ''};

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
      res = JSON.parse(res);
      if (res.error === true) {
        result.message = res.message;
      } else {
        result.error = false;
        result.message = res.results;
      }
      return result;
    })
    .catch(err => {
      result.message = err;
      return result;
    })
}

exports.handleResponse = function(crowd) {
  return function(response) {
    if (response) {
      if (crowd.type === 'static' && response.error === true) {
        crowd.tagAdded = -1;
      } else if (crowd.type === 'static' || crowd.type === 'dynamic') {
        crowd.tagAdded = 1;
      }
    }
    return crowd;
  }
}

/**
 * Create the crowd on the engine if the crowd type is static.
 * If the crowd type is dynamic then just update the tag.
 *
 * Note: comment out console.log for production. 
 */
// exports.createAtRemoteServer = function(crowd, type) {
//   return function (onto) {
//     if (onto) {
//       var prefix, suffix;
//       if (type === 'single') {
//         prefix = config.CrowdPrefix.single;
//         suffix = config.CrowdFileSuffix.single;
//       } else {
//         prefix = config.CrowdPrefix.multi;
//         suffix = config.CrowdFileSuffix.multi;
//       }
//       if (crowd.type === 'static') { // if the crowd type is static.
//         // Create request body for the engine.
//         var body = JSON.stringify({
//           crowdIndex: onto.profile.crowdIndex,
//           searchCond: {
//             target: onto.profile.target,
//             selector: others.generateCond(crowd.selector, onto.profile)
//           }
//         });
        
//         //Use the utility 'post' function to make request to the engine.
//         post('crowd/v1/create', 'name=' + prefix + crowd.crowdName + 
//           '&limit=' + config.userLimit, body, function(err, response) {
//           var res = JSON.parse(response);
//           //console.log('Response from engine:\n', res);
//           if (!res || res.error === true) {
//             // If fail to create crowd on the engine
//             // set crowd's tag to -1 (failed).
//             crowd.tagAdded = -1;
//           } else {
//             // If succesfully create crowd on the engine,
//             // set crowd's tag to 1 (success),
//             crowd.tagAdded = 1;
//             // set crowd's file name and
//             crowd.file = crowd.crowdName + suffix;
//             // write the response from engine to file.
//             others.writeToFile(response, config.dataPath, crowd.file);
//           }
//           return crowd;
//         });
//           }
//       } else if (crowd.type === 'dynamic') { // if the crowd type is dynamic.
//         crowd.tagAdded = 1; // update crowd's tag to 1 (success).
//         return crowd;
//       }
//     } else {
//       crowd.tagAdded = -1;
//     }
//   }
// }