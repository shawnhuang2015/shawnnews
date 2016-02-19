'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;

/**
 * Crowd Schema
 */
var CrowdSchema = new Schema({
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
    selector: [
        {
            ontology: [
                {
                    factor: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    operator: {
                        type: String,
                        enum:['>=','<=', ">", "<", "!=", "="]
                    },
                    weight: {
                        type: Number,
                        required: true,
                        min: 0,
                        max: 1
                    },
                    type: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    sequence: {
                        type: Number
                    }
                }
            ],
            behavior: [
                {
                    action: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    objectType: {
                        type: String,
                        required: true
                        //enum:['item','category', 'property']
                    },
                    objectId: {
                        type: String,
                        required: true
                    },
                    value: {
                        type: Number,
                        required: true,
                        min: 0
                    },
                    startTime: {
                        type: Number,
                        required: true,
                        trim: true
                    },
                    endTime: {
                        type: Number,
                        required: true,
                        trim: true
                    },
                    timeType: {
                        type: String,
                        enum: ["absolute", "relative", "hour", "day", "week", "month", "year"]
                    },
                    sequence: {
                        type: Number
                    }
                }
            ]
        }
    ]
});

mongoose.model('Crowd', CrowdSchema);