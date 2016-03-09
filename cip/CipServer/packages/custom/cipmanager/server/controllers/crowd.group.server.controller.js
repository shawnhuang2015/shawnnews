'use strict';
var utility = require('../utility/utility');
var rest = require('../controllers/rest.server.controller');
var mongoose = require('mongoose');
var CrowdSingle = mongoose.model('CrowdSingle');
var CrowdGroup = mongoose.model('CrowdGroup');

//create
exports.create = function(req, res) {
    var body = req.body;
    var selector = req.body.selector;
    CrowdSingle.find({crowdName: {$in: selector}}, function(err, crowds) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            body.selector = crowds;
            var crowdGroup = new CrowdGroup(req.body);
            console.log('selector = ' + JSON.stringify(selector));
            console.log('crowds = ' + JSON.stringify(crowds));
            console.log('crowdGroup = ' + JSON.stringify(body));

            crowdGroup.save(function(err)    {
                if (err) {
                    return res.send({
                        error: true,
                        message: utility.getErrorMessage(err)
                    });
                } else {
                    rest.createCombinedCrowdRemote(crowdGroup.crowdName, crowdGroup.type, JSON.stringify(crowdGroup.selector));
                    return res.json(crowdGroup);
                }
            });
        }
    });
};


//list
exports.list = function(req, res) {
    var pageId = Number(req.query.pageId);
    var pageSz = Number(req.query.pageSz);
    CrowdGroup.find().sort('-created').populate('selector').exec(function(err, crowds) {
        if (err) {
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
    CrowdGroup.find().sort('-created').exec(function(err, crowds) {
        if (err) {
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
    var crowdGroup = req.crowd;
    //console.log("crowdGroup:  " + JSON.stringify(crowdGroup));

    for (var field in req.body) {
        console.log('field: ' + field);
        //console.log("body value: " + req.body[field]);
        //console.log("crowdGroup value: " + crowdGroup[field]);
        crowdGroup[field] = req.body[field]
    }

    var selector = req.body.selector;
    console.log('selector = ' + JSON.stringify(selector));
    CrowdSingle.find({crowdName: {$in: selector}}, function(err, crowds) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else {
            crowdGroup.selector = crowds;
            console.log('selector = ' + JSON.stringify(selector));
            console.log('crowds = ' + JSON.stringify(crowds));
            console.log('crowdGroup = ' + JSON.stringify(crowdGroup));

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
            //rest.deleteCrowdRemote(crowd.crowdName);
            return res.json(crowd);
        }
    });
}

