'use strict';

const { google }  = require('googleapis');
const path        = require('path');
const fs          = require('fs');
const config      = require('../config');
const sheetsCfg   = require('../config/sheets');
const logger      = require('../utils/logger');
const appsScript  = require('./appsScriptService');

// ── Auth ──────────────────────────────────────────────────────────────────────
let _sheetsClient = null;

async function getSheetsClient() {
  if (_sheetsClient) return _sheetsClient;

  const keyPath = path.resolve(config.serviceAccountKeyPath);
  if (!fs.existsSync(keyPath)) {
    logger.warn('⚠️  Google service account key not found — running in MOCK mode');
    return null;
  }

  const auth = new google.auth.GoogleAuth({
    keyFile: keyPath,
    scopes:  ['https://www.googleapis.com/auth/spreadsheets'],
  });
  _sheetsClient = google.sheets({ version: 'v4', auth });
  logger.info('✅ Google Sheets client initialised');
  return _sheetsClient;
}

// ── Mock Data (used when no service account is configured) ───────────────────
// ── Build rich mock calls (18 records) ───────────────────────────────────────
const _now = Date.now();
const _mockCalls = [
  { id:'c01', callerNumber:'+919876543210', direction:'INBOUND',  status:'COMPLETED', duration:65,  menuSelection:'1', timestamp: new Date(_now - 4*60000).toISOString() },
  { id:'c02', callerNumber:'+918765432109', direction:'INBOUND',  status:'MISSED',    duration:0,   menuSelection:'',  timestamp: new Date(_now - 12*60000).toISOString() },
  { id:'c03', callerNumber:'+917654321098', direction:'OUTBOUND', status:'COMPLETED', duration:120, menuSelection:'2', timestamp: new Date(_now - 22*60000).toISOString() },
  { id:'c04', callerNumber:'+916543210987', direction:'INBOUND',  status:'COMPLETED', duration:45,  menuSelection:'1', timestamp: new Date(_now - 35*60000).toISOString() },
  { id:'c05', callerNumber:'+915432109876', direction:'INBOUND',  status:'FAILED',    duration:0,   menuSelection:'',  timestamp: new Date(_now - 48*60000).toISOString() },
  { id:'c06', callerNumber:'+914321098765', direction:'INBOUND',  status:'COMPLETED', duration:88,  menuSelection:'3', timestamp: new Date(_now - 63*60000).toISOString() },
  { id:'c07', callerNumber:'+919876543210', direction:'INBOUND',  status:'MISSED',    duration:0,   menuSelection:'',  timestamp: new Date(_now - 80*60000).toISOString() },
  { id:'c08', callerNumber:'+918765432109', direction:'OUTBOUND', status:'COMPLETED', duration:200, menuSelection:'2', timestamp: new Date(_now - 95*60000).toISOString() },
  { id:'c09', callerNumber:'+917001234567', direction:'INBOUND',  status:'COMPLETED', duration:55,  menuSelection:'1', timestamp: new Date(_now - 110*60000).toISOString() },
  { id:'c10', callerNumber:'+916007654321', direction:'INBOUND',  status:'COMPLETED', duration:73,  menuSelection:'1', timestamp: new Date(_now - 130*60000).toISOString() },
  { id:'c11', callerNumber:'+919900112233', direction:'INBOUND',  status:'MISSED',    duration:0,   menuSelection:'',  timestamp: new Date(_now - 155*60000).toISOString() },
  { id:'c12', callerNumber:'+918811223344', direction:'INBOUND',  status:'COMPLETED', duration:140, menuSelection:'3', timestamp: new Date(_now - 175*60000).toISOString() },
  { id:'c13', callerNumber:'+917722334455', direction:'OUTBOUND', status:'COMPLETED', duration:95,  menuSelection:'2', timestamp: new Date(_now - 200*60000).toISOString() },
  { id:'c14', callerNumber:'+916633445566', direction:'INBOUND',  status:'FAILED',    duration:0,   menuSelection:'',  timestamp: new Date(_now - 225*60000).toISOString() },
  { id:'c15', callerNumber:'+915544556677', direction:'INBOUND',  status:'COMPLETED', duration:60,  menuSelection:'1', timestamp: new Date(_now - 255*60000).toISOString() },
  { id:'c16', callerNumber:'+914455667788', direction:'INBOUND',  status:'MISSED',    duration:0,   menuSelection:'',  timestamp: new Date(_now - 285*60000).toISOString() },
  { id:'c17', callerNumber:'+913366778899', direction:'INBOUND',  status:'COMPLETED', duration:110, menuSelection:'3', timestamp: new Date(_now - 315*60000).toISOString() },
  { id:'c18', callerNumber:'+912277889900', direction:'OUTBOUND', status:'COMPLETED', duration:180, menuSelection:'2', timestamp: new Date(_now - 360*60000).toISOString() },
];

