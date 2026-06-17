'use strict';

const sheets        = require('../services/sheetsService');
const socketService = require('../services/socketService');

async function getApStatus(req, res, next) {
  try {
    const cfg = await sheets.getApConfig();
    res.json({ ok: true, data: cfg });
  } catch (e) { next(e); }
}

async function startAp(req, res, next) {
  try {
    const cfg = await sheets.getApConfig();
    cfg.status = 'Running';
    await sheets.saveApConfig(cfg);
    socketService.events.deviceStatus({ apStatus: 'Running' });
    res.json({ ok: true, message: 'AP started', data: cfg });
  } catch (e) { next(e); }
}

async function stopAp(req, res, next) {
  try {
    const cfg = await sheets.getApConfig();
    cfg.status = 'Stopped';
    cfg.connectedClients = 0;
    await sheets.saveApConfig(cfg);
    socketService.events.apClients([]);
    socketService.events.deviceStatus({ apStatus: 'Stopped' });
    res.json({ ok: true, message: 'AP stopped', data: cfg });
  } catch (e) { next(e); }
}

async function restartAp(req, res, next) {
  try {
    const cfg = await sheets.getApConfig();
    cfg.status = 'Running';
    await sheets.saveApConfig(cfg);
    socketService.events.deviceStatus({ apStatus: 'Restarting' });
    res.json({ ok: true, message: 'AP restarted', data: cfg });
  } catch (e) { next(e); }
}

async function getApClients(req, res, next) {
  try {
    // In production: ESP32 POSTs client list; we return cached Sheets value
    const clients = sheets.MOCK.apConfig.clients || [];
    res.json({ ok: true, data: clients });
  } catch (e) { next(e); }
}

async function getApConfig(req, res, next) {
  try {
    const cfg = await sheets.getApConfig();
    res.json({ ok: true, data: cfg });
  } catch (e) { next(e); }
}

async function updateApConfig(req, res, next) {
  try {
    const update = { ...await sheets.getApConfig(), ...req.body };
    await sheets.saveApConfig(update);
    res.json({ ok: true, message: 'AP config saved', data: update });
  } catch (e) { next(e); }
}

module.exports = { getApStatus, startAp, stopAp, restartAp, getApClients, getApConfig, updateApConfig };
