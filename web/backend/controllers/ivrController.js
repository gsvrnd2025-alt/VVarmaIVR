'use strict';

const { v4: uuid }   = require('uuid');
const sheets         = require('../services/sheetsService');
const socketService  = require('../services/socketService');

async function getIvrLogs(req, res, next) {
  try {
    const logs = await sheets.getIvrLogs();
    res.json({ ok: true, data: logs });
  } catch (e) { next(e); }
}

async function initiateCall(req, res, next) {
  try {
    const { callerNumber, menuSelection } = req.body;
    const log = {
      id:            uuid(),
      callerNumber,
      menuSelection: menuSelection || '',
      callDuration:  0,
      callStatus:    'RINGING',
      timestamp:     new Date().toISOString(),
    };
    await sheets.appendIvrLog(log);
    socketService.events.newCall(log);
    res.status(201).json({ ok: true, message: `IVR call initiated to ${callerNumber}`, data: log });
  } catch (e) { next(e); }
}

async function hangupCall(req, res, next) {
  try {
    const { id, duration } = req.body;
    // Update log entry status
    const logs = await sheets.getIvrLogs();
    const target = logs.find(l => l.id === id);
    if (target) {
      target.callStatus   = 'COMPLETED';
      target.callDuration = duration || 0;
    }
    socketService.events.callEnd({ id, duration });
    res.json({ ok: true, message: `Call ${id} ended`, duration });
  } catch (e) { next(e); }
}

module.exports = { getIvrLogs, initiateCall, hangupCall };