const MOCK = {
  // totalCalls is always kept in sync with the calls array length
  get dashboard() {
    return { mode:'AP', deviceStatus:'RUNNING', wifiStatus:'CONNECTED', ipAddress:'192.168.4.1', connectedClients:2, signalStrength:'-52 dBm', totalCalls: _mockCalls.length, lastCaller: _mockCalls[0]?.callerNumber || '---', lastCallTime:'4 mins ago', uptime:'02h 45m 12s', updatedAt: new Date().toISOString() };
  },
  calls: _mockCalls,
  sms: [
    { id:'s1', sender:'+919876543210', message:'STATUS',              type:'RECEIVED', status:'DELIVERED', timestamp: new Date(_now-120000).toISOString() },
    { id:'s2', sender:'ESP32_IVR',     message:'ESP32 IVR: Status is Normal.', type:'SENT', status:'DELIVERED', timestamp: new Date(_now-119000).toISOString() },
  ],
  apConfig:   { ssid:'ESP32_IVR_AP', password:'esp32ivr2024', gateway:'192.168.4.1', maxClients:4, channel:6, security:'WPA2-PSK', status:'Running', updatedAt: new Date().toISOString() },
  staConfig:  { ssid:'HomeRouter', password:'', ip:'192.168.1.120', gateway:'192.168.1.1', dns:'8.8.8.8', subnet:'255.255.255.0', rssi:'-55 dBm', status:'CONNECTED', updatedAt: new Date().toISOString() },
  eeprom:     [
    { key:'sta_ssid',     value:'HomeRouter',   bytesUsed:10, writeCycles:3, lastUpdated: new Date().toISOString() },
    { key:'sta_password', value:'••••••••',      bytesUsed:8,  writeCycles:3, lastUpdated: new Date().toISOString() },
  ],
  system:     { firmware:'v1.2.0-beta', heapFree:145408, heapTotal:327680, cpuLoad:24, flashUsed:1594294, flashCapacity:4194304, temperature:42.5, uptime:9912 },
  ivr:        [],
};

// ── Helpers ──────────────────────────────────────────────────────────────────
const SPREADSHEET_ID = config.spreadsheetId;

async function readSheet(tab, range = null) {
  const client = await getSheetsClient();
  if (!client) return null;          // caller falls back to mock
  const r = range ? `${tab}!${range}` : tab;
  const res = await client.spreadsheets.values.get({ spreadsheetId: SPREADSHEET_ID, range: r });
  return res.data.values || [];
}

async function appendRow(tab, values) {
  const client = await getSheetsClient();
  if (!client) return null;
  await client.spreadsheets.values.append({
    spreadsheetId: SPREADSHEET_ID,
    range:         `${tab}!A1`,
    valueInputOption: 'USER_ENTERED',
    resource: { values: [values] },
  });
}

async function updateRow(tab, rowIndex, values) {
  const client = await getSheetsClient();
  if (!client) return null;
  await client.spreadsheets.values.update({
    spreadsheetId: SPREADSHEET_ID,
    range:         `${tab}!A${rowIndex}`,
    valueInputOption: 'USER_ENTERED',
    resource: { values: [values] },
  });
}

