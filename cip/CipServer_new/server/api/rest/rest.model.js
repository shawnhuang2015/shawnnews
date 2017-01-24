'use strict';

import mongoose from 'mongoose';

var SemanticMetaDataSchema = new mongoose.Schema({
  name: {
        type: String,
        unique: true
    },
    created: {
        type: Date,
        default: Date.now
    },
    profile: {
        target: String,
        crowdIndex: {vtype: String, etype: String},
        ontology: [
            {
                name: String,
                vtype: String,
                large: Boolean,
                tree: [
                    {
                        parent: String,
                        children: [String]
                    }
                ]
            }
        ],
        tag: [
            {
                name: String,
                vtype: String,
                element: [String],
                datetype: {
                    type: String,
                    enum: ['itemset', 'number']
                }
            }
        ],
        interest_intent: [
            {
                ontology: String,
            }
        ],
        object_ontology: [
            {
                object: String,
                ontology: [
                    {
                        name: String,
                        etype: String
                    }
                ]
            }
        ],
        behaviour: [
            {
                name: String,
                subject: [
                    {
                        name: String,
                        etype: String
                    }
                ],
                object: [
                    {
                        name: String,
                        etype: String
                    }
                ]
            }
        ]
    }
});

export default mongoose.model('SemanticMetaData', SemanticMetaDataSchema);
