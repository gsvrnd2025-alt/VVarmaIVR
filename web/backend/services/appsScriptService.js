'use strict';

const config = require('../config');
const logger  = require('../utils/logger');

const SCRIPT_URL = config.appsScriptUrl ||
  'https://script.google.com/macros/s/AKfycbzkOue39iUgjHOLB-MPqJHx8_ApO2QIl50zSc7lVXzusphvuh9HMFy6zs_SpwDkCJ9cTg/exec';

// API key used by Code.gs for auth (params.key check)
const API_KEY = config.appsScriptApiKey || 'esp32_ivr_dev_key_change_in_production';

/**
 * Low-level fetch to the Apps Script web app.
 * NOTE: Action names MUST match Code.gs doGet() switch cases exactly.
 *   Code.gs actions: dashboard | calls | sms | ap | sta | eeprom | analytics | system | ivr
 */
async function callAppsScript(action, params = {}) {
  try {
    const { default: fetch } = await import('node-fetch');
    const qs = new URLSearchParams({ action, key: API_KEY, ...params }).toString();
    const url = `${SCRIPT_URL}?${qs}`;
    logger.info(`[AppsScript] GET ${action}`);
    const res = await fetch(url, { method: 'GET', redirect: 'follow', timeout: 12000 });
    if (!res.ok) {
      const text = await res.text();
      throw new Error(`HTTP ${res.status}: ${text.substring(0, 120)}`);
    }
    const data = await res.json();
    logger.info(`[AppsScript] ${action} OK`);
    return { ok: true, data };
  } catch (err) {
    logger.warn(`[AppsScript] ${action} FAILED: ${err.message}`);
    return { ok: false, error: err.message };
  }
}

// Actions match Code.gs doGet() switch cases: dashboard | calls | sms | ap | sta | eeprom | analytics | system | ivr
async function getDashboard()         { return callAppsScript('dashboard'); }
async function getCalls(params = {})  { return callAppsScript('calls', params); }
async function getSms()               { return callAppsScript('sms'); }
async function getAnalytics()         { return callAppsScript('analytics'); }
async function getAPConfig()          { return callAppsScript('ap'); }
async function getSTAConfig()         { return callAppsScript('sta'); }
async function getEEPROM()            { return callAppsScript('eeprom'); }
async function getSystem()            { return callAppsScript('system'); }
async function getIVRLogs()           { return callAppsScript('ivr'); }

module.exports = {
  callAppsScript,
  getDashboard,
  getCalls,
  getSms,
  getAnalytics,
  getAPConfig,
  getSTAConfig,
  getEEPROM,
  getSystem,
  getIVRLogs,
};
