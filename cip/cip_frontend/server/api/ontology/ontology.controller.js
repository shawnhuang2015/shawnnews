/**
 * Using Rails-like standard naming convention for endpoints.
 * GET     /api/meta_datas              ->  index
 */

'use strict';

import _ from 'lodash';
import rest from '../../utility/rest';
import Ontology from './ontology.model';
import config from '../../config/environment';

function respondWithResult(res, statusCode) {
  statusCode = statusCode || 200;
  return function(entity) {
    if (entity) {
      res.status(statusCode).json(entity);
    }
  };
}

function saveUpdates(updates) {
  return function(entity) {
    var updated = _.merge(entity, updates);
    return updated.save()
      .then(updated => {
        return updated;
      });
  };
}

function handleEntityNotFound(res) {
  return function(entity) {
    if (!entity) {
      res.status(404).end();
      return null;
    }
    return entity;
  };
}

function handleError(res, statusCode) {
  statusCode = statusCode || 500;
  return function(err) {
    res.status(statusCode).send(err);
  };
}

// Gets a Ontology
export function index(req, res) {
  Ontology.findOne({ name: 'MetaData' }, function(err, data) {
    if (err) {
      // If error happens
      return res.status(500).send(err);
    }
    if (data === null) {
      // If not found then call the engine to fetch a new meta data
      //console.log('Create');
      return create(req, res);
    } else {
      var curTime = new Date().getTime(); // get the current time
      if (curTime - data.created.getTime() >= config.semanticSyncTime * 1000) {
        // If the meta data expires we neet to update it
        //console.log('Update'); 
        return update(req, res);
      }
      else {
        // Otherwise simply return the meta data
        //console.log('Return');
        return res.status(200).json(data);
      }
    }
  })
}

// Creates a new Ontology in the DB
function create(req, res) {
  // Use the utility 'rest.get' function to make request to the engine
  rest.get('get_profile', 'threshold=' + config.ontoLimit, function(err, response) {
    var data = JSON.parse(response);
    //console.log('Data fetch from server:', data);
    if (data === null || data.error === true) {
      // If fail to get the data from the server,
      if (data === null) {
        return res.status(500).send(err);
      } else {
        return res.status(500).send(data.message);
      }
    } else {
      // If succesfully get the data from the server,
      var newOntology = new Ontology({
        name: 'MetaData',
        profile: data.results
      })

      return Ontology.create(newOntology)
        .then(respondWithResult(res, 201))
        .catch(handleError(res)); 
    }
  })
}

// Updates an existing Ontology in the DB
function update(req, res) {
  // Use the utility 'rest.get' function to make request to the engine
  rest.get('get_profile', 'threshold=' + config.ontoLimit, function(err, response) {
    var data = JSON.parse(response);
    //console.log('Data fetch from server:', data);
    if (data === null || data.error === true) {
      // If fail to get the data from the server,
      if (data === null) {
        return res.status(500).send(err);
      } else {
        return res.status(500).send(data.message);
      }
    } else {
      // If succesfully get the data from the server,
      return Ontology.findOne({ name: 'MetaData' }).exec()
        .then(handleEntityNotFound(res))
        .then(saveUpdates(data.results))
        .then(respondWithResult(res))
        .catch(handleError(res));
    }
  })
}
