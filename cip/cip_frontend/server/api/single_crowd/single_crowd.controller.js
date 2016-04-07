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
import SingleCrowd from './single_crowd.model';
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
 * Create the crowd on the engine if the crowd type is static.
 * If the crowd type is dynamic then just update the tag.
 *
 * Note: comment out console.log for production. 
 */
function createAtRemoteServer() {
  // Create a default updates.
  var updates = { tagAdded: 0, file: '' };

  return function(entity) {
    if (entity) {
      if (entity.type === 'static') { // if the crowd type is static.
        Ontology.findOne({ name: 'MetaData' }, function(err, data) {
          if (err || !data) {
            // If the meta data can not be found,
            // set crowd's tag to -1 (failed).
            updates.tagAdded = -1;
          } else {
            // Create request body for the engine.
            var body = JSON.stringify({
              crowdIndex: data.profile.crowdIndex,
              searchCond: {
                target: data.profile.target,
                selector: others.generateCond(entity.selector, data.profile)
              }
            });
            
            //Use the utility 'rest.post' function to make request to the engine.
            rest.post('crowd/v1/create', 'name=' + config.CrowdPrefix.single + 
              entity.crowdName + '&limit=' + config.userLimit, body, function(err, response) {
              var res = JSON.parse(response);
              //console.log('Response from engine:\n', res);
              if (!res || res.error === true) {
                // If fail to create crowd on the engine
                // set crowd's tag to -1 (failed).
                updates.tagAdded = -1;
              } else {
                entity.save();
                // If succesfully create crowd on the engine,
                // set crowd's tag to 1 (success),
                updates.tagAdded = 1;
                // set crowd's file name and
                updates.file = entity.crowdName + config.CrowdFileSuffix.single;
                // write the response from engine to file.
                others.writeToFile(response, config.dataPath, entity.crowdName + 
                  config.CrowdFileSuffix.single);
              }
              // Save the new crowd's tag and crowd's file name.
              return entity.save()
                .then(saveUpdates(updates));
            });
          }
        });
      } else { // if the crowd type is dynamic.
        updates.tagAdded = 1; // update crowd's tag to 1 (success).
      }
    } 
    // Save the new crowd's tag and crowd's file name.
    return entity.save()
      .then(saveUpdates(updates));
  }
}

/**
 * Delete the crowd on the engine.
 *
 * Note: comment out console.log for production. 
 */
function destroyAtRemoteServer() {
  return function(entity) {
    if (entity && entity.type === 'static') {
      Ontology.findOne({ name: 'MetaData' }, function(err, data) {
        if (err || !data) {
          // If cannot find the meta data, do nothing.
          //console.log('Error in retrieving meta data');
        } else {
          // Otherwise, use the utility 'rest.get' function to make request to the engine.
          rest.get('crowd/v1/delete', 'name=' + config.CrowdPrefix.single + entity.crowdName + 
            '&type=' + data.profile.crowdIndex.vtype, function (err, response) {
              var res = JSON.parse(response);
              //console.log('Response from engine:\n', res);
              if (!res || res.error === true) {
                console.log('Error in deleting crowd at remote server');
              } else {
                console.log('Succesfully deleted crowd', entity.crowdName);
              }
          });
        }
      });
    } 
    return entity;
  }
}

/**
 * Get a list of SingleCrowds.
 * @param  req [request object]
 * @param  res [response object]
 */
export function index(req, res) {
  // Retrieve the query parameters.
  var pageId = Number(req.query.page_id);
  var pageSz = Number(req.query.page_size);
  //console.log('Page ID:', pageId, ', Page size:', pageSz);

  // Sort the single crowds by date created
  // and return the crowds within the required range.
  return SingleCrowd.find().sort({ created: -1 })
                           .limit((pageId + 1) * pageSz)
                           .skip(pageId * pageSz).exec()
    .then(respondWithResult(res))
    .catch(handleError(res));
}

/**
 * Get the number of SingleCrowd objects.
 * @param  req [request object]
 * @param  res [response object]
 */
