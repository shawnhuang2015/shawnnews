'use strict';

var express = require('express');
var controller = require('./rest.controller');

var router = express.Router();

//call remote
router.get('/ontology', controller.readMetadata);
router.get('/crowd/sample', controller.crowdSampleByGet);
router.post('/crowd/detail', controller.getCrowdDetailByPost);
router.post('/crowd/count', controller.getCrowdCountByPost);
router.post('/group/crowd/detail', controller.getGroupCrowdDetailByPost);
router.post('/group/crowd/count', controller.getGroupCrowdCountByPost);

//download
router.get('/download', controller.download);

module.exports = router;
