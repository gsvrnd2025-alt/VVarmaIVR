'use strict';

const sheets = require('../services/sheetsService');

async function getEeprom(req, res, next) {
  try {
    const entries = await sheets.getEeprom();
    const bytesUsed   = entries.reduce((s, e) => s + Number(e.bytesUsed || 0), 0);
    const writeCycles = entries.reduce((s, e) => s + Number(e.writeCycles || 0), 0);
    res.json({ ok: true, data: { entries, bytesUsed, bytesCapacity: 512, writeCycles } });
  } catch (e) { next(e); }
}

async function saveEeprom(req, res, next) {
  try {
    const { key, value } = req.body;
    const entry = { key, value, bytesUsed: Buffer.byteLength(value, 'utf8') + 4, writeCycles: 1, lastUpdated: new Date().toISOString() };
    await sheets.saveEepromEntry(entry);
    res.json({ ok: true, message: `"${key}" written to EEPROM`, data: entry });
  } catch (e) { next(e); }
}

async function loadEeprom(req, res, next) {
  try {
    const entries = await sheets.getEeprom();
    res.json({ ok: true, message: 'EEPROM loaded', data: entries });
  } catch (e) { next(e); }
}

async function clearEeprom(req, res, next) {
  try {
    await sheets.clearEeprom();
    res.json({ ok: true, message: 'EEPROM cleared' });
  } catch (e) { next(e); }
}

async function factoryReset(req, res, next) {
  try {
    await sheets.clearEeprom();
    await sheets.saveApConfig({ ssid: 'ESP32_IVR_AP', password: 'esp32ivr2024', gateway: '192.168.4.1', maxClients: 4, channel: 6, security: 'WPA2-PSK', status: 'Running' });
    res.json({ ok: true, message: 'Factory reset complete — EEPROM cleared and AP defaults restored' });
  } catch (e) { next(e); }
}

module.exports = { getEeprom, saveEeprom, loadEeprom, clearEeprom, factoryReset };
