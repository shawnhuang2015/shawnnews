/**
 *
 * This contains the REST functions to contact with the engine.
 * Includes: GET request.
 *           POST request.
 * 
 */

'use strict'

import request from 'request';
import config from '../config/environment';

/**
 * Convert string to byte
 * @param  {string} string [the string to convert to byte]
 * @return {byte}          [the processed string in byte]
 */
function getBytes(string) {
  return Buffer.byteLength(string, 'utf8');
}

/**
 * Create the URL from end point and query string
 * @param  {string} endpoint [the end point on the engine]
 * @param  {string} query    [the query string]
 * @return {string}          [the complete URL]
 *
 * Note: comment out console.log for production.
 */
function createURL(endpoint, query) {
  var url = 'http://' + config.remoteServer.crowdServer.host + ':' 
                      + config.remoteServer.crowdServer.port + '/'
                      + endpoint;
  if (query != '') {
    url += '?' + query;
  }
  //console.log('GET request path:', url);

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
    // debugger;
		if (err) {
			//console.log('Error:', err);
			callback(err, null);
		} else {
  		//console.log('Response body:\n', body);
      try {
        JSON.parse(body);
        callback(null, body);
      }
      catch (e) {
        callback(body, null);
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
exports.post = function(endpoint, query, data, callback) {
  request({
  	url: createURL(endpoint, query),
    method: 'POST',
    timeout: 60000,
		headers: { 
      'Content-Type': 'application/json',
      'Content-Length': getBytes(data)
    }
  }, (err, res, body) => {
  	if (err) {
  		console.log('Error:', err);
  		callback(err, null);
  	} else {
  		console.log('Response body:\n', body);
  		callback(null, body);
  	}
  });
}
