'use strict';
var utility = require('../utility/utility');
var rest = require('../controllers/rest.server.controller');
var mongoose = require('mongoose');
var CrowdSingle = mongoose.model('CrowdSingle');
var CrowdGroup = mongoose.model('CrowdGroup');
var config = require('meanio').loadConfig();

//create
exports.create = function(req, res) {
    console.log('Combined Crowd|Func create');
    var body = req.body;
    var selector = req.body.selector;
    CrowdSingle.find({crowdName: {$in: selector}}, function(err, crowds) {
        if (err) {
            console.log('create error: ' + utility.getErrorMessage(err))
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            body.selector = crowds;
            var crowdGroup = new CrowdGroup(req.body);

            console.log('New crowdGroup: ' + JSON.stringify(body));

            crowdGroup.save(function(err)    {
                if (err) {
                    console.log('create error: ' + utility.getErrorMessage(err))
                    return res.send({
                        error: true,
                        message: utility.getErrorMessage(err)
                    });
                } else {
                    rest.createCombinedCrowdRemote(config.CrowdPrefix.multi, crowdGroup.crowdName,
                        crowdGroup.type, JSON.stringify(crowdGroup.selector));
                    return res.json(crowdGroup);
                }
            });
        }
    });
};


//list
exports.list = function(req, res) {
    console.log('Combined Crowd|Func list');
    var pageId = Number(req.query.pageId);
    var pageSz = Number(req.query.pageSz);
    CrowdGroup.find().sort('-created').populate('selector').exec(function(err, crowds) {
        if (err) {
            console.log('list error: ' + utility.getErrorMessage(err));
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else if (crowds.length <= pageId * pageSz) {
            console.log('list error: Out of Bound');
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
    console.log('Combined Crowd|Func count');
    CrowdGroup.find().sort('-created').exec(function(err, crowds) {
        if (err) {
            console.log('count error: ' + utility.getErrorMessage(err));
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
    console.log('Combined Crowd|Func crowdByName');
    CrowdGroup.findOne({crowdName: name}).populate('selector').exec(function(err, crowd) {
        if (err) {
            req.error = utility.getErrorMessage(err);
        } else {
            req.crowd = crowd;
        }
        next();
    });
};

exports.read = function(req, res) {
    console.log('Combined Crowd|Func read');
    res.json(req.crowd);
};

//update
exports.update = function(req, res) {
    console.log('Combined Crowd|Func update');
    if (req.error) {
        console.log('update error: ' + req.error);
        return res.send({
            error: true,
            message: req.error
        });
    }
    var crowdGroup = req.crowd;

    for (var field in req.body) {
        crowdGroup[field] = req.body[field]
    }

    var selector = req.body.selector;
    CrowdSingle.find({crowdName: {$in: selector}}, function(err, crowds) {
        if (err) {
            console.log('update error: ' + utility.getErrorMessage(err));
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            crowdGroup.selector = crowds;
            console.log('Updated crowdGroup: ' + JSON.stringify(crowdGroup));

            crowdGroup.save(function(err) {
                if (err) {
                    return res.send({
                        error: true,
                        message: utility.getErrorMessage(err)
                    });
                } else {
                    return res.json(crowdGroup);
                }
            });
        }
    });
};

//delete
exports.delete = function(req, res) {
    console.log('Combined Crowd|Func delete');
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
            rest.deleteCrowdRemote(config.CrowdPrefix.multi + crowd.crowdName, crowd.type);
            return res.json(crowd);
        }
    });
}

