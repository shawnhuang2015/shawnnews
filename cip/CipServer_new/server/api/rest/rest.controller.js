'use strict';
var utility = require('../utility/utility');
var fs = require('fs');
require('./rest.model.js');
require('../crowdSingle/crowdSingle.model.js');
require('../crowdGroup/crowdGroup.model.js');
var mongoose = require('mongoose');
var SemanticMetaData = mongoose.model('SemanticMetaData');
var CrowdSingle = mongoose.model('CrowdSingle');
var CrowdGroup = mongoose.model('CrowdGroup');
import config from '../../config/environment'; 

var generateCond = function(input_cond, metadata) {
    console.log('Func generateCond');
    //map the vertex name to vertex type
    var tagT = {};
    for (var k = 0; k < metadata.tag.length; ++k) {
        var name = metadata.tag[k].name;
        var vtype = metadata.tag[k].vtype;
        tagT[name] = vtype;
    }
    var ontoT = {};
    for (var k = 0; k < metadata.ontology.length; ++k) {
        var name = metadata.ontology[k].name;
        var vtype = metadata.ontology[k].vtype;
        ontoT[name] = vtype;
    }

    //use condition in db to generate condition sent to crowding service
    if (!(input_cond instanceof Array)) {
        input_cond = [input_cond];
    }

    console.log("tagT: " + JSON.stringify(tagT));
    console.log("ontoT: " + JSON.stringify(ontoT));

    var res = [];
    for (var index = 0; index < input_cond.length; ++index) {
        var cond = {
            ontology: [],
            behavior: []
        };
        var tag = input_cond[index].tag;
        var onto = input_cond[index].ontology;
        var behr = input_cond[index].behavior;
        for (var k = 0; k < tag.length; ++k) {
            var item = tag[k];
            item.type = tagT[item.name];
            cond.ontology.push(item);
        }
        for (var k = 0; k < onto.length; ++k) {
            var item = onto[k];
            item.type = ontoT[item.name];
            cond.ontology.push(item);
        }
        for (var k = 0; k < behr.length; ++k) {
            var item = behr[k];
            item.ontologyType = ontoT[item.ontologyType];
            item.startTime /= 1000; //ms -> seconds
            item.endTime /= 1000;
            if (item.timeType === 'day' || item.timeType === 'hour') {
                item.timeType = 'relative';
            }
            cond.behavior.push(item);
        }
        res.push(cond);
    }

    return res;
}


/*
 Get the user id list which belong to the crowd.
 Method: POST
 Input: selector conditions - which express the crowding condition that the client choose on UI
 Output: 1. user id list 2. user count
 */
exports.getCrowdDetailByPost = function(req, res) {
    console.log('Func getCrowdDetailByPost');
    if (!req.body.selector) {
        return res.send({
            error: true,
            message: 'miss "selector"'
        });
    } else if (!utility.validateSelector(req.body.selector)) {
        return res.send({
            error: true,
            message: 'wrong "selector"'
        });
    }

    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err || md === null) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        }

        var body = {
            target: md.profile.target,
            selector: generateCond(req.body.selector, md.profile)
        };

        var bodyStr = JSON.stringify(body);
        utility.post('crowd/v1/user_search', '', bodyStr, function(err, resp) {
            if (err) {
                console.log('Error to get crowd: ' + bodyStr);
                return res.send({
                    error: true,
                    message: err.message
                });
            }
            return res.send(resp);
        });

    });
}

/*
 Get the user id list which belong to the multi-crowd.
 Method: POST
 Input: selector conditions [Array] - which express the crowding condition that the client choose on UI
 Output: 1. user id list 2. user count
 */
exports.getGroupCrowdDetailByPost = function(req, res) {
    console.log('Func getGroupCrowdDetailByPost');
    if (!req.body.selector) {
        return res.send({
            error: true,
            message: 'miss "selector"'
        });
    } else if (!utility.validateSelectorArray(req.body.selector)) {
        return res.send({
            error: true,
            message: 'wrong "selector"'
        });
    }


    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err || md === null) {
            console.log('Error to get crowd detail');
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        }

        var body = {
            target: md.profile.target,
            selector: generateCond(req.body.selector, md.profile)
        };

        var bodyStr = JSON.stringify(body);
        utility.post('crowd/v1/user_search', '', bodyStr, function(err, resp) {
            if (err) {
                console.log('Error to get crowd: ' + bodyStr);
                return res.send({
                    error: true,
                    message: err.message
                });
            }
            return res.send(resp);
        });

    });
}


