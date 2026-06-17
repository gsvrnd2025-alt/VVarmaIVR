'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/apController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.get('/status',        ctrl.getApStatus);
router.post('/start',        ctrl.startAp);
router.post('/stop',         ctrl.stopAp);
router.post('/restart',      ctrl.restartAp);
router.get('/clients',       ctrl.getApClients);
router.get('/config',        ctrl.getApConfig);
router.post('/config',       validate(schemas.apConfig), ctrl.updateApConfig);

module.exports = router;
