/**
 * This contains other utility functions.
 */

'use strict'

import fs from 'fs';
import config from '../config/environment';

exports.createRequestBody = function(crowd) {
  return function(onto) {
    if (onto) {
      var body = JSON.stringify({
        crowdIndex: onto.profile.crowdIndex,
        searchCond: {
          target: onto.profile.target,
          selector: createCondition(crowd.selector, onto.profile)
        }
      });
      return body;
    }
  }
}

function createCondition(condition, metadata) {
  condition = JSON.parse(JSON.stringify(condition));
  // Map the vertex name to vertex type.
  var tagT = {};
  var k = 0;
  var name = '';
  var vtype = '';
  for (k = 0; k < metadata.tag.length; ++k) {
    name = metadata.tag[k].name;
    vtype = metadata.tag[k].vtype;
    tagT[name] = vtype;
  }
  var ontoT = {};
  for (k = 0; k < metadata.ontology.length; ++k) {
    name = metadata.ontology[k].name;
    vtype = metadata.ontology[k].vtype;
    ontoT[name] = vtype;
  }

  // Use condition in database to generate condition sent to crowding service.
  if (!(condition instanceof Array)) {
    condition = [condition];
  }

  //console.log('Tag name to type:\n', JSON.stringify(tagT));
  //console.log('Ontology name to type:\n', JSON.stringify(ontoT));

  var res = [];
  for (var index = 0; index < condition.length; ++index) {
    var cond = {
      ontology: [],
      behavior: []
    };
    var tag = condition[index].tag;
    var onto = condition[index].ontology;
    var behr = condition[index].behavior;
    var item;
    for (k = 0; k < tag.length; ++k) {
      item = tag[k];
      item.type = tagT[item.name];
      cond.ontology.push(item);
    }
    for (k = 0; k < onto.length; ++k) {
      item = onto[k];
      item.type = ontoT[item.name];
      cond.ontology.push(item);
    }
    for (k = 0; k < behr.length; ++k) {
      item = behr[k];
      item.ontologyType = ontoT[item.ontologyType];
      item.startTime /= 1000; // ms -> seconds
      item.endTime /= 1000; // ms -> seconds
      if (item.timeType === 'day' || item.timeType === 'hour') {
        item.timeType = 'relative';
      }
      cond.behavior.push(item);
    }
    res.push(cond);
  }

  return res;
}

/**
 * Generate the condition to create vertex on the engine.
 * @param  {JSON} condition [the input condition]
 * @param  {JSON} metadata  [the information on Ontology]
 * @return {JSON}           [the generated condition]
 *
 * Note: comment out console.log for production.
 */
exports.generateCond = function(condition, metadata) {
  condition = JSON.parse(JSON.stringify(condition));
  // Map the vertex name to vertex type.
  var tagT = {};
  var k = 0;
  var name = '';
  var vtype = '';
  for (k = 0; k < metadata.tag.length; ++k) {
  	name = metadata.tag[k].name;
  	vtype = metadata.tag[k].vtype;
  	tagT[name] = vtype;
  }
  var ontoT = {};
  for (k = 0; k < metadata.ontology.length; ++k) {
  	name = metadata.ontology[k].name;
    vtype = metadata.ontology[k].vtype;
    ontoT[name] = vtype;
  }

  // Use condition in database to generate condition sent to crowding service.
  if (!(condition instanceof Array)) {
  	condition = [condition];
  }

  //console.log('Tag name to type:\n', JSON.stringify(tagT));
  //console.log('Ontology name to type:\n', JSON.stringify(ontoT));

  var res = [];
  for (var index = 0; index < condition.length; ++index) {
  	var cond = {
      ontology: [],
      behavior: []
    };
    var tag = condition[index].tag;
    var onto = condition[index].ontology;
    var behr = condition[index].behavior;
    var item;
    for (k = 0; k < tag.length; ++k) {
      item = tag[k];
      item.type = tagT[item.name];
      cond.ontology.push(item);
    }
    for (k = 0; k < onto.length; ++k) {
      item = onto[k];
      item.type = ontoT[item.name];
      cond.ontology.push(item);
    }
    for (k = 0; k < behr.length; ++k) {
      item = behr[k];
      item.ontologyType = ontoT[item.ontologyType];
      item.startTime /= 1000; // ms -> seconds
      item.endTime /= 1000; // ms -> seconds
      if (item.timeType === 'day' || item.timeType === 'hour') {
        item.timeType = 'relative';
      }
      cond.behavior.push(item);
    }
    res.push(cond);
  }

  return res;
}

/**
 * Helper function to write response to file.
 * @param  {string} path     [the file path]
 * @param  {string} fileName [the file name]
 * @param  {JSON}   data     [the response from server]
 *
 * Note: comment out console.log for production.
 */
function write(path, fileName, data) {
  fs.writeFile(path + '/' + fileName.toLowerCase(), data, function(err) {
    if (err) {
      console.log('Error in writing to file:', path, '\n', err);
    } else {
      //console.log('Success in writing to file:', path);
    }
  });
}

/**
 * Write the response from engine to file.
 * @param  {string} res      [the response from server]
 * @param  {string} path     [the file path]
 * @param  {string} fileName [the file name]
 *
 * Note: comment out console.log for production.
 */
exports.writeToFile = function(res, path, fileName) {
  res = JSON.parse(res);

  var data = 'UserCount: ' + res.results.count + '\n';
  for (var k = 0; k < res.results.userIds.length; ++k) {
    data += res.results.userIds[k] + '\n';
  } 
  if (!fs.existsSync(path)) { // if the folder does not exist.
    fs.mkdir(path, function(err) { // try to create new folder.
      if (err) {
        console.log('Error in creating folder:', path, '\n', err);
      }
      else {
        write(path, fileName, data);
      }
    })
  } else { // if the folder exists, just create new file.
    write(path, fileName, data);
  }
  
}
