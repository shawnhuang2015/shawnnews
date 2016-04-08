'use strict';

var express = require('express');
var controller = require('./misc.controller');

var router = express.Router();

router.get('/download', controller.download);

module.exports = router;
