/******************************************************************************
 * Copyright (c) 2015, GraphSQL Inc.                                          *
 * All rights reserved                                                        *
 * Unauthorized copying of this file, via any medium is strictly prohibited   *
 * Proprietary and confidential                                               *
 ******************************************************************************/
(function(undefined) {
  "use strict";
  console.log('Loading gvis.graph')

  gvis.graph = function(_this) {

    this._this = _this;

    var data;
    var _nodesIdCount = 0;
    var _nodesTypeCount = 0;
    var _linksTypeCount = 0;

    data = {
      /*
      * The main data array.
      */
      array: {
        nodes: [],
        links: []
      },

      /*
      * For the internel and external id mapping.
      */
      idMap: {
        nodesID: Object.create(null), 
        nodesType: Object.create(null),
        linksType: Object.create(null)
      },

      /*
      * global indices for nodes and links.
      */
      index: {
        nodes: Object.create(null),
        links: Object.create(null)
      },

      /*
      * These indices refer from node to their neighboring nodes. Each key is an internal-id, and each
      * value is the array of the internal-ids of related nodes.
      */
      neighbors: {
        in: Object.create(null),
        out: Object.create(null),
        all: Object.create(null),
        inCount: Object.create(null),
        outCount: Object.create(null),
        allCount: Object.create(null)
      }
    }

    this.read = function(subgraph) {

      this.clear();

      for (var node in subgraph.nodes) {
        this.addNode(subgraph.nodes[node]);
      }

      for (var link in subgraph.links) {
        this.addLink(subgraph.links[link]);
      }
    }

    this.addSubgraph = function(subgraph) {
      for (var node in subgraph.nodes) {
        this.addNode(subgraph.nodes[node]);
      }

      for (var link in subgraph.links) {
        this.addLink(subgraph.links[link]);
      }
    }

    this.addNode = function(node) {
      if (Object(node) != node || arguments.length != 1) {
        throw 'addNode: Wrong arguments.';
      }

      if (typeof node.id != 'string' || typeof node.type != 'string') {
        throw 'The node must have a string id and type.';
      }

      node = gvis.utils.clone(node);

      gvis.behaviors.style.initializeNode(node);

      //1. do the id map.
      var in_id;
      var in_type;
      var ex_id = node.id;
      var ex_type = node.type;

      // update id map for node id.
      if (!!data.idMap.nodesID[ex_id]) {
        in_id = data.idMap.nodesID[ex_id];
      }
      else {
        data.idMap.nodesID[ex_id] = (++_nodesIdCount).toString();
        in_id = data.idMap.nodesID[ex_id];
      }

      // update id map for node type.
      if (!!data.idMap.nodesType[ex_type]) {
        in_type = data.idMap.nodesType[ex_type];
      }
      else {
        data.idMap.nodesType[ex_type] = (++_nodesTypeCount).toString();
        in_type = data.idMap.nodesType[ex_type];
      }
      // end 1.

      // 2. add node to index
      var key = generateNodeKey(in_type, in_id);

      if (!!data.index.nodes[key]) {
        try {
          throw 'The node "' + ex_type + '-' + ex_id + '" already exists.';
        }
        catch(err) {
          console.log(err)
          return;
        }
      }
      else {
        node[gvis.settings.key] = key;
        data.index.nodes[key] = node;
      }
      // end 2.

      // 3. add node in array.
      data.array.nodes.push(node)
      // end 3.

      // 4. add empty neighbors object for this node.
      data.neighbors.in[key] = Object.create(null);
      data.neighbors.out[key] = Object.create(null);
      data.neighbors.all[key] = Object.create(null);

      data.neighbors.inCount[key] = 0;
      data.neighbors.outCount[key] = 0;
      data.neighbors.allCount[key] = 0;

      // end 4.
    }

    this.addLink = function(link) {
      if (Object(link) != link || arguments.length != 1) {
        throw 'addLink: Wrong arguments.';
      }

      if (typeof link.type != 'string') {
        throw 'The link must have a string type.';
      }

      if (typeof link.source.id != 'string' || typeof link.source.type != 'string'
        || typeof link.target.id != 'string' || typeof link.target.type != 'string') {
        throw 'The link source node and target node must have string id and type.';
      }

      link = gvis.utils.clone(link);

      gvis.behaviors.style.initializeLink(link);

      // 1. check vaildation of new link. a) check node id map for exist node. b) update link id map. c) 
      var ex_source_type = link.source.type;
      var ex_source_id = link.source.id;
      
      var ex_target_type = link.target.type;
      var ex_target_id = link.target.id;

      var ex_link_type = link.type;

      var in_source_type;
      var in_source_id;
      var in_target_type;
      var in_target_id;
      var in_link_type;

      var key_source;
      var key_target;
      var key_link;

      if (!data.idMap.nodesType[ex_source_type] || !data.idMap.nodesID[ex_source_id]) {
        throw 'The link source node must have an existing type and id.'
      }
      else {
        in_source_type = data.idMap.nodesType[ex_source_type];
        in_source_id = data.idMap.nodesID[ex_source_id];
      }

      if (!data.idMap.nodesType[ex_target_type] || !data.idMap.nodesID[ex_target_id]) {
        throw 'The link target node must have an existing type and id.'
      }
      else {
        in_target_type = data.idMap.nodesType[ex_target_type];
        in_target_id = data.idMap.nodesID[ex_target_id];
      }

      if (!!data.idMap.linksType[ex_link_type]) {
        in_link_type = data.idMap.linksType[ex_link_type];
      }
      else {
        data.idMap.linksType[ex_link_type] = (++_linksTypeCount).toString();
        in_link_type = data.idMap.linksType[ex_link_type];
      }

      key_source = generateNodeKey(in_source_type, in_source_id);
      key_target = generateNodeKey(in_target_type, in_target_id);
      key_link = generateLinkKey(key_source, key_target, in_link_type);

      if (!data.index.nodes[key_source] || !data.index.nodes[key_target]) {
        throw 'The link target or source node must be an existing node.'
      }
      // end 1.

      // 2. add link to index

      link.source = data.index.nodes[key_source];
      link.target = data.index.nodes[key_target];


      if (!!data.index.links[key_link]) {
        try {
          throw 'The link "' + generateLinkKey(generateNodeKey(ex_source_type, ex_source_id), generateNodeKey(ex_target_type, ex_target_id), ex_link_type) + '" already exists.';
        }
        catch (err) {
          console.log(err);
          return;
        }
      }
      else {
        link[gvis.settings.key] = key_link;
        data.index.links[key_link] = link;
      }
      // end 2.

      // 3. add link to array
      data.array.links.push(link);
      // end 3.

      // 4. update neighbors object

      // neighbors in object.
      if (!data.neighbors.in[key_target][key_source]) {
        data.neighbors.in[key_target][key_source] = Object.create(null);
      }

      data.neighbors.in[key_target][key_source][key_link] = link;

      // neighbors out object.

      if (!data.neighbors.out[key_source][key_target]) {
        data.neighbors.out[key_source][key_target] = Object.create(null);
      }

      data.neighbors.out[key_source][key_target][key_link] = link;

      // neighbors all object
      if (!data.neighbors.all[key_target][key_source]) {
        data.neighbors.all[key_target][key_source] = Object.create(null);
      }

      data.neighbors.all[key_target][key_source][key_link] = link;

      if (!data.neighbors.all[key_source][key_target]) {
        data.neighbors.all[key_source][key_target] = Object.create(null);
      }

      data.neighbors.all[key_source][key_target][key_link] = link;

      // update the count
      /*
        inCount: Object.create(null),
        outCount: Object.create(null),
        allCount: Object.create(null)
      */
      data.neighbors.inCount[key_target]++;
      data.neighbors.outCount[key_source]++;
      data.neighbors.allCount[key_source]++;
      data.neighbors.allCount[key_target]++;
      // end 4.
    }

    this.dropNode = function(type, id) {
      if (typeof id != 'string' || typeof type != 'string') {
        throw 'The node must have a string id and type.';
      }

      var key = getInNodeKey(type, id).key;

      if (!data.index.nodes[key]) {
        try {
          throw 'The node ' + generateNodeKey(type, id) + ' does not exist or has been removed.';
        }
        catch (err) {
          console.log(err);
          return;
        } 
      }

      // Remove the node from graph.
      // 1. remove from index.
      delete data.index.nodes[key];

      // 2. remove from array
      for (var i=0; i<data.array.nodes.length; ++i) {
        if (data.array.nodes[i][gvis.settings.key] == key) {
          data.array.nodes.splice(i, 1);
          break;
        }
      }

      // 3. remove related links
      for (var i=data.array.links.length-1; i>=0; --i) {
        var link = data.array.links[i];

        if (link.source[gvis.settings.key] == key || link.target[gvis.settings.key] == key) {
          this.dropLink(link.source.type, link.source.id, link.target.type, link.target.id, link.type, true)
        }
      }

      // 5. remove object related with dropped node in neighbors

      // 5.1 for dropped node self.
      delete data.neighbors.in[key];
      delete data.neighbors.out[key];
      delete data.neighbors.all[key];

      delete data.neighbors.inCount[key];
      delete data.neighbors.outCount[key];
      delete data.neighbors.allCount[key];

      // 5.2 remove neighbors of other nodes that related dropped node.
      for (var i in data.index.nodes) {
        var other_node = data.index.nodes[i];
        var other_key = other_node[gvis.settings.key];

        delete data.neighbors.in[other_key][key];
        delete data.neighbors.out[other_key][key];
        delete data.neighbors.all[other_key][key];
      }

      // 6. remove type of node from id map if no more type exist.
      var checkType = false;

      for (var i in data.array.nodes) {
        var temp_node = data.array.nodes[i];
        if (temp_node.type == type) {
          checkType = true;
          break;
        }
      }

      if (!checkType) {
        delete data.idMap.nodesType[type];
      }

      // end Remove the node from graph.

      // if it is root node, pick the first node as root node.
      if (this._this.layouts._rootNodeKey == key && this.nodes().length) {
        this._this.layouts._rootNodeKey = this.nodes()[0][gvis.settings.key];
      }
    }

    this.dropLink = function(source_type, source_id, target_type, target_id, link_type, remove_nodes) {
      //console.log(source_type, source_id, target_type, target_id, link_type);
      if (typeof source_type != 'string' || typeof source_id != 'string' 
        || typeof target_type != 'string' || typeof target_id != 'string' 
        || typeof link_type != 'string') {
        throw 'dropLink: Wrong arguments.';
      }

      if (!remove_nodes) {
        // remove_nodes == undefined or false; we don't remove any nodes.
        remove_nodes = false;
      }
      else {
        // remove_nodes == true; we do remove source and target nodes of this link, if the node is isolated.
        remove_nodes = true;
      }

      var inMap = getInLinkKey(source_type, source_id, target_type, target_id, link_type);

      var key_source = inMap.source.key;
      var key_target = inMap.target.key;
      var key_link = inMap.key;

      if (!data.index.links[key_link]) {
        throw 'The link ' + generateLinkKey(generateNodeKey(source_type, source_id), generateNodeKey(target_type, target_id), link_type) + ' does not exist';
      }

      // Remove the link from graph
      // 1. remove the link from index
      delete data.index.links[key_link];

      // 2. remove the link from array
      for (var i=0; i<data.array.links.length; ++i) {
        if (data.array.links[i][gvis.settings.key] == key_link) {
          data.array.links.splice(i, 1);
          break;
        }
      }

      // 3. remove neighbors by the dropped link.
      // 3.1 for in
      delete data.neighbors.in[key_target][key_source][key_link];
      if (!Object.keys(data.neighbors.in[key_target][key_source]).length) {
        delete data.neighbors.in[key_target][key_source];
      }

      // 3.2 for out
      delete data.neighbors.out[key_source][key_target][key_link];
      if (!Object.keys(data.neighbors.out[key_source][key_target]).length) {
        delete data.neighbors.out[key_source][key_target];
      }

      // 3.3 for all
      delete data.neighbors.all[key_target][key_source][key_link];
      if (!Object.keys(data.neighbors.all[key_target][key_source]).length) {
        delete data.neighbors.all[key_target][key_source];
      }

      delete data.neighbors.all[key_source][key_target][key_link];
      if (!Object.keys(data.neighbors.all[key_source][key_target]).length) {
        delete data.neighbors.all[key_source][key_target];
      }

      // 4. update neighbors count
      data.neighbors.inCount[key_target]--;
      data.neighbors.outCount[key_source]--;
      data.neighbors.allCount[key_target]--;
      data.neighbors.allCount[key_source]--;

      // 5. remove node without link.
      if (remove_nodes) {
        if (data.neighbors.allCount[key_target] == 0) {
          if (!!data.index.nodes[key_target]) {
            this.dropNode(target_type, target_id);
          }
        }

        if (data.neighbors.allCount[key_source] == 0) {
          if (!!data.index.nodes[key_source]) {
            this.dropNode(source_type, source_id);
          }
        }
      }

      // 6. remove link type in idMap if this type is not in use anymore.
      // 6. remove type of node from id map if no more type exist.
      var checkType = false;

      for (var i in data.array.links) {
        var temp_link = data.array.links[i];
        if (temp_link.type == link_type) {
          checkType = true;
          break;
        }
      }

      if (!checkType) {
        delete data.idMap.linksType[link_type];
      }


      // end Remove the link from graph.
    }


    // clear the graph.
    this.clear = function() {
      data = {
        array: {
          nodes: [],
          links: []
        },
        idMap: {
          nodesID: Object.create(null), 
          nodesType: Object.create(null),
          linksType: Object.create(null)
        },
        index: {
          nodes: Object.create(null),
          links: Object.create(null)
        },
        neighbors: {
          in: Object.create(null),
          out: Object.create(null),
          all: Object.create(null),
          inCount: Object.create(null),
          outCount: Object.create(null),
          allCount: Object.create(null)
        }
      }
    }

    this.nodes = function() {
      switch (arguments.length) {
        case 0:
          //return all data object.
          return data.array.nodes;
        break;
        case 1:
          //return an array of selected node.
          var array = arguments[0];
          var result = [];
          if (Object.prototype.toString.call(array) === '[object Array]') {
            for (var i=0; i<array.length; ++i) {
              var inMap = getInNodeKey(array[i].type, array[i].id);
              if (!data.index.nodes[inMap.key]) {
                throw 'node ' + generateNodeKey(array[i].type, array[i].id) + ' does not exist.'
              }
              else {
                result.push(data.index.nodes[inMap.key]);
              }
            }
          }
          else if (Object.prototype.toString.call(array) === '[object Object]') {
            var ex_type = array.type;
            var ex_id = array.id;

            var inMap = getInNodeKey(ex_type, ex_id);

            if (!data.index.nodes[inMap.key]) {
              throw 'node ' + generateNodeKey(ex_type, ex_id) + ' does not exist.'
            }
            else {
              return data.index.nodes[inMap.key]; 
            }  
          }
          else if (typeof array === 'string') {
            if (!data.index.nodes[array]) {
              throw 'node ' + array + ' does not exist.'
            }
            else {
              return data.index.nodes[array]; 
            } 
          }
          else {
            throw array + ' not an array or an object.'
          }

          return result;
        break;
        case 2:
          var ex_type = arguments[0];
          var ex_id = arguments[1];

          var inMap = getInNodeKey(ex_type, ex_id);

          if (!data.index.nodes[inMap.key]) {
            throw 'node ' + generateNodeKey(ex_type, ex_id) + ' does not exist.'
          }
          else {
            return data.index.nodes[inMap.key]; 
          }  
        break;

        default:
      }

      throw 'nodes: Wrong arguments.'
    }

    this.links = function() {
      switch (arguments.length) {
        case 0:
          //return all data object.
          return data.array.links;
        break;
        case 1:
          //return an array of selected links.
          var array = arguments[0];
          var result = [];
          if (Object.prototype.toString.call(array) === '[object Array]') {
            for (var i=0; i<array.length; ++i) {
              var inMap = getInLinkKey(array[i].source.type, array[i].source.id, array[i].target.type, array[i].target.id, array[i].type);
              if (!data.index.links[inMap.key]) {
                throw 'link ' + generateLinkKey(generateNodeKey(array[i].source.type, array[i].source.id), generateNodeKey(array[i].target.type, array[i].target.id), array[i].type) + ' does not exist.'
              }
              else {
                result.push(data.index.links[inMap.key]);
              }
            }
          }
          else if (Object.prototype.toString.call(array) === '[object Object]') {
            var ex_source_type = array.source.type;
            var ex_source_id = array.source.id;
            var ex_target_type = array.target.type;
            var ex_target_id = array.target.id;
            var ex_link_type = array.type;

            var inMap = getInLinkKey(ex_source_type, ex_source_id, ex_target_type, ex_target_id, ex_link_type);

            if (!data.index.links[inMap.key]) {
              throw 'node ' + generateLinkKey(generateNodeKey(ex_source_type, ex_source_id), generateNodeKey(ex_target_type, ex_target_id), ex_link_type) + ' does not exist.'
            }
            else {
              return data.index.links[inMap.key]; 
            } 
          }
          else if (typeof array === 'string') {
            if (!data.index.links[array]) {
              throw 'link ' + array + ' does not exist.'
            }
            else {
              return data.index.links[array]; 
            } 
          }
          else {
            throw array + ' not an array or an object.'
          }

          return result;
        break;
        case 5:
          var ex_source_type = arguments[0];
          var ex_source_id = arguments[1];
          var ex_target_type = arguments[2];
          var ex_target_id = arguments[3];
          var ex_link_type = arguments[4];

          var inMap = getInLinkKey(ex_source_type, ex_source_id, ex_target_type, ex_target_id, ex_link_type);

          if (!data.index.links[inMap.key]) {
            throw 'node ' + generateLinkKey(generateNodeKey(ex_source_type, ex_source_id), generateNodeKey(ex_target_type, ex_target_id), ex_link_type) + ' does not exist.'
          }
          else {
            return data.index.links[inMap.key]; 
          }  
        break;

        default:
      }

      throw 'links: Wrong arguments.'
    }

    this.degree = function() {
      switch (arguments.length) {
        case 1:
          //return an array of selected node.
          var array = arguments[0];
          var result = [];
          if (Object.prototype.toString.call(array) === '[object Array]') {
            for (var i=0; i<array.length; ++i) {
              var inMap = getInNodeKey(array[i].type, array[i].id);
              if (!data.index.nodes[inMap.key]) {
                throw 'node ' + generateNodeKey(array[i].type, array[i].id) + ' does not exist.'
              }
              else {
                var item = {
                  in: data.neighbors.inCount[inMap.key],
                  out: data.neighbors.outCount[inMap.key],
                  all: data.neighbors.allCount[inMap.key]
                }

                result.push(item);
              }
            }
          }
          else if (Object.prototype.toString.call(array) === '[object Object]') {
            var ex_type = array.type;
            var ex_id = array.id;

            var inMap = getInNodeKey(ex_type, ex_id);

            if (!data.index.nodes[inMap.key]) {
              throw 'node ' + generateNodeKey(ex_type, ex_id) + ' does not exist.'
            }
            else {
              return {
                  in: data.neighbors.inCount[inMap.key],
                  out: data.neighbors.outCount[inMap.key],
                  all: data.neighbors.allCount[inMap.key]
                };
            }  
          }
          else {
            throw array + ' not an array or an object.'
          }

          return result;
        break;
        case 2:
          var ex_type = arguments[0];
          var ex_id = arguments[1];

          var inMap = getInNodeKey(ex_type, ex_id);

          if (!data.index.nodes[inMap.key]) {
            throw 'node ' + generateNodeKey(ex_type, ex_id) + ' does not exist.'
          }
          else {
            return {
                  in: data.neighbors.inCount[inMap.key],
                  out: data.neighbors.outCount[inMap.key],
                  all: data.neighbors.allCount[inMap.key]
                };
          }  
        break;

        default:
      }

      throw 'nodes: Wrong arguments.'
    }

    this.data = function() {
      return data;
    }

    this.outgoingLinks = function(nodeKey) {
      return data.neighbors.out[nodeKey];
    }

    this.incomingLinks = function(nodeKey) {
      return data.neighbors.in[nodeKey];
    }

    this.inoutLinks = function(nodeKey) {
      return data.neighbors.all[nodeKey];
    }

    function generateNodeKey(in_type, in_id) {
      return in_type + gvis.settings.nodeKeyConcChar + in_id;
    }

    function generateLinkKey(key_source, key_target, in_link_type) {
      return key_source + gvis.settings.linkKeyConcChar + key_target + gvis.settings.linkKeyConcChar +  in_link_type;
    }

    function getInNodeKey(ex_type, ex_id) {
      var result = {
        type: '',
        id: '',
        key: ''
      }

      if (!!data.idMap.nodesType[ex_type]) {
        result.type = data.idMap.nodesType[ex_type];
      }
      else {
        throw 'node type '+ ex_type +' does not exist.';
      }

      if (!!data.idMap.nodesID[ex_id]) {
        result.id = data.idMap.nodesID[ex_id];
      }
      else {
        throw 'node id does not exist.';
      }

      result.key = generateNodeKey(result.type, result.id);

      return result;
    }

    function getInLinkKey(ex_source_type, ex_source_id, ex_target_type, ex_target_id, ex_link_type) {
      var result = {
        source: '',
        target: '',
        type: '',
        key: '',
      }

      result.source = getInNodeKey(ex_source_type, ex_source_id);
      result.target = getInNodeKey(ex_target_type, ex_target_id);

      if (!!data.idMap.linksType[ex_link_type]) {
        result.type = data.idMap.linksType[ex_link_type];
      }
      else {
        throw 'Link type ' + ex_link_type + ' does not exist.';
      }
      
      result.key = generateLinkKey(result.source.key, result.target.key, result.type);

      return result;
    }
  }
}).call(this)