async function clearSheet(tab) {
  const client = await getSheetsClient();
  if (!client) return null;
  await client.spreadsheets.values.clear({ spreadsheetId: SPREADSHEET_ID, range: tab });
}

/** Convert raw row array → named object using column map */
function rowToObject(row, colMap) {
  const obj = {};
  Object.entries(colMap).forEach(([name, idx]) => {
    const key = name.toLowerCase().replace(/_([a-z])/g, (_, l) => l.toUpperCase());
    obj[key] = row[idx] ?? null;
  });
  return obj;
}

// ── Public API ───────────────────────────────────────────────────────────────

// Dashboard — try Apps Script first, then Sheets, then MOCK
async function getDashboard() {
  // 1. Try real Apps Script
  const scriptResult = await appsScript.getDashboard();
  if (scriptResult.ok && scriptResult.data) {
    const d = scriptResult.data;
    // Ensure totalCalls reflects actual call count if script doesn't provide
    if (d.totalCalls === undefined && Array.isArray(d.calls)) d.totalCalls = d.calls.length;
    return d;
  }
  // 2. Try Google Sheets direct
  const rows = await readSheet(sheetsCfg.tabs.DASHBOARD);
  if (rows) {
    const data = rows[1] || [];
    return rowToObject(data, sheetsCfg.columns.dashboard);
  }
  // 3. MOCK fallback
  logger.warn('[getDashboard] Falling back to MOCK data');
  return MOCK.dashboard;
}

async function updateDashboard(data) {
  const client = await getSheetsClient();
  if (!client) {
    // MOCK.dashboard is a getter derived from _mockCalls — no mutation needed in mock mode
    return;
  }
  await updateRow(sheetsCfg.tabs.DASHBOARD, 2, [...Object.values(data), new Date().toISOString()]);
}

// Calls — try Apps Script first, then Sheets, then MOCK
async function getCalls() {
  // 1. Try real Apps Script
  const scriptResult = await appsScript.getCalls();
  if (scriptResult.ok && Array.isArray(scriptResult.data)) {
    return scriptResult.data;
  }
  // 2. Try Google Sheets direct
  const rows = await readSheet(sheetsCfg.tabs.CALLS);
  if (rows) return rows.slice(1).map(r => rowToObject(r, sheetsCfg.columns.calls));
  // 3. MOCK fallback
  logger.warn('[getCalls] Falling back to MOCK data');
  return MOCK.calls;
}

async function appendCall(call) {
  await appendRow(sheetsCfg.tabs.CALLS, [
    call.id, call.callerNumber, call.direction, call.status,
    call.duration, call.menuSelection, call.timestamp,
  ]);
  MOCK.calls.push(call);
}

async function deleteCallById(id) {
  // In production: find row by ID and delete it via batchUpdate
  MOCK.calls = MOCK.calls.filter(c => c.id !== id);
  logger.info(`Call ${id} deleted (mock/sheet)`);
}

async function clearCalls() {
  const client = await getSheetsClient();
  if (!client) {
    MOCK.calls = [];
    return;
  }
  await client.spreadsheets.values.clear({ spreadsheetId: SPREADSHEET_ID, range: `${sheetsCfg.tabs.CALLS}!A2:Z` });
  MOCK.calls = [];
}

// SMS
async function getSms() {
  const rows = await readSheet(sheetsCfg.tabs.SMS);
  if (!rows) return MOCK.sms;
  return rows.slice(1).map(r => rowToObject(r, sheetsCfg.columns.sms));
}

async function appendSms(sms) {
  await appendRow(sheetsCfg.tabs.SMS, [
    sms.id, sms.sender, sms.message, sms.type, sms.status, sms.timestamp,
  ]);
  MOCK.sms.push(sms);
}

