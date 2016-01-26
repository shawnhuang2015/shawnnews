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
    crowd_name: {
        type: String,
        required: true,
        trim: true
    },
    description: {
        type: String,
        required: true,
        trim: true
    },
    type: {
        type: String,
        required: true,
        trim: true
    },
    selector: {
        demographic: {
            tag_id: {
                type: String,
                required: true,
                trim: true
            },
            conditions: [
                {
                    factor: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    operator: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    value: {
                        type: String,
                        required: true,
                        trim: true
                    }
                }
            ]
        },
        interest: {
            tag_id: {
                type: String,
                required: true,
                trime: true
            },
            conditions: [
                {
                    category: {
                        type: String,
                        required: true,
                        trim: true
                    }
                }
            ]
        },
        behavior: {
            tag_id: {
                type: String,
                required: true,
                trim: true
            },
            conditions: [
                {
                    action: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    object: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    value: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    start_time: {
                        type: String,
                        required: true,
                        trim: true
                    },
                    end_time: {
                        type: String,
                        required: true,
                        trim: true
                    }
                }
            ]
        }
    }
});

mongoose.model('Crowd', CrowdSchema);

