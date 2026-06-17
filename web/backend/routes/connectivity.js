'use strict';
const router = require('express').Router();
const ctrl   = require('../controllers/connectivityController');
router.get('/', ctrl.getConnectivity);
module.exports = router;
