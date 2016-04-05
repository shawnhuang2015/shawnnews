/**
 * Ontology model events
 */

'use strict';

import {EventEmitter} from 'events';
import Ontology from './ontology.model';
var OntologyEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
OntologyEvents.setMaxListeners(0);

// Model events
var events = {
  'save': 'save',
  'remove': 'remove'
};

// Register the event emitter to the model events
for (var e in events) {
  var event = events[e];
  Ontology.schema.post(e, emitEvent(event));
}

function emitEvent(event) {
  return function(doc) {
    OntologyEvents.emit(event + ':' + doc._id, doc);
    OntologyEvents.emit(event, doc);
  }
}

export default OntologyEvents;
