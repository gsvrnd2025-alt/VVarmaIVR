'use strict';

const { v4: uuid }   = require('uuid');
const sheets         = require('../services/sheetsService');
const socketService  = require('../services/socketService');
const { toCsv, csvHeaders } = require('../utils/csvExporter');

async function getSms(req, res, next) {
  try {
    let sms = await sheets.getSms();

    const { type, search } = req.query;
    if (type)   sms = sms.filter(s => s.type?.toUpperCase() === type.toUpperCase());
    if (search) sms = sms.filter(s => s.sender?.includes(search) || s.message?.toLowerCase().includes(search.toLowerCase()));

    if (req.query.format === 'csv') {
      const csv = toCsv(sms);
      Object.entries(csvHeaders('sms_logs.csv')).forEach(([k,v]) => res.setHeader(k,v));
      return res.send(csv);
    }

    const page     = Math.max(1, parseInt(req.query.page  || '1'));
    const pageSize = Math.min(100, parseInt(req.query.pageSize || '10'));
    const total    = sms.length;
    const paged    = sms.slice((page-1)*pageSize, page*pageSize);

    res.json({ ok: true, data: paged, meta: { total, page, pageSize, totalPages: Math.ceil(total/pageSize) } });
  } catch (e) { next(e); }
}

async function addSms(req, res, next) {
  try {
    const sms = {
      id:        uuid(),
      sender:    req.body.sender,
      message:   req.body.message,
      type:      req.body.type    || 'RECEIVED',
      status:    req.body.status  || 'DELIVERED',
      timestamp: new Date().toISOString(),
    };
    await sheets.appendSms(sms);
    socketService.events.newSms(sms);
    res.status(201).json({ ok: true, message: 'SMS logged', data: sms });
  } catch (e) { next(e); }
}

async function deleteSms(req, res, next) {
  try {
    // Filter from mock (Sheets deletion handled in production by batchUpdate)
    sheets.MOCK.sms = sheets.MOCK.sms.filter(s => s.id !== req.params.id);
    res.json({ ok: true, message: `SMS ${req.params.id} deleted` });
  } catch (e) { next(e); }
}

module.exports = { getSms, addSms, deleteSms };
