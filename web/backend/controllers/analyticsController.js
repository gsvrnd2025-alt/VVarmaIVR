'use strict';

const analyticsService = require('../services/analyticsService');

async function getAnalytics(req, res, next) {
  try {
    const data = await analyticsService.buildAnalytics();
    res.json({ ok: true, data });
  } catch (e) { next(e); }
}

module.exports = { getAnalytics };