/*
Get the number of users which belong to the crowd.
Method: POST
Input: selector conditions - which express the crowding condition that the client choose on UI
*/
exports.getCrowdCountByPost = function(req, res) {
    console.log('Func getCrowdCountByPost');
    console.log('Get Count: ' + JSON.stringify(req.body));
    var rid = req.query.rid;
    if (!req.body.selector) {
        return res.send({
            error: true,
            message: 'miss "selector"'
        });
    } else if (!utility.validateSelector(req.body.selector)) {
        return res.send({
            error: true,
            message: 'wrong "selector"'
        });
    }

    if (rid === null) {
        return res.send({
            error: true,
            message: 'rid missed'
        });
    }

    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err || md === null) {
            console.log('Error to get crowd count');
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        }
        var body = {
            target: md.profile.target,
            selector: generateCond(req.body.selector, md.profile)
        };
        var bodyStr = JSON.stringify(body);
        utility.post('crowd/v1/user_search', 'limit=0&rid=' + rid, bodyStr, function(err, resp) {
            if (err) {
                console.log('Error to create crowd count');
                return res.send({
                    error: true,
                    message: err.message
                });
            } else {
                var jsresp = JSON.parse(resp);
                if (jsresp.error === true) {
                    var ret = JSON.parse('{"results":{"count":0,"userIds":[]},"error":false,"message":"","debug":""}');
                    if (rid.length > 0) {
                        ret.results.requestId = rid;
                    }
                    return res.send(ret);
                } else {
                    return res.send(resp);
                }
            }
        });
    });
}

/*
 Get the number of users which belong to the multi-crowd.
 Method: POST
 Input: selector conditions [Array] - which express the crowding condition that the client choose on UI
 */
exports.getGroupCrowdCountByPost = function(req, res) {
    console.log('Func getGroupCrowdCountByPost');
    console.log('Get Count: ' + JSON.stringify(req.body));
    var rid = req.query.rid;
    if (!req.body.selector) {
        return res.send({
            error: true,
            message: 'miss "selector"'
        });
    } else if (!utility.validateSelectorArray(req.body.selector)) {
        return res.send({
            error: true,
            message: 'wrong "selector"'
        });
    }

    if (rid === null) {
        return res.send({
            error: true,
            message: 'rid missed'
        });
    }

    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err || md === null) {
            console.log('Error to get crowd count');
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        }
        var body = {
            target: md.profile.target,
            selector: generateCond(req.body.selector, md.profile)
        };
        var bodyStr = JSON.stringify(body);
        utility.post('crowd/v1/user_search', 'limit=0&rid=' + rid, bodyStr, function(err, resp) {
            if (err) {
                console.log('Error to create crowd count');
                return res.send({
                    error: true,
                    message: err.message
                });
            } else {
                var jsresp = JSON.parse(resp);
                if (jsresp.error === true) {
                    var ret = JSON.parse('{"results":{"count":0,"userIds":[]},"error":false,"message":"","debug":""}');
                    if (rid.length > 0) {
                        ret.results.requestId = rid;
                    }
                    return res.send(ret);
                } else {
                    return res.send(resp);
                }
            }
        });
    });
}

function writeUserToFile(res, path) {
    console.log('Func writeUserToFile');
    var data = 'UserCount: ' + res.results.count + "\n";
    for (var k = 0; k < res.results.userIds.length; ++k) {
        data += res.results.userIds[k] + '\n';
    }
    fs.writeFile(path, data, function(err) {
        if (err) {
            console.log('Error to wirte file: ' + path);
        } else {
            console.log('Success to wirte file: ' + path);
        }
    });
}

