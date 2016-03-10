'use strict';
var http = require('http');
var config = require('meanio').loadConfig();

function getBytes(string) {
    return Buffer.byteLength(string, 'utf8');
}

//http GET restful API
//Input: 1. endpoint
//       2. queryStr - the part after '?' in the url
//       3. callback function - the format looks like callback(error, result)
exports.get = function(endpoint, queryStr, cb) {
    var result = '';
    var path = '/' + endpoint;
    if (queryStr !== '') {
        path += '?' + queryStr;
    }

    console.log('GET request path: ' + path);
    var options = {
        hostname: config.remoteServer.crowdServer.host,
        port: config.remoteServer.crowdServer.port,
        path: path,
        method: 'GET'
    };

    var req = http.request(options, function (res) {
        res.on('data', function (chunk) {
            result += chunk;
        });
        res.on('end', function() {
            console.log('GET response: ' + result);
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

//http POST restful API
//Input: 1. endpoint
//       2. queryStr - the part after '?' in the url
//       3. data - data payload appear in the post data part
//       4. callback function - the format looks like callback(error, result)
exports.post = function(endpoint, queryStr, data, cb) {
    var result = '';
    var path = '/' + endpoint;
    if (queryStr !== '') {
        path += '?' + queryStr;
    }

    console.log('POST request path: ' + path + '\nbody = ' + data);
    var options = {
        hostname: config.remoteServer.crowdServer.host,
        port: config.remoteServer.crowdServer.port,
        path: path,
        method: 'POST',
        headers:{
            'Content-Type': 'application/json',
            'Content-Length': getBytes(data)
        }
    };

    var req = http.request(options, function (res) {
        res.on('data', function (chunk) {
            result += chunk;
        });
        res.on('end', function() {
            console.log('POST response: ' + result);
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

var validateCondition = function(selector) {
    var tag = selector.tag;
    var ontology = selector.ontology;
    var behavior = selector.behavior;

    if (tag === undefined || ontology === undefined || behavior === undefined) {
        console.log('Func validateCondition| miss tag, ontology or behavior');
        return false;
    }

    for (var i = 0; i < tag.length; ++i) {
        var item = tag[i];
        if (item.factor === undefined ||
            item.operator === undefined ||
            item.weight === undefined ||
            item.name === undefined) {
            console.log('Field absent in tag');
            return false;
        }
    }

    for (var i = 0; i < ontology.length; ++i) {
        var item = ontology[i];
        if (item.factor === undefined ||
            item.operator === undefined ||
            item.weight === undefined ||
            item.name === undefined) {
            console.log('Func validateCondition| miss fields in ontology');
            return false;
        }
    }

    for (var i = 0; i < behavior.length; ++i) {
        var item = behavior[i];
        if (item.objectType === undefined ||
            item.action === undefined ||
            item.operator === undefined ||
            item.value === undefined  ||
            item.timeType === undefined ||
            item.startTime === undefined ||
            item.endTime === undefined) {
            console.log('Func validateCondition| miss fields in hehavior');
            return false;
        }
        if ((item.objectType === 'Category' ||
            item.objectType === 'Contains') && (item.ontologyType === undefined ||
            item.objectId === undefined)  ||
            item.objectType === 'Item' && item.objectId === undefined)  {
            console.log('Func validateCondition| miss fields in hehavior');
            return false;
        }
    }

    return true;
}

//validate whether the crowd conditions are correct or not [Single]
exports.validateSelector = function(selector) {
    return validateCondition(selector);
}

//validate whether the crowd conditions are correct or not [Array]
exports.validateSelectorArray = function(selector) {
    if (selector instanceof Array) {
        for (var i = 0; i < selector.length; ++i) {
            if (!validateCondition(selector[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

exports.getErrorMessage = function(err) {
    if (err.errors) {
        for (var errName in err.errors) {
            if (err.errors[errName].message) return err.errors[errName].message;
        }
    } else {
        return 'Unknown server error';
    }
};
