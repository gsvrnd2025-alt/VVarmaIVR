'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/smsController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.get('/',       ctrl.getSms);
router.post('/',      validate(schemas.smsLog), ctrl.addSms);
router.delete('/:id', ctrl.deleteSms);

module.exports = router;
