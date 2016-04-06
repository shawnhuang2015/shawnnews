'use strict';

var express = require('express');
var controller = require('./single_crowd.controller');

var router = express.Router();

router.get('/', controller.index);
router.get('/count', controller.count);
router.get('/:id', controller.show);
router.post('/', controller.create);
router.put('/:id', controller.update);
router.patch('/:id', controller.update);
router.delete('/:id', controller.destroy);
router.get('/:id/sample', controller.sample);
router.post('/user_list', controller.userList);
router.post('/user_count', controller.userCount);

module.exports = router;
