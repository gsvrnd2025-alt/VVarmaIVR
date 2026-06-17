'use strict';

const sheets         = require('../services/sheetsService');
const socketService  = require('../services/socketService');
const { timeAgo }    = require('../utils/helpers');

async function getDashboard(req, res, next) {
  try {
    const data = await sheets.getDashboard();
    socketService.events.dashboardUpdate(data);
    res.json({ ok: true, data });
  } catch (e) { next(e); }
}

async function updateDashboard(req, res, next) {
  try {
    const current = await sheets.getDashboard();
    const update = { ...current, ...req.body };
    await sheets.updateDashboard(update);
    socketService.events.dashboardUpdate(update);
    res.json({ ok: true, message: 'Dashboard updated', data: update });
  } catch (e) { next(e); }
}

module.exports = { getDashboard, updateDashboard };
