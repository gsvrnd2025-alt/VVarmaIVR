/**
 * V-VARMA IVR — Google Apps Script Backend
 * Script ID: 1JY8HDpGbMYZ0Dw5lWRpXgKj4aeZBZeH7HSRyADqwr6uAc9lyT3hXFBwr
 *
 * Handles:
 *   doGet  → getVarmaData | validateUser | get_calls | get_messages | get_mobiles
 *   doPost → call_log | sms_log | help_center_log | ivr_log | device_update
 *
 * Sheet tabs used:
 *   IVR          — legacy call logs (existing tab, preserved)
 *   Call_Logs    — full call records
 *   SMS_Logs     — SMS records
 *   Warranties   — warranty registrations
 *   Complaints   — complaint tickets
 *   Help_Center  — HC interaction log
 *   Registered_Mobiles — verified customer phones
 */

// ── Spreadsheet binding ────────────────────────────────────────────────────────
// Leave VARMA_SS_ID blank to use the bound spreadsheet (when deployed as Web App
// from within the spreadsheet). Set to a specific ID only if deploying standalone.
var VARMA_SS_ID = "";
var REQUEST_SPREADSHEET_ID = "";

var VARMA_TABS = {
  IVR:          "IVR",
  CALLS:        "Call_Logs",
  SMS:          "SMS_Logs",
  WARRANTIES:   "Warranties",
  COMPLAINTS:   "Complaints",
  HELP_CENTER:  "Help_Center",
  MOBILES:      "Registered_Mobiles",
  HC_NUMBERS:   "Help_Center_Numbers",
};

// ── Helpers ────────────────────────────────────────────────────────────────────
function getVarmaSpreadsheet() {
  var id = REQUEST_SPREADSHEET_ID || VARMA_SS_ID;
  return id
    ? SpreadsheetApp.openById(id)
    : SpreadsheetApp.getActiveSpreadsheet();
}

function getVarmaSheet(tabName) {
  var ss = getVarmaSpreadsheet();
  var sheet = ss.getSheetByName(tabName);
  if (!sheet) sheet = ss.insertSheet(tabName);
  return sheet;
}

function varmaSheetToObjects(sheet) {
  var data = sheet.getDataRange().getValues();
  if (data.length < 2) return [];
  var headers = data[0];
  return data.slice(1).map(function(row) {
    return headers.reduce(function(obj, h, i) {
      obj[String(h)] = row[i];
      return obj;
    }, {});
  });
}

function varmaAppendRow(tabName, values) {
  getVarmaSheet(tabName).appendRow(values);
}

function varmaCorsResponse(data) {
  return ContentService
    .createTextOutput(JSON.stringify(data))
    .setMimeType(ContentService.MimeType.JSON);
}

// ── Date filter ────────────────────────────────────────────────────────────────
function varmaFilterByDate(rows, filter, dateField, customDate) {
  if (!filter || filter === "all") return rows;
  var now   = new Date();
  var start = new Date();
  if (filter === "daily") {
    start.setHours(0, 0, 0, 0);
  } else if (filter === "weekly") {
    start.setDate(now.getDate() - 7);
  } else if (filter === "monthly") {
    start.setDate(1); start.setHours(0, 0, 0, 0);
  } else if (filter === "custom" && customDate) {
    var parts = customDate.split("-");
    if (parts.length === 2) {
      start = new Date(parseInt(parts[0]), parseInt(parts[1]) - 1, 1);
      var end = new Date(parseInt(parts[0]), parseInt(parts[1]), 0, 23, 59, 59);
      return rows.filter(function(r) {
        var d = new Date(r[dateField]);
        return !isNaN(d) && d >= start && d <= end;
      });
    } else {
      start = new Date(customDate);
      start.setHours(0, 0, 0, 0);
    }
  }
  return rows.filter(function(r) {
    var d = new Date(r[dateField]);
    return !isNaN(d) && d >= start;
  });
}

