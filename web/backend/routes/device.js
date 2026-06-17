'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/deviceController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.post('/reboot',         ctrl.rebootDevice);
router.post('/reset',          ctrl.resetDevice);
router.post('/update',         validate(schemas.deviceUpdate), ctrl.updateDevice);

module.exports = router;
