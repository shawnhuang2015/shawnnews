/**
 * SingleCrowd model events
 */

'use strict';

import {EventEmitter} from 'events';
import SingleCrowd from './single_crowd.model';
var SingleCrowdEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
SingleCrowdEvents.setMaxListeners(0);

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
    SingleCrowdEvents.emit(event + ':' + doc._id, doc);
    SingleCrowdEvents.emit(event, doc);
  }
}

export default SingleCrowdEvents;
