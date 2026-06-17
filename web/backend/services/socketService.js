'use strict';

let _io = null;

/** Call once from server.js to inject the Socket.IO instance */
function init(io) { _io = io; }

/** Broadcast a named event to ALL connected clients */
function broadcast(event, payload) {
  if (_io) _io.emit(event, { event, payload, timestamp: new Date().toISOString() });
}

// Convenience emitters for each domain event
const events = {
  newCall:         (call)    => broadcast('call:new',        call),
  callEnd:         (data)    => broadcast('call:end',        data),
  newSms:          (sms)     => broadcast('sms:new',         sms),
  deviceStatus:    (status)  => broadcast('device:status',   status),
  apClients:       (clients) => broadcast('ap:clients',      clients),
  signalUpdate:    (rssi)    => broadcast('signal:update',   { rssi }),
  dashboardUpdate: (data)    => broadcast('dashboard:update',data),
};

module.exports = { init, broadcast, events };
