/**
 * Created by xunyang on 2/2/16.
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

exports.getCrowdDetailByPost = function(req, res) {
    if (!req.body["selector"]) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelector(req.body)) {
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

exports.getCrowdCountByPost = function(req, res) {
    if (!req.body["selector"]) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelector(req.body)) {
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

exports.createCrowd = function(req, res) {
    if (!req.body["selector"]) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector'"
        });
    } else if (!utility.validateSelector(req.body)) {
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

exports.crowdSampleByPost = function(req, res) {
    var selector = JSON.stringify(req.body);
    var count = req.query["count"];

    if (!req.body["selector"] || !count) {
        return res.send({
            error: true,
            message: "Need Parameters 'selector', 'count'"
        });
    } else if (!utility.validateSelector(req.body)) {
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
                                {"parent": "pic_tag", "children": ["flower", "food"]},
                                {"parent": "food", "children": ["apple", "orange"]}
                            ]
                        },
                        {"name": "channel_tag",
                            "tree": [
                                {"parent": "channel_tag", "children": ["china", "USA"]},
                                {"parent": "china", "children": ["cctv1", "cctv2"]},
                                {"parent": "USA", "children": ["youtube"]}
                            ]
                        }
                    ],

                    "object_ontology": [
                        {"object": "pic", "ontologies": ["pic_type", "pic_tag"] },
                        {"object": "user", "ontologies": ["demo", "interest", "pic_type"] },
                        {"object": "theme", "ontologies": ["pic_tag"] },
                        {"object": "channel", "ontologies": ["channel_tag"] }
                    ],
                    "behaviour": [
                        {"name": "browse", "subject": [{"vtype": "user", "etype": "user_browse"}],
                            "object": [{"vtype": "theme", "etype": "browse_theme"}]},
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