// ── doGet ──────────────────────────────────────────────────────────────────────
function doGet(e) {
  var params = e.parameter || {};
  var action = params.action || "";

  // Parse sheetId dynamically per request
  var sid = params.sheetId || params.spreadsheetId || "";
  if (sid) {
    REQUEST_SPREADSHEET_ID = sid.trim();
  }

  try {
    // ── VARMA Panel main data ────────────────────────────────────────────────
    if (action === "getVarmaData") {
      var filter     = params.filter || "daily";
      var customDate = params.date   || "";

      // Complaints stats
      var complaints = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.COMPLAINTS));
      complaints = varmaFilterByDate(complaints, filter, "Date", customDate);
      var total    = complaints.length;
      var assigned = complaints.filter(function(r) { return String(r["Status"] || "").toLowerCase() === "assigned"; }).length;
      var pending  = complaints.filter(function(r) { return String(r["Status"] || "").toLowerCase() === "pending";  }).length;

      // Warranties
      var warranties = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.WARRANTIES));
      warranties = varmaFilterByDate(warranties, filter, "Date", customDate);

      // Call logs
      var callRows = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.CALLS));
      callRows = varmaFilterByDate(callRows, filter, "Timestamp", customDate);
      var callsAttended = callRows.filter(function(r) { return String(r["Status"] || "").toUpperCase() === "COMPLETED"; }).length;
      var callsMissed   = callRows.filter(function(r) { return String(r["Status"] || "").toUpperCase() === "MISSED";    }).length;

      // SMS logs
      var smsRows = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.SMS));
      smsRows = varmaFilterByDate(smsRows, filter, "Timestamp", customDate);

      // Help Center log
      var helpRows = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.HELP_CENTER));
      helpRows = varmaFilterByDate(helpRows, filter, "Date", customDate);

      return varmaCorsResponse({
        ok: true,
        complaints: { total: total, assigned: assigned, pending: pending },
        stats: {
          warranty: warranties.length,
          calls:    callsAttended,
          missed:   callsMissed,
          sms:      smsRows.length,
        },
        warranties: warranties.map(function(r) {
          return { date: r["Date"]||"", phone: r["Phone"]||"", product: r["Product"]||"", serial: r["Serial"]||"", status: r["Status"]||"" };
        }),
        callHistories: callRows.map(function(r) {
          return { date: r["Timestamp"]||"", number: r["CallerNumber"]||"", type: r["Direction"]||"", option: r["MenuSelection"]||"" };
        }),
        smsDetails: smsRows.map(function(r) {
          return { date: r["Timestamp"]||"", recipient: r["Sender"]||"", message: r["Message"]||"", status: r["Status"]||"" };
        }),
        helpCenter: helpRows.map(function(r) {
          return { date: r["Date"]||"", direction: r["Direction"]||"", info: r["Info"]||"", details: r["Details"]||"" };
        }),
      });
    }

    // ── Validate caller (called by ESP32 on incoming call) ───────────────────
    if (action === "validateUser") {
      var phone   = (params.phone || "").replace(/\s/g, "");
      var mobiles = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.MOBILES));
      var match   = null;
      for (var i = 0; i < mobiles.length; i++) {
        var row = mobiles[i];
        var rp  = String(row["Phone"] || row["Mobile"] || "").replace(/\s/g, "");
        if (rp === phone) { match = row; break; }
      }
      if (match) {
        return varmaCorsResponse({ registered: true, name: match["Name"]||"", phone: phone });
      }
      return varmaCorsResponse({ registered: false, phone: phone });
    }

    // ── Raw call list for ESP32 firmware ─────────────────────────────────────
    if (action === "get_calls") {
      var rows = varmaSheetToObjects(getVarmaSheet(VARMA_TABS.CALLS));
      return varmaCorsResponse(rows.map(function(r) {
        return {
          timeStr:     r["Timestamp"]     || "",
          phoneNumber: r["CallerNumber"]  || "",
          dtmfData:    r["MenuSelection"] || "",
          callStatus:  r["Status"]        || "",
        };
      }));
    }

    // ── SMS list ──────────────────────────────────────────────────────────────
    if (action === "get_messages") {
      return varmaCorsResponse(varmaSheetToObjects(getVarmaSheet(VARMA_TABS.SMS)));
    }

    // ── Registered mobiles list ───────────────────────────────────────────────
    if (action === "get_mobiles") {
      return varmaCorsResponse(varmaSheetToObjects(getVarmaSheet(VARMA_TABS.MOBILES)));
    }

    // ── Help Center numbers list ───────────────────────────────────────────────
    if (action === "get_hc_numbers") {
      return varmaCorsResponse(varmaSheetToObjects(getVarmaSheet(VARMA_TABS.HC_NUMBERS)));
    }

    if (action === "save_hc_number") {
      var id = (params.id || "").trim();
      var number = (params.number || "").trim();
      var name = (params.name || "").trim();
      var role = (params.role || "").trim();
      var forwardEnabled = (params.forwardEnabled === "true" || params.forwardEnabled === "1") ? "true" : "false";
      var smsEnabled = (params.smsEnabled === "true" || params.smsEnabled === "1") ? "true" : "false";
      
      var sheet = getVarmaSheet(VARMA_TABS.HC_NUMBERS);
      var data = sheet.getDataRange().getValues();
      var foundIdx = -1;
      if (id) {
        for (var i = 1; i < data.length; i++) {
          if (String(data[i][0]).trim() === id) {
            foundIdx = i;
            break;
          }
        }
      }
      
      if (foundIdx !== -1) {
        // Edit existing
        sheet.getRange(foundIdx + 1, 1, 1, 6).setValues([[id, number, name, role, forwardEnabled, smsEnabled]]);
        return varmaCorsResponse({ ok: true, message: "Help Center number updated" });
      } else {
        // Append new
        var newId = id || String(Date.now());
        sheet.appendRow([newId, number, name, role, forwardEnabled, smsEnabled]);
        return varmaCorsResponse({ ok: true, message: "Help Center number added", id: newId });
      }
    }

    if (action === "delete_hc_number") {
      var id = (params.id || "").trim();
      var sheet = getVarmaSheet(VARMA_TABS.HC_NUMBERS);
      var data = sheet.getDataRange().getValues();
      for (var i = 1; i < data.length; i++) {
        if (String(data[i][0]).trim() === id) {
          sheet.deleteRow(i + 1);
          return varmaCorsResponse({ ok: true, message: "Help Center number deleted" });
        }
      }
      return varmaCorsResponse({ ok: false, error: "Number not found" });
    }

    // ── Health check ──────────────────────────────────────────────────────────
    if (action === "status") {
      return varmaCorsResponse({ ok: true, status: "online", time: new Date().toISOString() });
    }

    return varmaCorsResponse({ ok: false, error: "Unknown action: " + action });
  } catch (err) {
    return varmaCorsResponse({ ok: false, error: err.message });
  }
}