// AP Config
async function getApConfig() {
  const rows = await readSheet(sheetsCfg.tabs.AP_CONFIG);
  if (!rows) return MOCK.apConfig;
  return rowToObject(rows[1] || [], sheetsCfg.columns.ap);
}

async function saveApConfig(data) {
  const row = [data.ssid, data.password, data.gateway, data.maxClients, data.channel, data.security, data.status, new Date().toISOString()];
  await updateRow(sheetsCfg.tabs.AP_CONFIG, 2, row);
  Object.assign(MOCK.apConfig, data);
}

// STA Config
async function getStaConfig() {
  const rows = await readSheet(sheetsCfg.tabs.STA_CONFIG);
  if (!rows) return MOCK.staConfig;
  return rowToObject(rows[1] || [], sheetsCfg.columns.sta);
}

async function saveStaConfig(data) {
  const row = [data.ssid, data.password, data.ip, data.gateway, data.dns, data.subnet, data.rssi, data.status, new Date().toISOString()];
  await updateRow(sheetsCfg.tabs.STA_CONFIG, 2, row);
  Object.assign(MOCK.staConfig, data);
}

// EEPROM
async function getEeprom() {
  const rows = await readSheet(sheetsCfg.tabs.EEPROM);
  if (!rows) return MOCK.eeprom;
  return rows.slice(1).map(r => rowToObject(r, sheetsCfg.columns.eeprom));
}

async function saveEepromEntry(entry) {
  await appendRow(sheetsCfg.tabs.EEPROM, [entry.key, entry.value, entry.bytesUsed, entry.writeCycles, new Date().toISOString()]);
}

async function clearEeprom() {
  await clearSheet(sheetsCfg.tabs.EEPROM);
  MOCK.eeprom = [];
}

// Analytics
async function getAnalyticsRows() {
  const rows = await readSheet(sheetsCfg.tabs.ANALYTICS);
  if (!rows) return [];
  return rows.slice(1).map(r => rowToObject(r, sheetsCfg.columns.analytics));
}

async function appendAnalyticsRow(row) {
  await appendRow(sheetsCfg.tabs.ANALYTICS, [
    row.date, row.total, row.completed, row.missed, row.failed, row.avgDuration,
  ]);
}

// System Info
async function getSystemInfo() {
  const rows = await readSheet(sheetsCfg.tabs.SYSTEM);
  if (!rows) return MOCK.system;
  const obj = {};
  rows.slice(1).forEach(r => { if (r[0]) obj[r[0]] = r[1]; });
  return obj;
}

async function updateSystemInfo(data) {
  const client = await getSheetsClient();
  if (!client) { Object.assign(MOCK.system, data); return; }
  await clearSheet(sheetsCfg.tabs.SYSTEM);
  await appendRow(sheetsCfg.tabs.SYSTEM, ['Key','Value','Timestamp']);
  for (const [key, val] of Object.entries(data)) {
    await appendRow(sheetsCfg.tabs.SYSTEM, [key, val, new Date().toISOString()]);
  }
}

// IVR Logs
async function getIvrLogs() {
  const rows = await readSheet(sheetsCfg.tabs.IVR);
  if (!rows) return MOCK.ivr;
  return rows.slice(1).map(r => rowToObject(r, sheetsCfg.columns.ivr));
}

async function appendIvrLog(log) {
  await appendRow(sheetsCfg.tabs.IVR, [
    log.id, log.callerNumber, log.menuSelection, log.callDuration, log.callStatus, log.timestamp,
  ]);
  MOCK.ivr.push(log);
}

module.exports = {
  getDashboard, updateDashboard,
  getCalls, appendCall, deleteCallById, clearCalls,
  getSms, appendSms,
  getApConfig, saveApConfig,
  getStaConfig, saveStaConfig,
  getEeprom, saveEepromEntry, clearEeprom,
  getAnalyticsRows, appendAnalyticsRow,
  getSystemInfo, updateSystemInfo,
  getIvrLogs, appendIvrLog,
  MOCK,
};
