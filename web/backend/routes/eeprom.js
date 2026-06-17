'use strict';
const router   = require('express').Router();
const ctrl     = require('../controllers/eepromController');
const validate = require('../middleware/validate');
const schemas  = require('../utils/validators');

router.get('/',            ctrl.getEeprom);
router.post('/save',       validate(schemas.eepromSave), ctrl.saveEeprom);
router.post('/load',       ctrl.loadEeprom);
router.post('/clear',      ctrl.clearEeprom);
router.post('/reset',      ctrl.factoryReset);
router.post('/factory-reset', ctrl.factoryReset);

module.exports = router;
