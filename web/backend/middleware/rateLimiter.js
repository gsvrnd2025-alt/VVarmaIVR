'use strict';
const rateLimit = require('express-rate-limit');
const config    = require('../config');

module.exports = rateLimit({
  windowMs: config.rateLimitWindowMs,   // default: 60 000 ms (1 minute)
  max:      config.rateLimitMax,        // default: 100 requests per window
  standardHeaders: true,
  legacyHeaders:   false,
  message: {
    error:   'Too many requests',
    retryIn: `${config.rateLimitWindowMs / 1000}s`,
  },
});
