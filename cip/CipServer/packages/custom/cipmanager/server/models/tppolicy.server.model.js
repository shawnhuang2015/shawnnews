'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;


/**
 * TPPolicy Schema
 */
var TPPolicySchema = new Schema({
    created: {
        type: Date,
        default: Date.now
    },
    touchpoint_policy_name: {
        type: String,
        required: true,
        trim: true
    },
    description: {
        type: String,
        required: true,
        trim: true
    },
    touchpoint: [
        {
            type: String,
            required: true,
            trim: true
        }
    ],
    policy_logic: {
        type: String,
        required: true,
        trim: true
    }
});

mongoose.model('TPPolicy', TPPolicySchema);