export function count(req, res) {
  return SingleCrowd.count(function(err, data) {
    if (err) {
      return res.status(500).send(err);
    } else {
      return res.status(200).json({ count: data });
    }
  });
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
    .then(createAtRemoteServer())
    .then(respondWithResult(res, 201))
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
  var count = req.query.count || 10;
  //console.log('Count:', count);
  
  SingleCrowd.findById(req.params.id).exec(function(err, data) {
    if (err) { // if there is an error in retrieving the crowd.
      return res.status(500).send(err);
    }
    if (!data) { // if cannot find the crowd.
      return res.status(404).send(err);
    }
    // Create the crowd name with prefix to query on the engine.
    var crowdName = config.CrowdPrefix.single + data.crowdName;
    Ontology.findOne({ name: 'MetaData' }, function(err, onto) {
      if (err || !onto) {
        // If there is an error in retrieving the meta data,
        // or the meta data is empty, send the error.
        return res.status(500).send(err);
      } else {
        // Otherwise, use the utility 'rest.get' function to make request to the engine.
        rest.get('crowd/v1/get', 'name=' + crowdName + '&type=' + 
          onto.profile.crowdIndex.vtype + '&limit=' + count, function(err, response) {
          var responseJSON = JSON.parse(response);
          //console.log('Response from engine:\n', responseJSON);
          if (!responseJSON || responseJSON.error === true) {
            // If fail to get the data from the server,
            // return the error.
            if (!responseJSON) {
              return res.status(500).send(err);
            } else {
              return res.status(500).send(responseJSON.message);  
            }
          } else {
            // If succesfully get the data from the server,
            // return the response from engine.
            return res.status(200).send(responseJSON.results);
          }
        });
      }
    });
  });
}

/**
 * THIS IS NOT USED!
 * 
 * Rerturn the full list of users from the engine.
 * @param  req [request object]
 * @param  res [response object]
 *
 * Note: comment out console.log for production. 
 */
// export function userList(req, res) {
//   Ontology.findOne({ name: 'MetaData' }, function(err, data) {
//     if (err || !data) {
//       // If there is an error in retrieving the meta data,
//       // or the meta data is empty, send the error.
//       return res.status(500).send(err);
//     } else {
//       // Create request body for the engine.
//       var body = JSON.stringify({
//         target: data.profile.target,
//         selector: others.generateCond(req.body.selector, data.profile)
//       });

//       // Use the utility 'rest.post' function to make request to the engine.
//       rest.post('crowd/v1/user_search', '', body, function (err, response) {
//         var responseJSON = JSON.parse(response);
//         console.log('Response from engine:\n', responseJSON);
//         if (!responseJSON || responseJSON.error === true) {
//           // If fail to get the data from the server,
//           // return the error.
//           if (!responseJSON) {
//             return res.status(500).send(err);
//           } else {
//             return res.status(500).send(responseJSON.message);  
//           }
//         } else {
//           // If succesfully get the data from the server,
//           // return the response from engine.
//           return res.status(200).send(responseJSON.results);
//         }
//       });
//     }
//   });
// }

/**
 * Return the number of user from the crowd.
 * @param  req [request object]
 * @param  res [response object]
 *
 * Note: comment out console.log for production.
 */
export function userCount(req, res) {
  Ontology.findOne({ name: 'MetaData' }, function(err, data) {
    if (err || !data) {
      // If there is an error in retrieving the meta data,
      // or the meta data is empty, send the error.
      return res.status(500).send(err);
    } else {
      // Create request body for the engine.
      var body = JSON.stringify({
        target: data.profile.target,
        selector: others.generateCond(req.body.selector, data.profile)
      });

      // Use the utility 'rest.post' function to make request to the engine.
      rest.post('crowd/v1/user_search', 'limit=0&rid=' + req.query.rid, body, function(err, response) {
        var responseJSON = JSON.parse(response);
        console.log('Response from engine:\n', responseJSON);
        if (!responseJSON || responseJSON.error === true) {
          // If fail to get the data from the server,
          // return the error.
          if (!responseJSON) {
            return res.status(500).send(err);
          } else {
            return res.status(500).send(responseJSON.message); 
          }
        } else {
          // If succesfully get the data from the server,
          // return the response from engine.
          return res.status(200).send(responseJSON.results);
        }
      });
    }
  });
}
