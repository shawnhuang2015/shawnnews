/**
 * Created by xunyang on 2/22/16.
 */
'use strict';
var http = require('http');
var qs = require('querystring');

exports.get = function(endpoint, queryStr, cb) {
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

exports.post = function(endpoint, queryStr, data, cb) {
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

exports.validateSelector = function(selector) {
    var ontology = selector["selector"]["ontology"];
    var behavior = selector["selector"]["behavior"];

    if (!ontology || !behavior) {
        console.log("behavior & ontology");
        return false;
    }

    for (var i = 0; i < ontology.length; ++i) {
        var item = ontology[i];
        if (!item["factor"] || !item["operator"] || !item["weight"] || !item["type"]) {
            console.log("in ontology");
            return false;
        }
    }

    for (var i = 0; i < behavior.length; ++i) {
        var item = behavior[i];
        if (!item["action"] || !item["objectType"] || !item["objectId"]
            || !item["operator"] || !item["value"] || !item["startTime"]
            || !item["endTime"] || !item["timeType"]) {
            console.log("in behavior");
            return false;
        }
    }

    return true;
}

exports.getErrorMessage = function(err) {
    if (err.errors) {
        for (var errName in err.errors) {
            if (err.errors[errName].message) return err.errors[errName].message;
        }
    } else {
        return "Unknown server error";
    }
};
