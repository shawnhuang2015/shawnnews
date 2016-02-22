'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;

/**
 * sementic meta data Schema
 */
var SemanticMetaDataSchema = new Schema({
    name: {
        type: String,
        unique: true
    },
    created: {
        type: Date,
        default: Date.now
    },
    profile: {
        ontology: [
            {
                name: String,
                tree: [
                    {
                        parent: String,
                        children: [String]
                    }
                ]
            }
        ],
        object_ontology: [
            {
                object: String,
                ontologies: String
            }
        ],
        behaviour: [
            {
                name: String,
                subject: [
                    {
                        vtype: String,
                        etype: String
                    }
                ],
                object: [
                    {
                        vtype: String,
                        etype: String
                    }
                ]
            }
        ]
    }
});

mongoose.model('SemanticMetaData', SemanticMetaDataSchema);