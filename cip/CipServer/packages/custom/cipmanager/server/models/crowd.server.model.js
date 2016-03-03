'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;

/**
 * Crowd Schema
 */
var CrowdSingleSchema = new Schema({
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
    tagAdded: Boolean,
    selector: {
        tag: [
            {
                factor: {
                    type: String,
                    required: true,
                    trim: true
                },
                operator: {
                    type: String,
                    enum:["<>", "="],
                    required: true
                },
                weight: {
                    type: Number,
                    required: true,
                    default: 1.0,
                    min: 0,
                    max: 1
                },
                name: {
                    type: String,
                    required: true,
                    trim: true
                },
                count: Number
            }
        ],
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
                    min: -50.0,
                    max: 50.0
                },
                name: {
                    type: String,
                    required: true,
                    trim: true
                },
                count: Number
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
                    enum:['Category','Item','Contains'],
                    required: true
                },
                objectCategory: {
                    type: String,
                    required: true
                },
                ontologyType: {
                    type: String
                    //required: true
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
                count: Number
            }
        ]
    }
});

var CrowdGroupSchema = new Schema({
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
    tagAdded: Boolean,
    selector: [{type: Schema.Types.ObjectId, ref: 'CrowdSingle'}]
});

mongoose.model('CrowdSingle', CrowdSingleSchema);
mongoose.model('CrowdGroup', CrowdGroupSchema);
