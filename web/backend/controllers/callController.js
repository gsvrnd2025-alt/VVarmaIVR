'use strict';

const { v4: uuid }   = require('uuid');
const sheets         = require('../services/sheetsService');
const socketService  = require('../services/socketService');
const { toCsv, csvHeaders } = require('../utils/csvExporter');

async function getCalls(req, res, next) {
  try {
    let calls = await sheets.getCalls();

    // ── Filters ──────────────────────────────────────────────────────────────
    const { status, direction, search, from, to } = req.query;
    if (status)    calls = calls.filter(c => c.status?.toUpperCase()    === status.toUpperCase());
    if (direction) calls = calls.filter(c => c.direction?.toUpperCase() === direction.toUpperCase());
    if (search)    calls = calls.filter(c => c.callerNumber?.includes(search));
    if (from)      calls = calls.filter(c => new Date(c.timestamp) >= new Date(from));
    if (to)        calls = calls.filter(c => new Date(c.timestamp) <= new Date(to));

    // ── Sort ──────────────────────────────────────────────────────────────────
    const sort  = req.query.sort  || 'timestamp';
    const order = req.query.order || 'desc';
    calls.sort((a, b) => {
      const av = a[sort] || '', bv = b[sort] || '';
      return order === 'asc' ? av.localeCompare(bv) : bv.localeCompare(av);
    });

    // ── CSV export ────────────────────────────────────────────────────────────
    if (req.query.format === 'csv') {
      const csv = toCsv(calls);
      Object.entries(csvHeaders('call_logs.csv')).forEach(([k,v]) => res.setHeader(k,v));
      return res.send(csv);
    }

    // ── Pagination ────────────────────────────────────────────────────────────
    const page     = Math.max(1, parseInt(req.query.page  || '1'));
    const pageSize = Math.min(100, parseInt(req.query.pageSize || '10'));
    const total    = calls.length;
    const start    = (page - 1) * pageSize;
    const paged    = calls.slice(start, start + pageSize);

    res.json({ ok: true, data: paged, meta: { total, page, pageSize, totalPages: Math.ceil(total / pageSize) } });
  } catch (e) { next(e); }
}

async function addCall(req, res, next) {
  try {
    const call = {
      id:            uuid(),
      callerNumber:  req.body.callerNumber,
      direction:     req.body.direction,
      status:        req.body.status,
      duration:      req.body.duration,
      menuSelection: req.body.menuSelection || '',
      timestamp:     new Date().toISOString(),
    };
    await sheets.appendCall(call);
    socketService.events.newCall(call);
    res.status(201).json({ ok: true, message: 'Call logged', data: call });
  } catch (e) { next(e); }
}

async function deleteCall(req, res, next) {
  try {
    await sheets.deleteCallById(req.params.id);
    res.json({ ok: true, message: `Call ${req.params.id} deleted` });
  } catch (e) { next(e); }
}

async function clearAllCalls(req, res, next) {
  try {
    await sheets.clearCalls();
    res.json({ ok: true, message: 'All calls cleared successfully' });
  } catch (e) { next(e); }
}

module.exports = { getCalls, addCall, deleteCall, clearAllCalls };
