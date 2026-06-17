'use strict';

const socketService = require('../services/socketService');
const sheets        = require('../services/sheetsService');
const logger        = require('../utils/logger');

module.exports = (io) => {
  socketService.init(io);

  io.on('connection', (socket) => {
    logger.info(`🔌 Socket connected: ${socket.id}`);

    // Send current dashboard state on connect
    sheets.getDashboard().then(data => {
      socket.emit('dashboard:update', { event: 'dashboard:update', payload: data, timestamp: new Date().toISOString() });
    }).catch(() => {});

    socket.on('disconnect', () => {
      logger.info(`🔌 Socket disconnected: ${socket.id}`);
    });

    // Allow client to request a manual refresh
    socket.on('request:dashboard', async () => {
      try {
        const data = await sheets.getDashboard();
        socket.emit('dashboard:update', { event: 'dashboard:update', payload: data, timestamp: new Date().toISOString() });
      } catch (e) { /* ignore */ }
    });
  });

  // Broadcast a simulated live signal update every 15 seconds
  setInterval(async () => {
    const rssi = -(Math.floor(Math.random() * 30) + 40);
    socketService.events.signalUpdate(`${rssi} dBm`);
  }, 15000);
};
