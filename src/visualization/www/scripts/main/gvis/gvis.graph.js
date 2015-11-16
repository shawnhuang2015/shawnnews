(function(undefined) {
  "use strict";
  console.log('Loading gvis.graph')

  gvis.graph = function() {

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
      * global indices for nodes and edges.
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

    this.addNode = function(node) {
      if (Object(node) != node || arguments.length != 1) {
        throw 'addNode: Wrong arguments.';
      }

      if (typeof node.id != 'string' || typeof node.type != 'string') {
        throw 'The node must have a string id and type.';
      }

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
        data.idMap.nodesID[ex_id] = ++_nodesIdCount;
        in_id = data.idMap.nodesID[ex_id];
      }

      // update id map for node type.
      if (!!data.idMap.nodesType[ex_type]) {
        in_type = data.idMap.nodesType[ex_type];
      }
      else {
        data.idMap.nodesType[ex_type] = ++_nodesTypeCount;
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
        node.key = key;
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
        throw 'The edge must have a string type.';
      }

      if (typeof link.source.id != 'string' || typeof link.source.type != 'string'
        || typeof link.target.id != 'string' || typeof link.target.type != 'string') {
        throw 'The edge source node and target node must have string id and type.';
      }

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
        throw 'The edge source node must have an existing type and id.'
      }
      else {
        in_source_type = data.idMap.nodesType[ex_source_type];
        in_source_id = data.idMap.nodesID[ex_source_id];
      }

      if (!data.idMap.nodesType[ex_target_type] || !data.idMap.nodesID[ex_target_id]) {
        throw 'The edge target node must have an existing type and id.'
      }
      else {
        in_target_type = data.idMap.nodesType[ex_target_type];
        in_target_id = data.idMap.nodesID[ex_target_id];
      }

      if (!!data.idMap.linksType[ex_link_type]) {
        in_link_type = data.idMap.linksType[ex_link_type];
      }
      else {
        data.idMap.linksType[ex_link_type] = ++_linksTypeCount;
        in_link_type = data.idMap.linksType[ex_link_type];
      }

      key_source = generateNodeKey(in_source_type, in_source_id);
      key_target = generateNodeKey(in_target_type, in_target_id);
      key_link = generateLinkKey(key_source, key_target, in_link_type);

      if (!data.index.nodes[key_source] || !data.index.nodes[key_target]) {
        throw 'The edge target or source node must be an existing node.'
      }
      // end 1.


      // 2. add link to index
      if (!!data.index.links[key_link]) {
        try {
          throw 'The link "' + generateLinkKey(generateNodeKey(ex_source_type, ex_source_id), generateNodeKey(ex_target_type, ex_target_id), ex_link_type) + '" already exists.';
        }
        catch (err) {
          console.log(err);
        }
      }
      else {
        link.key = key_link;
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

      console.log(data);
    }

    this.dropNode = function(type, id) {

    }

    this.dropLink = function(source_type, source_id, target_type, target_id, link_type) {

    }

    this.data = function() {
      return data;
    }

    function generateNodeKey(in_type, in_id) {
      return in_type + gvis.settings.nodeKeyConcChar + in_id;
    }

    function generateLinkKey(key_source, key_target, in_link_type) {
      return key_source + gvis.settings.linkKeyConcChar + key_target + gvis.settings.linkKeyConcChar +  in_link_type;
    }
  }

}).call(this)