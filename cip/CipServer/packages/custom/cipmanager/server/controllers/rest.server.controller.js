/**
 * Created on 2/2/16.
 */
'use strict';
var qs = require('querystring');
var utility = require('../utility/utility');
/**
 * Module dependencies.
 */
var mongoose = require('mongoose'),
    SemanticMetaData = mongoose.model('SemanticMetaData'),
    config = require('meanio').loadConfig(),
    _ = require('lodash');

/*
 Get the user id list which belong to the crowd.
 Method: POST
 Input: selector conditions - which express the crowding condition that the client choose on UI
 Output: 1. user id list 2. user count
 */
exports.getCrowdDetailByPost = function(req, res) {
    if (!req.body["selector"]) {
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
    var selector = JSON.stringify(req.body);
    /*
     utility.post('getCrowd', '', selector, function(err, result) {
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
            "userIds": ["user_1", "user_2", "user_3"]
        },
        "error": false,
        "message": ""
    });
}

/*
 Get the user id list which belong to the multi-crowd.
 Method: POST
 Input: selector conditions [Array] - which express the crowding condition that the client choose on UI
 Output: 1. user id list 2. user count
 */
exports.getGroupCrowdDetailByPost = function(req, res) {
    if (!req.body["selector"]) {
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
    var selector = JSON.stringify(req.body);
    /*
     utility.post('getCrowd', '', selector, function(err, result) {
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
            "userIds": ["user_1", "user_2", "user_3"]
        },
        "error": false,
        "message": ""
    });
}

/*
Get the user id list which belong to the crowd.
It can be called just when the crowd has been create on crowding service.
Method: GET
Input: cname - crowd name
Output: 1. user id list 2. user count
 */
exports.getCrowdDetailByGet = function(req, res) {
    var crowdName = req.query["cname"];
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
            "userIds": ["user_1", "user_2", "user_3"]
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
    if (!req.body["selector"]) {
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
    var selector = JSON.stringify(req.body);
    /*
     utility.post('getCrowdCount', '', selector, fucntion(err, result){
        if (err) {
            return res.send({
                message: err.message
            });
        } else {
            return res.send(result);
        }
    }); */
    res.send({
        "results": {
            "count": 3
        },
        "error": false,
        "message": ""
    });
}

/*
 Get the number of users which belong to the multi-crowd.
 Method: POST
 Input: selector conditions [Array] - which express the crowding condition that the client choose on UI
 */
exports.getGroupCrowdCountByPost = function(req, res) {
    if (!req.body["selector"]) {
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
    var selector = JSON.stringify(req.body);
    /*
     utility.post('getCrowdCount', '', selector, fucntion(err, result){
     if (err) {
     return res.send({
     message: err.message
     });
     } else {
     return res.send(result);
     }
     }); */
    res.send({
        "results": {
            "count": 3
        },
        "error": false,
        "message": ""
    });
}

/*
Get the number of users which belong to the crowd.
It can be called just when the crowd has been create on crowding service.
Method: GET
Input: cname - crowd name
 */
exports.getCrowdCountByGet = function(req, res) {
    var crowdName = req.query["cname"];
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

/*
create a crowd on crowding service
Input: 1. selector conditions - which express the crowding condition that the client choose on UI
       2. crowd name
 */
exports.createCrowd = function(req, res) {
    if (!req.body["selector"]) {
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
    var selector = JSON.stringify(req.body);
    /*
     utility.post('createCrowd', '', selector, function(err, result) {
        if (err) {
            return res.send({
                message: err.message
            });
        } else {
            return res.send(result);
        }
    });*/
    res.send({
        "results": {
            "count": 10
        },
        "error": false,
        "message": ""
    });
}

/*
Method: POST
Input: 1. selector conditions - which express the crowding condition that the client choose on UI
       2. count - the number of persons that the client want to seee
Output: Sample users in the crowd, the number of returned users' size equals to 'count'
 */
exports.crowdSampleByPost = function(req, res) {
    var selector = JSON.stringify(req.body);
    var count = req.query["count"];

    if (!req.body["selector"] || !count) {
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
    var crowdName = req.query["cname"];
    var count = req.query["count"];
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
        "message": ""
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
exports.deleteCrowd = function(req, res) {
    var crowdName = req.query["cname"];
    if (!crowdName) {
        return res.send({
            error: true,
            message: "Need Parameters 'cname'"
        });
    }
    /*
     utility.get('deleteCrowd', 'crowdName=' + crowdName, function(err, result) {
        if (err) {
            return res.send({
                message: err.message
            });
        } else {
            return res.send(result);
        }
    });
    */
    res.send('{"results": {"count": 10}, "error": false, "message": ""}');
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
    SemanticMetaData.findOne({name: "SemanticMetaData"}, function(err, md) {
        if (err) {
            return res.send({
                error: true,
                message: utility.getErrorMessage(err)
            });
        } else if (md && (curTime - md["created"].getTime() < 10000)) {
            console.log("time delta = " + (curTime - md["created"].getTime()));
            return res.send(md["profile"]);
        } else {
            /*
            utility.get('get_profile',
                '',
                function(err, result) {
                    if (err) {
                        return res.status(400).send({
                            message: err.message
                        });
                    } else {
                        return res.json(result);
                    }
                }
            );
            */
            var result = {
                "error": false,
                "message": "",
                "results": {
                    "ontology": [
                        {"name": "demo",
                            "tree": [
                                {"parent": "demo", "children": ["gender", "age"]},
                                {"parent": "gender", "children": ["male", "female"]},
                                {"parent": "age", "children": ["0~10", "10~20"]}
                            ]
                        },
                        {"name": "interest",
                            "tree": [
                                {"parent": "interest", "children": ["food", "book"]},
                                {"parent": "food", "children": ["apple", "orange"]},
                                {"parent": "book", "children": ["c++", "web development"]}
                            ]
                        },
                        {"name": "pic_type",
                            "tree": [
                                {"parent": "pic_type", "children": ["flower", "food"]},
                                {"parent": "food", "children": ["apple", "orange"]}
                            ]
                        },
                        {"name": "pic_tag",
                            "tree": [
                                {"parent": "pic_tag", "children": ["flower2", "food2"]},
                                {"parent": "food", "children": ["apple2", "orange2"]}
                            ]
                        },
                        {"name": "channel_tag",
                            "tree": [
                                {"parent": "channel_tag", "children": ["china", "USA"]},
                                {"parent": "china", "children": ["cctv1", "cctv2"]},
                                {"parent": "USA", "children": ["youtube"]}
                            ]
                        },
                        {"name": "lol",

                        }

                    ],

                    "object_ontology": [
                        {"object": "pic", "ontology": [{"name": "pic_type", "etype": "pic_to_pic_type"}, {"name":"pic_tag", "etype": "pic_to_pic_tag"}] },
                        {"object": "user", "ontology": [{"name": "demo", "etype": "user_to_demo"}, {"name": "interest", "etype": "user_to_interest"}] },
                        {"object": "theme", "ontology": [{"name": "pic_tag", "etype": "theme_to_pic_tag"}] },
                        {"object": "channel", "ontology": [{"name": "channel_tag", "etype": "channel_to_channel_tag"}] }
                    ],
                    "behaviour": [
                        {"name": "browse", "subject": [{"vtype": "user", "etype": "user_browse"}],
                            "object": [{"vtype": "theme", "etype": "browse_theme"},{"vtype": "pic", "etype": "post_pic"}]},
                        {"name": "post", "subject": [{"vtype": "user", "etype": "user_post"}],
                            "object": [{"vtype": "pic", "etype": "post_pic"}]},
                        {"name": "visit", "subject": [{"vtype": "user", "etype": "user_visit"}],
                            "object": [{"vtype": "channel", "etype": "visit_channel"}]},
                        {"name": "login", "subject": [{"vtype": "user", "etype": "user_login"}]}
                    ]
                }
            };
            SemanticMetaData.remove(function(err) {
                if (err) {
                    return res.send({
                        error: true,
                        message: utility.getErrorMessage(err)
                    });
                } else {
                    var metadata = new SemanticMetaData({name: "SemanticMetaData", profile: result["results"]});
                    metadata.save(function(err) {
                        if (err) {
                            return res.send({
                                error: true,
                                message: utility.getErrorMessage(err)
                            });
                        } else {
                            return res.json(metadata["profile"]);
                        }
                    });
                }
            });
        }
    });
}
