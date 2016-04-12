'use strict';

import mongoose from 'mongoose';

var GroupCrowdSchema = new mongoose.Schema({
  created: {
    type: Date,
    default: Date.now
  },
  crowdName: {
    type: String,
    unique: true
  },
  description: {
    type: String,
    required: true
  },
  type: {
    type: String,
    enum:[ 'static', 'dynamic' ]
  },
  count: {
    type: Number
  },
  tagAdded: {
    type: Number,
    required: true,
    default: 0
  }, // done ＝1, ongoing ＝0, failed ＝－1
  file: {
    type: String,
    default: ''
  },
  logic: {
    type: String,
    enum: [ 'or', 'and' ]
  },
  selector: [{ type: mongoose.Schema.Types.ObjectId, ref: 'SingleCrowd' }]
});

export default mongoose.model('GroupCrowd', GroupCrowdSchema);
