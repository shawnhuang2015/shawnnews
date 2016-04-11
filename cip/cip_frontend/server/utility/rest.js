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
exports.get = function(endpoint, query, callback) {
  var result = { error: true, message: ''};

  return request({
    url: createURL(endpoint, query),
    method: 'GET',
    timeout: 60000,
  })
    .then(res => {
      // console.log('Resonponse from server:\n', res);
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

/**
 * POST API for making POST request to the engine.
 * @param  {string}   endpoint [the end point on the engine]
 * @param  {string}   query    [the query string]
 * @param  {string}   data     [the data to past over to the engine]
 *
 * Note: comment out console.log for production.
 */
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
      // console.log('Resonponse from server:\n', res);
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
