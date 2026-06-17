'use strict';
const router = require('express').Router();
const ctrl   = require('../controllers/dashboardController');
router.get('/',  ctrl.getDashboard);
router.post('/', ctrl.updateDashboard);
module.exports = router;
