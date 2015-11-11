(function(undefined) {
  "use strict";
  console.log('Loading gvis.graph')

  gvis.graph = function() {

    var data;

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
      idSet: {
        nodes: Object.create(null),
        links: Object.create(null)
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
        all: Object.create(null)
      }
    }

    this.addNode = function(newNode) {
      if (Object(newNode) != newNode || arguments.length != 1) {
        throw 'addNode: Wrong arguments.';
      }


      console.log(this);
    }

    this.addLink = function(newLink) {

    }

    this.data = function() {
      return data;
    }
  }

}).call(this)