exports.createCombinedCrowdRemote = function(prefix, name, crowdtype, condition) {
    console.log('Func createCombinedCrowdRemote');
    function updateTag(name, value) {
        CrowdGroup.findOne({crowdName: name}, function(err, crowd) {
            if (err) {
                console.log('Error to read crowd: ' + name);
            } else {
                crowd.tagAdded = value;
                crowd.file = name + config.CrowdFileSuffix.multi;
                crowd.save(function(err) {
                    if (err) {
                        console.log('Error to create crowd: ' + name);
                        return;
                    }
                });
            }
        });
    }

    if (crowdtype === 'static') {
        var selector = [];
        condition = JSON.parse(condition);
        for (var index = 0; index < condition.length; ++index) {
            selector.push(condition[index].selector);
        }

        SemanticMetaData.findOne({name: 'SemanticMetaData'}, function (err, md) {
            if (err || md === null) {
                updateTag(name, -1);
                console.log('Error to create crowd: ' + name);
                return;
            }
            var body = {
                crowdIndex: md.profile.crowdIndex,
                searchCond: {
                    target: md.profile.target,
                    selector: generateCond(selector, md.profile)
                }
            };
            var bodyStr = JSON.stringify(body);
            utility.post('crowd/v1/create', 'name=' + prefix + name + '&limit=' + config.maxUserLimit, bodyStr, function (err, res) {
                if (err) {
                    updateTag(name, -1);
                    console.log('Error to create crowd: ' + name);
                } else {
                    var jsRes = JSON.parse(res);
                    if (jsRes.error === false) {
                        writeUserToFile(jsRes, config.dataPath + name + config.CrowdFileSuffix.multi);
                        updateTag(name, 1);
                    } else {
                        updateTag(name, -1);
                        console.log('Error to create crowd: ' + name + ', error: ' + jsRes.message);
                    }
                }
            });
        });
    } else if (crowdtype === 'dynamic') {
        updateTag(name, 1);
        return;
    } else {
        console.log('wrong crowd type: ' + crowdtype);
    }

}
/*
 create a crowd on crowding service
 Input: 1. selector conditions - which express the crowding condition that the client choose on UI
        2. crowd name
 */
exports.createSingleCrowdRemote = function(prefix, name, crowdtype,  condition) {
    console.log('Func createSingleCrowdRemote');
    function updateTag(name, value) {
        CrowdSingle.findOne({crowdName: name}, function(err, crowd) {
            if (err) {
                console.log('Error to read crowd: ' + name);
            } else {
                console.log("Crowd: " + crowd);
                crowd.tagAdded = value;
                crowd.file = name + config.CrowdFileSuffix.single;
                crowd.save(function(err) {
                    if (err) {
                        console.log('Error to create crowd: ' + name);
                        return;
                    }
                });
            }
        });
    }

    condition = JSON.parse(condition);
    if (crowdtype === 'static') {
        SemanticMetaData.findOne({name: 'SemanticMetaData'}, function (err, md) {
            if (err || md === null) {
                updateTag(name, -1);
                console.log('Error to create crowd: ' + name);
                return;
            }
            var body = {
                crowdIndex: md.profile.crowdIndex,
                searchCond: {
                    target: md.profile.target,
                    selector: generateCond(condition, md.profile)
                }
            };
            var bodyStr = JSON.stringify(body);
            utility.post('crowd/v1/create', 'name=' + prefix + name + '&limit=' + config.maxUserLimit, bodyStr, function (err, res) {
                if (err) {
                    updateTag(name, -1);
                    console.log('Error to create crowd: ' + name);
                } else {
                    var jsRes = JSON.parse(res);
                    if (jsRes.error === false) {
                        writeUserToFile(jsRes, config.dataPath + name + config.CrowdFileSuffix.single);
                        updateTag(name, 1);
                    } else {
                        updateTag(name, -1);
                        console.log('Error to create crowd: ' + name + ', error: ' + jsRes.message);
                    }
                }
            });
        });
    } else if (crowdtype === 'dynamic') {
        updateTag(name, 1);
        return;
    } else {
        console.log('wrong crowd type: ' + crowdtype);
    }
}

