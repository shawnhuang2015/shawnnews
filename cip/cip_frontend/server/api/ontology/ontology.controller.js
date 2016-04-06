/**
 * Using Rails-like standard naming convention for endpoints.
 * GET     /api/meta_datas              ->  index
 */

'use strict';

import _ from 'lodash';
import Ontology from './ontology.model';
import rest from '../../utility/rest';
import config from '../../config/environment';

/**
 * Respond to the client with the result retrieve from the database.
 * @param  res        [response object]
 * @param  statusCode [status of the response]
 */
function respondWithResult(res, statusCode) {
  statusCode = statusCode || 200;
  return function(entity) {
    if (entity) {
      res.status(statusCode).json(entity);
    }
  };
}

/**
 * Perform update on the database object.
 * @param  {Ontology} updates [the new data]
 */
function saveUpdates(updates) {
  return function(entity) {
    var updated = _.merge(entity, updates);
    return updated.save()
      .then(updated => {
        return updated;
      });
  };
}

/**
 * Set status code if entity is not found.
 * @param  res [response object]
 */
function handleEntityNotFound(res) {
  return function(entity) {
    if (!entity) {
      res.status(404).end();
      return null;
    }
    return entity;
  };
}

/**
 * Handle error in processing any functionality on the database.
 * @param  res        [response object]
 * @param  statusCode [status of the response]
 */
function handleError(res, statusCode) {
  statusCode = statusCode || 500;
  return function(err) {
    res.status(statusCode).send(err);
  };
}

/**
 * Get a new Ontology for the client.
 * @param  req [request object]
 * @param  res [response object]
 *
 * Note: comment out console.log for production.
 */
export function index(req, res) {
  Ontology.findOne({ name: 'MetaData' }, function(err, data) {
    if (err) {
      // If error happens.
      return res.status(500).send(err);
    }
    if (!data) {
      // If not found then call the engine to fetch a new meta data.
      //console.log('Create new ontology');
      return create(req, res);
    } else {
      var curTime = new Date().getTime(); // get the current time
      if (curTime - data.created.getTime() >= config.semanticSyncTime * 1000) {
        // If the meta data expires we neet to update it.
        //console.log('Update ontology'); 
        return update(req, res);
      }
      else {
        // Otherwise simply return the meta data.
        //console.log('Return ontology');
        return res.status(200).json(data);
      }
    }
  })
}

/**
 * Get a new Ontology from the engine.
 * @param  req [request object]
 * @param  res [response object]
 * 
 * Note: comment out console.log for production. 
 */
function create(req, res) {
  // Use the utility 'rest.get' function to make request to the engine.
  rest.get('get_profile', 'threshold=' + config.ontoLimit, function(err, response) {
    var data = JSON.parse(response);
    //console.log('Data fetch from server:', data);
    if (!data || data.error === true) {
      // If fail to get the data from the server,
      // return the error.
      if (!data) {
        return res.status(500).send(err);
      } else {
        return res.status(500).send(data.message);
      }
    } else {
      // If succesfully get the data from the server,
      // create a new ontology.
      var newOntology = new Ontology({
        name: 'MetaData',
        profile: data.results
      });

      return Ontology.create(newOntology)
        .then(respondWithResult(res, 200))
        .catch(handleError(res)); 
    }
  })
}

/**
 * Update the Ontology with new data from the engine.
 * @param  req [request object]
 * @param  res [response object]
 * 
 * Note: comment out console.log for production.
 */
function update(req, res) {
  // Use the utility 'rest.get' function to make request to the engine.
  rest.get('get_profile', 'threshold=' + config.ontoLimit, function(err, response) {
    var data = JSON.parse(response);
    //console.log('Data fetch from server:', data);
    if (!data || data.error === true) {
      // If fail to get the data from the server,
      // return the error.
      if (!data) {
        return res.status(500).send(err);
      } else {
        return res.status(500).send(data.message);
      }
    } else {
      // If succesfully get the data from the server,
      // update the ontology.
      return Ontology.findOne({ name: 'MetaData' }).exec()
        .then(handleEntityNotFound(res))
        .then(saveUpdates(data.results))
        .then(respondWithResult(res))
        .catch(handleError(res));
    }
  })
}
