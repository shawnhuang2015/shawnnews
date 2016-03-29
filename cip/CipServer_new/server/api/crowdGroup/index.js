'use strict';

var express = require('express');
var controller = require('./crowdGroup.controller');

var router = express.Router();

//db crowd group
router.get('/crowd', controller.list);
router.get('/crowdcount', controller.count);
router.get('/crowd/:crowdGroupName', controller.read);
router.post('/crowd', controller.create);
router.put('/crowd/:crowdGroupName', controller.update);
router.delete('/crowd/:crowdGroupName', controller.delete);

//router.param('crowdGroupName', controller.crowdByName);

module.exports = router;
