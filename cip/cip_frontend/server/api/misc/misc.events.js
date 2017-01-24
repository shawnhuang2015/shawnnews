/**
 * Misc model events
 */

'use strict';

import {EventEmitter} from 'events';
import Misc from './misc.model';
var MiscEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
MiscEvents.setMaxListeners(0);

// Model events
var events = {
  'save': 'save',
  'remove': 'remove'
};

// Register the event emitter to the model events
for (var e in events) {
  var event = events[e];
  Misc.schema.post(e, emitEvent(event));
}

function emitEvent(event) {
  return function(doc) {
    MiscEvents.emit(event + ':' + doc._id, doc);
    MiscEvents.emit(event, doc);
  }
}

export default MiscEvents;
