'use strict';
const config = require('../config');

/**
 * API Key middleware.
 * Reads the X-API-Key header and compares it to the value in .env.
 * Returns 401 if missing, 403 if wrong.
 */
module.exports = (req, res, next) => {
  const key = req.headers['x-api-key'];
  if (!key) {
    return res.status(401).json({ error: 'Missing X-API-Key header' });
  }
  if (key !== config.apiKey) {
    return res.status(403).json({ error: 'Invalid API Key' });
  }
  next();
};