/*
The API can be invoked just when the crowd has been created on crowding service
Method: GET
Input: 1. cname - crowd name
       2. count - the number of persons that the client want to seee
Output: Sample users in the crowd, the number of returned users' size equals to 'count'
 */
exports.crowdSampleByGet = function(req, res) {
    console.log('Func crowdSampleByGet');
    var crowdName = req.query.cname;
    var count = req.query.count;
    var type = req.query.type; //single or multi
    if (crowdName === undefined || count === undefined || type === undefined) {
        return res.send({
            error: true,
            message: 'miss "cname", "count" or "type"'
        });
    }

    if (type !== 'single' && type !== 'multi') {
        return res.send({
            error: true,
            message: 'parameter type should be single/multi'
        });
    } else if (type === 'single') {
        crowdName = config.CrowdPrefix.single + crowdName;
    } else if (type === 'multi') {
        crowdName = config.CrowdPrefix.multi + crowdName;
    }

    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function (err, md) {
        if (err || md === null) {
            console.log('Error to sample crowd: ' + crowdName);
            return;
        }
        var vtype = md.profile.crowdIndex.vtype;
        utility.get('crowd/v1/get', 'name=' + crowdName + '&type=' + vtype + '&limit=' + count, function(err, resp) {
            if (err) {
                return res.send({
                    error: true,
                    message: err.message
                });
            } else {
                return res.send(resp);
            }
        });
    });
}

exports.readCrowd = function(crowdName) {

}

/*
The API will be invoked just when the crowd is static.
It is used to delete the crowd on crowding service.
Input: cname - crowd name
Output: return the number of persons in the deleted crowd
 */
exports.deleteCrowdRemote = function(name, type) {
    console.log('Func deleteCrowdRemote');
    if (type === 'dynamic') {
        return;
    }
    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function (err, md) {
        if (err || md === null) {
            console.log('Error to delete crowd: ' + name);
            return;
        }
        var vtype = md.profile.crowdIndex.vtype;
        utility.get('crowd/v1/delete', 'name=' + name + '&type=' + vtype, function (err, resp) {
            if (err) {
                console.log('Error to delete crowd: ' + name);
            } else {
                var jsRes = JSON.parse(resp);
                if (jsRes.error === true) {
                    console.log('Error to delete crowd: ' + name + ', mesg = ' + jsRes.message);
                } else {
                    console.log('delete crowd: ' + name);
                }
            }
        });
    });
}


exports.analyzeCrowd = function(crowdName, param) {

}

