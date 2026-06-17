'use strict';

/** Format seconds into "02h 45m 12s" */
const formatUptime = (secs) => {
  const h = Math.floor(secs / 3600);
  const m = Math.floor((secs % 3600) / 60);
  const s = Math.floor(secs % 60);
  return `${String(h).padStart(2,'0')}h ${String(m).padStart(2,'0')}m ${String(s).padStart(2,'0')}s`;
};

/** Convert bytes to KB / MB string */
const formatBytes = (bytes) => {
  if (bytes >= 1048576) return `${(bytes / 1048576).toFixed(2)} MB`;
  if (bytes >= 1024)    return `${(bytes / 1024).toFixed(1)} KB`;
  return `${bytes} B`;
};

/** Clamp a value between min and max */
const clamp = (val, min, max) => Math.min(max, Math.max(min, val));

/** Get ISO date string for today: 'YYYY-MM-DD' */
const todayISO = () => new Date().toISOString().slice(0, 10);

/** Random int between min and max inclusive */
const randomInt = (min, max) => Math.floor(Math.random() * (max - min + 1)) + min;

/** Build a standardised "time ago" string */
const timeAgo = (isoString) => {
  const diff = Date.now() - new Date(isoString).getTime();
  const mins  = Math.floor(diff / 60000);
  const hours = Math.floor(diff / 3600000);
  if (mins < 1)    return 'Just now';
  if (mins < 60)   return `${mins} min${mins > 1 ? 's' : ''} ago`;
  if (hours < 24)  return `${hours} hour${hours > 1 ? 's' : ''} ago`;
  return new Date(isoString).toLocaleDateString();
};

/** Pad a number to 2 digits */
const pad = (n) => String(n).padStart(2, '0');

module.exports = { formatUptime, formatBytes, clamp, todayISO, randomInt, timeAgo, pad };
