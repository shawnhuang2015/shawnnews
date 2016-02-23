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
    count: {
        type: Number
    },
    selector: {
        ontology: [
            {
                factor: {
                    type: String,
                    required: true,
                    trim: true
                },
                operator: {
                    type: String,
                    enum:['>=','<=', ">", "<", "<>", "=", "like", "dislike"],
                    required: true
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
                count: Number,
                sequence: Number
            }
        ],
        behavior: [
            {
                action: {
                    type: String,
                    required: true,
                    trim: true
                },
                objectCategory: {
                    type: String,
                    required: true
                },
                objectType: {
                    type: String,
                    required: true
                },
                objectId: {
                    type: String,
                    required: true
                },
                operator: {
                    type: String,
                    enum:['>=','<=', ">", "<", "<>", "="],
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
                    enum: ["absolute", "relative", "hour", "day", "week", "month", "year"],
                    required: true
                },
                count: Number,
                sequence: Number
            }
        ]
    }
});

mongoose.model('Crowd', CrowdSchema);