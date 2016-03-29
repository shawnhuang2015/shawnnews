'use strict';

import mongoose from 'mongoose';
require('../crowdSingle/crowdSingle.model.js');
var Schema = mongoose.Schema;

var CrowdGroupSchema = new mongoose.Schema({
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
        enum:['static','dynamic']
    },
    count: {
        type: Number
    },
    tagAdded: {
        type: Number,
        required: true,
        default: 0
    }, //done ＝1, ongoing ＝0, failed ＝－1
    file: {
        type: String,
        default: 'test.user'
    },
    logic: {
        type: String,
        enum: ['or', 'and']
    },
    selector: [{type: Schema.Types.ObjectId, ref: 'CrowdSingle'}]
});

export default mongoose.model('CrowdGroup', CrowdGroupSchema);
