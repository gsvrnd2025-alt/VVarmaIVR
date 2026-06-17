'use strict';

module.exports = {
  port:            process.env.PORT              || 3000,
  nodeEnv:         process.env.NODE_ENV          || 'development',
  apiKey:          process.env.API_KEY            || 'esp32_ivr_dev_key_change_in_production',
  corsOrigin:      process.env.CORS_ORIGIN        || 'http://localhost:8000',
  socketCorsOrigin:process.env.SOCKET_CORS_ORIGIN || 'http://localhost:8000',
  spreadsheetId:   process.env.GOOGLE_SPREADSHEET_ID || '',
  serviceAccountKeyPath: process.env.GOOGLE_SERVICE_ACCOUNT_KEY_PATH || './config/service-account.json',
  appsScriptUrl:   process.env.APPS_SCRIPT_URL   || '',
  appsScriptApiKey:process.env.APPS_SCRIPT_API_KEY || '',
  rateLimitWindowMs: parseInt(process.env.RATE_LIMIT_WINDOW_MS || '60000'),
  rateLimitMax:      parseInt(process.env.RATE_LIMIT_MAX       || '100'),
  logLevel:          process.env.LOG_LEVEL        || 'info',
  logFile:           process.env.LOG_FILE         || './logs/app.log',
};
