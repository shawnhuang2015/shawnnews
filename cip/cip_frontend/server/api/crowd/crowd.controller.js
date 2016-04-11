/**
 * Using Rails-like standard naming convention for endpoints.
 * GET     /api/crowds              ->  index
 * GET     /api/crowds/count        ->  count
 * POST    /api/crowds              ->  create
 * GET     /api/crowds/:id          ->  show
 * PUT     /api/crowds/:id          ->  update
 * PATCH   /api/crowds/:id          ->  update
 * DELETE  /api/crowds/:id          ->  destroy
 * GET     /api/crowds/:id/sample   ->  sample
 * POST    /api/crowds/use_list     ->  userList
 * POST    /api/crowds/user_count   ->  userCount
 */

'use strict';

import _ from 'lodash';
import SingleCrowd from './crowd.model';
import Ontology from '../ontology/ontology.model';
import rest from '../../utility/rest';
import others from '../../utility/others';
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
 * @param  {SingleCrowd} updates [the new data]
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
 * Remove the specified object from the database.
 * @param  res [response object]
 */
function removeEntity(res) {
  return function(entity) {
    if (entity) {
      return entity.remove()
        .then(() => {
          res.status(204).end();
        });
    } else {
      return null;
    }
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
 * Get a list of SingleCrowds.
 * @param  req [request object]
 * @param  res [response object]
 */
export function index(req, res) {
  // Sort the single crowds by date created
  // and return the crowds within the required range.
  return SingleCrowd.find().sort({ created: -1 })
                           .limit((req.query.page_id + 1) * req.query.page_size)
                           .skip(req.query.page_id * req.query.page_size).exec()
    .then(respondWithResult(res))
    .catch(handleError(res));
}

/**
 * Get the number of SingleCrowd objects.
 * @param  req [request object]
 * @param  res [response object]
 */
export function count(req, res) {
  return SingleCrowd.count()
    .then(respondWithResult(res))
    .catch(handleError(res));
}

/**
 * Get a single SingleCrowd from the database.
 * @param  req [request object]
 * @param  res [response object]
 */
export function show(req, res) {
  return SingleCrowd.findById(req.params.id).exec()
    .then(handleEntityNotFound(res))
    .then(respondWithResult(res))
    .catch(handleError(res));
}

function createAtRemoteServer() {
  return function(crowd) {
    if (crowd) {
      if (crowd.type === 'dynamic') {
        crowd.tagAdded = 1;
        return crowd;
      }
      else if (crowd.type === 'static') {
        return Ontology.findOne({name: 'MetaData'}).exec()
          .then(onto => {
            return JSON.stringify({
              crowdIndex: onto.profile.crowdIndex,
              searchCond: {
                target: onto.profile.target,
                selector: others.createCondition(crowd.selector, onto.profile)
              }
            });
          })
          .then(body => {
            var crowdName = config.CrowdPrefix.single + crowd.crowdName;
            return rest.post('crowd/v1/create', 'name=' + crowdName + '&limit=' + config.userLimit, body);
          })
          .then(response => {
            if (response.error === true) {
              crowd.tagAdded = -1;
            }
            else {
              crowd.tagAdded = 1;
              crowd.file = crowd.crowdName + config.CrowdFileSuffix.single;
              others.writeToFile(response.message, config.dataPath, crowd.file);
            }
            return crowd;
          })
          .catch(() => {
            crowd.tagAdded = -1;
            return crowd;
          })
      }
    }
  }
}

/**
 * Create a new SingleCrowd in the database.
 * @param  req [request object]
 * @param  res [response object]
 */
export function create(req, res) {
  return SingleCrowd.create(req.body)
    .then(createAtRemoteServer())
    .then(crowd => {
      return SingleCrowd.findOne({ crowdName: crowd.crowdName }).exec()
        .then(handleEntityNotFound(res))
        .then(saveUpdates(crowd))
        .then(respondWithResult(res, 201))
        .catch(handleError(res));
    })
    .catch(handleError(res));
}

/**
 * Update an existing SingleCrowd in the database.
 * @param  req [request object]
 * @param  res [response object]
 */
export function update(req, res) {
  if (req.body._id) {
    delete req.body._id;
  }
  return SingleCrowd.findById(req.params.id).exec()
    .then(handleEntityNotFound(res))
    .then(saveUpdates(req.body))
    .then(respondWithResult(res))
    .catch(handleError(res));
}

function destroyAtRemoteServer() {
  return function(crowd){
    if (crowd) {
      if (crowd.type === 'dynamic') {
        return crowd;
      } else if (crowd.type === 'static') {
        return Ontology.findOne({ name: 'MetaData' }).exec()
          .then(onto => {
            var crowdName = config.CrowdPrefix.single + crowd.crowdName;
            return rest.get('crowd/v1/delete', 'name=' + crowdName + '&type=' + onto.profile.crowdIndex.vtype);
          })
          .then(response => {
            if (response.error === true) {
              console.log('Error in deleting crowd at remote server\n', response.message);
            } else {
              // console.log('Succesfully deleted crowd', crowd.crowdName);
            }
            return crowd;
          })
          .catch(err => {
            console.log('Error in deleting crowd\n', err);
          })
      }
    }
  }
}

/**
 * Delete a SingleCrowd from the database.
 * @param  req [request object]
 * @param  res [response object]
 */
export function destroy(req, res) {
  return SingleCrowd.findById(req.params.id).exec()
    .then(handleEntityNotFound(res))
    .then(destroyAtRemoteServer())
    .then(removeEntity(res))
    .catch(handleError(res));
}

/**
 * Return a sample of users from the engine.
 * @param  req [request object]
 * @param  res [response object]
 *
 * Note: comment out console.log for production. 
 */
export function sample(req, res) {
  return SingleCrowd.findById(req.params.id).exec()
    .then(crowd => {
      return Ontology.findOne({ name: 'MetaData' }).exec()
        .then(onto => {
          var crowdName = config.CrowdPrefix.single + crowd.crowdName;
          return rest.get('crowd/v1/get', 'name=' + crowdName + '&type=' + 
            onto.profile.crowdIndex.vtype + '&limit=' + req.query.count);
        })
        .then(response => {
          if (response.error === true) {
            res.status(500).send(response.message);
            return null;
          } else {
            return response.message;
          }
        })
        .catch(handleError(res));
    })
    .then(respondWithResult(res))
    .catch(handleError(res));
}

/**
 * Return the number of user from the crowd.
 * @param  req [request object]
 * @param  res [response object]
 *
 * Note: comment out console.log for production.
 */
export function userCount(req, res) {
  return Ontology.findOne({ name: 'MetaData' })
    .then(onto => {
      return JSON.stringify({
        target: onto.profile.target,
        selector: others.createCondition(req.body.selector, onto.profile)
      });
    })
    .then(body => {
      return rest.post('crowd/v1/user_search', 'limit=0&rid=' + req.query.rid, body)
    })
    .then(response => {
      if (response.error === true) {
        res.status(500).send(response.message);
        return null;
      } else {
        return response.message;
      }
    })
    .then(respondWithResult(res))
    .catch(handleError(res));
}
