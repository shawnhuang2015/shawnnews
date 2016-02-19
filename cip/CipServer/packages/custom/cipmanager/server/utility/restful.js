/**
 * Created by xunyang on 2/2/16.
 */
var http = require('http');
var qs = require('querystring');
var config = require('meanio').loadConfig();

function get(endpoint, queryStr, cb) {
    //var content = qs.stringify(queryStr);
    var result = '';
    var path = '/' + endpoint;
    if (queryStr != '') {
        path += '?' + queryStr;
    }

    var options = {
        hostname: config.remoteServer.platform.host,
        port: config.remoteServer.platform.port,
        path: path,
        method: 'GET'
    };

    var req = http.request(options, function (res) {
        res.on('data', function (chunk) {
            result += chunk;
            console.log(result);
        });
        res.on('end', function() {
            console.log(result);
            if (cb) {
                cb(null, result);
            }
        });
    });

    req.on('error', function (e) {
        console.log('problem with GET request: ' + e.message);
        if (cb) {
            cb(e, null);
        }
    });

    req.end();
}

function post(endpoint, queryStr, data, cb) {
    //var content = qs.stringify(queryStr);
    var result = '';
    var path = '/' + endpoint;
    if (queryStr != '') {
        path += '?' + queryStr;
    }
    var options = {
        hostname: config.remoteServer.platform.host,
        port: config.remoteServer.platform.port,
        path: path,
        method: 'POST',
        headers:{
            'Content-Type': 'application/json',
            'Content-Length': data.length
        }
    };

    var req = http.request(options, function (res) {
        res.on('data', function (chunk) {
            result += chunk;
        });
        res.on('end', function() {
            console.log(result);
            if (cb) {
                cb(null, result);
            }
        });
    });

    req.on('error', function (e) {
        console.log('problem with POST request' + e.message);
        if (cb) {
            cb(e, null);
        }
    });

    console.log('data: ' + data);
    req.write(data);
    req.end();
}

exports.getCrowdDetailByPost = function(req, res) {
    /*
    var cond = JSON.stringify(req.body);;
    post('getCrowd', '', cond, function(err, result) {
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
    /*
     var crowdName = req.query["name"];
     get('getCrowd', 'crowdName=' + crowdName, function(err, result) {
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
    /*
    var cond = JSON.stringify(req.body);
    post('getCrowdCount', '', cond, fucntion(err, result){
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
     /*
     var crowdName = req.query["name"];
     get('getCrowdCount', 'crowdName=' + crowdName, fucntion(err, result) {
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
    /*
    var cond = JSON.stringify(req.body);
    post('createCrowd', '', cond, function(err, result) {
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
    var cond = JSON.stringify(req.body);
    var count = req.query["count"];

    /*
    post('crowdSample', 'count=' + count, cond, function(err, result) {
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
    var crowdName = req.query["name"];
    var count = req.query["count"];

    /*
     get('crowdSample', 'crowdName=' + crowdName + '&count=' + count, function(err, result) {
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
    /*
    var crowdName = req.query("name");
    get('deleteCrowd', 'crowdName=' + crowdName, function(err, result) {
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

/*
param: {"crowdCount": boolean, "sampleCount": int}
 */
exports.analyzeCrowd = function(crowdName, param) {

}

exports.readMetadata = function(req, res) {
    /*
    get('get_profile',
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
    res.send({
        "error": false,
        "message": "",
        "results": {
            "ontology": [
                {"demo": [
                    {"demo": ["gender", "age"]},
                    {"gender": ["male", "female"]},
                    {"age": ["0~10", "10~20"]}
                ]
                },
                {"interest": [
                    {"interest": ["food", "book"]},
                    {"food": ["apple", "orange"]},
                    {"book": ["c++", "web development"]}
                ]
                },
                {"pic_type": [
                    {"pic_type": ["flower", "food"]},
                    {"food": ["apple", "orange"]}
                ]
                },
                {"pic_tag": [
                    {"pic_tag": ["flower", "food"]},
                    {"food": ["apple", "orange"]}
                ]
                },
                {"channel_tag": [
                    {"channel_tag": ["china", "USA"]},
                    {"china": ["cctv1", "cctv2"]},
                    {"USA": ["youtube"]}
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
                {"browse": {"subject": [{"vtype": "user", "etype": "user_browse"}],
                    "object": [{"vtype": "theme", "etype": "browse_theme"}]}},
                {"post": {"subject": [{"vtype": "user", "etype": "user_post"}],
                    "object": [{"vtype": "pic", "etype": "post_pic"}]}},
                {"visit": {"subject": [{"vtype": "user", "etype": "user_visit"}],
                    "object": [{"vtype": "channel", "etype": "visit_channel"}]}},
                {"login": {"subject": [{"vtype": "user", "etype": "user_login"}]}}
            ]
        }
    });
}
