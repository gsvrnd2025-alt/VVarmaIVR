'use strict';

const sheets        = require('../services/sheetsService');
const { formatBytes, formatUptime } = require('../utils/helpers');

async function getSystemInfo(req, res, next) {
  try {
    const raw = await sheets.getSystemInfo();
    // Return both raw and formatted values
    const heapFree   = Number(raw.heapFree    || raw.HeapFree    || 145408);
    const heapTotal  = Number(raw.heapTotal   || raw.HeapTotal   || 327680);
    const flashUsed  = Number(raw.flashUsed   || raw.FlashUsed   || 1594294);
    const flashCap   = Number(raw.flashCapacity || raw.FlashCapacity || 4194304);
    const cpuLoad    = Number(raw.cpuLoad     || raw.CPULoad     || 24);
    const temp       = Number(raw.temperature || raw.Temperature || 42.5);
    const uptime     = Number(raw.uptime      || raw.Uptime      || 0);

    res.json({
      ok: true,
      data: {
        firmware:      raw.firmware || raw.FirmwareVersion || 'v1.2.0-beta',
        model:         raw.model || 'ESP32-WROOM-32E',
        heapFree:      formatBytes(heapFree),
        freeHeap:      heapFree,
        heapTotal:     formatBytes(heapTotal),
        heapUsedPct:   Math.round(((heapTotal - heapFree) / heapTotal) * 100),
        cpuLoad:       `${cpuLoad}%`,
        flashUsed:     formatBytes(flashUsed),
        flashCapacity: formatBytes(flashCap),
        flashUsedPct:  Math.round((flashUsed / flashCap) * 100),
        temperature:   `${temp.toFixed(1)} °C`,
        tasksCount:    Number(raw.tasksCount || 8),
        uptime:        formatUptime(uptime),
        sdkVersion:    raw.sdkVersion || '5.1.2',
        macSta:        raw.macSta || 'AA:BB:CC:DD:EE:FF',
        macAp:         raw.macAp  || 'AA:BB:CC:DD:EE:FE',
        chipRevision:  raw.chipRevision || '1',
        flashSize:     formatBytes(flashCap),
        retrievedAt:   new Date().toISOString(),
      },
    });
  } catch (e) { next(e); }
}

module.exports = { getSystemInfo };
