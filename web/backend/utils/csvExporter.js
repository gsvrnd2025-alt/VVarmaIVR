'use strict';

/**
 * Convert an array of objects into a CSV string.
 * @param {object[]} rows    - Array of flat objects
 * @param {string[]} [fields] - Specific keys to include (defaults to all keys from first row)
 * @returns {string} CSV text
 */
function toCsv(rows, fields = null) {
  if (!rows || rows.length === 0) return '';
  const keys   = fields || Object.keys(rows[0]);
  const header = keys.join(',');
  const lines  = rows.map(row =>
    keys.map(k => {
      const val = row[k] ?? '';
      const str = String(val).replace(/"/g, '""');
      return str.includes(',') || str.includes('\n') || str.includes('"') ? `"${str}"` : str;
    }).join(',')
  );
  return [header, ...lines].join('\r\n');
}

/**
 * Return Express-compatible headers for a CSV download response.
 * @param {string} filename
 */
function csvHeaders(filename) {
  return {
    'Content-Type':        'text/csv; charset=utf-8',
    'Content-Disposition': `attachment; filename="${filename}"`,
  };
}

module.exports = { toCsv, csvHeaders };
