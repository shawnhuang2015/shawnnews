'use strict';

/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Schema = mongoose.Schema;

/**
 * sementic meta data Schema
 * which contains:
 * 1. ontology information - one or more tree structure
 * 2. record which item and user will use the every ontology
 * 3. behavior definition
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