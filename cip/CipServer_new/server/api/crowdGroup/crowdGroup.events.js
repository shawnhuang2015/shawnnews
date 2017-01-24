/**
 * CrowdGroup model events
 */

'use strict';

import {EventEmitter} from 'events';
import CrowdGroup from './crowdGroup.model';
var CrowdGroupEvents = new EventEmitter();

// Set max event listeners (0 == unlimited)
CrowdGroupEvents.setMaxListeners(0);

// Model events
var events = {
  'save': 'save',
  'remove': 'remove'
};

// Register the event emitter to the model events
for (var e in events) {
  var event = events[e];
  CrowdGroup.schema.post(e, emitEvent(event));
}

function emitEvent(event) {
  return function(doc) {
    CrowdGroupEvents.emit(event + ':' + doc._id, doc);
    CrowdGroupEvents.emit(event, doc);
  }
}

export default CrowdGroupEvents;