// ── doPost ─────────────────────────────────────────────────────────────────────
// NOTE: The original ivr.js doPost is still active. This function handles
//       the newer structured actions from the VARMA dashboard.
//       To avoid conflicts both functions can coexist (Apps Script merges files).
function doPost_varma(e) {
  var body = {};
  try { body = JSON.parse(e.postData.contents || "{}"); } catch (_) { body = e.parameter || {}; }

  // Set request-scoped spreadsheet ID if passed in body or parameters
  var sid = body.sheetId || body.spreadsheetId || (e.parameter && (e.parameter.sheetId || e.parameter.spreadsheetId)) || "";
  if (sid) {
    REQUEST_SPREADSHEET_ID = sid.trim();
  }

  var action = body.action || "";
  var data   = body.data   || body;

  try {
    switch (action) {

      case "call_log":
        varmaAppendRow(VARMA_TABS.CALLS, [
          data.id, data.callerNumber, data.direction, data.status,
          data.duration, data.menuSelection, data.timestamp,
        ]);
        return varmaCorsResponse({ ok: true, message: "Call logged" });

      case "sms_log":
        varmaAppendRow(VARMA_TABS.SMS, [
          data.id, data.sender, data.message, data.type, data.status, data.timestamp,
        ]);
        return varmaCorsResponse({ ok: true, message: "SMS logged" });

      case "help_center_log":
        varmaAppendRow(VARMA_TABS.HELP_CENTER, [
          data.date, data.direction, data.info, data.details,
        ]);
        return varmaCorsResponse({ ok: true, message: "HC entry logged" });

      case "ivr_log":
        varmaAppendRow(VARMA_TABS.IVR, [
          data.id, data.callerNumber, data.menuSelection,
          data.callDuration, data.callStatus, data.timestamp,
        ]);
        return varmaCorsResponse({ ok: true, message: "IVR log appended" });

      case "device_update": {
        var ts = new Date().toISOString();
        return varmaCorsResponse({ ok: true, message: "Device state noted", time: ts });
      }

      default:
        return varmaCorsResponse({ ok: false, error: "Unknown action: " + action });
    }
  } catch (err) {
    return varmaCorsResponse({ ok: false, error: err.message });
  }
}

// ── initVarmaSheets — run once to create VARMA-specific tabs ─────────────────
function initVarmaSheets() {
  var headers = {};
  headers[VARMA_TABS.CALLS]        = ["ID","CallerNumber","Direction","Status","Duration","MenuSelection","Timestamp"];
  headers[VARMA_TABS.SMS]          = ["ID","Sender","Message","Type","Status","Timestamp"];
  headers[VARMA_TABS.WARRANTIES]   = ["Date","Phone","Product","Serial","Status"];
  headers[VARMA_TABS.COMPLAINTS]   = ["Date","Phone","Name","Issue","Status","AssignedTo"];
  headers[VARMA_TABS.HELP_CENTER]  = ["Date","Direction","Info","Details"];
  headers[VARMA_TABS.MOBILES]      = ["Phone","Name","Email","Product","RegisteredOn"];
  headers[VARMA_TABS.HC_NUMBERS]   = ["ID","Number","Name","Role","ForwardEnabled","SmsEnabled"];

  Object.keys(headers).forEach(function(tabName) {
    var cols  = headers[tabName];
    var sheet = getVarmaSheet(tabName);
    if (sheet.getLastRow() === 0) {
      sheet.appendRow(cols);
      sheet.getRange(1, 1, 1, cols.length)
        .setFontWeight("bold")
        .setBackground("#1a1a2e")
        .setFontColor("#f39c12");
    }
  });

  SpreadsheetApp.getUi().alert("✅ VARMA IVR sheets initialised!");
}
