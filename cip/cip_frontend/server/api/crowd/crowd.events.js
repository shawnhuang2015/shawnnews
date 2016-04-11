/**
 * SingleCrowd model events
 */

'use strict';

import {EventEmitter} from 'events';
import SingleCrowd from './crowd.model';
var crowdEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
crowdEvents.setMaxListeners(0);

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
    crowdEvents.emit(event + ':' + doc._id, doc);
    crowdEvents.emit(event, doc);
  }
}

export default crowdEvents;
