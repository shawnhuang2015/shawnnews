/**
 * SingleCrowd model events
 */

'use strict';

import {EventEmitter} from 'events';
import SingleCrowd from './crowd.model';
var CrowdEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
CrowdEvents.setMaxListeners(0);

// Model events
var events = {
  'save': 'save',
  'remove': 'remove'
};

// Register the event emitter to the model events
for (var e in events) {
  var event = events[e];
  SingleCrowd.schema.post(e, emitEvent(event));
}

function emitEvent(event) {
  return function(doc) {
    CrowdEvents.emit(event + ':' + doc._id, doc);
    CrowdEvents.emit(event, doc);
  }
}

export default CrowdEvents;
