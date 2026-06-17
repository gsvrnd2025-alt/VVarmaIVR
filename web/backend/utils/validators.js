'use strict';
const Joi = require('joi');

const phoneNumber = Joi.string().pattern(/^\+?[0-9]{7,15}$/).messages({
  'string.pattern.base': 'Must be a valid phone number (+countrycode optional, 7–15 digits)',
});

module.exports = {
  // STA Connect / Save
  staConnect: Joi.object({
    ssid:     Joi.string().min(1).max(32).required(),
    password: Joi.string().min(0).max(63).allow('').required(),
  }),

  // AP Config update
  apConfig: Joi.object({
    ssid:       Joi.string().min(1).max(32).required(),
    password:   Joi.string().min(8).max(63).required(),
    channel:    Joi.number().integer().min(1).max(13).default(6),
    maxClients: Joi.number().integer().min(1).max(8).default(4),
  }),

  // EEPROM save
  eepromSave: Joi.object({
    key:   Joi.string().min(1).max(64).required(),
    value: Joi.string().allow('').max(256).required(),
  }),

  // Post a call log
  callLog: Joi.object({
    callerNumber: phoneNumber.required(),
    direction:    Joi.string().valid('INBOUND','OUTBOUND').required(),
    status:       Joi.string().valid('COMPLETED','MISSED','FAILED','BUSY').required(),
    duration:     Joi.number().integer().min(0).required(),
    menuSelection:Joi.string().allow('').max(16).default(''),
  }),

  // Post an SMS
  smsLog: Joi.object({
    sender:  phoneNumber.required(),
    message: Joi.string().min(1).max(160).required(),
    type:    Joi.string().valid('RECEIVED','SENT','AUTO_REPLY').default('RECEIVED'),
    status:  Joi.string().valid('DELIVERED','PENDING','FAILED').default('DELIVERED'),
  }),

  // IVR call
  ivrCall: Joi.object({
    callerNumber:  phoneNumber.required(),
    menuSelection: Joi.string().allow('').max(16).default(''),
  }),

  // Device update payload (from ESP32)
  deviceUpdate: Joi.object({
    firmware:    Joi.string().optional(),
    heapFree:    Joi.number().optional(),
    heapTotal:   Joi.number().optional(),
    cpuLoad:     Joi.number().min(0).max(100).optional(),
    flashUsed:   Joi.number().optional(),
    flashCapacity:Joi.number().optional(),
    temperature: Joi.number().optional(),
    uptime:      Joi.number().optional(),
  }),
};
