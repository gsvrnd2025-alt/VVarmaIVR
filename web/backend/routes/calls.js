'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/callController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.get('/',          ctrl.getCalls);
router.post('/',         validate(schemas.callLog), ctrl.addCall);
router.delete('/:id',    ctrl.deleteCall);
router.delete('/',       ctrl.clearAllCalls);

module.exports = router;
