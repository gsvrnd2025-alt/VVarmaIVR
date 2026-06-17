'use strict';

require('dotenv').config();

const express      = require('express');
const http         = require('http');
const cors         = require('cors');
const morgan       = require('morgan');
const { Server }   = require('socket.io');

const config         = require('./config');
const logger         = require('./utils/logger');
const errorHandler   = require('./middleware/errorHandler');
const rateLimiter    = require('./middleware/rateLimiter');
const auth           = require('./middleware/auth');
const socketHandler  = require('./socket/socketHandler');

// ── Routes ──────────────────────────────────────────────────────────────────
const dashboardRoutes   = require('./routes/dashboard');
const apRoutes          = require('./routes/ap');
const staRoutes         = require('./routes/sta');
const connectivityRoutes= require('./routes/connectivity');
const eepromRoutes      = require('./routes/eeprom');
const callRoutes        = require('./routes/calls');
const smsRoutes         = require('./routes/sms');
const analyticsRoutes   = require('./routes/analytics');
const systemRoutes      = require('./routes/system');
const ivrRoutes         = require('./routes/ivr');
const deviceRoutes      = require('./routes/device');

// ── App Setup ────────────────────────────────────────────────────────────────
const app    = express();
const server = http.createServer(app);

// Socket.IO
const io = new Server(server, {
  cors: {
    origin: config.socketCorsOrigin,
    methods: ['GET', 'POST'],
  },
});
app.set('io', io);    // make io accessible in controllers
socketHandler(io);

// ── Global Middleware ────────────────────────────────────────────────────────
app.use(cors({ origin: config.corsOrigin, credentials: true }));
app.use(express.json({ limit: '2mb' }));
app.use(express.urlencoded({ extended: true }));
app.use(morgan('combined', {
  stream: { write: msg => logger.http(msg.trim()) },
}));
app.use(rateLimiter);

// Serve static frontend files from parent directory
const path = require('path');
app.use(express.static(path.join(__dirname, '../')));

// ── Health Check (no auth required) ─────────────────────────────────────────
app.get('/health', (req, res) => {
  res.json({
    status:  'OK',
    uptime:  process.uptime(),
    version: process.env.npm_package_version || '1.0.0',
    time:    new Date().toISOString(),
  });
});

// ── Authenticated API Routes ─────────────────────────────────────────────────
app.use('/api', auth);               // every /api/* requires X-API-Key
app.use('/api/dashboard',   dashboardRoutes);
app.use('/api/ap',          apRoutes);
app.use('/api/sta',         staRoutes);
app.use('/api/connectivity',connectivityRoutes);
app.use('/api/eeprom',      eepromRoutes);
app.use('/api/calls',       callRoutes);
app.use('/api/sms',         smsRoutes);
app.use('/api/analytics',   analyticsRoutes);
app.use('/api/system',      systemRoutes);
app.use('/api/ivr',         ivrRoutes);
app.use('/api/device',      deviceRoutes);

// ── 404 catch ────────────────────────────────────────────────────────────────
app.use((req, res) => {
  res.status(404).json({ error: 'Endpoint not found', path: req.path });
});

// ── Global Error Handler ─────────────────────────────────────────────────────
app.use(errorHandler);

// ── Start Server ─────────────────────────────────────────────────────────────
const PORT = config.port;
if (process.env.NODE_ENV !== 'test') {
  server.listen(PORT, () => {
    logger.info(`🚀 ESP32 IVR Backend running on http://localhost:${PORT}`);
    logger.info(`📡 Socket.IO ready  (CORS: ${config.socketCorsOrigin})`);
    logger.info(`🌐 Frontend CORS allowed for: ${config.corsOrigin}`);
    logger.info(`🔑 API Key auth: ${config.nodeEnv === 'production' ? 'ENABLED' : 'ENABLED (dev key)'}`);
  });
}

module.exports = { app, server, io };
