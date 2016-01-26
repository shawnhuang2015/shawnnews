'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;

/**
 * Campaign Schema
 */
var CampaignSchema = new Schema({
    created: {
        type: Date,
        default: Date.now
    },
    campaign_name: {
        type: String,
        required: true,
        trim: true
    },
    description: {
        type: String,
        required: true,
        trim: true
    },
    start_date: {
        type: String,
        required: true,
        trim: true
    },
    end_date: {
        type: String,
        required: true,
        trim: true
    },
    audience_crowd: {
        type: String,
        required: true,
        trim: true
    },
    touchpoints: [
        {
            type: String,
            required: true,
            trim: true
        }
    ],
    touchpoint_policy: [
        {
            type: String,
            required: true,
            trim: true
        }
    ],
    priority: {
        type: String,
        required: true,
        trim: true
    },
    comments: {
        type: String,
        required: true,
        trim: true
    },
    intelligenceEngineConfig: {
        type: {
            type: String,
            required: true,
            trim: true
        },
        config: {
            type: String,
            required: true,
            trim: true
        }
    }
});

mongoose.model('Campaign', CampaignSchema);