/*
Get Semantic Info and Ontology which will be used to select the crowd.
We will store the information to mongodb in case sending a restful API each time.
There is an expiration time for the information stored in mongodb,
we will update the data if it exceed the expiration time.
*/
exports.readMetadata = function(req, res) {
    console.log('Func readMetadata');
    var curTime = new Date().getTime();
    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else if (md && (curTime - md.created.getTime() < config.semanticSyncTime * 1000)) {
            console.log('time delta = ' + (curTime - md.created.getTime()));
            return res.send(md.profile);
        } else {
            var TestFlag = false;

            if (!TestFlag) {
                utility.get('get_profile', 'threshold=' + config.OntoLimit, function (err, resp) {
                        if (err) {
                            return res.send({
                                error: true,
                                message: err.message
                            });
                        } else {
                            var result = JSON.parse(resp);
                            if (result.error === true) {
                                return res.send({
                                    error: true,
                                    message: result.message
                                });
                            }
                            SemanticMetaData.remove(function (err) {
                                if (err) {
                                    return res.send({
                                        error: true,
                                        message: utility.getErrorMessage(err)
                                    });
                                } else {
                                    var metadata = new SemanticMetaData({
                                        name: 'SemanticMetaData',
                                        profile: result.results
                                    });
                                    metadata.save(function (err) {
                                        if (err) {
                                            return res.send({
                                                error: true,
                                                message: utility.getErrorMessage(err)
                                            });
                                        } else {
                                            return res.json(metadata.profile);
                                        }
                                    });
                                }
                            });
                        }
                    }
                );
            } else {
                var result = {/*
                    "error": false,
                    "message": '',
                    "results": {
                        "target": "user",
                        "crowdIndex": {"vtype": "__crowd_index", "etype": "__user_to_crowd_index"},
                        "ontology": [
                            {
                                "name": "tag",
                                "vtype": "tag_vtype",
                                "large": false,
                                "tree": [
                                    {"parent": "demo", "children": ["gender", "age"]},
                                    {"parent": "gender", "children": ["male", "female"]},
                                    {"parent": "age", "children": ["0~10", "10~20"]}
                                ]
                            },
                            {
                                "name": "interest",
                                "vtype": "interest",
                                "large": false,
                                "tree": [
                                    {"parent": "interest", "children": ["food", "book"]},
                                    {"parent": "food", "children": ["apple", "orange"]},
                                    {"parent": "book", "children": ["c++", "web development"]}
                                ]
                            },
                            {
                                "name": "pic_type",
                                "vtype": "pic_vtype",
                                "large": false,
                                "tree": [
                                    {"parent": "pic_type", "children": ["flower", "food"]},
                                    {"parent": "food", "children": ["apple", "orange"]}
                                ]
                            },
                            {
                                "name": "pic_tag",
                                "vtype": "pic_tag",
                                "large": false,
                                "tree": [
                                    {"parent": "pic_tag", "children": ["flower", "food"]},
                                    {"parent": "food", "children": ["apple", "orange"]}
                                ]
                            },
                            {
                                "name": "channel_tag",
                                "vtype": "channel_tag_vtype",
                                "large": false,
                                "tree": [
                                    {"parent": "channel_tag", "children": ["china", "USA"]},
                                    {"parent": "china", "children": ["cctv1", "cctv2"]},
                                    {"parent": "USA", "children": ["youtube"]}
                                ]
                            }
                        ],
                        "tag": [
                            {"name": "gender", "vtype": "demo", "element": ["male", "female"], "datatype": "itemset"},
                            {"name": "age", "vtype": "demo", "element": ["0~10", "10~20"], "datatype": "number"}
                        ],
                        "interest_intent": [
                            {"ontology": "interest"},
                            {"ontology": "pic_tag"}
                        ],

                        "object_ontology": [
                            {
                                "object": "pic",
                                "ontology": [{"name": "pic_type", "etype": "pic_to_pic_type"}, {
                                    "name": "pic_tag",
                                    "etype": "pic_tag_pic"
                                }]
                            },
                            {
                                "object": "user",
                                "ontology": [{"name": "demo", "etype": "user_to_demo"}, {
                                    "name": "interest",
                                    "etype": "user_to_interest"
                                }]
                            },
                            {"object": "theme", "ontology": [{"name": "pic_tag", "etype": "theme_to_pic_tag"}]},
                            {
                                "object": "channel",
                                "ontology": [{"name": "channel_tag", "etype": "channel_to_channel_tag"}]
                            }
                        ],
                        "behaviour": [
                            {
                                "name": "browse", "subject": [{"name": "user", "etype": "user_browse"}],
                                "object": [{"name": "pic", "etype": "browse_pic"}]
                            },
                            {
                                "name": "post", "subject": [{"name": "user", "etype": "user_post"}],
                                "object": [{"name": "pic", "etype": "post_pic"}]
                            },
                            {
                                "name": "visit", "subject": [{"name": "user", "etype": "user_visit"}],
                                "object": [{"name": "channel", "etype": "visit_channel"}]
                            },
                            {"name": "login", "subject": [{"name": "user", "etype": "user_login"}]}
                        ]
                    }*/
                };
                SemanticMetaData.remove(function (err) {
                    if (err) {
                        return res.send({
                            error: true,
                            message: utility.getErrorMessage(err)
                        });
                    } else {
                        var metadata = new SemanticMetaData({name: 'SemanticMetaData', profile: result.results});
                        metadata.save(function (err) {
                            if (err) {
                                return res.send({
                                    error: true,
                                    message: utility.getErrorMessage(err)
                                });
                            } else {
                                return res.json(metadata.profile);
                            }
                        });
                    }
                });
            }
        }
    });
}

exports.download = function(req, res) {
    console.log('Func download');
    var file = req.query.file;
    res.download(config.dataPath + file);
};
