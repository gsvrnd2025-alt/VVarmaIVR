/**
 * ivr.js — Unified doPost entry point
 *
 * Routes incoming POST requests:
 *   • New structured VARMA actions (action field present) → doPost_varma()
 *   • Legacy ESP32 call-log format (timeStr / phoneNumber fields) → legacy IVR tab
 *
 * doGet is defined in ivr_varma.js (getVarmaData, validateUser, etc.)
 */
function doPost(e) {
  try {
    var body = {};
    try { body = JSON.parse(e.postData.contents || '{}'); } catch (_) { body = e.parameter || {}; }

    // Set request-scoped spreadsheet ID if passed in body or parameters
    var sid = body.sheetId || body.spreadsheetId || (e.parameter && (e.parameter.sheetId || e.parameter.spreadsheetId)) || "";
    if (sid) {
      REQUEST_SPREADSHEET_ID = sid.trim();
    }

    // Route structured VARMA actions to the VARMA handler
    if (body.action) {
      return doPost_varma(e);
    }

    // ── Legacy ESP32 call-log (no action field) ────────────────────────────
    var ss    = getVarmaSpreadsheet();
    var sheet = ss.getSheetByName('IVR');
    if (!sheet) sheet = ss.insertSheet('IVR');

    if (sheet.getLastRow() === 0) {
      sheet.appendRow(['Uptime', 'Phone Number', 'DTMF Input', 'Call Status', 'Sync Time']);
    }

    sheet.appendRow([
      body.timeStr     || '',
      body.phoneNumber || '',
      body.dtmfData    || '',
      body.callStatus  || '',
      new Date().toLocaleString('en-IN', { timeZone: 'Asia/Kolkata' })
    ]);

    // Also mirror into the new Call_Logs tab for VARMA dashboard
    var callsSheet = getVarmaSheet(VARMA_TABS.CALLS);
    if (callsSheet.getLastRow() === 0) {
      callsSheet.appendRow(['ID', 'CallerNumber', 'Direction', 'Status', 'Duration', 'MenuSelection', 'Timestamp']);
    }
    callsSheet.appendRow([
      '',
      body.phoneNumber || '',
      'Inbound',
      body.callStatus  || '',
      '',
      body.dtmfData    || '',
      new Date().toISOString()
    ]);

    return varmaCorsResponse({ status: 'success', message: 'Call logged to IVR + Call_Logs' });

  } catch (err) {
    return varmaCorsResponse({ status: 'error', message: err.message });
  }
}

function doOptions(e) {
  return ContentService.createTextOutput('')
    .setMimeType(ContentService.MimeType.TEXT);
}

// Helper: authorize and test connection
function authorizeMe() {
  var ss = getVarmaSpreadsheet();
  Logger.log('Authorized! Spreadsheet: ' + ss.getName());
}