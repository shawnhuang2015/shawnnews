/**
 * Created on 2/2/16.
 */
'use strict';
var qs = require('querystring');
var utility = require('../utility/utility');
var util = require('util');
var fs = require('fs');
/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    SemanticMetaData = mongoose.model('SemanticMetaData'),
    CrowdSingle = mongoose.model('CrowdSingle'),
    config = require('meanio').loadConfig(),
    _ = require('lodash');

/*
 Get the user id list which belong to the crowd.
 Method: POST
 Input: selector conditions - which express the crowding condition that the client choose on UI
 Output: 1. user id list 2. user count
 */
exports.getCrowdDetailByPost = function(req, res) {
    if (!req.body['selector']) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelector(req.body.selector)) {
        return res.send({
            error: true,
            message: "wrong 'selector'"
        });
    }

    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        }

        var body = {
            target: md.profile.target,
            selector: generateCond(req.body['selector'], md.profile)
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

/*
    res.send({
        "results": {
            "count": 3,
            "userIds": ["user_1", "user_2", "user_3"]
        },
        "error": false,
        "message": ""
    });*/
}

/*
 Get the user id list which belong to the multi-crowd.
 Method: POST
 Input: selector conditions [Array] - which express the crowding condition that the client choose on UI
 Output: 1. user id list 2. user count
 */
exports.getGroupCrowdDetailByPost = function(req, res) {
    if (!req.body['selector']) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelectorArray(req.body.selector)) {
        return res.send({
            error: true,
            message: "wrong 'selector'"
        });
    }


    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err) {
            console.log('Error to get crowd detail');
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        }

        var body = {
            target: md.profile.target,
            selector: generateCond(req.body["selector"], md.profile)
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
/*
    res.send({
        "results": {
            "count": 3,
            "userIds": ["user_1", "user_2", "user_3"]
        },
        "error": false,
        "message": ""
    });*/
}

/*
Get the user id list which belong to the crowd.
It can be called just when the crowd has been create on crowding service.
Method: GET
Input: cname - crowd name
Output: 1. user id list 2. user count
 */
exports.getCrowdDetailByGet = function(req, res) {
    var crowdName = req.query['cname'];
    if (!crowdName) {
        return res.send({
            error: true,
            message: "Need Parameters 'cname'"
        });
    }
    /*
     utility.get('getCrowd', 'crowdName=' + crowdName, function(err, result) {
         if (err) {
             return res.send({
             message: err.message
             });
         } else {
             return res.send(result);
         }
     });
     */
    res.send({
        "results": {
            "count": 3,
            "userIds": ['user_1', 'user_2', 'user_3']
        },
        "error": false,
        "message": ""
    });
}

/*
Get the number of users which belong to the crowd.
Method: POST
Input: selector conditions - which express the crowding condition that the client choose on UI
*/
exports.getCrowdCountByPost = function(req, res) {
    console.log('Get Count: ' + JSON.stringify(req.body));
    if (!req.body['selector']) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelector(req.body.selector)) {
        return res.send({
            error: true,
            message: "wrong 'selector'"
        });
    }

    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err) {
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
        utility.post('crowd/v1/user_search', 'limit=0', bodyStr, function(err, resp) {
            if (err) {
                console.log('Error to create crowd count');
                return res.send({
                    error: true,
                    message: err.message
                });
            } else {
                return res.send(resp);
            }
        });
    });
/*
    res.send({
        "results": {
            "count": 3,
            "userIds": []
        },
        "error": false,
        "message": ""
    });*/
}

/*
 Get the number of users which belong to the multi-crowd.
 Method: POST
 Input: selector conditions [Array] - which express the crowding condition that the client choose on UI
 */
exports.getGroupCrowdCountByPost = function(req, res) {
    if (!req.body['selector']) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelectorArray(req.body.selector)) {
        return res.send({
            error: true,
            message: "wrong 'selector'"
        });
    }

    //var selector = JSON.stringify(req.body);
    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err) {
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
        utility.post('crowd/v1/user_search', 'limit=0', bodyStr, function(err, resp) {
            if (err) {
                console.log('Error to create crowd count');
                return res.send({
                    error: true,
                    message: err.message
                });
            } else {
                return res.send(resp);
            }
        });
    });
/*
    res.send({
        "results": {
            "count": 3,
            "userIds": []
        },
        "error": false,
        "message": ""
    });*/
}

/*
Get the number of users which belong to the crowd.
It can be called just when the crowd has been create on crowding service.
Method: GET
Input: cname - crowd name
 */
exports.getCrowdCountByGet = function(req, res) {
    var crowdName = req.query['cname'];
    if (!crowdName) {
        return res.send({
            error: true,
            message: "Need Parameters 'cname'"
        });
    }
    /*
     utility.get('getCrowdCount', 'crowdName=' + crowdName, fucntion(err, result) {
         if (err) {
             return res.send({
                message: err.message
             });
         } else {
             return res.send(result);
         }
     });
    */
    res.send({
        "results": {
            "count": 3
        },
        "error": false,
        "message": ""
    });
}


var generateCond = function(input_cond, metadata) {
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
    //console.log("input_cond: " + input_cond);
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
            item.startTime /= 1000; //need remove
            item.endTime /= 1000;   //need remove
            cond.behavior.push(item);
        }
        res.push(cond);
    }

    return res;
}

/*
 create a crowd on crowding service
 Input: 1. selector conditions - which express the crowding condition that the client choose on UI
        2. crowd name
 */
