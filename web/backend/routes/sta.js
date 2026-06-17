'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/staController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.get('/status',                    ctrl.getStaStatus);
router.post('/connect',  validate(schemas.staConnect), ctrl.connectSta);
router.post('/disconnect',               ctrl.disconnectSta);
router.post('/save',     validate(schemas.staConnect), ctrl.saveSta);
router.get('/scan',                      ctrl.scanNetworks);

module.exports = router;
