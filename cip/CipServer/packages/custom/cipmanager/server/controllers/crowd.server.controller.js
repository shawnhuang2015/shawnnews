'use strict';
var rest = require('../utility/restful');
var util = require('util');
/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    Crowd = mongoose.model('Crowd'),
    config = require('meanio').loadConfig(),
    _ = require('lodash');

var getErrorMessage = function(err) {
    if (err.errors) {
        for (var errName in err.errors) {
            if (err.errors[errName].message) return err.errors[errName].message;
        }
    } else {
        return "Unknown server error";
    }
};

//create
exports.create = function(req, res) {
    var crowd = new Crowd(req.body);

    crowd.save(function(err) {
        if (err) {
            return res.send({
                error: true,
                message: getErrorMessage(err)
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
    Crowd.find().sort("-created").exec(function(err, crowds) {
        if (err) {
            return res.send({
                error: true,
                message: getErrorMessage(err)
            });
        } else if (crowds.length < (pageId + 1) * pageSz) {
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
    Crowd.find().sort("-created").exec(function(err, crowds) {
        if (err) {
            return res.send({
                error: true,
                message: getErrorMessage(err)
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
    Crowd.findOne({crowdName: name}, function(err, crowd) {
        if (err) {
            req.error = getErrorMessage(err);
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
    var crowd = req.crowd;

    for (var field in req.body) {
        console.log("field: " + field);
        crowd[field] = req.body[field]
    }

    crowd.save(function(err) {
        if (err) {
            return res.send({
                error: true,
                message: getErrorMessage(err)
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
                message: getErrorMessage(err)
            });
        } else {
            return res.json(crowd);
        }
    });
}