exports.createCrowdRemote = function(name, crowdtype,  condition) {
    function updateTag(name, value) {
        CrowdSingle.findOne({crowdName: name}, function(err, crowd) {
            if (err) {
                console.log('Error to read crowd: ' + name);
            } else {
                crowd.tagAdded = value;
                crowd.file = name + '.user';
                crowd.save(function(err) {
                    if (err) {
                        console.log('Error to create crowd: ' + name);
                        return;
                    }
                });
            }
        });
    }

    function writeUserToFile(res, path) {
        var data = 'UserCount: ' + res.results.count + "\n";
        for (var k = 0; k < res.results.userIds.length; ++k) {
            data += res.results.userIds[k] + "\n";
        }
        fs.writeFile(path, data,  function(err) {
            if (err) {
                console.log('Error to wirte file: ' + path);
            } else {
                console.log('Success to wirte file: ' + path);
            }
        });
    }

    condition = JSON.parse(condition);
    if (crowdtype == 'static') {
        SemanticMetaData.findOne({name: 'SemanticMetaData'}, function (err, md) {
            if (err) {
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
            utility.post('crowd/v1/create', 'name=' + name, bodyStr, function (err, res) {
                if (err) {
                    updateTag(name, -1);
                    console.log('Error to create crowd: ' + name);
                } else {
                    var jsRes = JSON.parse(res);
                    if (jsRes.error == false) {
                        writeUserToFile(jsRes, config.dataPath + name + '.user');
                        updateTag(name, 1);
                    } else {
                        updateTag(name, -1);
                        console.log('Error to create crowd: ' + name + ', error: ' + jsRes.message);
                    }
                }
            });
        });
    } else if (crowdtype == 'dynamic') {
        updateTag(name, 1);
        return;
    } else {
        console.log('wrong crowd type: ' + crowdtype);
    }
}

/*
Method: POST
Input: 1. selector conditions - which express the crowding condition that the client choose on UI
       2. count - the number of persons that the client want to seee
Output: Sample users in the crowd, the number of returned users' size equals to 'count'
 */
exports.crowdSampleByPost = function(req, res) {
    var selector = JSON.stringify(req.body);
    var count = req.query['count'];

    if (!req.body['selector'] || !count) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector', 'count'"
        });
    } else if (!utility.validateSelector(req.body.selector)) {
        return res.send({
            error: true,
            message: "wrong 'selector'"
        });
    }

    /*
     utility.post('crowdSample', 'count=' + count, cond, function(err, result) {
        if (err) {
            return res.send({
                message: err.message
            });
        } else {
            return res.send(result);
        }
    });
    */

    res.send({
        "results": {
            "count": 10
        },
        "error": false,
        "message": ""
    });

}

/*
The API can be invoked just when the crowd has been created on crowding service
Method: GET
Input: 1. cname - crowd name
       2. count - the number of persons that the client want to seee
Output: Sample users in the crowd, the number of returned users' size equals to 'count'
 */
exports.crowdSampleByGet = function(req, res) {
    var crowdName = req.query['cname'];
    var count = req.query['count'];
    if (!crowdName || !count) {
        return res.send({
            error: true,
            message: "Need Parameters 'cname', 'count'"
        });
    }

    /*
     utility.get('crowdSample', 'crowdName=' + crowdName + '&count=' + count, function(err, result) {
         if (err) {
             return res.send({
                message: err.message
             });
         } else {
            return res.send(result);
         }
     });
     */

    res.send({
        "results": {
            "count": 10
        },
        "error": false,
        "message": ''
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
exports.deleteCrowdRemote = function(name) {
     utility.get('crowd/v1/delete', 'name=' + name, function(err, result) {
         if (err) {
             console.log('delete crowd error: ' + err.message);
             return;
         }
         var jsRes = JSON.parse(result);
         if (jsRes.error) {
             console.log('delete crowd ' + name + ', error: ' + jsRes.message);
             return;
         } else {
             console.log('delete crowd: ' + name + ', count: ' + jsRes.results.count);
         }
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
    var curTime = new Date().getTime();
    SemanticMetaData.findOne({name: 'SemanticMetaData'}, function(err, md) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else if (md && (curTime - md['created'].getTime() < config.semanticSyncTime * 1000)) {
            console.log('time delta = ' + (curTime - md['created'].getTime()));
            return res.send(md['profile']);
        } else {
            var TestFlag = false;

            if (!TestFlag) {
                utility.get('get_profile',
                    '',
                    function (err, resp) {
                        if (err) {
                            return res.send({
                                error: true,
                                message: err.message
                            });
                        } else {
                            //return res.json(resp);
                            var result = JSON.parse(resp);
                            if (result.error == true) {
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
                                        profile: result['results']
                                    });
                                    metadata.save(function (err) {
                                        if (err) {
                                            return res.send({
                                                error: true,
                                                message: utility.getErrorMessage(err)
                                            });
                                        } else {
                                            return res.json(metadata['profile']);
                                        }
                                    });
                                }
                            });
                        }
                    }
                );
            } else {
                var result = {
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
                    }
                };
                SemanticMetaData.remove(function (err) {
                    if (err) {
                        return res.send({
                            error: true,
                            message: utility.getErrorMessage(err)
                        });
                    } else {
                        var metadata = new SemanticMetaData({name: 'SemanticMetaData', profile: result['results']});
                        metadata.save(function (err) {
                            if (err) {
                                return res.send({
                                    error: true,
                                    message: utility.getErrorMessage(err)
                                });
                            } else {
                                return res.json(metadata['profile']);
                            }
                        });
                    }
                });
            }
        }
    });
}

exports.download = function(req, res) {
    var file = req.query['file'];
    res.download(config.dataPath + file);
};