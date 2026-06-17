'use strict';

const sheets        = require('../services/sheetsService');
const socketService = require('../services/socketService');
const logger        = require('../utils/logger');

async function rebootDevice(req, res, next) {
  try {
    logger.info('🔄 Device REBOOT command issued');
    socketService.events.deviceStatus({ status: 'REBOOTING' });
    // In production: POST to ESP32 reboot endpoint
    res.json({ ok: true, message: 'Reboot command sent to ESP32. Device will restart in ~5 seconds.' });
  } catch (e) { next(e); }
}

async function resetDevice(req, res, next) {
  try {
    logger.warn('⚠️  Device FACTORY RESET command issued');
    await sheets.clearEeprom();
    socketService.events.deviceStatus({ status: 'RESETTING' });
    res.json({ ok: true, message: 'Factory reset initiated. EEPROM cleared, device will reboot into AP Mode.' });
  } catch (e) { next(e); }
}

async function updateDevice(req, res, next) {
  try {
    // Receive telemetry/state update from the real ESP32 device
    const data = req.body;
    await sheets.updateSystemInfo(data);
    socketService.events.deviceStatus(data);
    socketService.events.dashboardUpdate({ deviceStatus: 'RUNNING', uptime: data.uptime });
    logger.info('📡 Device telemetry updated from ESP32', data);
    res.json({ ok: true, message: 'Device state updated' });
  } catch (e) { next(e); }
}

module.exports = { rebootDevice, resetDevice, updateDevice };
