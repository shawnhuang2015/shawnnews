'use strict';
var utility = require('../utility/utility');
var rest = require('../controllers/rest.server.controller');
var mongoose = require('mongoose');
var CrowdSingle = mongoose.model('CrowdSingle');
var config = require('meanio').loadConfig();

//create
exports.create = function(req, res) {
    console.log('Single Crowd|Func create');
    var crowd = new CrowdSingle(req.body);

    crowd.save(function(err) {
        if (err) {
            console.log('create error: ' + utility.getErrorMessage(err))
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            rest.createSingleCrowdRemote(config.CrowdPrefix.single, crowd.crowdName,
                crowd.type, JSON.stringify(crowd.selector));
            console.log('New Crowd: ' + JSON.stringify(crowd));
            return res.json(crowd);
        }
    });
};

//list
exports.list = function(req, res) {
    console.log('Single Crowd|Func list');
    var pageId = Number(req.query.pageId);
    var pageSz = Number(req.query.pageSz);
    CrowdSingle.find().sort('-created').exec(function(err, crowds) {
        if (err) {
            console.log('list error: ' + utility.getErrorMessage(err))
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else if (crowds.length <= pageId * pageSz) {
            return res.send({
                error: true,
                message: 'Out of Bound'
            });
        } else {
            return res.json(crowds.slice(pageId * pageSz, (pageId + 1) * pageSz));
        }
    });
};

//count
exports.count = function(req, res) {
    console.log('Single Crowd|Func count');
    CrowdSingle.find().sort('-created').exec(function(err, crowds) {
        if (err) {
            console.log('count error: ' + utility.getErrorMessage(err))
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            return res.send({
                error: false,
                message: '',
                results: {
                    count: crowds.length
                }
            });
        }
    });
};

//read
exports.crowdByName = function(req, res, next, name) {
    console.log('Single Crowd|Func crowdByName');
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
    console.log('Single Crowd|Func read');
    res.json(req.crowd);
};

//update
exports.update = function(req, res) {
    console.log('Single Crowd|Func update');
    if (req.error) {
        console.log('update error: ' + req.error);
        return res.send({
            error: true,
            message: req.error
        });
    }
    var crowd = req.crowd;

    for (var field in req.body) {
        crowd[field] = req.body[field]
    }

    crowd.save(function(err) {
        if (err) {
            console.log('update error: ' + utility.getErrorMessage(err));
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            console.log('Updated Crowd: ' + JSON.stringify(crowd));
            return res.json(crowd);
        }
    });
};

//delete
exports.delete = function(req, res) {
    console.log('Single Crowd|Func delete');
    if (req.error) {
        console.log('delete error: ' + req.error);
        return res.send({
            error: true,
            message: req.error
        });
    }

    var crowd = req.crowd;

    crowd.remove(function(err) {
        if (err) {
            console.log('delete error: ' + utility.getErrorMessage(err));
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            rest.deleteCrowdRemote(config.CrowdPrefix.single + crowd.crowdName, crowd.type);
            return res.json(crowd);
        }
    });
}