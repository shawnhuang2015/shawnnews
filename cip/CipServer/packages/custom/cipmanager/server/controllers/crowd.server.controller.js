'use strict';

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
            return res.status(400).send({
                message: getErrorMessage(err)
            });
        } else {
            return res.json(crowd);
        }
    });
};

//list
exports.list = function(req, res) {
    Crowd.find().sort("-created").exec(function(err, crowds) {
        if (err) {
            return res.status(400).send({
                message: getErrorMessage(err)
            });
        } else {
            return res.json(crowds);
        }
    });
};

//read
exports.crowdByName = function(req, res, next, name) {
    Crowd.find({crowd_name: name}, function(err, crowd) {
        if (err) return next(err);
        if (!crowd) return next(new Error('Failed to load crowd: ' + name));

        req.crowd = crowd[0];
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

    crowd.save(function(err){
        if (err) {
            return res.status(400).send({
                message: getErrorMessage(err)
            });
        } else {
            res.json(crowd);
        }
    });
};

//delete
exports.delete = function(req, res) {
    var crowd = req.crowd;

    crowd.remove(function(err) {
        if (err) {
            return res.status(400).send({
                message: getErrorMessage(err)
            });
        } else {
            res.json(crowd);
        }
    });
};

