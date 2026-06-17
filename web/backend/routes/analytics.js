'use strict';
const router = require('express').Router();
const ctrl   = require('../controllers/analyticsController');
router.get('/', ctrl.getAnalytics);
module.exports = router;
