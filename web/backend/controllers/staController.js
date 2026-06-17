'use strict';

const sheets        = require('../services/sheetsService');
const socketService = require('../services/socketService');
const { randomInt } = require('../utils/helpers');

// Simulated nearby networks (real ESP32 would POST scan results)
const NEARBY_NETWORKS = [
  { ssid: 'HomeRouter_5G',   rssi: -42, security: 'WPA2', channel: 6  },
  { ssid: 'AndroidAP',       rssi: -58, security: 'WPA2', channel: 1  },
  { ssid: 'JIONET_2.4G',     rssi: -65, security: 'WPA2', channel: 11 },
  { ssid: 'TP-Link_A3F2',    rssi: -71, security: 'WPA3', channel: 3  },
  { ssid: 'Airtel_Hotspot',  rssi: -79, security: 'WPA2', channel: 8  },
];

async function getStaStatus(req, res, next) {
  try {
    const sta = await sheets.getStaConfig();
    res.json({ ok: true, data: sta });
  } catch (e) { next(e); }
}

async function connectSta(req, res, next) {
  try {
    const { ssid, password } = req.body;
    const update = {
      ssid, password,
      ip:      `192.168.1.${randomInt(100, 220)}`,
      gateway: '192.168.1.1',
      dns:     '8.8.8.8',
      subnet:  '255.255.255.0',
      rssi:    `${-randomInt(40, 75)} dBm`,
      status:  'CONNECTED',
    };
    await sheets.saveStaConfig(update);
    socketService.events.deviceStatus({ staStatus: 'CONNECTED', ip: update.ip });
    socketService.events.dashboardUpdate({ wifiStatus: 'CONNECTED', ipAddress: update.ip });
    res.json({ ok: true, message: `Connected to "${ssid}"`, data: update });
  } catch (e) { next(e); }
}

async function disconnectSta(req, res, next) {
  try {
    const current = await sheets.getStaConfig();
    current.status = 'DISCONNECTED';
    current.ip     = '0.0.0.0';
    await sheets.saveStaConfig(current);
    socketService.events.deviceStatus({ staStatus: 'DISCONNECTED' });
    res.json({ ok: true, message: 'WiFi disconnected' });
  } catch (e) { next(e); }
}

async function saveSta(req, res, next) {
  try {
    const { ssid, password } = req.body;
    const current = await sheets.getStaConfig();
    await sheets.saveStaConfig({ ...current, ssid, password });
    res.json({ ok: true, message: 'Credentials saved to EEPROM' });
  } catch (e) { next(e); }
}

async function scanNetworks(req, res, next) {
  try {
    // In production: trigger ESP32 scan and wait for result via Sheets
    const networks = NEARBY_NETWORKS.map(n => ({
      ...n,
      rssi: n.rssi + randomInt(-5, 5),   // add jitter
    }));
    res.json({ ok: true, data: networks });
  } catch (e) { next(e); }
}

module.exports = { getStaStatus, connectSta, disconnectSta, saveSta, scanNetworks };
