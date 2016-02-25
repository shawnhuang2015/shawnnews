'use strict';
var util = require('util');
var utility = require('../utility/utility');
/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    CrowdSingle = mongoose.model('CrowdSingle'),
    CrowdGroup = mongoose.model('CrowdGroup'),
    config = require('meanio').loadConfig(),
    _ = require('lodash');

//create
exports.create = function(req, res) {
    var crowd = new CrowdSingle(req.body);
    console.log("crowd = " + JSON.stringify(req.body));

    crowd.save(function(err) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            return res.json(crowd);
        }
    });
};

//list
exports.list = function(req, res) {
    var pageId = Number(req.query["pageId"]);
    var pageSz = Number(req.query["pageSz"]);
    CrowdSingle.find().sort("-created").exec(function(err, crowds) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else if (crowds.length <= pageId * pageSz) {
            return res.send({
                error: true,
                message: "Out of Bound"
            });
        } else {
            return res.json(crowds.slice(pageId * pageSz, (pageId + 1) * pageSz));
        }
    });
};

//count
exports.count = function(req, res) {
    CrowdSingle.find().sort("-created").exec(function(err, crowds) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            return res.send({
                error: false,
                message: "",
                results: {
                    count: crowds.length
                }
            });
        }
    });
};

//read
exports.crowdByName = function(req, res, next, name) {
    CrowdSingle.findOne({crowdName: name}, function(err, crowd) {
        if (err) {
            req.error = utility.getErrorMessage(err);
        } else {
            req.crowd = crowd;
        }
        next();
    });
};

exports.read = function(req, res) {
    res.json(req.crowd);
};

//update
exports.update = function(req, res) {
    if (req.error) {
        console.log(req.error);
        return res.send({
            error: true,
            message: req.error
        });
    }
    var crowd = req.crowd;

    for (var field in req.body) {
        console.log("field: " + field);
        crowd[field] = req.body[field]
    }

    crowd.save(function(err) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            return res.json(crowd);
        }
    });
};

//delete
exports.delete = function(req, res) {
    if (req.error) {
        console.log(req.error);
        return res.send({
            error: true,
            message: req.error
        });
    }

    var crowd = req.crowd;

    crowd.remove(function(err) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            return res.json(crowd);
        }
    });
}

