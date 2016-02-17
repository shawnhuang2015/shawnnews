/**
 * Created by xunyang on 2/2/16.
 */
var http = require('http');
var qs = require('querystring');
var config = require('meanio').loadConfig();

function get(endpoint, queryStr, cb) {
    //var content = qs.stringify(queryStr);
    var result = '';

    var options = {
        hostname: config.remoteServer.platform.host,
        port: config.remoteServer.platform.port,
        path: '/' + endpoint + '?' + queryStr,
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

    var options = {
        hostname: config.remoteServer.platform.host,
        port: config.remoteServer.platform.port,
        path: '/' + endpoint + '?' + queryStr,
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

exports.createCrowd = function(crowdName) {

};

exports.readCrowd = function(crowdName) {

};

exports.deleteCrowd = function(crowdName) {

};

/*
param: {"crowdCount": boolean, "sampleCount": int}
 */
exports.analyzeCrowd = function(crowdName, param) {

};

exports.readMetadata = function(req, res) {
    post('commonTransactions',
        'starttime=201601011700&endtime=201601011800',
        '[{"type": "Merchant","ids": ["M1", "M2"]},{"type": "Card","ids": ["C1", "C2"]}]',
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
};
