'use strict';

const sheets      = require('./sheetsService');
const { todayISO } = require('../utils/helpers');

/**
 * Build full analytics summary from all call logs.
 * Returns chart-ready JSON.
 */
async function buildAnalytics() {
  const calls = await sheets.getCalls();

  // ── Aggregate totals ──────────────────────────────────────────────────────
  let completed = 0, missed = 0, failed = 0, totalDuration = 0;
  const dailyMap   = {};   // 'YYYY-MM-DD' → count
  const weeklyMap  = {};   // 'YYYY-Www'   → count
  const monthlyMap = {};   // 'YYYY-MM'    → count

  calls.forEach(c => {
    const ts = c.timestamp || c.Timestamp || new Date().toISOString();
    const d  = new Date(ts);
    const day    = ts.slice(0, 10);
    const week   = `${d.getFullYear()}-W${String(getWeekNum(d)).padStart(2,'0')}`;
    const month  = ts.slice(0, 7);

    dailyMap[day]   = (dailyMap[day]   || 0) + 1;
    weeklyMap[week] = (weeklyMap[week] || 0) + 1;
    monthlyMap[month]=(monthlyMap[month]|| 0) + 1;

    const st = (c.status || '').toUpperCase();
    if (st === 'COMPLETED')  { completed++; totalDuration += Number(c.duration || 0); }
    else if (st === 'MISSED')  missed++;
    else if (st === 'FAILED')  failed++;
  });

  const total       = calls.length;
  const avgDuration = completed > 0 ? Math.round(totalDuration / completed) : 0;

  // ── Build sorted arrays for charts ───────────────────────────────────────
  const sortedDays   = Object.entries(dailyMap).sort(([a],[b]) => a.localeCompare(b)).slice(-30);
  const sortedWeeks  = Object.entries(weeklyMap).sort(([a],[b]) => a.localeCompare(b)).slice(-12);
  const sortedMonths = Object.entries(monthlyMap).sort(([a],[b]) => a.localeCompare(b)).slice(-12);

  // Pie data
  const pie = [
    { label: 'Completed', value: completed, color: '#00e676' },
    { label: 'Missed',    value: missed,    color: '#ffd740' },
    { label: 'Failed',    value: failed,    color: '#ff5252' },
  ];

  return {
    summary: { total, completed, missed, failed, avgDuration },
    daily:   { labels: sortedDays.map(([d]) => d),   data: sortedDays.map(([,v]) => v) },
    weekly:  { labels: sortedWeeks.map(([w]) => w),  data: sortedWeeks.map(([,v]) => v) },
    monthly: { labels: sortedMonths.map(([m]) => m), data: sortedMonths.map(([,v]) => v) },
    pie,
    generatedAt: new Date().toISOString(),
  };
}

function getWeekNum(d) {
  const oneJan = new Date(d.getFullYear(), 0, 1);
  return Math.ceil((((d - oneJan) / 86400000) + oneJan.getDay() + 1) / 7);
}

module.exports = { buildAnalytics };
