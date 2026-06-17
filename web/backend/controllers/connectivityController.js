'use strict';

const sheets = require('../services/sheetsService');

async function getConnectivity(req, res, next) {
  try {
    const sta = await sheets.getStaConfig();
    const isConnected = sta.status === 'CONNECTED';
    res.json({
      ok: true,
      data: {
        internet:       isConnected,
        internetAvailable: isConnected,
        esp32Reachable: isConnected,
        wifiConnected:  isConnected,
        apRunning:      sheets.MOCK.apConfig.status === 'Running',
        latency:        isConnected ? `${Math.floor(Math.random()*15)+3}ms` : null,
        packetLoss:     '0%',
        dns:            sta.dns || '8.8.8.8',
        pingGateway:    isConnected ? parseFloat((2.5 + Math.random()*2.5).toFixed(1)) : null,
        pingInternet:   isConnected ? parseFloat((8.0 + Math.random()*10).toFixed(1)) : null,
        checkedAt:      new Date().toISOString(),
      },
    });
  } catch (e) { next(e); }
}

module.exports = { getConnectivity };
