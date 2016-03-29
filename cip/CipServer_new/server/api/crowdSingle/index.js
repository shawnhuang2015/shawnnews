'use strict';

var express = require('express');
var controller = require('./crowdSingle.controller');

var router = express.Router();

//db single crowd
router.get('/crowd', controller.list);
router.get('/crowdcount', controller.count);
router.get('/crowd/:crowdName', controller.read);
router.post('/crowd', controller.create);
router.put('/crowd/:crowdName', controller.update);
router.delete('/crowd/:crowdName', controller.delete);

//router.param('crowdName', crowdsingle.crowdByName);

module.exports = router;
