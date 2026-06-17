'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/ivrController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.get('/',           ctrl.getIvrLogs);
router.post('/call',      validate(schemas.ivrCall), ctrl.initiateCall);
router.post('/hangup',    ctrl.hangupCall);

module.exports = router;
