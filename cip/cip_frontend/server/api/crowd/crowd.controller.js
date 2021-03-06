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
 * POST    /api/crowds/user_count   ->  userCount
 */

'use strict';

import _ from 'lodash';
import SingleCrowd from './crowd.model';
import rest from '../../utility/rest';
import config from '../../config/environment';

/**
 * Respond to the client with the result retrieve from the database.
 * @param  res        [response object]
 * @param  statusCode [status of the response]
 */
function respondWithResult(res, statusCode) {
  statusCode = statusCode || config.statusCode.SUCCESS;
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
          res.status(config.statusCode.DELETED).end();
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
      res.status(config.statusCode.NOTFOUND).end();
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
  statusCode = statusCode || config.statusCode.ERROR;
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

/**
 * Create a new SingleCrowd in the database.
 * @param  req [request object]
 * @param  res [response object]
 */
export function create(req, res) {
  return SingleCrowd.create(req.body)
    .then(rest.createAtRemoteServer('single'))
    .then(updates => {
      // Save the new information.
      return SingleCrowd.findOne({ crowdName: updates.crowdName }).exec()
        .then(handleEntityNotFound(res))
        .then(saveUpdates(updates))
        .then(respondWithResult(res, config.statusCode.CREATED))
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

/**
 * Delete a SingleCrowd from the database.
 * @param  req [request object]
 * @param  res [response object]
 */
export function destroy(req, res) {
  return SingleCrowd.findById(req.params.id).exec()
    .then(handleEntityNotFound(res))
    .then(rest.destroyAtRemoteServer('single'))
    .then(removeEntity(res))
    .catch(handleError(res));
}

/**
 * Return a sample of users from the engine.
 * @param  req [request object]
 * @param  res [response object]
 */
export function sample(req, res) {
  return SingleCrowd.findById(req.params.id).exec()
    .then(rest.sample('single', req, res))
    .then(respondWithResult(res))
    .catch(handleError(res));
}

/**
 * Return the number of user from the crowd.
 * @param  req [request object]
 * @param  res [response object]
 */
export function userCount(req, res) {
  return rest.userCount('single', req, res)
    .then(respondWithResult(res))
    .catch(handleError(res));
}
