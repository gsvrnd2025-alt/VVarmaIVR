'use strict';
const logger = require('../utils/logger');

/**
 * Global error handler — must be the LAST app.use() in server.js.
 * Converts any error thrown in route handlers into a consistent JSON response.
 */
// eslint-disable-next-line no-unused-vars
module.exports = (err, req, res, next) => {
  const status  = err.status || err.statusCode || 500;
  const message = err.message || 'Internal Server Error';

  logger.error(`[${status}] ${req.method} ${req.path} — ${message}`, {
    stack: err.stack,
    body:  req.body,
  });

  res.status(status).json({
    error:   message,
    path:    req.path,
    method:  req.method,
    ...(process.env.NODE_ENV !== 'production' && { stack: err.stack }),
  });
};
