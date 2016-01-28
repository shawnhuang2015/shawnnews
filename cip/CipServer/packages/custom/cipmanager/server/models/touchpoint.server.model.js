'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;


/**
 * TouchPoint Schema
 */
var TouchPointSchema = new Schema({
    created: {
        type: Date,
        default: Date.now
    },
    touchpoint_name: {
        type: String,
        required: true,
        trim: true
    },
    description: {
        type: String,
        required: true,
        trim: true
    },
    priority: {
        type: String,
        required: true,
        trim: true
    },
    device: {
        type: String,
        required: true,
        trim: true
    },
    screenshot: {
        type: String,
        required: true,
        trim: true
    },
    policy_logic: {
        type: String,
        required: true,
        trim: true
    },
    context: {
        type: String,
        required: true,
        trim: true
    }
});

mongoose.model('TouchPoint', TouchPointSchema);

