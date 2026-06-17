/**
 * ESP32 IVR System Dashboard — Complete JavaScript Controller
 * ============================================================
 * Features:
 *   - SPA navigation router
 *   - Simulated REST API with fetch() wrappers (ready for real ESP32 endpoints)
 *   - Real-time uptime ticker
 *   - Connectivity auto-refresh (5 seconds)
 *   - Call history: search, filter, sort, paginate, export CSV
 *   - Chart.js analytics charts (daily, weekly, monthly, pie)
 *   - AP/STA/EEPROM interactive controls
 *   - Toast notification system
 *   - Responsive sidebar behaviour
 */

'use strict';

// ============================================================
// GLOBAL STATE
// ============================================================
const isRealESP32 = (window.location.port === '' || window.location.port === '80') && window.location.hostname !== 'localhost' && window.location.hostname !== '127.0.0.1' && window.location.protocol !== 'file:';
let currentSection = 'dashboard';
let systemUptimeSeconds = 86450; // Set a default starting uptime (1d 0h 0m 50s)
let connectivityCountdown = 5;
let callsData = [];
let filteredCalls = [];
let currentPage = 1;
const PAGE_SIZE = 10;
let chartDaily, chartWeekly, chartMonthly, chartRssiTrend;
let isScanningAp = false;
let isScanningSta = false;
let wsTimer = null;
let rssiDataPoints = [-58, -60, -62, -59, -58, -57, -58, -60, -61, -58];

// -- Real-Time Notifications State -------------------------------------------
let notificationsList = JSON.parse(localStorage.getItem('vvarma_notifications') || '[]');
let unreadNotificationsCount = parseInt(localStorage.getItem('vvarma_unread_notifications_count') || '0');
let _esp32WasOnline = null; // Track connectivity state for transition notifications

function addNotification(title, message, type = 'info') {
  const notif = {
    id: Date.now(),
    title,
    message,
    type,
    time: new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' })
  };
  notificationsList.unshift(notif);
  if (notificationsList.length > 30) {
    notificationsList = notificationsList.slice(0, 30);
  }
  unreadNotificationsCount++;
  localStorage.setItem('vvarma_notifications', JSON.stringify(notificationsList));
  localStorage.setItem('vvarma_unread_notifications_count', unreadNotificationsCount.toString());
  
  renderNotifications();
}

function renderNotifications() {
  const badge = document.getElementById('notification-count');
  if (badge) {
    if (unreadNotificationsCount > 0) {
      badge.textContent = unreadNotificationsCount > 99 ? '99+' : unreadNotificationsCount;
      badge.classList.remove('d-none');
    } else {
      badge.classList.add('d-none');
    }
  }

  // Update hint text in dropdown header
  const hint = document.getElementById('notif-read-hint');
  if (hint) {
    hint.textContent = unreadNotificationsCount > 0 ? `${unreadNotificationsCount} unread` : '';
  }

  const container = document.getElementById('notification-items-container');
  if (container) {
    if (notificationsList.length === 0) {
      container.innerHTML = '<p class="text-muted small text-center py-3 m-0">No new notifications</p>';
      return;
    }

    container.innerHTML = notificationsList.map(n => `
      <div class="notification-item type-${n.type}">
        <div class="notification-item-title">${n.title}</div>
        <div class="notification-item-desc">${n.message}</div>
        <div class="notification-item-time">${n.time}</div>
      </div>
    `).join('');
  }
}

// ── Help Center Numbers (persisted to localStorage) ─────────────────────────
let helpCenterNumbers = JSON.parse(localStorage.getItem('help_center_numbers') || '[]');
if (!helpCenterNumbers.length) {
  helpCenterNumbers = [
    { id: 1, name: 'GSV Support Line',    number: '+919876543210', role: 'L1 Support',   forwardEnabled: true,  smsEnabled: true  },
    { id: 2, name: 'Tech Dept Hotline',   number: '+919876543211', role: 'Technical',    forwardEnabled: true,  smsEnabled: true  },
    { id: 3, name: 'Sales Enquiries',     number: '+919876543212', role: 'Sales',        forwardEnabled: false, smsEnabled: true  },
  ];
  localStorage.setItem('help_center_numbers', JSON.stringify(helpCenterNumbers));
}

// -- Registered Customers (loaded live from Google Sheets) --------------------
// Populated by fetchRegisteredCustomers() via get_mobiles endpoint.
// NO mock/fake numbers — only your real Registered_Mobiles sheet data.
let registeredCustomers = JSON.parse(sessionStorage.getItem('registered_customers') || '[]');

async function fetchRegisteredCustomers() {
  try {
    const url = localStorage.getItem('sheets_script_url') || (typeof esp32State !== 'undefined' && esp32State.googleSheets?.scriptUrl) || '';
    if (!url && !isRealESP32) return;
    const fetchUrl = isRealESP32 ? '/get_mobiles' : `${url}?action=get_mobiles`;
    const res  = await fetch(fetchUrl);
    const data = await res.json();
    if (Array.isArray(data) && data.length) {
      registeredCustomers = data.map(r => ({
        phone:   String(r.Phone   || r.Mobile || ''),
        name:    String(r.Name    || ''),
        product: String(r.Product || ''),
        serial:  String(r.Serial  || ''),
        status:  String(r.Status  || ''),
      }));
      sessionStorage.setItem('registered_customers', JSON.stringify(registeredCustomers));
      console.log(`[Customers] Loaded ${registeredCustomers.length} registered numbers from Sheets`);
    }
  } catch (e) {
    console.warn('[Customers] Could not load registered numbers:', e.message);
  }
}

// -- Incoming Call Popup State ------------------------------------------------
let incomingCallTimer  = null;
let incomingCallActive = false;
let lastRingingNumber  = '';   // Prevent duplicate popups for same call

// Unified call counter synchronizer
function updateAllCallCounters() {
  const count = callsData.length;
  
  const dashEl = document.getElementById('dash-total-calls');
  if (dashEl) dashEl.textContent = count;
  
  const staEl = document.getElementById('sta-total-calls');
  if (staEl) staEl.textContent = count;
  
  const sumEl = document.getElementById('sum-calls');
  if (sumEl) sumEl.textContent = count;
  
  const navEl = document.getElementById('nav-call-count');
  if (navEl) {
    navEl.textContent = count;
    if (count > 0) {
      navEl.classList.remove('d-none');
    } else {
      navEl.classList.add('d-none');
    }
  }
}


// ============================================================
// MOCK ESP32 STATE DATABASE
// (Simulates the in-memory state of the hardware)
// ============================================================
const esp32State = {
  system: {
    model:         'V-VARMA IVR Controller',
    firmware:      '--',
    flashSize:     '--',
    chipRevision:  '--',
    macSta:        '--',
    macAp:         '--',
    freeHeap:      0,
    temperature:   0,
    tasksCount:    0,
    sdkVersion:    '--'
  },
  ap: {
    status:    'Stopped',    // 'Running' | 'Stopped'
    ssid:      '--',
    password:  '--',
    gateway:   '--',
    clients:   []
  },
  sta: {
    status:   'DISCONNECTED', // 'CONNECTED' | 'CONNECTING' | 'DISCONNECTED'
    ssid:     '--',
    password: '--',
    ip:       '--',
    rssi:     -110,
    subnet:   '--',
    gateway:  '--',
    dns:      '--'
  },
  connectivity: {
    internetAvailable: false,
    esp32Reachable:    false,
    pingGateway:       0,
    pingInternet:      0,
    httpStatus:        0
  },
  eeprom: {
    storedSsid:     '--',
    storedPassword: '--',
    credStatus:     '--',
    bytesUsed:      0,
    writeCycles:    0
  },
  sim: {
    status: 'INACTIVE',
    carrier: '--',
    rssi: -113,
    imei: '--',
    regStatus: '0',
    network: '--',
    simNumber: '--'
  },
  autoReply: {
    enabled: false,
    rules: []
  },
  googleSheets: {
    sheetId: localStorage.getItem('vvarma_sheet_id') || '--',
    sheetName: '--',
    scriptUrl: localStorage.getItem('sheets_script_url') || '',
    status: 'DISCONNECTED',
    lastSyncTime: '--',
    recordsUploaded: 0,
    errorsCount: 0,
    todayCount: 0,
    smsCount: 0,
    syncIncoming: true,
    syncOutgoing: true,
    syncSms: true,
    syncSystem: false,
    syncNetwork: false
  },
  networkMode: 'STA',
  callSystemActive: true,
  smsSystemActive: true
};

// ============================================================
// REAL REST API LAYER
// ============================================================
const BACKEND_URL = isRealESP32 ? window.location.origin : 'http://localhost:3000';
const API_KEY = 'esp32_ivr_dev_key_change_in_production';

let useMockApi = !isRealESP32;


async function espFetch(url, method = 'GET', data = null) {
  const options = {
    method,
    headers: { 'Accept': 'application/json' }
  };
  if (data && (method === 'POST' || method === 'PUT' || method === 'PATCH')) {
    options.headers['Content-Type'] = 'application/json';
    options.body = JSON.stringify(data);
  }
  try {
    const response = await fetch(url, options);
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
    return await response.json();
  } catch (err) {
    console.warn(`[API Connection Failed] URL: ${url}`, err);
    throw err;
  }
}

async function simulateApiCall(url, method = 'GET', data = null) {
  console.log(`[API Adapter] Intercepting ${method} ${url}`, data);
  try {
    if (url.startsWith('/api/eeprom')) {
      const res = await fetch('/factory_reset');
      return { ok: res.ok, data: null };
    } 
    else if (url.startsWith('/api/sta') && data && data.action === 'save') {
      const params = new URLSearchParams();
      params.append('ssid', data.ssid || '');
      params.append('pass', data.password || '');
      const res = await fetch('/save_net', { method: 'POST', body: params });
      return { ok: res.ok, data: null };
    }
    else if (url.startsWith('/sd_status')) {
      const res = await fetch('/sd_status');
      if (res.ok) return await res.json();
    }
    else if (url.startsWith('/sd_files')) {
      const res = await fetch('/sd_files');
      if (res.ok) return await res.json();
    }
    else if (url.startsWith('/get_ivr_config')) {
      const res = await fetch('/get_ivr_config');
      if (res.ok) return await res.json();
    }
    return { ok: true, data: null }; // Fallback for unsupported legacy UI actions
  } catch(e) {
    console.error(`[API Adapter Error]`, e);
    throw e;
  }
}

function parseUptimeToSeconds(str) {
  if (!str) return 0;
  let days = 0, hours = 0, minutes = 0, seconds = 0;
  const dayMatch = str.match(/(\d+)d/);
  if (dayMatch) days = parseInt(dayMatch[1]) || 0;
  const hourMatch = str.match(/(\d+)h/);
  if (hourMatch) hours = parseInt(hourMatch[1]) || 0;
  const minMatch = str.match(/(\d+)m/);
  if (minMatch) minutes = parseInt(minMatch[1]) || 0;
  const secMatch = str.match(/(\d+)s/);
  if (secMatch) seconds = parseInt(secMatch[1]) || 0;
  return days * 86400 + hours * 3600 + minutes * 60 + seconds;
}

// Helper: random integer between min and max (inclusive)
function randomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

// ============================================================
// ENTRY POINT
// ============================================================
// FIRST-RUN SETUP WIZARD CONTROLLER
// ============================================================
const froWizard = {
  /** Simulated nearby WiFi networks for the scan dropdown */
  mockNetworks: [
    { ssid: 'HomeRouter_5G',    rssi: -42, sec: 'WPA2', ch: 6  },
    { ssid: 'AndroidAP',        rssi: -58, sec: 'WPA2', ch: 1  },
    { ssid: 'JIONET_2.4G',      rssi: -65, sec: 'WPA2', ch: 11 },
    { ssid: 'TP-Link_A3F2',     rssi: -71, sec: 'WPA3', ch: 3  },
    { ssid: 'Airtel_Hotspot',   rssi: -79, sec: 'WPA2', ch: 8  },
    { ssid: 'Open_CafeWifi',    rssi: -83, sec: 'Open', ch: 2  },
  ],

  goToStep(n) {
    [1,2,3,4].forEach(i => {
      const card = document.getElementById(`fro-step-${i}`);
      if (card) card.classList.toggle('d-none', i !== n);

      const bar = document.getElementById(`fro-bar-${i}`);
      if (bar) {
        bar.classList.remove('active', 'done');
        if (i < n)  bar.classList.add('done');
        if (i === n) bar.classList.add('active');
      }
    });
    [1,2,3].forEach(i => {
      const line = document.getElementById(`fro-line-${i}-${i+1}`);
      if (line) line.classList.toggle('done', i < n);
    });
  },

  show() {
    const overlay = document.getElementById('first-run-overlay');
    if (overlay) overlay.classList.remove('fro-hidden');
    this.goToStep(1);
  },

  hide() {
    const overlay = document.getElementById('first-run-overlay');
    if (overlay) overlay.classList.add('fro-hidden');
  },

  passwordStrength(pw) {
    if (!pw) return { pct: 0, label: 'Password strength', color: '#ff5252' };
    let score = 0;
    if (pw.length >= 8)  score++;
    if (pw.length >= 12) score++;
    if (/[A-Z]/.test(pw))  score++;
    if (/[0-9]/.test(pw))  score++;
    if (/[^A-Za-z0-9]/.test(pw)) score++;
    const map = [
      { pct: 20,  label: 'Very Weak',  color: '#ff5252' },
      { pct: 40,  label: 'Weak',       color: '#ff5252' },
      { pct: 60,  label: 'Fair',       color: '#ffd740' },
      { pct: 80,  label: 'Strong',     color: '#69f0ae' },
      { pct: 100, label: 'Very Strong',color: '#00e676' },
    ];
    return map[Math.min(score, 4)];
  },

  addLog(msg, type = 'ok') {
    const log = document.getElementById('fro-connect-log');
    if (!log) return;
    const now = new Date();
    const t = `${String(now.getHours()).padStart(2,'0')}:${String(now.getMinutes()).padStart(2,'0')}:${String(now.getSeconds()).padStart(2,'0')}`;
    const div = document.createElement('div');
    div.className = 'fro-log-line';
    div.innerHTML = `<span class="fro-log-time">[${t}]</span><span class="fro-log-${type}">${msg}</span>`;
    log.appendChild(div);
    log.scrollTop = log.scrollHeight;
  },

  async runConnection(ssid, password) {
    this.goToStep(3);
    const statusEl = document.getElementById('fro-connect-status');
    const logEl    = document.getElementById('fro-connect-log');
    if (logEl) logEl.innerHTML = '';

    const steps = [
      [400,  'Saving credentials to EEPROM…',          'Credentials written to flash (offset 0x00)', 'ok'],
      [700,  'Verifying stored credentials…',           'EEPROM readback OK — checksum matches', 'ok'],
      [600,  'Stopping Access Point…',                  'AP interface shutdown — SSID broadcast off', 'ok'],
      [800,  `Connecting to "${ssid}"…`,               'Sending association request…', 'warn'],
      [1200, 'Waiting for DHCP lease…',                 'DHCP offer received from router', 'ok'],
      [600,  'Verifying internet connectivity…',        'Ping 8.8.8.8 → 12ms — OK', 'ok'],
      [400,  'Switching to STA Mode — complete!',       'ESP32 now in Station Mode ✓', 'ok'],
    ];

    for (const [delay, status, log, logType] of steps) {
      if (statusEl) statusEl.textContent = status;
      await new Promise(r => setTimeout(r, delay));
      this.addLog(log, logType);
    }

    // Simulate an IP address
    const ip = `192.168.1.${Math.floor(Math.random() * 100) + 100}`;

    // Sync state to backend
    try {
      await simulateApiCall('/api/sta', 'POST', { action: 'save', ssid, password });
      await simulateApiCall('/api/ap', 'POST', { action: 'stop' });
      await simulateApiCall('/api/sta', 'POST', { action: 'connect', ssid, password });
      await simulateApiCall('/api/dashboard', 'POST', { mode: 'STA', wifiStatus: 'CONNECTED', ipAddress: ip });
    } catch (apiErr) {
      console.error('Failed to save wizard settings to API:', apiErr);
    }

    // Update ESP32 state
    esp32State.networkMode        = 'STA';
    esp32State.sta.status         = 'CONNECTED';
    esp32State.sta.ssid           = ssid;
    esp32State.sta.password       = password;
    esp32State.sta.ip             = ip;
    esp32State.sta.rssi           = -randomInt(40, 65);
    esp32State.sta.gateway        = '192.168.1.1';
    esp32State.sta.dns            = '8.8.8.8';
    esp32State.ap.status          = 'Stopped';
    esp32State.ap.clients         = [];
    esp32State.connectivity.internetAvailable = true;
    esp32State.connectivity.esp32Reachable    = true;
    esp32State.eeprom.storedSsid     = ssid;
    esp32State.eeprom.storedPassword = password;
    esp32State.eeprom.credStatus     = 'VALID (Saved)';
    esp32State.eeprom.writeCycles++;

    // Persist flag
    localStorage.setItem('ivr_setup_done', '1');
    localStorage.setItem('ivr_sta_ssid',   ssid);

    // Populate step 4 results
    if (document.getElementById('fro-result-ssid'))  document.getElementById('fro-result-ssid').textContent  = ssid;
    if (document.getElementById('fro-result-ssid-2')) document.getElementById('fro-result-ssid-2').textContent = ssid;
    if (document.getElementById('fro-result-ip'))    document.getElementById('fro-result-ip').textContent    = ip;

    // Render all views with new STA state before showing success
    renderAllViews();
    updateSidebarDots();

    await new Promise(r => setTimeout(r, 300));
    this.goToStep(4);
  },

  init() {

    /* ─── Step 1 buttons ─── */
    document.getElementById('fro-btn-start')?.addEventListener('click', () => this.goToStep(2));
    document.getElementById('fro-btn-skip')?.addEventListener('click', () => {
      localStorage.setItem('ivr_setup_done', '1');
      this.hide();
    });

    /* ─── Step 2 — Scan ─── */
    document.getElementById('fro-btn-scan')?.addEventListener('click', async (e) => {
      e.currentTarget.innerHTML = '<i class="fa-solid fa-circle-notch fa-spin"></i>';
      await new Promise(r => setTimeout(r, 1000));
      e.currentTarget.innerHTML = '<i class="fa-solid fa-magnifying-glass"></i>';

      const listEl = document.getElementById('fro-networks-list');
      if (!listEl) return;
      listEl.innerHTML = '';
      this.mockNetworks.forEach(n => {
        const bars = n.rssi > -55 ? 4 : n.rssi > -65 ? 3 : n.rssi > -75 ? 2 : 1;
        const secIcon = n.sec === 'Open' ? '🔓' : '🔒';
        const item = document.createElement('div');
        item.className = 'fro-network-item';
        item.innerHTML = `
          <i class="fa-solid fa-wifi" style="color:${bars >= 3 ? '#00e676' : bars === 2 ? '#ffd740' : '#ff5252'}; font-size:0.9rem;"></i>
          <span class="fro-network-ssid">${n.ssid}</span>
          <span class="fro-network-meta">${secIcon} ${n.sec} &nbsp;·&nbsp; ch${n.ch} &nbsp;·&nbsp; ${n.rssi} dBm</span>`;
        item.addEventListener('click', () => {
          const inp = document.getElementById('fro-ssid');
          if (inp) { inp.value = n.ssid; inp.focus(); }
          listEl.classList.add('d-none');
        });
        listEl.appendChild(item);
      });
      listEl.classList.remove('d-none');
    });

    // Close dropdown on outside click
    document.addEventListener('click', e => {
      if (!e.target.closest('#fro-btn-scan') && !e.target.closest('#fro-networks-list')) {
        document.getElementById('fro-networks-list')?.classList.add('d-none');
      }
    });

    /* ─── Step 2 — Eye toggle ─── */
    document.getElementById('fro-btn-eye')?.addEventListener('click', () => {
      const inp = document.getElementById('fro-password');
      const ico = document.getElementById('fro-eye-icon');
      if (!inp) return;
      const isHidden = inp.type === 'password';
      inp.type = isHidden ? 'text' : 'password';
      if (ico) ico.className = `fa-solid fa-${isHidden ? 'eye-slash' : 'eye'}`;
    });

    /* ─── Step 2 — Password strength meter ─── */
    document.getElementById('fro-password')?.addEventListener('input', e => {
      const { pct, label, color } = this.passwordStrength(e.target.value);
      const fill = document.getElementById('fro-strength-fill');
      const lbl  = document.getElementById('fro-strength-label');
      if (fill) { fill.style.width = `${pct}%`; fill.style.background = color; }
      if (lbl)  lbl.textContent = label;
    });

    /* ─── Step 2 — Save & Connect ─── */
    document.getElementById('fro-btn-save-wifi')?.addEventListener('click', () => {
      const ssid  = document.getElementById('fro-ssid')?.value?.trim();
      const pass  = document.getElementById('fro-password')?.value;
      const ssidEl = document.getElementById('fro-ssid');
      const passEl = document.getElementById('fro-password');

      let hasError = false;
      if (!ssid) {
        ssidEl?.classList.add('fro-error', 'fro-shake');
        setTimeout(() => ssidEl?.classList.remove('fro-shake'), 500);
        hasError = true;
      } else {
        ssidEl?.classList.remove('fro-error');
      }
      if (!pass || pass.length < 1) {
        passEl?.classList.add('fro-error', 'fro-shake');
        setTimeout(() => passEl?.classList.remove('fro-shake'), 500);
        hasError = true;
      } else {
        passEl?.classList.remove('fro-error');
      }
      if (hasError) return;

      this.runConnection(ssid, pass);
    });

    /* ─── Step 2 — Back ─── */
    document.getElementById('fro-btn-back-1')?.addEventListener('click', () => this.goToStep(1));

    /* ─── Step 4 — Go to STA ─── */
    document.getElementById('fro-btn-go-sta')?.addEventListener('click', () => {
      this.hide();
      // Navigate to STA Mode section
      document.querySelector('.sidebar-link[data-target="sta-mode"]')?.click();
      showToast('Setup Complete', '🎉 ESP32 is now connected in STA Mode!', 'success');
    });

    // Auto-bypass: always mark setup as done (wizard is optional)
    localStorage.setItem('ivr_setup_done', '1');
    this.hide();
  }
};

// ============================================================
// ENTRY POINT
// ============================================================
function checkAuth() {
  const loggedIn = localStorage.getItem('ivr_logged_in') === '1' || localStorage.getItem('vvarma_auth_token') === 'AUTH_VALID';
  const overlay = document.getElementById('login-overlay');
  if (overlay) {
    if (loggedIn) {
      overlay.classList.add('d-none');
    } else {
      overlay.classList.remove('d-none');
    }
  }
}

document.addEventListener('DOMContentLoaded', () => {
  // Auto-seed Google Sheets Script URL into localStorage so VARMA Panel works immediately
  if (!localStorage.getItem('sheets_script_url')) {
    localStorage.setItem('sheets_script_url', esp32State.googleSheets.scriptUrl);
  }

  // Pre-populate Sheet settings from localStorage once on startup
  const idInput = document.getElementById('sheets-id');
  if (idInput) idInput.value = localStorage.getItem('vvarma_sheet_id') || '';
  const scriptInput = document.getElementById('sheets-script-url');
  if (scriptInput) scriptInput.value = localStorage.getItem('sheets_script_url') || '';

  checkAuth();
  initTheme();
  setupNavigation();
  initCharts();
  renderAllViews();
  updateSidebarDots();
  startUptimeTicker();
  bindEventHandlers();
  applyLogsFilters();   // Initial table render
  setupRealPoller();
  renderNotifications(); // Draw real-time notifications dropdown
  froWizard.init();    // First-run setup wizard (auto-bypassed)

  // Initialise Help Center panel
  renderHelpCenterNumbers();
  fetchHelpCenterNumbers();

  // Load registered customer numbers from Google Sheets (silently, no popup on failure)
  fetchRegisteredCustomers();
});


// ============================================================
// NAVIGATION ROUTER
// ============================================================
function setupNavigation() {
  const links     = document.querySelectorAll('.sidebar-link[data-target]');
  const sections  = document.querySelectorAll('.page-section');
  const titleEl   = document.getElementById('current-page-title');
  const sidebar   = document.getElementById('sidebar');
  const backdrop  = document.getElementById('sidebar-backdrop');
  const toggleBtn = document.getElementById('sidebar-toggle-btn');

  links.forEach(link => {
    link.addEventListener('click', e => {
      e.preventDefault();
      const target = link.dataset.target;
      if (!target || target === currentSection) return;

      // Update active link
      links.forEach(l => l.classList.remove('active'));
      link.classList.add('active');

      // Swap visible section
      sections.forEach(sec => {
        sec.classList.toggle('active', sec.id === target);
      });

      // Update title
      const textEl = link.querySelector('.sidebar-link-text');
      if (textEl) titleEl.textContent = textEl.textContent.trim();

      currentSection = target;
      updateSummaryBannerVisibility();

      // Close mobile sidebar
      sidebar.classList.remove('show');
      backdrop.classList.remove('show');

      // Trigger chart resize when analytics becomes visible
      if (target === 'analytics') {
        setTimeout(() => resizeAllCharts(), 120);
      }

      // Re-render connectivity data if switching to that page
      if (target === 'connectivity') {
        renderConnectivity();
      }

      // Draw/update SCADA topology line geometry when switching to system-mode topology tab
      if (target === 'system-mode') {
        setTimeout(drawScadaLines, 100);
      }
    });
  });

  // Mobile toggle
  toggleBtn.addEventListener('click', () => {
    sidebar.classList.add('show');
    backdrop.classList.add('show');
  });

  backdrop.addEventListener('click', () => {
    sidebar.classList.remove('show');
    backdrop.classList.remove('show');
  });

  // Brand logo also navigates to dashboard
  const brandLink = document.getElementById('brand-link');
  if (brandLink) {
    brandLink.addEventListener('click', e => {
      e.preventDefault();
      document.querySelector('.sidebar-link[data-target="dashboard"]')?.click();
    });
  }
}

// ============================================================
// MASTER RENDER ORCHESTRATOR
// ============================================================
function safeRender(fn, name) {
  try {
    fn();
  } catch (e) {
    console.warn(`Error rendering ${name}:`, e);
  }
}

function renderAllViews() {
  safeRender(renderTopNavbar, 'TopNavbar');
  safeRender(renderDashboard, 'Dashboard');
  safeRender(renderApPage, 'ApPage');
  safeRender(renderStaPage, 'StaPage');
  safeRender(renderConnectivity, 'Connectivity');
  safeRender(renderEeprom, 'Eeprom');
  safeRender(renderSystemInfo, 'SystemInfo');
  safeRender(updateAnalyticsStats, 'AnalyticsStats');
  safeRender(renderMessageMenu, 'MessageMenu');
  safeRender(renderSystemModePage, 'SystemModePage');
  safeRender(updateSummaryBannerVisibility, 'SummaryBannerVisibility');
  safeRender(updateSummaryBanner, 'SummaryBanner');
  safeRender(renderGoogleSheets, 'GoogleSheets');
  safeRender(updateIntegrationsSidebarDots, 'IntegrationsSidebarDots');
  safeRender(updateNetworkTopology, 'NetworkTopology');
}

// Helper to update custom mini signal bars
function updateMiniSignalBars(containerId, activeCount) {
  const container = document.getElementById(containerId);
  if (!container) return;
  const bars = container.querySelectorAll('.mini-bar');
  bars.forEach((bar, index) => {
    if (index < activeCount) {
      bar.classList.add('active');
    } else {
      bar.classList.remove('active');
    }
  });
}

// ============================================================
// TOP NAVBAR
// ============================================================
function renderTopNavbar() {
  const { ap, sta, connectivity, sim, googleSheets } = esp32State;

  document.getElementById('nav-uptime').textContent = formatUptime(systemUptimeSeconds);

  // WiFi Indicator color & icon
  const wifiIcon = document.getElementById('nav-wifi-icon');
  let wifiBars = 0;
  if (sta.status === 'CONNECTED') {
    const dbm = sta.rssi;
    if (dbm > -50) { wifiBars = 4; wifiIcon.className = 'fa-solid fa-wifi me-1 text-success'; }
    else if (dbm > -70) { wifiBars = 3; wifiIcon.className = 'fa-solid fa-wifi me-1 text-warning'; }
    else if (dbm > -85) { wifiBars = 2; wifiIcon.className = 'fa-solid fa-wifi me-1 text-warning'; }
    else if (dbm > -95) { wifiBars = 1; wifiIcon.className = 'fa-solid fa-wifi me-1 text-danger'; }
    else { wifiBars = 0; wifiIcon.className = 'fa-solid fa-wifi me-1 text-danger'; }
  } else {
    wifiIcon.className = 'fa-solid fa-circle-exclamation me-1 text-danger';
  }
  updateMiniSignalBars('nav-wifi-bars', wifiBars);

  // Tower/GSM Indicator color & icon
  const towerIcon = document.getElementById('nav-tower-icon');
  let towerBars = 0;
  if (sim.status === 'ACTIVE') {
    const dbm = sim.rssi;
    if (dbm > -70) { towerBars = 4; towerIcon.className = 'fa-solid fa-tower-cell me-1 text-success'; }
    else if (dbm > -85) { towerBars = 3; towerIcon.className = 'fa-solid fa-tower-cell me-1 text-warning'; }
    else if (dbm > -98) { towerBars = 2; towerIcon.className = 'fa-solid fa-tower-cell me-1 text-warning'; }
    else if (dbm > -110) { towerBars = 1; towerIcon.className = 'fa-solid fa-tower-cell me-1 text-danger'; }
    else { towerBars = 0; towerIcon.className = 'fa-solid fa-tower-cell me-1 text-danger'; }
  } else {
    towerIcon.className = 'fa-solid fa-circle-exclamation me-1 text-danger';
  }
  updateMiniSignalBars('nav-tower-bars', towerBars);

  // Database Indicator
  const dbIcon = document.getElementById('db-icon');
  const dbText = document.getElementById('db-text');
  if (dbIcon && dbText) {
    if (googleSheets && googleSheets.status === 'CONNECTED') {
      dbIcon.className = 'fa-solid fa-database text-success';
      dbText.textContent = 'DB Active';
    } else {
      dbIcon.className = 'fa-solid fa-database text-danger';
      dbText.textContent = 'DB Offline';
    }
  }

  const dot  = document.getElementById('esp32-status-indicator');
  const text = document.getElementById('esp32-status-text');
  if (connectivity.esp32Reachable) {
    dot.className  = 'status-dot online';
    text.textContent = 'ESP32 Online';
  } else {
    dot.className  = 'status-dot offline';
    text.textContent = 'ESP32 Offline';
  }
}

// ============================================================
// DASHBOARD
// ============================================================
function renderDashboard() {
  const { ap, sta, connectivity, sim } = esp32State;
  const isAp  = ap.status === 'Running';
  const isSta = sta.status === 'CONNECTED';

  // 1. Device Status
  const dashStatus      = document.getElementById('dash-status');
  const dashStatusDesc  = document.getElementById('dash-status-desc');
  const dashStatusTrend = document.getElementById('dash-status-trend');
  if (connectivity.esp32Reachable) {
    dashStatus.textContent = 'ACTIVE';
    dashStatus.className = 'metric-value text-success';
    dashStatusDesc.textContent = 'System online & reachable';
    dashStatusTrend.innerHTML = '<i class="fa-solid fa-circle-check"></i> Healthy';
    dashStatusTrend.className = 'metric-trend positive';
  } else {
    dashStatus.textContent = 'OFFLINE';
    dashStatus.className = 'metric-value text-danger';
    dashStatusDesc.textContent = 'No active connection';
    dashStatusTrend.innerHTML = '<i class="fa-solid fa-triangle-exclamation"></i> Disconnected';
    dashStatusTrend.className = 'metric-trend negative text-danger';
  }

  // 2. Internet Status
  const dashInternet      = document.getElementById('dash-internet');
  const dashInternetDesc  = document.getElementById('dash-internet-desc');
  const dashInternetTrend = document.getElementById('dash-internet-trend');
  if (connectivity.internetAvailable) {
    dashInternet.textContent = 'ONLINE';
    dashInternet.className = 'metric-value text-success';
    dashInternetDesc.textContent = `Latency: ${connectivity.pingInternet} ms`;
    dashInternetTrend.innerHTML = '<i class="fa-solid fa-cloud text-success"></i> Connected';
    dashInternetTrend.className = 'metric-trend positive';
  } else {
    dashInternet.textContent = 'OFFLINE';
    dashInternet.className = 'metric-value text-danger';
    dashInternetDesc.textContent = 'No WAN network link';
    dashInternetTrend.innerHTML = '<i class="fa-solid fa-cloud-slash text-danger"></i> Disconnected';
    dashInternetTrend.className = 'metric-trend negative text-danger';
  }

  // 3. SIM Status
  const dashSim      = document.getElementById('dash-sim');
  const dashSimDesc  = document.getElementById('dash-sim-desc');
  const dashSimTrend = document.getElementById('dash-sim-trend');
  let towerBars = 0;
  if (sim.status === 'ACTIVE') {
    dashSim.textContent = 'ACTIVE';
    dashSim.className = 'metric-value text-success';
    dashSimDesc.textContent = `${sim.carrier} (${sim.rssi} dBm)`;
    dashSimTrend.innerHTML = `<i class="fa-solid fa-tower-cell text-success"></i> Registered`;
    dashSimTrend.className = 'metric-trend positive';

    const dbm = sim.rssi;
    if (dbm > -70) towerBars = 4;
    else if (dbm > -85) towerBars = 3;
    else if (dbm > -98) towerBars = 2;
    else if (dbm > -110) towerBars = 1;
  } else {
    dashSim.textContent = 'NO SIM';
    dashSim.className = 'metric-value text-muted';
    dashSimDesc.textContent = 'Not inserted or locked';
    dashSimTrend.innerHTML = '<i class="fa-solid fa-circle-exclamation text-warning"></i> Sim Error';
    dashSimTrend.className = 'metric-trend neutral text-warning';
  }
  updateMiniSignalBars('card-tower-bars', towerBars);

  // 4. WiFi Status
  const dashWifiSsid  = document.getElementById('dash-wifi-ssid');
  const dashWifiRssi  = document.getElementById('dash-wifi-rssi');
  const dashWifiTrend = document.getElementById('dash-wifi-trend');
  let wifiBars = 0;
  if (isSta) {
    dashWifiSsid.textContent = sta.ssid;
    dashWifiRssi.textContent = `RSSI: ${sta.rssi} dBm (${rssiLabel(sta.rssi)})`;
    dashWifiTrend.innerHTML = '<i class="fa-solid fa-circle-check text-success"></i> Connected';
    dashWifiTrend.className = 'metric-trend positive';

    const dbm = sta.rssi;
    if (dbm > -50) wifiBars = 4;
    else if (dbm > -70) wifiBars = 3;
    else if (dbm > -85) wifiBars = 2;
    else if (dbm > -95) wifiBars = 1;
  } else if (isAp) {
    dashWifiSsid.textContent = 'AP active';
    dashWifiRssi.textContent = `SSID: ${ap.ssid}`;
    dashWifiTrend.innerHTML = '<i class="fa-solid fa-tower-broadcast text-info"></i> Broadcasting';
    dashWifiTrend.className = 'metric-trend positive text-info';
  } else {
    dashWifiSsid.textContent = 'DISCONNECTED';
    dashWifiRssi.textContent = 'RSSI: — dBm';
    dashWifiTrend.innerHTML = '<i class="fa-solid fa-circle-xmark text-danger"></i> Disconnected';
    dashWifiTrend.className = 'metric-trend negative text-danger';
  }
  updateMiniSignalBars('card-wifi-bars', wifiBars);

  // 5. Router Gateway IP
  const gatewayIp = isSta ? sta.gateway : (isAp ? ap.gateway : '—');
  document.getElementById('dash-ip').textContent = gatewayIp;
  const ipDesc = document.getElementById('dash-ip-desc');
  if (ipDesc) ipDesc.textContent = isSta ? `Device IP: ${sta.ip}` : 'Default gateway address';

  // 6. Clients
  const clientCount = ap.status === 'Running' ? ap.clients.length : 0;
  const dashClients = document.getElementById('dash-clients');
  if (dashClients) dashClients.textContent = clientCount;
  const dashClientsBadge = document.getElementById('dash-clients-badge');
  if (dashClientsBadge) dashClientsBadge.textContent = `${clientCount} online`;

  // Toggle visibility of AP Clients column card
  const clientsCol = document.getElementById('dash-card-clients-col');
  if (clientsCol) clientsCol.classList.toggle('d-none', !isAp);

  // 7. Uptime
  document.getElementById('dash-uptime').textContent = formatUptimeShort(systemUptimeSeconds);

  // 8. Total calls & Call count badge
  updateAllCallCounters();

  // 9. DHCP client list
  renderClientList();
}

function rssiLabel(dbm) {
  if (dbm > -50) return 'Excellent';
  if (dbm > -65) return 'Good';
  if (dbm > -75) return 'Fair';
  if (dbm > -85) return 'Weak';
  return 'None';
}


function renderClientList() {
  const listEl = document.getElementById('dash-client-list');
  if (!listEl) return;
  listEl.innerHTML = '';
  if (esp32State.ap.clients.length === 0) {
    listEl.innerHTML = '<p class="text-muted small text-center py-3 mb-0">No active AP clients connected</p>';
    return;
  }

  esp32State.ap.clients.forEach(c => {
    const ico = c.device === 'laptop' ? 'fa-laptop' : c.device === 'tablet' ? 'fa-tablet-screen-button' : 'fa-mobile-screen-button';
    listEl.insertAdjacentHTML('beforeend', `
      <div class="client-item">
        <div class="client-info">
          <div class="client-avatar"><i class="fa-solid ${ico}"></i></div>
          <div class="client-details">
            <div class="client-name">${c.name}</div>
            <div class="client-ip">${c.ip}</div>
          </div>
        </div>
        <div class="client-meta">
          <div>${c.mac}</div>
          <div>${c.rssi} dBm</div>
        </div>
      </div>
    `);
  });
}

// ============================================================
// AP MODE PAGE
// ============================================================
function renderApPage() {
  const running = esp32State.ap.status === 'Running';

  // Status badge
  const badge = document.getElementById('ap-status-badge');
  if (badge) {
    badge.className = running ? 'badge-status badge-status-running' : 'badge-status badge-status-stopped';
    badge.innerHTML = running
      ? '<i class="fa-solid fa-circle-play me-1"></i> Running'
      : '<i class="fa-solid fa-circle-stop me-1"></i> Stopped';
  }

  // Live indicator in header
  const indicator = document.getElementById('ap-live-indicator');
  const liveText  = document.getElementById('ap-live-text');
  if (indicator && liveText) {
    indicator.style.borderColor = running ? 'rgba(0,230,118,0.3)' : 'rgba(255,23,68,0.3)';
    indicator.style.background  = running ? 'rgba(0,230,118,0.08)' : 'rgba(255,23,68,0.08)';
    indicator.style.color       = running ? 'var(--color-success)' : 'var(--color-danger)';
    liveText.textContent        = running ? 'AP Running' : 'AP Stopped';
    const dot = indicator.querySelector('.ap-live-dot');
    if (dot) dot.style.background = running ? 'var(--color-success)' : 'var(--color-danger)';
  }

  setEl('ap-display-ssid',  esp32State.ap.ssid);
  setEl('ap-display-pwd',   running ? '••••••••' : '—');
  setEl('ap-client-count',  running ? `${esp32State.ap.clients.length} / 4 max` : '0 / 4 max');

  setDisabled('btn-start-ap',   running);
  setDisabled('btn-stop-ap',    !running);
}

// ============================================================
// STA MODE PAGE
// ============================================================
function renderStaPage() {
  renderStaDashboard();
}

// ============================================================
// SYSTEM NETWORK MODE PAGE
// ============================================================
function renderSystemModePage() {
  const mode = esp32State.networkMode; // 'AP' | 'STA' | 'AP+STA'

  // Sync radio button
  const radios = document.querySelectorAll('input[name="network-mode-radio"]');
  radios.forEach(r => { r.checked = r.value === mode; });

  // Highlight selected card
  const labels = { AP: 'label-mode-ap', STA: 'label-mode-sta', 'AP+STA': 'label-mode-dual' };
  ['label-mode-ap', 'label-mode-sta', 'label-mode-dual'].forEach(id => {
    document.getElementById(id)?.classList.remove('active');
  });
  if (labels[mode]) document.getElementById(labels[mode])?.classList.add('active');

  // Update topology visual
  updateTopologyDiagram(mode);

  // Update sidebar badge
  updateModeSidebarBadge(mode);
}

function updateTopologyDiagram(mode) {
  const isAP  = mode === 'AP'   || mode === 'AP+STA';
  const isSTA = mode === 'STA'  || mode === 'AP+STA';

  const router   = document.getElementById('topo-node-router');
  const clients  = document.getElementById('topo-node-clients');
  const gsmNode  = document.getElementById('topo-node-gsm');
  const esp32    = document.getElementById('topo-node-esp32');
  const linkSta  = document.getElementById('topo-link-sta');
  const linkAp   = document.getElementById('topo-link-ap');
  const linkGsm  = document.getElementById('topo-link-gsm');
  const linkIvr  = document.getElementById('topo-link-ivr');
  const linkCloud= document.getElementById('topo-link-cloud');
  const linkSd   = document.getElementById('topo-link-sd');
  const cloud    = document.getElementById('topo-node-cloud');
  const ivr      = document.getElementById('topo-node-ivr');
  const sdNode   = document.getElementById('topo-node-sd');
  const modeText = document.getElementById('topo-node-esp32-mode');

  if (!esp32) return;

  // Check if SD is mounted (from an API call earlier, or assume true if SD endpoints work)
  // We'll query the UI sd-status-text or we can fetch a cached state.
  const sdText = document.getElementById('sd-status-text')?.textContent;
  const hasSd = sdText && sdText.includes('Mounted');

  // Activate / deactivate nodes
  const hasGsm = esp32State.sim.status === 'ACTIVE';
  const hasCloud = esp32State.googleSheets.status === 'CONNECTED';
  
  router?.classList.toggle('active', isSTA);
  clients?.classList.toggle('active', isAP);
  gsmNode?.classList.toggle('active', hasGsm);
  
  linkSta?.classList.toggle('active', isSTA);
  linkAp?.classList.toggle('active', isAP);
  linkGsm?.classList.toggle('active', hasGsm);
  
  linkIvr?.classList.toggle('active', true); // IVR link always active
  linkCloud?.classList.toggle('active', hasCloud);
  linkSd?.classList.toggle('active', hasSd);
  cloud?.classList.toggle('active', hasCloud);
  sdNode?.classList.toggle('active', hasSd);
  ivr?.classList.toggle('active', true);

  // ESP32 node mode-specific class + colour
  esp32.classList.toggle('active', true);
  esp32.classList.remove('ap', 'sta', 'dual');
  if      (mode === 'AP')     esp32.classList.add('ap');
  else if (mode === 'STA')    esp32.classList.add('sta');
  else                         esp32.classList.add('dual');

  // Update label under ESP32 node
  if (modeText) {
    const modeLabels = { 'AP': 'AP MODE', 'STA': 'STA MODE', 'AP+STA': 'DUAL MODE' };
    modeText.textContent = modeLabels[mode] || mode;
    modeText.className = `topo-node-sub digital-font ${mode === 'AP' ? 'text-info' : mode === 'STA' ? 'text-success' : 'text-warning'}`;
  }
}

function updateModeSidebarBadge(mode) {
  const badge = document.getElementById('nav-mode-badge');
  if (!badge) return;
  badge.textContent = mode;
  badge.className = 'badge rounded-pill ms-auto text-dark';
  if      (mode === 'AP')     badge.classList.add('bg-info');
  else if (mode === 'STA')    badge.classList.add('bg-success');
  else                         badge.classList.add('bg-warning');
}



// ============================================================
// CONNECTIVITY PAGE
// ============================================================
function renderConnectivity() {
  const { sta, ap, connectivity } = esp32State;
  const isWifi     = sta.status === 'CONNECTED';
  const isInternet = connectivity.internetAvailable;
  const isAp       = ap.status === 'Running';
  const isReach    = connectivity.esp32Reachable;

  setHealthCard('health-wifi',     isWifi,     isWifi     ? `SSID: ${sta.ssid}` : 'Disconnected');
  setHealthCard('health-internet', isInternet, isInternet ? 'DNS 8.8.8.8 OK'   : 'Internet Offline');
  setHealthCard('health-ap',       isAp,       isAp       ? `SSID: ${ap.ssid}` : 'Host AP Offline');
  setHealthCard('health-esp32',    isReach,    isReach    ? 'Reachable'         : 'Unreachable');
}

function setHealthCard(id, online, descText) {
  const card = document.getElementById(id);
  const desc = document.getElementById(`${id}-desc`);
  if (!card) return;

  card.className = `health-card ${online ? 'health-online' : 'health-offline'}`;
  if (desc) desc.textContent = descText;
}

// ============================================================
// EEPROM PAGE
// ============================================================
function renderEeprom() {
  const { eeprom } = esp32State;
  const hasSsid = eeprom.storedSsid && eeprom.storedSsid.length > 0;

  setEl('eeprom-stored-ssid',  hasSsid ? eeprom.storedSsid : '(Empty)');

  const credEl = document.getElementById('eeprom-cred-status');
  if (credEl) {
    credEl.textContent = hasSsid ? 'VALID (Saved)' : 'EMPTY';
    credEl.className   = `info-value fw-bold ${hasSsid ? 'text-success' : 'text-danger'}`;
  }

  setEl('eeprom-bytes-used',   `${eeprom.bytesUsed} Bytes`);
  setEl('eeprom-write-cycles', `${eeprom.writeCycles} cycles`);

  const pct = Math.max(0, Math.min(100, Math.round((eeprom.bytesUsed / 512) * 100)));
  setEl('eeprom-pct-used', `${pct}% Used`);
  const bar = document.getElementById('eeprom-progress-bar');
  if (bar) bar.style.width = `${pct}%`;
}

// ============================================================
// SYSTEM INFO PAGE
// ============================================================
function renderSystemInfo() {
  const { system } = esp32State;
  setEl('sys-flash-size',        system.flashSize);
  setEl('sys-chip-revision',     system.chipRevision);
  setEl('sys-mac-sta',           system.macSta);
  setEl('sys-mac-ap',            system.macAp);
  setEl('sys-firmware-version',  system.firmware);
  setEl('sys-free-heap',         `${system.freeHeap.toLocaleString()} Bytes`);
  setEl('sys-temp',              `${(42 + Math.random()).toFixed(1)} °C`);
  setEl('sys-soc-model',         system.socModel || '—');
  setEl('sys-core-freq',         system.coreFreq || '—');
  setEl('sys-sdk-version',       system.sdkVersion || '—');
  setEl('sys-min-heap',          system.minHeap ? `${system.minHeap.toLocaleString()} Bytes` : '—');
  setEl('sys-rtos-tasks',        system.rtosTasks || '—');
  setEl('sys-uptime-long',       formatUptimeLong(systemUptimeSeconds));
  document.getElementById('sidebar-mac').textContent = system.macSta;
}

// ============================================================
// SIDEBAR STATUS DOTS
// ============================================================
function updateSidebarDots() {
  const apDot  = document.getElementById('nav-ap-dot');
  const staDot = document.getElementById('nav-sta-dot');
  if (apDot)  apDot.className  = `sidebar-status-dot${esp32State.ap.status  !== 'Running'      ? ' offline' : ''}`;
  if (staDot) staDot.className = `sidebar-status-dot${esp32State.sta.status !== 'CONNECTED'    ? ' offline' : ''}`;
}

// ============================================================
// UPTIME TICKER
// ============================================================
function startUptimeTicker() {
  setInterval(() => {
    systemUptimeSeconds++;
    document.getElementById('nav-uptime').textContent = formatUptime(systemUptimeSeconds);
    if (currentSection === 'dashboard') {
      document.getElementById('dash-uptime').textContent = formatUptimeShort(systemUptimeSeconds);
    }
    if (currentSection === 'eeprom-mgmt') {
      const el = document.getElementById('sys-uptime-long');
      if (el) el.textContent = formatUptimeLong(systemUptimeSeconds);
    }
  }, 1000);
}

// Connectivity auto-poller removed — diagnostics section cleared from UI.

// ============================================================
// EVENT HANDLERS
// ============================================================
function bindEventHandlers() {
  // ---- Theme Toggle ----
  bindClick('theme-toggle-btn', () => {
    const isLight = document.body.classList.toggle('light-theme');
    localStorage.setItem('theme', isLight ? 'light' : 'dark');

    const icon = document.getElementById('theme-toggle-icon');
    const text = document.getElementById('theme-toggle-text');
    if (icon && text) {
      if (isLight) {
        icon.className = 'fa-solid fa-sun text-warning';
        text.textContent = 'Light Mode';
      } else {
        icon.className = 'fa-solid fa-moon text-info';
        text.textContent = 'Dark Mode';
      }
    }
    updateChartsTheme();
    showToast('Theme Switcher', `Switched to ${isLight ? 'Light' : 'Dark'} theme`, 'success');
  });

  // ---- Global Search Bar ----
  const navSearchInput = document.getElementById('nav-search-input');
  if (navSearchInput) {
    navSearchInput.addEventListener('keypress', (e) => {
      if (e.key === 'Enter') {
        const term = navSearchInput.value.toLowerCase().trim();
        if (!term) return;
        
        const links = document.querySelectorAll('.sidebar-link[data-target]');
        let matched = false;
        for (const link of links) {
          if (link.textContent.toLowerCase().includes(term) || link.dataset.target.includes(term)) {
            link.click();
            matched = true;
            navSearchInput.value = '';
            break;
          }
        }
        if (!matched) {
          showToast('Search', `No results found for "${term}"`, 'warning');
        }
      }
    });
  }

  // ---- Global Settings Button ----
  bindClick('nav-settings-btn', () => {
    const sysModeBtn = document.getElementById('nav-system-mode');
    if (sysModeBtn) sysModeBtn.click();
  });

  // ---- Dashboard Refresh ----
  bindClick('btn-dash-refresh', async () => {
    showToast('Dashboard', 'Refreshing system status...', 'info');
    if (isRealESP32) {
      await loadRealESP32Data();
    } else {
      await simulateApiCall('/api/status');
      renderAllViews();
    }
  });

  bindClick('btn-sta-refresh', async () => {
    showToast('WiFi STA', 'Refreshing STA status...', 'info');
    if (isRealESP32) {
      await loadRealESP32Data();
    } else {
      await simulateApiCall('/api/status');
      renderAllViews();
    }
  });

  // ---- Network Mode Radio Card Live Highlight ----
  const modeRadios = document.querySelectorAll('input[name="network-mode-radio"]');
  modeRadios.forEach(radio => {
    radio.addEventListener('change', () => {
      // Remove active from all cards
      ['label-mode-ap', 'label-mode-sta', 'label-mode-dual'].forEach(id => {
        document.getElementById(id)?.classList.remove('active');
      });
      // Add active to selected card
      const selectedLabel = document.querySelector(`label[for="${radio.id}"]`);
      if (selectedLabel) selectedLabel.classList.add('active');
    });
  });

  // ---- Apply Network Mode Button ----
  bindClick('btn-apply-mode', async () => {
    const selected = document.querySelector('input[name="network-mode-radio"]:checked');
    if (!selected) {
      showToast('Mode', 'Please select a mode to apply', 'warning');
      return;
    }
    const newMode = selected.value;

    // Show loading overlay
    const overlay = document.getElementById('mode-loading-overlay');
    if (overlay) overlay.classList.remove('d-none');

    setLoading('btn-apply-mode', true, '<i class="fa-solid fa-arrows-rotate me-2"></i>Apply Mode Settings');
    showToast('Network Mode', `Switching to ${newMode} mode…`, 'info');

    // Simulate ESP32 re-init delay
    await new Promise(resolve => setTimeout(resolve, 1500));

    // Apply state changes based on selected mode
    esp32State.networkMode = newMode;

    if (newMode === 'AP') {
      esp32State.ap.status  = 'Running';
      if (esp32State.ap.ssid === '—') esp32State.ap.ssid = 'ESP32_IVR_AP';
      esp32State.sta.status = 'DISCONNECTED';
      esp32State.connectivity.internetAvailable = false;

      // Update backend
      await Promise.all([
        simulateApiCall('/api/ap', 'POST', { action: 'start' }),
        simulateApiCall('/api/sta', 'POST', { action: 'disconnect' }),
        simulateApiCall('/api/dashboard', 'POST', { mode: 'AP', wifiStatus: 'DISCONNECTED', ipAddress: '192.168.4.1' })
      ]).catch(e => console.error(e));

    } else if (newMode === 'STA') {
      esp32State.ap.status  = 'Stopped';
      esp32State.ap.clients = [];
      esp32State.sta.status = 'CONNECTED';
      if (esp32State.sta.ssid === '—') esp32State.sta.ssid = 'HomeRouter';
      esp32State.sta.ip     = '192.168.1.120';
      esp32State.sta.rssi   = -55;
      esp32State.connectivity.internetAvailable = true;
      esp32State.connectivity.esp32Reachable    = true;

      // Update backend
      await Promise.all([
        simulateApiCall('/api/ap', 'POST', { action: 'stop' }),
        simulateApiCall('/api/sta', 'POST', { action: 'connect', ssid: esp32State.sta.ssid, password: esp32State.sta.password }),
        simulateApiCall('/api/dashboard', 'POST', { mode: 'STA', wifiStatus: 'CONNECTED', ipAddress: esp32State.sta.ip })
      ]).catch(e => console.error(e));

    } else {
      // AP+STA dual mode
      esp32State.ap.status  = 'Running';
      if (esp32State.ap.ssid === '—') esp32State.ap.ssid = 'ESP32_IVR_AP';
      esp32State.sta.status = 'CONNECTED';
      if (esp32State.sta.ssid === '—') esp32State.sta.ssid = 'HomeRouter';
      esp32State.sta.ip     = '192.168.1.120';
      esp32State.sta.rssi   = -58;
      esp32State.connectivity.internetAvailable = true;
      esp32State.connectivity.esp32Reachable    = true;

      // Update backend
      await Promise.all([
        simulateApiCall('/api/ap', 'POST', { action: 'start' }),
        simulateApiCall('/api/sta', 'POST', { action: 'connect', ssid: esp32State.sta.ssid, password: esp32State.sta.password }),
        simulateApiCall('/api/dashboard', 'POST', { mode: 'AP+STA', wifiStatus: 'CONNECTED', ipAddress: esp32State.sta.ip })
      ]).catch(e => console.error(e));
    }

    // Hide overlay
    if (overlay) overlay.classList.add('d-none');
    setLoading('btn-apply-mode', false, '<i class="fa-solid fa-arrows-rotate me-2"></i>Apply Mode Settings');

    renderAllViews();
    updateSidebarDots();

    const modeEmojis = { AP: '📡', STA: '🌐', 'AP+STA': '⚡' };
    showToast('Mode Applied', `${modeEmojis[newMode] || ''} ESP32 is now running in ${newMode} Mode`, 'success');
  });


  bindClick('btn-start-ap', async () => {
    setLoading('btn-start-ap', true, '<i class="fa-solid fa-play me-2"></i>Start AP');
    await simulateApiCall('/api/ap', 'POST', { action: 'start' });
    setLoading('btn-start-ap', false, '<i class="fa-solid fa-play me-2"></i>Start AP');
    showToast('AP Mode', 'Access Point started — broadcasting SSID', 'success');
    renderAllViews();
    updateSidebarDots();
  });

  bindClick('btn-stop-ap', async () => {
    setLoading('btn-stop-ap', true, '<i class="fa-solid fa-stop me-2"></i>Stop AP');
    await simulateApiCall('/api/ap', 'POST', { action: 'stop' });
    setLoading('btn-stop-ap', false, '<i class="fa-solid fa-stop me-2"></i>Stop AP');
    showToast('AP Mode', 'Access Point stopped — clients disconnected', 'warning');
    renderAllViews();
    updateSidebarDots();
  });

  bindClick('btn-restart-ap', async () => {
    setLoading('btn-restart-ap', true, '<i class="fa-solid fa-rotate-right me-2"></i>Restart AP');
    await simulateApiCall('/api/ap', 'POST', { action: 'restart' });
    setLoading('btn-restart-ap', false, '<i class="fa-solid fa-rotate-right me-2"></i>Restart AP');
    showToast('AP Mode', 'Access Point restarted successfully', 'success');
    renderAllViews();
  });

  // ---- AP Mode Credentials Form Binds (formerly STA Form) ----
  bindClick('btn-connect-wifi', async () => {
    const ssid = getVal('sta-ssid');
    const pwd  = getVal('sta-password');
    if (!ssid.trim()) {
      showToast('Validation', 'Please enter a WiFi SSID to connect', 'warning');
      return;
    }
    if (pwd.length > 0 && pwd.length < 8) {
      showToast('Validation', 'Password must be at least 8 characters long', 'warning');
      return;
    }
    
    if (isRealESP32) {
      setLoading('btn-connect-wifi', true, '<i class="fa-solid fa-play me-2"></i>Save & Connect');
      showToast('WiFi Settings', 'Saving network credentials and rebooting...', 'info');
      try {
        const params = new URLSearchParams();
        params.append('ssid', ssid);
        params.append('pass', pwd);
        const response = await fetch('/save_net', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: params
        });
        if (response.ok) {
          showToast('WiFi Configured', 'Rebooting ESP32... Please connect your device to the configured WiFi and access vvarmaivr.local.', 'success');
          setTimeout(() => { window.location.reload(); }, 5000);
        } else {
          showToast('WiFi Settings', 'Error saving credentials', 'danger');
        }
      } catch (err) {
        showToast('WiFi Settings', 'Error: ' + err.message, 'danger');
      } finally {
        setLoading('btn-connect-wifi', false, '<i class="fa-solid fa-play me-2"></i>Save & Connect');
      }
    } else {
      showToast('AP Mode', `Starting Access Point "${ssid}"…`, 'info');
      setLoading('btn-connect-wifi', true, '<i class="fa-solid fa-play me-2"></i>Start AP');
      setTimeout(() => {
        esp32State.ap.status = 'Running';
        esp32State.ap.ssid = ssid;
        esp32State.ap.password = pwd;
        esp32State.ap.clients = [
          { name: 'iPhone 14 Pro', ip: '192.168.4.2', mac: '3C:A6:2F:A2:38:09', rssi: -45, device: 'mobile' },
          { name: 'ThinkPad Carbon', ip: '192.168.4.3', mac: 'E4:A8:6B:F9:C8:F2', rssi: -32, device: 'laptop' }
        ];
        setLoading('btn-connect-wifi', false, '<i class="fa-solid fa-play me-2"></i>Start AP');
        showToast('AP Started', `Access Point "${ssid}" is now active!`, 'success');
        renderAllViews();
        updateSidebarDots();
      }, 1500);
    }
  });

  bindClick('btn-disconnect-wifi', async () => {
    if (isRealESP32) {
      showToast('AP Mode', 'AP control unavailable in STA mode', 'warning');
      return;
    }
    showToast('AP Mode', 'Stopping Access Point…', 'info');
    setLoading('btn-disconnect-wifi', true, '<i class="fa-solid fa-stop me-2"></i>Stop AP');
    
    setTimeout(() => {
      esp32State.ap.status = 'Stopped';
      esp32State.ap.clients = [];
      setLoading('btn-disconnect-wifi', false, '<i class="fa-solid fa-stop me-2"></i>Stop AP');
      showToast('AP Stopped', 'Access Point has been stopped', 'warning');
      renderAllViews();
      updateSidebarDots();
    }, 1200);
  });

  bindClick('btn-save-credentials', async () => {
    const ssid = getVal('sta-ssid');
    const pwd  = getVal('sta-password');
    if (!ssid.trim()) {
      showToast('Validation', 'SSID is required to save credentials', 'warning');
      return;
    }
    if (pwd.length > 0 && pwd.length < 8) {
      showToast('Validation', 'Password must be at least 8 characters long', 'warning');
      return;
    }
    
    if (isRealESP32) {
      setLoading('btn-save-credentials', true, 'Saving...');
      showToast('WiFi Settings', 'Saving network credentials and rebooting...', 'info');
      try {
        const params = new URLSearchParams();
        params.append('ssid', ssid);
        params.append('pass', pwd);
        const response = await fetch('/save_net', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: params
        });
        if (response.ok) {
          showToast('WiFi Configured', 'Rebooting ESP32...', 'success');
          setTimeout(() => { window.location.reload(); }, 5000);
        } else {
          showToast('WiFi Settings', 'Error saving credentials', 'danger');
        }
      } catch (err) {
        showToast('WiFi Settings', 'Error: ' + err.message, 'danger');
      } finally {
        setLoading('btn-save-credentials', false, 'Save Credentials');
      }
    } else {
      esp32State.ap.ssid = ssid;
      esp32State.ap.password = pwd;
      esp32State.eeprom.storedSsid = ssid;
      esp32State.eeprom.storedPassword = pwd;
      esp32State.eeprom.credStatus = 'VALID (Saved)';
      esp32State.eeprom.writeCycles++;
      esp32State.eeprom.bytesUsed = 80 + ssid.length + pwd.length + 40;
      showToast('EEPROM Saved', 'Access Point credentials stored to flash memory', 'success');
      renderAllViews();
    }
  });

  // STA password toggle
  bindClick('btn-toggle-sta-pwd', () => {
    const inp  = document.getElementById('sta-password');
    const icon = document.getElementById('btn-toggle-sta-pwd-icon');
    if (!inp) return;
    const show = inp.type === 'password';
    inp.type   = show ? 'text' : 'password';
    icon.className = show ? 'fa-solid fa-eye text-muted' : 'fa-solid fa-eye-slash text-muted';
  });

  // ---- EEPROM Buttons ----
  bindClick('btn-save-eeprom', async () => {
    await simulateApiCall('/api/sta', 'POST', {
      action: 'save',
      ssid:     esp32State.sta.ssid,
      password: esp32State.sta.password
    });
    renderEeprom();
  });

  bindClick('btn-load-eeprom', async () => {
    const stored = esp32State.eeprom.storedSsid;
    if (!stored) {
      showToast('EEPROM', 'No credentials stored in EEPROM', 'warning');
      return;
    }
    showToast('EEPROM', 'Loading credentials from flash…', 'info');
    esp32State.sta.ssid     = stored;
    esp32State.sta.password = esp32State.eeprom.storedPassword;
    await simulateApiCall('/api/sta', 'POST', { action: 'connect', ssid: stored, password: esp32State.sta.password });
    renderAllViews();
  });

  // Clear EEPROM — modal confirm
  const modalEl     = document.getElementById('eeprom-confirm-modal');
  const eepromModal = modalEl ? new bootstrap.Modal(modalEl) : null;

  bindClick('btn-clear-eeprom', () => eepromModal?.show());

  bindClick('btn-modal-confirm-clear', async () => {
    eepromModal?.hide();
    showToast('EEPROM', 'Erasing EEPROM partition…', 'info');
    await simulateApiCall('/api/eeprom', 'POST', { action: 'clear' });
    showToast('EEPROM Cleared', 'All credentials erased successfully', 'success');
    renderAllViews();
    updateSidebarDots();
  });

  bindClick('btn-factory-reset', async () => {
    if (!confirm('This will perform a full factory reset. All configuration will be lost. Continue?')) return;
    showToast('Factory Reset', 'Clearing registers and rebooting…', 'danger');
    await simulateApiCall('/api/eeprom', 'POST', { action: 'factory_reset' });
    setTimeout(() => {
      showToast('Rebooted', 'ESP32 is online in AP Mode', 'success');
      renderAllViews();
      updateSidebarDots();
    }, 1200);
  });

  // ---- WiFi Settings Handlers ----
  bindClick('btn-scan-wifi', async () => {
    const scanBtn = document.getElementById('btn-scan-wifi');
    const wifiSelect = document.getElementById('wifi-select-ssid');
    if (!scanBtn || !wifiSelect) return;

    scanBtn.disabled = true;
    scanBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin me-1"></i> Scanning';
    wifiSelect.innerHTML = '<option value="">Scanning for networks...</option>';

    try {
      const res = await fetch('/scan_wifi');
      if (!res.ok) throw new Error('Scan HTTP ' + res.status);
      const networks = await res.json();
      
      wifiSelect.innerHTML = '<option value="">-- Select a WiFi Network --</option>';
      if (Array.isArray(networks) && networks.length > 0) {
        networks.forEach(net => {
          const opt = document.createElement('option');
          opt.value = net.ssid;
          opt.textContent = `${net.ssid} (${net.rssi} dBm)`;
          wifiSelect.appendChild(opt);
        });
      } else {
        wifiSelect.innerHTML = '<option value="">No WiFi networks found</option>';
      }
    } catch (err) {
      console.error('WiFi scan error:', err);
      showToast('WiFi Scan', 'Failed to scan WiFi networks', 'danger');
      wifiSelect.innerHTML = '<option value="">Error scanning networks</option>';
    } finally {
      scanBtn.disabled = false;
      scanBtn.innerHTML = '<i class="fa-solid fa-sync"></i> Scan';
    }
  });

  bindClick('btn-save-wifi', async () => {
    const ssidSelect = document.getElementById('wifi-select-ssid');
    const passInput = document.getElementById('wifi-input-pass');
    if (!ssidSelect) return;

    const ssid = ssidSelect.value;
    const pass = passInput ? passInput.value : '';

    if (!ssid) {
      alert('Please select or enter a WiFi SSID');
      return;
    }

    if (!confirm(`Reboot and connect to WiFi network: ${ssid}?`)) return;

    showToast('Saving WiFi', 'Saving credentials and rebooting device...', 'info');

    try {
      const params = new URLSearchParams();
      params.append('ssid', ssid);
      params.append('pass', pass);
      
      const res = await fetch('/save_net', { method: 'POST', body: params });
      if (res.ok) {
        showToast('WiFi Saved', 'Device is restarting. Please wait...', 'success');
      } else {
        throw new Error('Save HTTP ' + res.status);
      }
    } catch (err) {
      console.error('WiFi save error:', err);
      showToast('WiFi Save Error', 'Failed to save WiFi credentials', 'danger');
    }
  });

  bindClick('btn-save-ap-mode', async () => {
    const apSsidInput = document.getElementById('ap-input-ssid');
    const apPassInput = document.getElementById('ap-input-pass');

    const apSsid = apSsidInput ? apSsidInput.value.trim() : '';
    const apPass = apPassInput ? apPassInput.value : '';

    if (!apSsid) {
      alert('AP SSID cannot be empty');
      return;
    }

    if (apPass && apPass.length < 8) {
      alert('AP Password must be at least 8 characters');
      return;
    }

    if (!confirm('Reboot ESP32 to apply new Access Point credentials?')) return;

    showToast('Saving AP Mode Credentials', 'Saving credentials and rebooting device...', 'info');

    if (isRealESP32) {
      try {
        const params = new URLSearchParams();
        params.append('apSsid', apSsid);
        if (apPass) params.append('apPass', apPass);

        const res = await fetch('/save_sys?' + params.toString());
        if (res.ok) {
          showToast('AP Mode Saved', 'Device is restarting. Please wait...', 'success');
        } else {
          throw new Error('Save HTTP ' + res.status);
        }
      } catch (err) {
        console.error('AP save error:', err);
        showToast('Save Error', 'Failed to save AP credentials', 'danger');
      }
    } else {
      esp32State.ap.ssid = apSsid;
      esp32State.ap.password = apPass || 'GSVIVR001';
      showToast('AP Mode Saved', 'AP configuration saved (Mock Mode)', 'success');
    }
  });

  bindClick('btn-reset-ap-mode', async () => {
    if (!confirm('Reset Access Point credentials to factory defaults (SSID: vvarmaivr, Password: GSVIVR001)? The device will reboot.')) return;

    showToast('Resetting AP Mode', 'Resetting AP credentials to defaults and rebooting...', 'info');

    if (isRealESP32) {
      try {
        const params = new URLSearchParams();
        params.append('apSsid', 'vvarmaivr');
        params.append('apPass', 'GSVIVR001');

        const res = await fetch('/save_sys?' + params.toString());
        if (res.ok) {
          showToast('AP Mode Reset', 'Device is restarting with default credentials...', 'success');
          const apSsidInput = document.getElementById('ap-input-ssid');
          const apPassInput = document.getElementById('ap-input-pass');
          if (apSsidInput) apSsidInput.value = 'vvarmaivr';
          if (apPassInput) apPassInput.value = 'GSVIVR001';
        } else {
          throw new Error('Reset HTTP ' + res.status);
        }
      } catch (err) {
        console.error('AP reset error:', err);
        showToast('Reset Error', 'Failed to reset AP credentials', 'danger');
      }
    } else {
      esp32State.ap.ssid = 'vvarmaivr';
      esp32State.ap.password = 'GSVIVR001';
      const apSsidInput = document.getElementById('ap-input-ssid');
      const apPassInput = document.getElementById('ap-input-pass');
      if (apSsidInput) apSsidInput.value = 'vvarmaivr';
      if (apPassInput) apPassInput.value = 'GSVIVR001';
      showToast('AP Mode Reset', 'AP credentials reset to defaults (Mock Mode)', 'success');
    }
  });

  bindClick('btn-save-admin-cred', async () => {
    const adminUserInput = document.getElementById('admin-input-user');
    const adminPassInput = document.getElementById('admin-input-pass');

    const adminUser = adminUserInput ? adminUserInput.value.trim() : '';
    const adminPass = adminPassInput ? adminPassInput.value : '';

    if (!adminUser) {
      alert('Admin Username cannot be empty');
      return;
    }

    if (!confirm('Reboot ESP32 to apply new admin credentials?')) return;

    showToast('Saving Admin Credentials', 'Saving credentials and rebooting device...', 'info');

    if (isRealESP32) {
      try {
        const params = new URLSearchParams();
        params.append('adminUser', adminUser);
        if (adminPass) params.append('adminPass', adminPass);

        const res = await fetch('/save_sys?' + params.toString());
        if (res.ok) {
          showToast('Admin Credentials Saved', 'Device is restarting. Please wait...', 'success');
        } else {
          throw new Error('Save HTTP ' + res.status);
        }
      } catch (err) {
        console.error('Admin save error:', err);
        showToast('Save Error', 'Failed to save admin credentials', 'danger');
      }
    } else {
      showToast('Admin Credentials Saved', 'Admin credentials saved (Mock Mode)', 'success');
    }
  });

  bindClick('btn-reset-admin-cred', async () => {
    if (!confirm('Reset Admin login credentials to factory defaults (Username: admin, Password: GSVIVR001)? The device will reboot.')) return;

    showToast('Resetting Admin Credentials', 'Resetting admin credentials to defaults and rebooting...', 'info');

    if (isRealESP32) {
      try {
        const params = new URLSearchParams();
        params.append('adminUser', 'admin');
        params.append('adminPass', 'GSVIVR001');

        const res = await fetch('/save_sys?' + params.toString());
        if (res.ok) {
          showToast('Admin Reset Complete', 'Device is restarting with default admin login...', 'success');
          const adminUserInput = document.getElementById('admin-input-user');
          const adminPassInput = document.getElementById('admin-input-pass');
          if (adminUserInput) adminUserInput.value = 'admin';
          if (adminPassInput) adminPassInput.value = 'GSVIVR001';
        } else {
          throw new Error('Reset HTTP ' + res.status);
        }
      } catch (err) {
        console.error('Admin reset error:', err);
        showToast('Reset Error', 'Failed to reset admin credentials', 'danger');
      }
    } else {
      const adminUserInput = document.getElementById('admin-input-user');
      const adminPassInput = document.getElementById('admin-input-pass');
      if (adminUserInput) adminUserInput.value = 'admin';
      if (adminPassInput) adminPassInput.value = 'GSVIVR001';
      showToast('Admin Reset Complete', 'Admin credentials reset to defaults (Mock Mode)', 'success');
    }
  });

  // ---- Call History Buttons ----
  bindClick('btn-refresh-logs', () => {
    applyLogsFilters();
    updateAnalyticsCharts();
    showToast('Call Logs', 'Refreshed call history data', 'info');
  });

  bindClick('btn-export-pdf', exportToPdf);

  bindClick('btn-clear-logs', async () => {
    if (!confirm('Clear all call log entries from session memory?')) return;
    await simulateApiCall('/api/calls', 'DELETE');
    applyLogsFilters();
    updateAnalyticsCharts();
    renderDashboard();
    showToast('Call Logs', 'All call logs cleared', 'success');
  });

  // Filters
  const searchInput  = document.getElementById('log-search-input');
  const filterStatus = document.getElementById('log-filter-status');
  const sortBy       = document.getElementById('log-sort-by');

  if (searchInput)  searchInput.addEventListener('input', () => { currentPage = 1; applyLogsFilters(); });
  if (filterStatus) filterStatus.addEventListener('change', () => { currentPage = 1; applyLogsFilters(); });
  if (sortBy)       sortBy.addEventListener('change', () => { currentPage = 1; applyLogsFilters(); });

  // ---- Google Sheets Config ----
  bindClick('btn-save-sheets-config', async () => {
    const sheetId = getVal('sheets-id');
    const sheetName = getVal('sheets-name');
    const scriptUrl = getVal('sheets-script-url').trim();
    
    let scriptId = scriptUrl;
    if (scriptUrl.includes('/s/')) {
      const parts = scriptUrl.split('/s/');
      if (parts[1]) {
        scriptId = parts[1].split('/')[0];
      }
    }

    const syncIncoming = document.getElementById('sync-incoming')?.checked ?? false;
    const syncOutgoing = document.getElementById('sync-outgoing')?.checked ?? false;
    const syncSms = document.getElementById('sync-sms')?.checked ?? false;
    const syncSystem = document.getElementById('sync-system')?.checked ?? false;
    const syncNetwork = document.getElementById('sync-network')?.checked ?? false;

    if (isRealESP32) {
      setLoading('btn-save-sheets-config', true, 'Saving...');
      showToast('Google Sheets', 'Saving sync settings...', 'info');
      try {
        const url = `/save_sys?script=${encodeURIComponent(scriptId)}` +
                    `&sheetId=${encodeURIComponent(sheetId)}` +
                    `&sheetName=${encodeURIComponent(sheetName)}` +
                    `&syncInc=${syncIncoming}` +
                    `&syncOut=${syncOutgoing}` +
                    `&syncSms=${syncSms}` +
                    `&syncSys=${syncSystem}` +
                    `&syncNet=${syncNetwork}` +
                    `&reboot=false`;
        const response = await fetch(url);
        if (response.ok) {
          localStorage.setItem('vvarma_sheet_id', sheetId);
          localStorage.setItem('sheets_script_url', scriptUrl);
          // Send sheetId dynamically to Apps Script via ESP32 proxy to avoid CORS
          await fetch(`/init_varma_sheets?sheetId=${encodeURIComponent(sheetId)}`)
            .catch(err => console.warn('Sync MANUAL_SS_ID property failed:', err));
          
          showToast('Settings Saved', 'Google Sheets configuration saved successfully.', 'success');
          
          // Update in-memory state dynamically
          esp32State.googleSheets.sheetId = sheetId;
          esp32State.googleSheets.sheetName = sheetName;
          esp32State.googleSheets.scriptUrl = scriptUrl;
          esp32State.googleSheets.syncIncoming = syncIncoming;
          esp32State.googleSheets.syncOutgoing = syncOutgoing;
          esp32State.googleSheets.syncSms = syncSms;
          esp32State.googleSheets.syncSystem = syncSystem;
          esp32State.googleSheets.syncNetwork = syncNetwork;
          
          renderGoogleSheets();
          updateIntegrationsSidebarDots();
          updateSummaryBanner();
          
          // Load registered customer numbers
          setTimeout(() => fetchRegisteredCustomers(), 500);
        } else {
          showToast('Settings Error', 'Failed to update system config', 'danger');
        }
      } catch (err) {
        showToast('Settings Error', err.message, 'danger');
      } finally {
        setLoading('btn-save-sheets-config', false, 'Save Configuration');
      }
    } else {
      if (sheetId) {
        localStorage.setItem('vvarma_sheet_id', sheetId);
      } else {
        localStorage.removeItem('vvarma_sheet_id');
      }
      if (scriptUrl) {
        localStorage.setItem('sheets_script_url', scriptUrl);
      } else {
        localStorage.removeItem('sheets_script_url');
      }
      esp32State.googleSheets.sheetId = sheetId;
      esp32State.googleSheets.sheetName = sheetName;
      esp32State.googleSheets.scriptUrl = scriptUrl;
      esp32State.googleSheets.syncIncoming = document.getElementById('sync-incoming')?.checked ?? false;
      esp32State.googleSheets.syncOutgoing = document.getElementById('sync-outgoing')?.checked ?? false;
      esp32State.googleSheets.syncSms = document.getElementById('sync-sms')?.checked ?? false;
      esp32State.googleSheets.syncSystem = document.getElementById('sync-system')?.checked ?? false;
      esp32State.googleSheets.syncNetwork = document.getElementById('sync-network')?.checked ?? false;

      showToast('Google Sheets', 'Sync configuration saved successfully (Mock Mode)', 'success');
      renderGoogleSheets();
      updateIntegrationsSidebarDots();
      updateSummaryBanner();
    }
  });

  bindClick('btn-test-sheets-conn', async () => {
    setLoading('btn-test-sheets-conn', true, '<i class="fa-solid fa-bolt me-2 text-warning"></i>Testing...');
    showToast('Google Sheets', 'Testing connection to Google Apps Script...', 'info');
    
    try {
      // Always try the real configured URL — no fake/mock success allowed
      const configuredUrl = getVal('sheets-script-url').trim() || localStorage.getItem('sheets_script_url') || esp32State.googleSheets.scriptUrl || '';
      if (!configuredUrl || !configuredUrl.includes('script.google.com')) {
        throw new Error('No valid Google Apps Script URL configured. Please enter the script URL first.');
      }
      const sheetId = getVal('sheets-id');
      if (isRealESP32) {
        // On real ESP32: proxy the test through the ESP32 to avoid CORS with a 50s timeout
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 50000);
        
        const response = await fetch(`/init_varma_sheets?sheetId=${encodeURIComponent(sheetId)}`, { signal: controller.signal });
        clearTimeout(timeoutId);
        
        if (!response.ok) throw new Error('ESP32 proxy returned HTTP ' + response.status);
        const data = await response.json();
        if (data && data.success === false) {
          throw new Error(data.message || 'Verification failed on Google Sheets.');
        }
        esp32State.googleSheets.status = 'CONNECTED';
        esp32State.googleSheets.lastSyncTime = new Date().toLocaleTimeString();
        showToast('Google Sheets', '✅ Connected! ESP32 → Google Sheets link is active.', 'success');
        addNotification('Sheets Connected', 'Google Sheets connection test passed.', 'success');
      } else {
        // Dev mode: try to reach the URL directly (will fail with CORS but we check reachability) with a 10s timeout
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 10000);
        
        const pingUrl = configuredUrl + '?action=ping';
        const response = await fetch(pingUrl, { mode: 'no-cors', cache: 'no-cache', signal: controller.signal });
        clearTimeout(timeoutId);
        
        // no-cors fetch always returns opaque response (status 0) if reachable
        esp32State.googleSheets.status = 'CONNECTED';
        esp32State.googleSheets.lastSyncTime = new Date().toLocaleTimeString();
        showToast('Google Sheets', '✅ Script URL is reachable. Connection test passed.', 'success');
        addNotification('Sheets Connected', 'Google Sheets script URL is reachable.', 'success');
      }
      renderGoogleSheets();
      updateIntegrationsSidebarDots();
      updateSummaryBanner();
    } catch (err) {
      esp32State.googleSheets.status = 'DISCONNECTED';
      renderGoogleSheets();
      updateIntegrationsSidebarDots();
      showToast('Google Sheets', '❌ Connection failed: ' + err.message, 'danger');
      addNotification('Sheets Test Failed', err.message, 'danger');
    } finally {
      setLoading('btn-test-sheets-conn', false, '<i class="fa-solid fa-bolt me-2 text-warning"></i>Test Connection');
    }
  });

  // ---- WiFi Scanning Buttons ----
  bindClick('btn-scan-wifi-ap', () => startWifiScan('ap'));
  bindClick('btn-refresh-scan-ap', () => startWifiScan('ap'));
  bindClick('btn-scan-wifi-sta', () => startWifiScan('sta'));
  bindClick('btn-refresh-scan-sta', () => startWifiScan('sta'));

  // ---- Login Authorization Events ----
  const btnToggleLoginPass = document.getElementById('btn-toggle-login-pass');
  if (btnToggleLoginPass) {
    btnToggleLoginPass.addEventListener('click', () => {
      const passInput = document.getElementById('admin-pass');
      const eyeIcon = document.getElementById('login-pass-eye-icon');
      if (passInput && eyeIcon) {
        if (passInput.type === 'password') {
          passInput.type = 'text';
          eyeIcon.className = 'fa-solid fa-eye-slash';
        } else {
          passInput.type = 'password';
          eyeIcon.className = 'fa-solid fa-eye';
        }
      }
    });
  }

  bindClick('btn-login-submit', async () => {
    const user = getVal('admin-user');
    const pass = getVal('admin-pass');
    const rememberMe = document.getElementById('admin-remember')?.checked ? '1' : '0';
    
    if (isRealESP32) {
      setLoading('btn-login-submit', true, 'Authenticating...');
      try {
        const response = await fetch(`/login?u=${encodeURIComponent(user)}&p=${encodeURIComponent(pass)}&r=${rememberMe}`);
        const result = await response.json();
        if (result && result.success) {
          // Persist session in localStorage so refresh doesn't trigger re-login
          localStorage.setItem('ivr_logged_in', '1');
          localStorage.setItem('vvarma_auth_token', 'AUTH_VALID');
          localStorage.setItem('ivr_logged_user', user);
          const overlay = document.getElementById('login-overlay');
          if (overlay) {
            overlay.classList.add('fade-out');
            setTimeout(() => overlay.classList.add('d-none'), 500);
          }
          showToast('Security Gatekeeper', 'Access Granted. Welcome back!', 'success');
          addNotification('Login Successful', `Authenticated as ${user}. Session started.`, 'success');
          loadRealESP32Data();
          document.getElementById('nav-dashboard')?.click();
        } else {
          showToast('Security Gatekeeper', 'Access Denied: Invalid credentials.', 'danger');
          addNotification('Login Failed', 'Invalid credentials entered.', 'danger');
        }
      } catch (err) {
        showToast('Security Gatekeeper', 'Auth Error: ' + err.message, 'danger');
      } finally {
        setLoading('btn-login-submit', false, 'Grant Access');
      }
    } else {
      if (user === 'admin' && pass === 'admin') {
        localStorage.setItem('ivr_logged_in', '1');
        localStorage.setItem('vvarma_auth_token', 'AUTH_VALID');
        const overlay = document.getElementById('login-overlay');
        if (overlay) {
          overlay.classList.add('fade-out');
          setTimeout(() => overlay.classList.add('d-none'), 500);
        }
        showToast('Security Gatekeeper', 'Access Granted. Welcome back!', 'success');
        document.getElementById('nav-dashboard')?.click();
      } else {
        showToast('Security Gatekeeper', 'Access Denied: Invalid credentials.', 'danger');
      }
    }
  });

  const btnLogout = document.getElementById('nav-logout');
  if (btnLogout) {
    btnLogout.addEventListener('click', (e) => {
      e.preventDefault();
      const modalEl = document.getElementById('logout-confirm-modal');
      if (modalEl) {
        const logoutModal = bootstrap.Modal.getInstance(modalEl) || new bootstrap.Modal(modalEl);
        logoutModal.show();
      }
    });
  }

  const btnConfirmLogout = document.getElementById('btn-modal-confirm-logout');
  if (btnConfirmLogout) {
    btnConfirmLogout.addEventListener('click', () => {
      // Hide modal
      const modalEl = document.getElementById('logout-confirm-modal');
      if (modalEl) {
        const logoutModal = bootstrap.Modal.getInstance(modalEl);
        if (logoutModal) logoutModal.hide();
      }

      // Perform logout
      localStorage.removeItem('ivr_logged_in');
      localStorage.removeItem('vvarma_auth_token');
      // Clear cookie
      document.cookie = "VARMA_SID_V4_SECURE=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;";

      const overlay = document.getElementById('login-overlay');
      if (overlay) {
        overlay.classList.remove('fade-out');
        overlay.classList.remove('d-none');
        const userEl = document.getElementById('admin-user');
        const passEl = document.getElementById('admin-pass');
        if (userEl) userEl.value = '';
        if (passEl) passEl.value = '';
      }
      showToast('Security Gatekeeper', 'Successfully logged out.', 'info');
    });
  }

  // ---- DTMF Keypad Dialer Test Events ----
  const dtmfFreqs = {
    '1': [697, 1209], '2': [697, 1336], '3': [697, 1477],
    '4': [770, 1209], '5': [770, 1336], '6': [770, 1477],
    '7': [852, 1209], '8': [852, 1336], '9': [852, 1477],
    '*': [941, 1209], '0': [941, 1336], '#': [941, 1477]
  };

  // Shared AudioContext — created lazily on first user interaction
  let _sharedAudioCtx = null;

  function getAudioCtx() {
    if (!_sharedAudioCtx || _sharedAudioCtx.state === 'closed') {
      _sharedAudioCtx = new (window.AudioContext || window.webkitAudioContext)();
    }
    return _sharedAudioCtx;
  }

  async function playDTMF(key) {
    const freqs = dtmfFreqs[key];
    if (!freqs) return;
    try {
      const audioCtx = getAudioCtx();
      // CRITICAL: Resume audioCtx - Chrome suspends it until user interaction
      if (audioCtx.state === 'suspended') {
        await audioCtx.resume();
      }
      const osc1 = audioCtx.createOscillator();
      const osc2 = audioCtx.createOscillator();
      const gainNode = audioCtx.createGain();

      osc1.type = 'sine';
      osc1.frequency.value = freqs[0];
      osc2.type = 'sine';
      osc2.frequency.value = freqs[1];

      gainNode.gain.setValueAtTime(0.15, audioCtx.currentTime);
      gainNode.gain.exponentialRampToValueAtTime(0.001, audioCtx.currentTime + 0.18);

      osc1.connect(gainNode);
      osc2.connect(gainNode);
      gainNode.connect(audioCtx.destination);

      osc1.start(audioCtx.currentTime);
      osc2.start(audioCtx.currentTime);
      osc1.stop(audioCtx.currentTime + 0.18);
      osc2.stop(audioCtx.currentTime + 0.18);
    } catch (err) {
      console.warn('[DTMF] AudioContext error:', err.message);
    }
  }

  // Use event delegation on the modal body — avoids binding issues on hidden elements
  const dialerModal = document.getElementById('dialer-test-modal');
  if (dialerModal) {
    dialerModal.addEventListener('click', async (e) => {
      const btn = e.target.closest('.dialer-key');
      if (!btn) return;
      const key = btn.getAttribute('data-key');
      if (!key) return;

      // Visual flash feedback
      btn.classList.add('dialer-key-pressed');
      setTimeout(() => btn.classList.remove('dialer-key-pressed'), 180);

      // Play DTMF tone
      await playDTMF(key);

      // Append to screen
      const screen = document.getElementById('dialer-screen');
      if (screen) {
        screen.textContent += key;
        // Auto-scroll to end
        screen.scrollLeft = screen.scrollWidth;
      }

      // Update dialer status
      const status = document.getElementById('dialer-status');
      if (status) {
        status.textContent = `Tone: ${key}`;
        status.className = 'text-warning';
        setTimeout(() => {
          if (status) { status.textContent = 'Ready'; status.className = 'text-success'; }
        }, 500);
      }
    });
  }

  bindClick('btn-dialer-clear', () => {
    const screen = document.getElementById('dialer-screen');
    if (screen) screen.textContent = '';
    const status = document.getElementById('dialer-status');
    if (status) { status.textContent = 'Cleared'; status.className = 'text-muted'; }
    setTimeout(() => { if (status) { status.textContent = 'Ready'; status.className = 'text-success'; } }, 600);
  });

  bindClick('btn-dialer-trigger-call', () => {
    const screen = document.getElementById('dialer-screen');
    const number = screen ? screen.textContent.trim() : '';
    if (number) {
      showToast('Hardware Dialer', `Placing real call to: ${number}…`, 'info');
      fetch(`/api/dial?number=${encodeURIComponent(number)}`, { method: 'POST' })
        .then(res => res.json())
        .then(data => {
          if (data.status === 'success') {
            showToast('Hardware Dialer', 'Dialing sequence started.', 'success');
          } else {
            showToast('Hardware Dialer', 'Dial failed: ' + data.message, 'danger');
          }
        })
        .catch(err => {
          showToast('Hardware Dialer', 'Connection failed: ' + err.message, 'danger');
        });
    }
    if (screen) screen.textContent = '';

    const modalEl = document.getElementById('dialer-test-modal');
    if (modalEl) {
      const modalInstance = bootstrap.Modal.getInstance(modalEl) || new bootstrap.Modal(modalEl);
      if (modalInstance) modalInstance.hide();
    }
  });

  // ---- Notification Dropdown Toggle & Clear ----
  const notifBellBtn = document.getElementById('notification-bell-btn');
  const notifDropdown = document.getElementById('notification-dropdown');
  const notifClearBtn = document.getElementById('btn-clear-notifications');

  if (notifBellBtn && notifDropdown) {
    notifBellBtn.addEventListener('click', (e) => {
      e.stopPropagation();
      const isHidden = notifDropdown.classList.contains('d-none');
      if (isHidden) {
        notifDropdown.classList.remove('d-none');
        unreadNotificationsCount = 0;
        localStorage.setItem('vvarma_unread_notifications_count', '0');
        renderNotifications();
      } else {
        notifDropdown.classList.add('d-none');
      }
    });

    // Close dropdown on click outside
    document.addEventListener('click', (e) => {
      if (!notifDropdown.classList.contains('d-none') && !notifDropdown.contains(e.target) && e.target !== notifBellBtn) {
        notifDropdown.classList.add('d-none');
      }
    });
  }

  if (notifClearBtn) {
    notifClearBtn.addEventListener('click', (e) => {
      e.stopPropagation();
      notificationsList = [];
      unreadNotificationsCount = 0;
      localStorage.setItem('vvarma_notifications', '[]');
      localStorage.setItem('vvarma_unread_notifications_count', '0');
      renderNotifications();
      showToast('Notifications', 'Cleared all notifications', 'info');
    });
  }
}

// ============================================================
// CALL HISTORY DATA GENERATOR
// ============================================================
function triggerMockCallEvent(customNumber) {
  if (incomingCallActive) return;
  const num = customNumber || `+91 98765 ${Math.floor(10000 + Math.random() * 90000)}`;
  
  if (!esp32State.callSystemActive) {
    showToast('Call Control', 'Call System is inactive. Incoming call auto-rejected immediately.', 'warning');
    const today = new Date();
    const dateStr = today.toISOString().split('T')[0];
    const timeStr = today.toTimeString().split(' ')[0];
    const newId = callsData.length > 0 ? Math.max(...callsData.map(c => c.id)) + 1 : 1001;
    callsData.push({ id: newId, number: num, date: dateStr, time: timeStr, duration: 0, status: 'Auto-Rejected' });
    applyLogsFilters();
    updateAnalyticsCharts();
    updateAllCallCounters();
    return;
  }
  
  lastRingingNumber = num;
  showIncomingCallPopup(num);
  updateNetworkTopology();
}

function generateDummyCallHistory() {
  // Mock data removed
}

// ============================================================
// FILTER / SORT / PAGINATE
// ============================================================
function applyLogsFilters() {
  const search = (document.getElementById('log-search-input')?.value || '').trim().toLowerCase();
  const status = (document.getElementById('log-filter-status')?.value || 'ALL');
  const sort   = (document.getElementById('log-sort-by')?.value || 'id-desc');

  filteredCalls = callsData.filter(c => {
    const matchSearch = c.number.toLowerCase().includes(search) || String(c.id).includes(search);
    const matchStatus = status === 'ALL' || c.status === status;
    return matchSearch && matchStatus;
  });

  if (sort === 'id-desc')        filteredCalls.sort((a, b) => b.id - a.id);
  else if (sort === 'id-asc')    filteredCalls.sort((a, b) => a.id - b.id);
  else if (sort === 'duration-desc') filteredCalls.sort((a, b) => b.duration - a.duration);
  else if (sort === 'duration-asc')  filteredCalls.sort((a, b) => a.duration - b.duration);

  renderLogsTable();
}

function renderLogsTable() {
  const tbody = document.getElementById('calls-table-body');
  if (!tbody) return;
  tbody.innerHTML = '';

  const total    = filteredCalls.length;
  const pages    = Math.ceil(total / PAGE_SIZE);
  if (currentPage > pages) currentPage = Math.max(1, pages);

  const start = (currentPage - 1) * PAGE_SIZE;
  const end   = Math.min(start + PAGE_SIZE, total);

  const info = document.getElementById('pagination-info-text');
  if (info) {
    info.textContent = total > 0
      ? `Showing ${start + 1} – ${end} of ${total} entries`
      : 'No entries match current filters';
  }

  if (total === 0) {
    tbody.insertAdjacentHTML('beforeend',
      '<tr><td colspan="6" class="text-center py-5 text-muted">No call logs match the current criteria</td></tr>'
    );
    renderPagination(0);
    return;
  }

  for (let i = start; i < end; i++) {
    const c   = filteredCalls[i];
    const dur = c.status === 'Completed' ? formatDuration(c.duration) : '—';
    tbody.insertAdjacentHTML('beforeend', `
      <tr>
        <td class="digital-font fw-semibold">#${String(c.id).padStart(4, '0')}</td>
        <td class="digital-font">${c.number}</td>
        <td>${c.date}</td>
        <td class="digital-font">${c.time}</td>
        <td class="digital-font">${dur}</td>
        <td>
          <span class="status-badge status-badge-${c.status.toLowerCase()}">
            <i class="fa-solid ${c.status === 'Completed' ? 'fa-circle-check' : 'fa-circle-xmark'}"></i>
            ${c.status}
          </span>
        </td>
      </tr>
    `);
  }

  renderPagination(pages);
}

function renderPagination(totalPages) {
  const ul = document.getElementById('logs-pagination');
  if (!ul) return;
  ul.innerHTML = '';

  if (totalPages <= 1) return;

  const mkItem = (label, page, disabled, active = false) => {
    const li = document.createElement('li');
    li.className = `page-item ${disabled ? 'disabled' : ''} ${active ? 'active' : ''}`;
    li.innerHTML = `<a class="page-link" href="#">${label}</a>`;
    if (!disabled && !active) {
      li.addEventListener('click', e => { e.preventDefault(); currentPage = page; renderLogsTable(); });
    }
    return li;
  };

  ul.appendChild(mkItem('<i class="fa-solid fa-chevron-left"></i>', currentPage - 1, currentPage === 1));

  let from = Math.max(1, currentPage - 2);
  let to   = Math.min(totalPages, from + 4);
  if (to - from < 4) from = Math.max(1, to - 4);

  for (let p = from; p <= to; p++) {
    ul.appendChild(mkItem(p, p, false, p === currentPage));
  }

  ul.appendChild(mkItem('<i class="fa-solid fa-chevron-right"></i>', currentPage + 1, currentPage === totalPages));
}

// ============================================================
// CSV EXPORT
// ============================================================
// A4 PDF EXPORT
function exportToPdf() {
  if (filteredCalls.length === 0) {
    showToast('Export', 'No records to export', 'warning');
    return;
  }

  try {
    const { jsPDF } = window.jspdf;
    const doc = new jsPDF({
      orientation: 'portrait',
      unit: 'mm',
      format: 'a4'
    });

    // Header Background
    doc.setFillColor(13, 21, 39); // Deep dark IoT background color
    doc.rect(0, 0, 210, 38, 'F');

    // Title
    doc.setTextColor(0, 242, 254); // Cyan accent
    doc.setFont('helvetica', 'bold');
    doc.setFontSize(20);
    doc.text('ESP32 IVR DASHBOARD', 15, 16);

    // Subtitle
    doc.setTextColor(255, 255, 255);
    doc.setFont('helvetica', 'normal');
    doc.setFontSize(12);
    doc.text('Call History Report (A4 Format)', 15, 24);

    // Metadata
    doc.setFontSize(9);
    doc.setTextColor(136, 153, 181); // Muted text color
    const generatedOn = new Date().toLocaleString();
    doc.text(`Generated: ${generatedOn}`, 15, 31);
    doc.text(`Total Records: ${filteredCalls.length}`, 145, 31);

    // Border line
    doc.setDrawColor(0, 242, 254);
    doc.setLineWidth(0.8);
    doc.line(0, 38, 210, 38);

    // Table Setup
    const headers = [['Call ID', 'Caller Number', 'Date', 'Time', 'Duration', 'Status']];
    const data = filteredCalls.map(r => [
      r.id,
      r.number,
      r.date,
      r.time,
      `${r.duration}s`,
      r.status
    ]);

    doc.autoTable({
      head: headers,
      body: data,
      startY: 45,
      theme: 'grid',
      headStyles: {
        fillColor: [12, 20, 40],
        textColor: [255, 255, 255],
        fontStyle: 'bold',
        halign: 'left'
      },
      alternateRowStyles: {
        fillColor: [240, 244, 248]
      },
      styles: {
        fontSize: 9,
        cellPadding: 3,
        font: 'helvetica'
      },
      margin: { left: 15, right: 15 },
      didDrawPage: function(pageData) {
        doc.setFontSize(8);
        doc.setTextColor(150, 150, 150);
        doc.text(`Page ${pageData.pageNumber}`, doc.internal.pageSize.width - 25, doc.internal.pageSize.height - 10);
        doc.text('ESP32 IVR System • Dashboard Session Logs', 15, doc.internal.pageSize.height - 10);
      }
    });

    const filename = `esp32_ivr_calls_${new Date().toISOString().slice(0, 10)}.pdf`;
    doc.save(filename);
    showToast('Export Complete', `Exported ${filteredCalls.length} records to A4 PDF`, 'success');
  } catch (err) {
    console.error('PDF export failed:', err);
    showToast('Export Failed', 'Could not generate PDF document', 'danger');
  }
}

// ============================================================
// ANALYTICS
// ============================================================
function buildAnalyticsSummary() {
  const total     = callsData.length;
  const completed = callsData.filter(c => c.status === 'Completed').length;
  const missed    = total - completed;
  const sumDur    = callsData.reduce((s, c) => s + c.duration, 0);
  const avgSecs   = completed > 0 ? Math.round(sumDur / completed) : 0;

  return {
    total,
    completed,
    missed,
    completedPct: total > 0 ? ((completed / total) * 100).toFixed(1) + '%' : '0.0%',
    missedPct:    total > 0 ? ((missed    / total) * 100).toFixed(1) + '%' : '0.0%',
    avgDuration:  formatDuration(avgSecs)
  };
}

function updateAnalyticsStats() {
  const s = buildAnalyticsSummary();
  setEl('stat-total-calls',     s.total);
  setEl('stat-completed-calls', s.completed);
  setEl('stat-missed-calls',    s.missed);
  setEl('stat-completed-pct',   s.completedPct);
  setEl('stat-missed-pct',      s.missedPct);
  setEl('stat-avg-duration',    s.avgDuration);
}

function updateAnalyticsCharts() {
  updateAnalyticsStats();
}


// ============================================================
// CHART.JS INITIALIZATION
// ============================================================
function initCharts() {
  if (typeof Chart === 'undefined') {
    console.warn('Chart.js library not loaded. Analytics charts will be disabled.');
    return;
  }
  Chart.defaults.color          = '#8899b5';
  Chart.defaults.font.family    = "'Inter', sans-serif";
  Chart.defaults.font.size      = 11;

  const CYAN   = '#f39c12';
  const BLUE   = '#a855f7'; // Neon Purple
  const GREEN  = '#3b82f6'; // Electric Blue

  // Daily Calls — Bar Chart
  const ctxDaily = document.getElementById('chart-daily');
  if (ctxDaily) {
    chartDaily = new Chart(ctxDaily, {
      type: 'bar',
      data: {
        labels: ['08:00', '10:00', '12:00', '14:00', '16:00', '18:00', '20:00'],
        datasets: [{
          label: 'Calls',
          data:  [12, 19, 25, 18, 32, 15, 8],
          backgroundColor: `rgba(243, 156, 18, 0.35)`,
          borderColor:     CYAN,
          borderWidth:     1.5,
          borderRadius:    6,
          hoverBackgroundColor: `rgba(243, 156, 18, 0.55)`
        }]
      },
      options: buildChartOptions()
    });
  }

  // Weekly Calls — Line Chart
  const ctxWeekly = document.getElementById('chart-weekly');
  if (ctxWeekly) {
    chartWeekly = new Chart(ctxWeekly, {
      type: 'line',
      data: {
        labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'],
        datasets: [{
          label: 'Calls',
          data:  [85, 92, 110, 88, 115, 75, 60],
          borderColor:     BLUE,
          backgroundColor: `rgba(79,172,254,0.08)`,
          tension:         0.4,
          fill:            true,
          borderWidth:     2,
          pointBackgroundColor: BLUE,
          pointRadius:     4,
          pointHoverRadius:6
        }]
      },
      options: buildChartOptions()
    });
  }

  // Monthly Calls — Area Chart
  const ctxMonthly = document.getElementById('chart-monthly');
  if (ctxMonthly) {
    chartMonthly = new Chart(ctxMonthly, {
      type: 'line',
      data: {
        labels: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun'],
        datasets: [{
          label: 'Calls',
          data:  [450, 480, 520, 490, 580, 510],
          borderColor:     GREEN,
          backgroundColor: `rgba(0,230,118,0.07)`,
          tension:         0.45,
          fill:            true,
          borderWidth:     2,
          pointBackgroundColor: GREEN,
          pointRadius:     4,
          pointHoverRadius:6
        }]
      },
      options: buildChartOptions()
    });
  }

  // Live RSSI Analytics trend chart
  initRssiTrendChart();
}

function buildChartOptions() {
  const isLight = document.body.classList.contains('light-theme');
  const gridColor = isLight ? 'rgba(0, 0, 0, 0.06)' : 'rgba(255, 255, 255, 0.04)';
  const tickColor = isLight ? '#334155' : '#8899b5';
  const tooltipBg = isLight ? 'rgba(255, 255, 255, 0.95)' : 'rgba(10, 15, 41, 0.85)';
  const tooltipBorder = isLight ? 'rgba(0, 0, 0, 0.1)' : 'rgba(243, 156, 18, 0.35)';
  const tooltipTitle = isLight ? '#0f172a' : '#fff';
  const tooltipBody = isLight ? '#334155' : '#8899b5';

  return {
    responsive:          true,
    maintainAspectRatio: false,
    interaction: { mode: 'index', intersect: false },
    plugins: {
      legend: { display: false },
      tooltip: {
        backgroundColor: tooltipBg,
        borderColor:     tooltipBorder,
        borderWidth:     1,
        titleColor:      tooltipTitle,
        bodyColor:       tooltipBody,
        padding:         10
      }
    },
    scales: {
      x: {
        grid:  { color: gridColor, drawBorder: false },
        ticks: { color: tickColor, font: { size: 10 } }
      },
      y: {
        grid:  { color: gridColor, drawBorder: false },
        ticks: { color: tickColor, font: { size: 10 } },
        beginAtZero: true
      }
    }
  };
}

// -------------------------------------------------------------
// Theme Management Helpers
// -------------------------------------------------------------
function initTheme() {
  const savedTheme = localStorage.getItem('theme') || 'dark';
  const isLight = savedTheme === 'light';
  
  document.body.classList.toggle('light-theme', isLight);
  
  const icon = document.getElementById('theme-toggle-icon');
  const text = document.getElementById('theme-toggle-text');
  if (icon && text) {
    if (isLight) {
      icon.className = 'fa-solid fa-sun text-warning';
      text.textContent = 'Light Mode';
    } else {
      icon.className = 'fa-solid fa-moon text-info';
      text.textContent = 'Dark Mode';
    }
  }
}

function updateChartsTheme() {
  const isLight = document.body.classList.contains('light-theme');
  const gridColor = isLight ? 'rgba(0, 0, 0, 0.06)' : 'rgba(255, 255, 255, 0.04)';
  const tickColor = isLight ? '#334155' : '#8899b5';
  const tooltipBg = isLight ? 'rgba(255, 255, 255, 0.95)' : 'rgba(8, 14, 28, 0.92)';
  const tooltipBorder = isLight ? 'rgba(0, 0, 0, 0.1)' : 'rgba(243, 156, 18, 0.2)';
  const tooltipTitle = isLight ? '#0f172a' : '#fff';
  const tooltipBody = isLight ? '#334155' : '#8899b5';

  [chartDaily, chartWeekly, chartMonthly].forEach(ch => {
    if (!ch) return;
    
    ch.options.scales.x.grid.color = gridColor;
    ch.options.scales.x.ticks.color = tickColor;
    ch.options.scales.y.grid.color = gridColor;
    ch.options.scales.y.ticks.color = tickColor;
    
    if (ch.options.plugins && ch.options.plugins.tooltip) {
      ch.options.plugins.tooltip.backgroundColor = tooltipBg;
      ch.options.plugins.tooltip.borderColor = tooltipBorder;
      ch.options.plugins.tooltip.titleColor = tooltipTitle;
      ch.options.plugins.tooltip.bodyColor = tooltipBody;
    }
    
    ch.update();
  });

  if (chartRssiTrend) {
    chartRssiTrend.options.scales.y.grid.color = gridColor;
    chartRssiTrend.options.scales.y.ticks.color = tickColor;
    chartRssiTrend.update();
  }
}

function resizeAllCharts() {
  [chartDaily, chartWeekly, chartMonthly, chartRssiTrend].forEach(ch => ch?.resize());
}

// ============================================================
// TOAST NOTIFICATION
// ============================================================
function showToast(title, body, type = 'info') {
  const toastEl = document.getElementById('action-toast');
  if (!toastEl) return;

  const icons = {
    success: 'fa-circle-check text-success',
    danger:  'fa-triangle-exclamation text-danger',
    warning: 'fa-circle-exclamation text-warning',
    info:    'fa-info-circle text-info'
  };

  document.getElementById('toast-icon').className = `fa-solid ${icons[type] || icons.info} me-2`;
  document.getElementById('toast-title').textContent  = title;
  document.getElementById('toast-body-text').textContent = body;
  document.getElementById('toast-time').textContent   = 'Just now';

  const toast = new bootstrap.Toast(toastEl, { delay: 4500 });
  toast.show();
}

// ============================================================
// UTILITY HELPERS
// ============================================================
function formatUptime(totalSecs) {
  const d = Math.floor(totalSecs / 86400);
  const h = Math.floor((totalSecs % 86400) / 3600);
  const m = Math.floor((totalSecs % 3600) / 60);
  const s = totalSecs % 60;
  const dStr = d > 0 ? `${d}d ` : '';
  return `${dStr}${pad(h)}h ${pad(m)}m ${pad(s)}s`;
}

function formatUptimeShort(totalSecs) {
  const d = Math.floor(totalSecs / 86400);
  const h = Math.floor((totalSecs % 86400) / 3600);
  const m = Math.floor((totalSecs % 3600) / 60);
  return d > 0 ? `${pad(d)}d ${pad(h)}h ${pad(m)}m` : `${pad(h)}h ${pad(m)}m`;
}

function formatUptimeLong(totalSecs) {
  const d = Math.floor(totalSecs / 86400);
  const h = Math.floor((totalSecs % 86400) / 3600);
  const m = Math.floor((totalSecs % 3600) / 60);
  const s = totalSecs % 60;
  return `${d} days, ${h} hours, ${m} minutes, ${s} seconds`;
}

function formatDuration(secs) {
  if (secs === 0 || isNaN(secs)) return '0s';
  const m = Math.floor(secs / 60);
  const s = secs % 60;
  return m > 0 ? `${m}m ${s}s` : `${s}s`;
}

function pad(n) { return String(n).padStart(2, '0'); }

function setEl(id, val) {
  const el = document.getElementById(id);
  if (el) el.textContent = val;
}

function getVal(id) {
  const el = document.getElementById(id);
  return el ? el.value : '';
}

function setDisabled(id, disabled) {
  const el = document.getElementById(id);
  if (el) el.disabled = disabled;
}

function bindClick(id, handler) {
  const el = document.getElementById(id);
  if (el) el.addEventListener('click', handler);
}

function setLoading(btnId, loading, defaultHtml) {
  const btn = document.getElementById(btnId);
  if (!btn) return;
  btn.disabled = loading;
  btn.innerHTML = loading
    ? '<span class="spinner-border spinner-border-sm me-2" role="status"></span>Processing…'
    : defaultHtml;
}

// ============================================================
// MESSAGE MENU (KEYWORD & REPLIES)
// ============================================================
let editingRuleId = null;

function renderMessageMenu() {
  const tbody = document.getElementById('rules-table-body');
  if (!tbody) return;
  tbody.innerHTML = '';

  const rules = esp32State.autoReply.rules;

  if (rules.length === 0) {
    tbody.innerHTML = '<tr><td colspan="4" class="text-muted text-center py-4">No keyword rules defined yet.</td></tr>';
    return;
  }

  rules.forEach(rule => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td><span class="badge bg-secondary digital-font text-white">${rule.keyword}</span></td>
      <td><span class="text-white small">${rule.reply}</span></td>
      <td>
        <div class="form-check form-switch">
          <input class="form-check-input switch-rule-status" type="checkbox" data-id="${rule.id}" ${rule.active ? 'checked' : ''}>
        </div>
      </td>
      <td class="text-end">
        <button class="btn btn-glass-secondary btn-sm me-2 btn-edit-rule" data-id="${rule.id}">
          <i class="fa-solid fa-pen"></i>
        </button>
        <button class="btn btn-glass-danger btn-sm btn-delete-rule" data-id="${rule.id}">
          <i class="fa-solid fa-trash"></i>
        </button>
      </td>
    `;
    tbody.appendChild(tr);
  });

  // Re-bind listeners for status toggle
  tbody.querySelectorAll('.switch-rule-status').forEach(input => {
    input.addEventListener('change', e => {
      const id = parseInt(e.target.dataset.id);
      const rule = rules.find(r => r.id === id);
      if (rule) {
        rule.active = e.target.checked;
        showToast('Rule Status', `Keyword "${rule.keyword}" response set to ${rule.active ? 'Active' : 'Inactive'}`, 'info');
      }
    });
  });

  // Re-bind edit buttons
  tbody.querySelectorAll('.btn-edit-rule').forEach(btn => {
    btn.addEventListener('click', e => {
      const id = parseInt(e.currentTarget.dataset.id);
      openRuleModal(id);
    });
  });

  // Re-bind delete buttons
  tbody.querySelectorAll('.btn-delete-rule').forEach(btn => {
    btn.addEventListener('click', e => {
      const id = parseInt(e.currentTarget.dataset.id);
      deleteRule(id);
    });
  });
}

function openRuleModal(id = null) {
  const modalEl = document.getElementById('rule-modal');
  if (!modalEl) return;
  const modal = new bootstrap.Modal(modalEl);

  const titleEl = document.getElementById('ruleModalLabel');
  const kwInput = document.getElementById('rule-keyword');
  const rpInput = document.getElementById('rule-reply');
  const acInput = document.getElementById('rule-active');
  const hiddenId = document.getElementById('rule-id-hidden');

  if (id) {
    // Edit Mode
    const rule = esp32State.autoReply.rules.find(r => r.id === id);
    if (!rule) return;
    titleEl.textContent = 'Edit Keyword Rule';
    kwInput.value = rule.keyword;
    rpInput.value = rule.reply;
    acInput.checked = rule.active;
    hiddenId.value = rule.id;
    editingRuleId = id;
  } else {
    // Add Mode
    titleEl.textContent = 'Add Keyword Rule';
    kwInput.value = '';
    rpInput.value = '';
    acInput.checked = true;
    hiddenId.value = '';
    editingRuleId = null;
  }

  modal.show();
}

function saveRule() {
  const kwInput = document.getElementById('rule-keyword');
  const rpInput = document.getElementById('rule-reply');
  const acInput = document.getElementById('rule-active');

  const keyword = kwInput.value.trim().toUpperCase();
  const reply = rpInput.value.trim();
  const active = acInput.checked;

  if (!keyword || !reply) {
    showToast('Validation Error', 'Please fill in both Keyword and Reply fields.', 'warning');
    return;
  }

  // Duplicate Check
  const duplicate = esp32State.autoReply.rules.find(r => r.keyword === keyword && r.id !== editingRuleId);
  if (duplicate) {
    showToast('Duplicate Error', `Rule for keyword "${keyword}" already exists!`, 'warning');
    return;
  }

  if (editingRuleId) {
    // Update existing rule
    const rule = esp32State.autoReply.rules.find(r => r.id === editingRuleId);
    if (rule) {
      rule.keyword = keyword;
      rule.reply = reply;
      rule.active = active;
      showToast('Rule Updated', `Keyword rule for "${keyword}" has been updated`, 'success');
    }
  } else {
    // Create new rule
    const newId = esp32State.autoReply.rules.length > 0
      ? Math.max(...esp32State.autoReply.rules.map(r => r.id)) + 1
      : 1;
    esp32State.autoReply.rules.push({ id: newId, keyword, reply, active });
    showToast('Rule Added', `Keyword rule for "${keyword}" has been created`, 'success');
  }

  // Dismiss modal
  const modalEl = document.getElementById('rule-modal');
  const modalInstance = bootstrap.Modal.getInstance(modalEl);
  if (modalInstance) modalInstance.hide();

  renderMessageMenu();
}

function deleteRule(id) {
  if (!confirm('Are you sure you want to delete this keyword auto-response rule?')) return;
  const index = esp32State.autoReply.rules.findIndex(r => r.id === id);
  if (index !== -1) {
    const kw = esp32State.autoReply.rules[index].keyword;
    esp32State.autoReply.rules.splice(index, 1);
    showToast('Rule Deleted', `Keyword rule for "${kw}" has been removed`, 'warning');
    renderMessageMenu();
  }
}

// SIMULATOR FUNCTIONALITY
function runSimulationTest() {
  const inputEl = document.getElementById('sim-keyword-input');
  const outputEl = document.getElementById('sim-console-output');
  if (!inputEl || !outputEl) return;

  const rawInput = inputEl.value.trim();
  if (!rawInput) return;

  const keyword = rawInput.toUpperCase();
  inputEl.value = '';

  // Append user command message
  const userMsg = document.createElement('div');
  userMsg.className = 'text-white mt-1';
  userMsg.textContent = `Client: ${rawInput}`;
  outputEl.appendChild(userMsg);

  // Simulated output generator
  const responseMsg = document.createElement('div');
  responseMsg.className = 'text-info';

  if (!esp32State.autoReply.enabled) {
    responseMsg.textContent = 'ESP32 response: [Auto-Reply Disabled Globally]';
    responseMsg.className = 'text-warning';
  } else {
    const matchedRule = esp32State.autoReply.rules.find(r => r.keyword === keyword && r.active);
    if (matchedRule) {
      // Process dynamic placeholders e.g., %UPTIME%
      let finalReply = matchedRule.reply;
      if (finalReply.includes('%UPTIME%')) {
        finalReply = finalReply.replace('%UPTIME%', formatUptimeLong(systemUptimeSeconds));
      }
      responseMsg.textContent = `ESP32 response: ${finalReply}`;

      // Side-effects check for system commands simulated via SMS
      if (keyword === 'AP' || keyword === 'ENABLE_AP') {
        setTimeout(async () => {
          showToast('Simulator Command', '🔄 SMS instruction received: Switching to AP Mode…', 'info');
          esp32State.networkMode = 'AP';
          esp32State.ap.status = 'Running';
          if (esp32State.ap.ssid === '—') esp32State.ap.ssid = 'ESP32_IVR_AP';
          esp32State.sta.status = 'DISCONNECTED';
          esp32State.connectivity.internetAvailable = false;
          
          await Promise.all([
            simulateApiCall('/api/ap', 'POST', { action: 'start' }),
            simulateApiCall('/api/sta', 'POST', { action: 'disconnect' }),
            simulateApiCall('/api/dashboard', 'POST', { mode: 'AP', wifiStatus: 'DISCONNECTED', ipAddress: '192.168.4.1' })
          ]).catch(e => console.error(e));
          
          renderAllViews();
          updateSidebarDots();
        }, 1000);
      } else if (keyword === 'SETUP' || keyword === 'WIFI_SETUP') {
        setTimeout(async () => {
          showToast('Simulator Command', '⚙️ SMS instruction received: Resetting WiFi config to trigger first-time Setup Wizard…', 'warning');
          
          // Clear credentials
          localStorage.removeItem('ivr_setup_done');
          localStorage.removeItem('ivr_sta_ssid');
          
          esp32State.networkMode = 'AP';
          esp32State.ap.status = 'Running';
          if (esp32State.ap.ssid === '—') esp32State.ap.ssid = 'ESP32_IVR_AP';
          esp32State.sta.status = 'DISCONNECTED';
          esp32State.connectivity.internetAvailable = false;
          
          await Promise.all([
            simulateApiCall('/api/eeprom', 'POST', { action: 'clear' }),
            simulateApiCall('/api/ap', 'POST', { action: 'start' }),
            simulateApiCall('/api/sta', 'POST', { action: 'disconnect' }),
            simulateApiCall('/api/dashboard', 'POST', { mode: 'AP', wifiStatus: 'DISCONNECTED', ipAddress: '192.168.4.1' })
          ]).catch(e => console.error(e));
          
          renderAllViews();
          updateSidebarDots();
          froWizard.show();
        }, 1000);
      } else if (keyword === 'REBOOT') {
        setTimeout(async () => {
          showToast('Simulator Command', '🔄 SMS instruction received: Issuing reboot…', 'danger');
          await simulateApiCall('/api/device/reboot', 'POST').catch(e => console.error(e));
        }, 1000);
      }
    } else {
      responseMsg.textContent = 'ESP32 response: [No active rule matches this keyword]';
      responseMsg.className = 'text-muted';
    }
  }

  outputEl.appendChild(responseMsg);
  outputEl.scrollTop = outputEl.scrollHeight;
}

// Initialize message menu event bindings
document.addEventListener('DOMContentLoaded', () => {
  const addBtn = document.getElementById('btn-add-keyword');
  if (addBtn) addBtn.addEventListener('click', () => openRuleModal());

  const saveBtn = document.getElementById('btn-save-rule');
  if (saveBtn) saveBtn.addEventListener('click', saveRule);

  const globalSwitch = document.getElementById('switch-auto-reply-enable');
  if (globalSwitch) {
    globalSwitch.addEventListener('change', e => {
      esp32State.autoReply.enabled = e.target.checked;
      showToast('Global Auto-Reply', `SMS Auto-responses set to ${esp32State.autoReply.enabled ? 'Enabled' : 'Disabled'}`, 'info');
    });
  }

  const simBtn = document.getElementById('btn-sim-test');
  if (simBtn) simBtn.addEventListener('click', runSimulationTest);

  const simInput = document.getElementById('sim-keyword-input');
  if (simInput) {
    simInput.addEventListener('keydown', e => {
      if (e.key === 'Enter') {
        runSimulationTest();
      }
    });
  }
});

// ============================================================
// ENHANCED UPGRADES MODULES
// ============================================================

function initRssiTrendChart() {
  const ctx = document.getElementById('chart-rssi-trend');
  if (!ctx) return;
  
  const isLight = document.body.classList.contains('light-theme');
  const gridColor = isLight ? 'rgba(0, 0, 0, 0.06)' : 'rgba(255, 255, 255, 0.04)';
  const tickColor = isLight ? '#334155' : '#8899b5';
  const tooltipBg = isLight ? 'rgba(255, 255, 255, 0.95)' : 'rgba(10, 15, 41, 0.85)';
  const tooltipBorder = isLight ? 'rgba(0, 0, 0, 0.1)' : 'rgba(243, 156, 18, 0.35)';
  const tooltipTitle = isLight ? '#0f172a' : '#fff';
  const tooltipBody = isLight ? '#334155' : '#8899b5';

  chartRssiTrend = new Chart(ctx, {
    type: 'line',
    data: {
      labels: Array(10).fill(''),
      datasets: [{
        label: 'Signal (dBm)',
        data: rssiDataPoints,
        borderColor: '#f39c12',
        backgroundColor: 'rgba(243, 156, 18, 0.05)',
        borderWidth: 2,
        pointRadius: 2,
        pointHoverRadius: 4,
        tension: 0.3,
        fill: true
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: { display: false },
        tooltip: {
          backgroundColor: tooltipBg,
          borderColor: tooltipBorder,
          borderWidth: 1,
          titleColor: tooltipTitle,
          bodyColor: tooltipBody,
          padding: 8
        }
      },
      scales: {
        x: {
          grid: { display: false },
          ticks: { display: false }
        },
        y: {
          grid: { color: gridColor, drawBorder: false },
          ticks: { color: tickColor, font: { size: 9 } },
          min: -100,
          max: -30
        }
      }
    }
  });
}

function updateRssiTrendChart(newRssi) {
  if (!chartRssiTrend) return;
  rssiDataPoints.push(newRssi);
  if (rssiDataPoints.length > 15) {
    rssiDataPoints.shift();
  }
  chartRssiTrend.data.datasets[0].data = rssiDataPoints;
  chartRssiTrend.data.labels = Array(rssiDataPoints.length).fill('');
  chartRssiTrend.update();
}

function updateSummaryBannerVisibility() {
  const banner = document.getElementById('global-summary-banner');
  if (!banner) return;
  const visibleSections = ['dashboard', 'connectivity', 'system-mode', 'test-panel'];
  if (visibleSections.includes(currentSection)) {
    banner.classList.remove('d-none');
  } else {
    banner.classList.add('d-none');
  }
}

function updateSummaryBanner() {
  const { ap, sta, connectivity, sim, googleSheets } = esp32State;

  // 1. Device Status
  const devEl = document.getElementById('sum-device');
  if (devEl) {
    if (connectivity.esp32Reachable) {
      devEl.textContent = 'Online';
      devEl.className = 'summary-card-value text-success';
    } else {
      devEl.textContent = 'Offline';
      devEl.className = 'summary-card-value text-danger';
    }
  }

  // 2. SIM Status
  const simEl = document.getElementById('sum-sim');
  if (simEl) {
    if (sim.status === 'ACTIVE') {
      simEl.textContent = sim.carrier || 'Active';
      simEl.className = 'summary-card-value text-success';
    } else {
      simEl.textContent = 'No SIM';
      simEl.className = 'summary-card-value text-muted';
    }
  }

  // 3. GSM Signal
  const gsmEl = document.getElementById('sum-gsm');
  if (gsmEl) {
    if (sim.status === 'ACTIVE') {
      gsmEl.textContent = `${sim.rssi} dBm`;
    } else {
      gsmEl.textContent = '—';
    }
  }

  // 4. WiFi Link
  const wifiEl = document.getElementById('sum-wifi');
  if (wifiEl) {
    if (sta.status === 'CONNECTED') {
      wifiEl.textContent = `${sta.ssid} (${sta.rssi} dBm)`;
      wifiEl.className = 'summary-card-value text-success';
    } else if (sta.status === 'CONNECTING') {
      wifiEl.textContent = 'Connecting...';
      wifiEl.className = 'summary-card-value text-warning';
    } else {
      wifiEl.textContent = 'Disconnected';
      wifiEl.className = 'summary-card-value text-danger';
    }
  }

  // 5. Internet
  const netEl = document.getElementById('sum-internet');
  if (netEl) {
    if (connectivity.internetAvailable) {
      netEl.textContent = 'Online';
      netEl.className = 'summary-card-value text-success';
    } else {
      netEl.textContent = 'Offline';
      netEl.className = 'summary-card-value text-danger';
    }
  }

  // 6. Uptime
  const uptimeEl = document.getElementById('sum-uptime');
  if (uptimeEl) {
    uptimeEl.textContent = formatUptime(systemUptimeSeconds);
  }

  // 7. Total Calls
  updateAllCallCounters();

  // 8. AP Clients
  const apEl = document.getElementById('sum-ap-clients');
  if (apEl) {
    apEl.textContent = ap.status === 'Running' ? ap.clients.length : 0;
  }

  // 9. IP Address
  const ipEl = document.getElementById('sum-ip');
  if (ipEl) {
    if (sta.status === 'CONNECTED') {
      ipEl.textContent = sta.ip || '—';
    } else if (ap.status === 'Running') {
      ipEl.textContent = '192.168.4.1';
    } else {
      ipEl.textContent = '—';
    }
  }

  // 10. Last Sync
  const syncEl = document.getElementById('sum-sync');
  if (syncEl) {
    syncEl.textContent = googleSheets.status === 'CONNECTED' ? googleSheets.lastSyncTime : '—';
  }
}

function updateNetworkTopology() {
  const { ap, sta, sim, googleSheets, firebase, telegram, connectivity } = esp32State;

  // Helpers
  const setActive = (id, isActive) => {
    const el = document.getElementById(id);
    if (el) el.classList.toggle('active', isActive);
  };
  const setLinkStatus = (id, status) => {
    const el = document.getElementById(id);
    if (!el) return;
    el.classList.remove('active', 'status-good', 'status-transfer', 'status-weak', 'status-bad', 'status-warning', 'status-error', 'status-inactive', 'status-static-white');
    if (status === 'good' || status === 'weak' || status === 'transfer' || status === 'static-white') {
      el.classList.add('active', `status-${status}`);
    } else {
      el.classList.add(`status-${status}`);
    }
  };
  const setText = (id, text) => {
    const el = document.getElementById(id);
    if (el) el.textContent = text || '--';
  };
  const toggleVisibility = (id, isVisible) => {
    const el = document.getElementById(id);
    if (el) el.classList.toggle('d-none', !isVisible);
  };

  // 1. Cellular Tower & GSM
  const isSimActive = sim.status === 'ACTIVE';
  const isGsmRegistered = isSimActive && (sim.regStatus === '1' || sim.regStatus === '5');
  const hasGsmSignal = isSimActive && sim.rssi > -113;
  const isNetworkConnected = isGsmRegistered && hasGsmSignal;

  // Tower is only active if registered and has signal
  setActive('topo-node-tower', isNetworkConnected);
  setActive('topo-node-gsm', isSimActive);
  
  let towerGsmStatus = 'error';
  if (isNetworkConnected) {
    if (incomingCallActive) towerGsmStatus = 'transfer';
    else if (sim.rssi > -85) towerGsmStatus = 'good';
    else towerGsmStatus = 'weak';
  }
  setLinkStatus('topo-link-tower-gsm', towerGsmStatus);

  let gsmEspStatus = 'error';
  if (connectivity.esp32Reachable) {
    if (incomingCallActive) gsmEspStatus = 'transfer';
    else gsmEspStatus = 'good';
  }
  setLinkStatus('topo-link-gsm-esp', gsmEspStatus);

  if (isSimActive) {
    const regMap = {
      '0': 'Not Registered',
      '1': 'Registered',
      '2': 'Searching...',
      '3': 'Denied',
      '4': 'Unknown',
      '5': 'Roaming'
    };
    const regStr = regMap[sim.regStatus] || sim.regStatus || 'Registered';
    const signalStr = sim.rssi <= -113 ? 'No Signal' : `${sim.rssi} dBm`;

    setText('topo-tower-network', isNetworkConnected ? (sim.carrier || 'Connected') : 'Searching...');
    setText('topo-gsm-carrier', isNetworkConnected ? (sim.carrier || 'Active') : 'No Carrier');
    setText('topo-gsm-network', isNetworkConnected ? (sim.network || '4G LTE') : 'Searching...');
    setText('topo-gsm-signal', signalStr);
    setText('topo-gsm-reg', regStr);
    setText('topo-gsm-sim', sim.simNumber || '+919876543210');
    setText('topo-gsm-imei', sim.imei || '358294058291048');
  } else {
    setText('topo-tower-network', 'Searching...');
    setText('topo-gsm-carrier', '--');
    setText('topo-gsm-network', '--');
    setText('topo-gsm-signal', '--');
    setText('topo-gsm-reg', 'Offline');
    setText('topo-gsm-sim', '--');
    setText('topo-gsm-imei', '--');
  }

  // 2. Local Router & WiFi
  const isWifiConnected = sta.status === 'CONNECTED';
  setActive('topo-node-router', isWifiConnected);
  toggleVisibility('topo-router-wave', isWifiConnected);

  let wifiStatus = 'error';
  if (isWifiConnected) {
    if (sta.rssi > -70) wifiStatus = 'good';
    else wifiStatus = 'weak';
  }
  setLinkStatus('topo-link-sta-esp', wifiStatus);

  if (isWifiConnected) {
    setText('topo-sta-ssid', sta.ssid || '--');
    setText('topo-sta-ip', sta.ip || 'DHCP');
    setText('topo-sta-gw', sta.gateway || '--');
    setText('topo-sta-rssi', `${sta.rssi} dBm`);
    setText('topo-sta-quality', sta.quality || (sta.rssi > -70 ? 'Excellent' : 'Fair'));
  } else {
    setText('topo-sta-ssid', '--');
    setText('topo-sta-ip', '--');
    setText('topo-sta-gw', '--');
    setText('topo-sta-rssi', '--');
    setText('topo-sta-quality', '--');
  }

  // 3. AP Clients
  const isApActive = ap.status === 'Running';
  const hasClients = isApActive && ap.clients && ap.clients.length > 0;
  setActive('topo-node-clients', hasClients);
  setText('topo-ap-clients', isApActive ? (ap.clients ? ap.clients.length : 0) : 0);
  setText('topo-ap-ips', hasClients ? ap.clients[0].ip + (ap.clients.length > 1 ? ', ...' : '') : '--');
  setText('topo-ap-traffic', hasClients ? '12 kbps' : 'Idle');
  setLinkStatus('topo-link-clients-esp', hasClients ? 'static-white' : 'error');

  // 4. ESP32 Core
  setActive('topo-node-esp32', connectivity.esp32Reachable);
  toggleVisibility('topo-esp-pulse', connectivity.esp32Reachable && (isWifiConnected || isApActive));
  
  const modeText = document.getElementById('topo-node-esp32-mode');
  if (modeText) {
    if (ap.status === 'Running' && sta.status === 'CONNECTED') modeText.textContent = 'DUAL MODE';
    else if (ap.status === 'Running') modeText.textContent = 'AP MODE';
    else modeText.textContent = 'STA MODE';
  }
  
  setText('topo-esp-cpu', connectivity.esp32Reachable ? (esp32State.system.cpuUsage || '--') : '--');
  setText('topo-esp-heap', connectivity.esp32Reachable ? (esp32State.system.freeHeap ? Math.floor(esp32State.system.freeHeap/1024) + ' KB' : '--') : '--');
  setText('topo-esp-uptime', connectivity.esp32Reachable ? formatUptime(systemUptimeSeconds) : '--');

  // 5. Cloud Services
  const isSheetsConnected = googleSheets && googleSheets.status === 'CONNECTED';
  const isFirebaseConnected = connectivity.internetAvailable && isWifiConnected;
  const isTelegramConnected = connectivity.internetAvailable && isWifiConnected;
  
  setActive('topo-node-sheets', isSheetsConnected);
  setActive('topo-node-firebase', isFirebaseConnected);
  setActive('topo-node-telegram', isTelegramConnected);
  
  setLinkStatus('topo-link-sheets-esp', isSheetsConnected ? 'static-white' : 'error');
  setLinkStatus('topo-link-firebase-esp', isFirebaseConnected ? 'static-white' : 'error');
  setLinkStatus('topo-link-telegram-esp', isTelegramConnected ? 'static-white' : 'error');

  // 6. IVR System
  setActive('topo-node-ivr', isSimActive && connectivity.esp32Reachable);
  setLinkStatus('topo-link-ivr-esp', (isSimActive && connectivity.esp32Reachable) ? (incomingCallActive ? 'transfer' : 'good') : 'error');
  toggleVisibility('topo-ivr-wave', incomingCallActive || (callsData.length > 0 && Math.random() > 0.5));
  
  setText('topo-ivr-status', isSimActive ? (incomingCallActive ? 'Processing' : 'Ready') : 'Offline');
  setText('topo-ivr-caller', incomingCallActive ? lastRingingNumber : '--');
  setText('topo-ivr-calls', callsData.length);

  // 7. SD Card
  const sdState = esp32State.sd || {};
  const isSdMounted = sdState.mounted === true;
  setActive('topo-node-sd', isSdMounted && connectivity.esp32Reachable);
  
  toggleVisibility('topo-sd-missing', !isSdMounted);
  toggleVisibility('topo-sd-metrics', isSdMounted);
  if (isSdMounted) {
    setText('topo-sd-status', 'Mounted');
    setText('topo-sd-size', sdState.size || '--');
    setText('topo-sd-used', sdState.used || '--');
    setText('topo-sd-free', sdState.free || '--');
    setText('topo-sd-files', sdState.files || '--');
  }
  setLinkStatus('topo-link-sd-esp', (isSdMounted && connectivity.esp32Reachable) ? 'static-white' : 'error');
}

function renderGoogleSheets() {
  const gs = esp32State.googleSheets;
  
  const incSwitch = document.getElementById('sync-incoming');
  if (incSwitch) incSwitch.checked = gs.syncIncoming;
  
  const outSwitch = document.getElementById('sync-outgoing');
  if (outSwitch) outSwitch.checked = gs.syncOutgoing;
  
  const smsSwitch = document.getElementById('sync-sms');
  if (smsSwitch) smsSwitch.checked = gs.syncSms;
  
  const sysSwitch = document.getElementById('sync-system');
  if (sysSwitch) sysSwitch.checked = gs.syncSystem;
  
  const netSwitch = document.getElementById('sync-network');
  if (netSwitch) netSwitch.checked = gs.syncNetwork;

  const statusEl = document.getElementById('sheets-sync-status');
  if (statusEl) {
    statusEl.textContent = gs.status;
    if (gs.status === 'CONNECTED') {
      statusEl.className = 'info-value text-success';
    } else {
      statusEl.className = 'info-value text-danger';
    }
  }

  const timeEl = document.getElementById('sheets-last-sync-time');
  if (timeEl) timeEl.textContent = gs.lastSyncTime || '—';

  const countEl = document.getElementById('sheets-records-count');
  if (countEl) countEl.textContent = gs.recordsUploaded;

  const errEl = document.getElementById('sheets-errors-count');
  if (errEl) {
    errEl.textContent = gs.errorsCount;
    if (gs.errorsCount > 0) {
      errEl.className = 'info-value text-danger';
    } else {
      errEl.className = 'info-value text-success';
    }
  }

  const todayEl = document.getElementById('sheets-today-count');
  if (todayEl) todayEl.textContent = gs.todayCount;

  const smsCountEl = document.getElementById('sheets-sms-count');
  if (smsCountEl) smsCountEl.textContent = gs.smsCount;

  const lastUpdateEl = document.getElementById('sheets-last-update-date');
  if (lastUpdateEl) {
    if (gs.lastSyncTime !== '—' && gs.lastSyncTime) {
      const today = new Date().toLocaleDateString();
      lastUpdateEl.textContent = `${today} ${gs.lastSyncTime}`;
    } else {
      lastUpdateEl.textContent = '—';
    }
  }
}

function updateIntegrationsSidebarDots() {
  const sheetsDot = document.getElementById('nav-sheets-dot');
  if (sheetsDot) {
    sheetsDot.className = 'status-dot-sidebar ' + (esp32State.googleSheets.status === 'CONNECTED' ? 'connected' : 'disconnected');
  }
}

async function startWifiScan(type) {
  if (type === 'ap') {
    if (isScanningAp) return;
    isScanningAp = true;
    
    const btn = document.getElementById('btn-scan-wifi-ap');
    const refreshBtn = document.getElementById('btn-refresh-scan-ap');
    const spinner = document.getElementById('spinner-scan-ap');
    const icon = document.getElementById('icon-scan-ap');
    
    if (btn) btn.disabled = true;
    if (refreshBtn) refreshBtn.disabled = true;
    if (spinner) spinner.style.display = 'inline-block';
    if (icon) icon.classList.add('fa-spin');
    
    const tbody = document.getElementById('scan-body-ap');
    if (tbody) {
      tbody.innerHTML = '';
      for (let i = 0; i < 4; i++) {
        tbody.innerHTML += `
          <tr>
            <td colspan="5">
              <div class="skeleton-row">
                <div class="skeleton-block" style="width: 35%"></div>
                <div class="skeleton-block" style="width: 20%"></div>
                <div class="skeleton-block" style="width: 15%"></div>
                <div class="skeleton-block" style="width: 10%"></div>
                <div class="skeleton-block" style="width: 15%"></div>
              </div>
            </td>
          </tr>
        `;
      }
    }

    try {
      let networks;
      if (isRealESP32) {
        const response = await fetch('/scan_wifi');
        const data = await response.json();
        networks = data.map(n => ({
          ssid: n.ssid,
          rssi: n.rssi,
          security: 'WPA2-PSK',
          channel: 1
        }));
      } else {
        await new Promise(r => setTimeout(r, 1500));
        networks = [
          { ssid: 'ESP32_IVR_DEV', rssi: -38, security: 'WPA2-PSK', channel: 1 },
          { ssid: 'Engineering_Lab_STA', rssi: -54, security: 'WPA2-Enterprise', channel: 6 },
          { ssid: 'Campus_Guest_AP', rssi: -72, security: 'OPEN', channel: 11 },
          { ssid: 'Linksys_105', rssi: -84, security: 'WPA2-PSK', channel: 6 }
        ];
      }
      isScanningAp = false;
      if (btn) btn.disabled = false;
      if (refreshBtn) refreshBtn.disabled = false;
      if (spinner) spinner.style.display = 'none';
      if (icon) icon.classList.remove('fa-spin');
      
      renderApWifiScan(networks);
      showToast('WiFi Scan', `AP Mode scanner found ${networks.length} networks.`, 'success');
    } catch (err) {
      isScanningAp = false;
      if (btn) btn.disabled = false;
      if (refreshBtn) refreshBtn.disabled = false;
      if (spinner) spinner.style.display = 'none';
      if (icon) icon.classList.remove('fa-spin');
      showToast('WiFi Scan', 'Scan failed: ' + err.message, 'danger');
    }
    
  } else if (type === 'sta') {
    if (isScanningSta) return;
    isScanningSta = true;
    
    const btn = document.getElementById('btn-scan-wifi-sta');
    const refreshBtn = document.getElementById('btn-refresh-scan-sta');
    const icon = document.getElementById('icon-scan-sta');
    
    if (btn) btn.disabled = true;
    if (refreshBtn) refreshBtn.disabled = true;
    if (icon) icon.classList.add('fa-spin');
    
    const tbody = document.getElementById('scan-body-sta');
    if (tbody) {
      tbody.innerHTML = '';
      for (let i = 0; i < 4; i++) {
        tbody.innerHTML += `
          <tr>
            <td colspan="4">
              <div class="skeleton-row">
                <div class="skeleton-block" style="width: 40%"></div>
                <div class="skeleton-block" style="width: 25%"></div>
                <div class="skeleton-block" style="width: 20%"></div>
                <div class="skeleton-block" style="width: 10%"></div>
              </div>
            </td>
          </tr>
        `;
      }
    }

    try {
      let networks;
      if (isRealESP32) {
        const response = await fetch('/scan_wifi');
        const data = await response.json();
        networks = data.map(n => ({
          ssid: n.ssid,
          rssi: n.rssi,
          security: 'WPA2-PSK',
          channel: 1
        }));
      } else {
        await new Promise(r => setTimeout(r, 1500));
        networks = [
          { ssid: 'Google_Guest_2.4G', rssi: -48, security: 'WPA2/WPA3', channel: 6 },
          { ssid: 'Home_Net_Router', rssi: -62, security: 'WPA2-PSK', channel: 11 },
          { ssid: 'Cafe_Public_WiFi', rssi: -78, security: 'OPEN', channel: 1 },
          { ssid: 'TP-Link_Extender', rssi: -55, security: 'WPA2-PSK', channel: 8 },
          { ssid: 'Netgear_LTE_M', rssi: -85, security: 'WPA2-PSK', channel: 3 }
        ];
      }
      isScanningSta = false;
      if (btn) btn.disabled = false;
      if (refreshBtn) refreshBtn.disabled = false;
      if (icon) icon.classList.remove('fa-spin');
      
      renderStaWifiScan(networks);
      showToast('WiFi Scan', `STA Mode scanner found ${networks.length} networks.`, 'success');
    } catch (err) {
      isScanningSta = false;
      if (btn) btn.disabled = false;
      if (refreshBtn) refreshBtn.disabled = false;
      if (icon) icon.classList.remove('fa-spin');
      showToast('WiFi Scan', 'Scan failed: ' + err.message, 'danger');
    }
  }
}

function renderApWifiScan(networks) {
  const tbody = document.getElementById('scan-body-ap');
  if (!tbody) return;
  tbody.innerHTML = '';
  
  networks.forEach(net => {
    const tr = document.createElement('tr');
    tr.className = 'wifi-network-row';
    tr.setAttribute('data-ssid', net.ssid);
    
    tr.innerHTML = `
      <td><strong class="text-white">${net.ssid}</strong></td>
      <td>${getSignalBarsHtml(net.rssi)}</td>
      <td><span class="badge bg-secondary text-white">${net.security}</span></td>
      <td class="digital-font">${net.channel}</td>
      <td>
        <button class="btn btn-glass-primary btn-xs btn-select-network" data-ssid="${net.ssid}" style="font-size: 0.65rem; padding: 0.2rem 0.4rem;">
          Select
        </button>
      </td>
    `;
    
    tbody.appendChild(tr);
  });
  
  tbody.querySelectorAll('.wifi-network-row').forEach(row => {
    row.addEventListener('click', () => {
      const ssid = row.getAttribute('data-ssid');
      selectWifiNetwork(ssid);
    });
  });
  tbody.querySelectorAll('.btn-select-network').forEach(btn => {
    btn.addEventListener('click', (e) => {
      e.stopPropagation();
      const ssid = btn.getAttribute('data-ssid');
      selectWifiNetwork(ssid);
    });
  });
}

function renderStaWifiScan(networks) {
  const tbody = document.getElementById('scan-body-sta');
  if (!tbody) return;
  tbody.innerHTML = '';
  
  networks.forEach(net => {
    const tr = document.createElement('tr');
    tr.className = 'wifi-network-row';
    tr.setAttribute('data-ssid', net.ssid);
    
    tr.innerHTML = `
      <td><strong class="text-white">${net.ssid}</strong></td>
      <td>${getSignalBarsHtml(net.rssi)}</td>
      <td><span class="badge bg-secondary text-white">${net.security}</span></td>
      <td class="digital-font">${net.channel}</td>
    `;
    
    tbody.appendChild(tr);
  });
  
  tbody.querySelectorAll('.wifi-network-row').forEach(row => {
    row.addEventListener('click', () => {
      const ssid = row.getAttribute('data-ssid');
      selectWifiNetwork(ssid);
    });
  });
}

function getSignalBarsHtml(rssi) {
  let barCount = 1;
  if (rssi > -55) barCount = 4;
  else if (rssi > -70) barCount = 3;
  else if (rssi > -85) barCount = 2;
  
  let html = `<div class="d-flex align-items-center gap-2">
    <span class="small digital-font text-white">${rssi} dBm</span>
    <div class="mini-signal-bars" style="display:inline-flex;">`;
  for (let i = 1; i <= 4; i++) {
    const active = i <= barCount ? 'active' : '';
    html += `<span class="mini-bar ${active}" style="height:${i * 3}px; width:2px; display:inline-block; background:${i <= barCount ? 'var(--color-info)' : 'rgba(255,255,255,0.1)'}; margin-right:1px; border-radius:1px;"></span>`;
  }
  html += `</div></div>`;
  return html;
}

function selectWifiNetwork(ssid) {
  const ssidInput = document.getElementById('fro-ssid');
  const pwdInput = document.getElementById('fro-password');
  
  if (ssidInput) ssidInput.value = ssid;
  
  if (pwdInput) {
    setTimeout(() => pwdInput.focus(), 150);
  }
  
  showToast('WiFi Scanner', `Auto-filled SSID: "${ssid}". Please enter password.`, 'success');
}

let socket = null;

async function loadInitialData() {
  try {
    console.log('Loading initial status and logs from API...');
    await Promise.all([
      simulateApiCall('/api/status'),
      simulateApiCall('/api/calls')
    ]);
    renderAllViews();
    updateSidebarDots();
    applyLogsFilters();
    updateAnalyticsCharts();
  } catch (err) {
    console.error('Error loading initial data:', err);
  }
}

function setupRealPoller() {
  if (wsTimer) clearInterval(wsTimer);
  const wsPill = document.getElementById('ws-pill');
  if (wsPill) wsPill.classList.remove('d-none');
  const wsIcon = document.getElementById('ws-icon');
  const wsText = document.getElementById('ws-text');
  if (wsIcon && wsText) {
    wsIcon.className = 'fa-solid fa-circle-nodes text-success ws-glow';
    wsText.textContent = 'ESP32 Link';
  }
  loadRealESP32Data();
  wsTimer = setInterval(() => { loadRealESP32Data(); }, 3000);
}

async function loadRealESP32Data() {
  try {
    const statusRes = await fetch('/status');
    if (!statusRes.ok) throw new Error('Status HTTP ' + statusRes.status);
    const data = await statusRes.json();
    
    esp32State.networkMode = data.mode || 'AP';
    esp32State.sta.status = data.wifi ? 'CONNECTED' : 'DISCONNECTED';
    esp32State.sta.ssid = data.ssid || '';
    esp32State.sta.ip = data.ip || '—';
    esp32State.sta.rssi = data.rssi || 0;
    esp32State.connectivity.internetAvailable = data.wifi;
    esp32State.connectivity.esp32Reachable = true;

    // Update WiFi status fields dynamically
    const wifiStatusBadge = document.getElementById('wifi-current-status');
    if (wifiStatusBadge) {
      wifiStatusBadge.className = data.wifi ? 'badge bg-success' : 'badge bg-danger';
      wifiStatusBadge.textContent = data.wifi ? 'CONNECTED' : 'DISCONNECTED';
    }
    const wifiCurrentSsid = document.getElementById('wifi-current-ssid');
    if (wifiCurrentSsid) {
      wifiCurrentSsid.textContent = data.ssid || 'Not Connected';
    }

    // Pre-populate AP & Admin inputs once
    if (!window.apAdminLoaded && data.apSsid) {
      const apSsidInput = document.getElementById('ap-input-ssid');
      if (apSsidInput) apSsidInput.value = data.apSsid;
      const apPassInput = document.getElementById('ap-input-pass');
      if (apPassInput) apPassInput.value = data.apPass || '';
      const adminUserInput = document.getElementById('admin-input-user');
      if (adminUserInput) adminUserInput.value = data.devUser || '';
      const adminPassInput = document.getElementById('admin-input-pass');
      if (adminPassInput) adminPassInput.value = data.devPass || '';
      window.apAdminLoaded = true;
    }

    // Pre-populate Sheets inputs once from ESP32 real data
    if (!window.googleSheetsESP32Loaded) {
      if (data.sheetId || data.scriptId) {
        const idInput = document.getElementById('sheets-id');
        if (idInput && data.sheetId) idInput.value = data.sheetId;
        const nameInput = document.getElementById('sheets-name');
        if (nameInput && data.sheetName) nameInput.value = data.sheetName;
        const scriptInput = document.getElementById('sheets-script-url');
        if (scriptInput && data.scriptId) {
          scriptInput.value = `https://script.google.com/macros/s/${data.scriptId}/exec`;
        }
        window.googleSheetsESP32Loaded = true;
      } else if (data.sheetId === '' && data.scriptId === '') {
        window.googleSheetsESP32Loaded = true;
      }
    }

    esp32State.callSystemActive = data.callSystemActive !== undefined ? data.callSystemActive : true;
    esp32State.smsSystemActive = data.smsSystemActive !== undefined ? data.smsSystemActive : true;
    
    if (data.callSystemActive !== undefined) {
      const switchCall = document.getElementById('switch-call-system');
      if (switchCall && !switchCall.dataset.userToggled) {
        switchCall.checked = data.callSystemActive;
        const cardCall = document.getElementById('card-call-system');
        if (cardCall) {
          cardCall.classList.toggle('system-disabled', !data.callSystemActive);
          const statusText = cardCall.querySelector('.control-status-text');
          if (statusText) {
            statusText.innerHTML = data.callSystemActive 
              ? '<i class="fa-solid fa-circle-check"></i> SYSTEM ACTIVE' 
              : '<i class="fa-solid fa-circle-xmark"></i> SYSTEM INACTIVE';
          }
        }
      }
    }
    if (data.smsSystemActive !== undefined) {
      const switchSms = document.getElementById('switch-sms-system');
      if (switchSms && !switchSms.dataset.userToggled) {
        switchSms.checked = data.smsSystemActive;
        const cardSms = document.getElementById('card-sms-system');
        if (cardSms) {
          cardSms.classList.toggle('system-disabled', !data.smsSystemActive);
          const statusText = cardSms.querySelector('.control-status-text');
          if (statusText) {
            statusText.innerHTML = data.smsSystemActive 
              ? '<i class="fa-solid fa-circle-check"></i> SYSTEM ACTIVE' 
              : '<i class="fa-solid fa-circle-xmark"></i> SYSTEM INACTIVE';
          }
        }
      }
    }
    
    esp32State.callerValidationEnabled = data.callerValidationEnabled !== undefined ? data.callerValidationEnabled : false;
    if (data.callerValidationEnabled !== undefined) {
      const switchValidation = document.getElementById('switch-validation-system');
      if (switchValidation && !switchValidation.dataset.userToggled) {
        switchValidation.checked = data.callerValidationEnabled;
        const cardValidation = document.getElementById('card-validation-system');
        if (cardValidation) {
          cardValidation.classList.toggle('system-disabled', !data.callerValidationEnabled);
          const statusText = cardValidation.querySelector('.control-status-text');
          if (statusText) {
            statusText.innerHTML = data.callerValidationEnabled 
              ? '<i class="fa-solid fa-circle-check"></i> VALIDATION ON' 
              : '<i class="fa-solid fa-circle-xmark"></i> VALIDATION OFF';
            statusText.className = data.callerValidationEnabled 
              ? 'control-status-text text-warning mt-2 small fw-bold digital-font' 
              : 'control-status-text text-muted mt-2 small fw-bold digital-font';
          }
        }
      }
    }
    
    esp32State.customIvrEnabled = data.customIvrEnabled !== undefined ? data.customIvrEnabled : false;
    if (data.customIvrEnabled !== undefined) {
      const switchCustomIvr = document.getElementById('switch-custom-ivr');
      if (switchCustomIvr && !switchCustomIvr.dataset.userToggled) {
        switchCustomIvr.checked = data.customIvrEnabled;
        const cardCustomIvr = document.getElementById('card-custom-ivr');
        if (cardCustomIvr) {
          cardCustomIvr.classList.toggle('system-disabled', !data.customIvrEnabled);
          const statusText = cardCustomIvr.querySelector('.control-status-text');
          if (statusText) {
            statusText.innerHTML = data.customIvrEnabled 
              ? '<i class="fa-solid fa-circle-check"></i> IVR FLOW ON' 
              : '<i class="fa-solid fa-circle-xmark"></i> IVR FLOW OFF';
            statusText.className = data.customIvrEnabled 
              ? 'control-status-text text-success mt-2 small fw-bold digital-font' 
              : 'control-status-text text-muted mt-2 small fw-bold digital-font';
          }
        }
      }
    }
    
    esp32State.sim.status = (data.gsm && data.gsm.sim && data.gsm.sim.trim().toUpperCase() === 'READY') ? 'ACTIVE' : 'INACTIVE';
    esp32State.sim.carrier = (data.gsm && data.gsm.operator) ? data.gsm.operator : 'Searching...';
    if (data.gsm && data.gsm.signal) {
      const csq = parseInt(data.gsm.signal);
      esp32State.sim.rssi = (csq && csq !== 99) ? (csq * 2 - 113) : -113;
    } else {
      esp32State.sim.rssi = -113;
    }
    esp32State.sim.imei = (data.gsm && data.gsm.imei) ? data.gsm.imei : 'Unknown';
    esp32State.sim.network = (data.gsm && data.gsm.network) ? data.gsm.network : 'Unknown';
    esp32State.sim.simNumber = (data.gsm && data.gsm.simNumber) ? data.gsm.simNumber : 'Unknown';
    esp32State.sim.regStatus = (data.gsm && data.gsm.registration) ? data.gsm.registration : 'Offline';
    
    if (data.sheetId !== undefined) {
      esp32State.googleSheets.sheetId = data.sheetId;
      if (data.sheetId) {
        localStorage.setItem('vvarma_sheet_id', data.sheetId);
      } else {
        localStorage.removeItem('vvarma_sheet_id');
      }
    } else {
      esp32State.googleSheets.sheetId = localStorage.getItem('vvarma_sheet_id') || '';
    }

    esp32State.googleSheets.sheetName = data.sheetName || esp32State.googleSheets.sheetName || '';

    if (data.scriptId !== undefined) {
      if (data.scriptId) {
        const newUrl = `https://script.google.com/macros/s/${data.scriptId}/exec`;
        const wasEmpty = !esp32State.googleSheets.scriptUrl || !esp32State.googleSheets.scriptUrl.includes('script.google.com');
        esp32State.googleSheets.scriptUrl = newUrl;
        localStorage.setItem('sheets_script_url', newUrl);
        // First-time: auto-fetch VARMA panel data if not already cached
        if (wasEmpty && !localStorage.getItem('vvarma_sheets_cache')) {
          setTimeout(() => fetchVarmaData(), 500);
        }
      } else {
        esp32State.googleSheets.scriptUrl = '';
        localStorage.removeItem('sheets_script_url');
      }
    } else {
      esp32State.googleSheets.scriptUrl = localStorage.getItem('sheets_script_url') || '';
    }
    esp32State.googleSheets.status = data.db ? 'CONNECTED' : 'DISCONNECTED';
    esp32State.googleSheets.lastSyncTime = data.lastSyncTime || '—';
    esp32State.googleSheets.recordsUploaded = data.syncCount || 0;
    esp32State.googleSheets.errorsCount = data.syncErrors || 0;

    if (data.syncIncoming !== undefined) esp32State.googleSheets.syncIncoming = data.syncIncoming;
    if (data.syncOutgoing !== undefined) esp32State.googleSheets.syncOutgoing = data.syncOutgoing;
    if (data.syncSms !== undefined) esp32State.googleSheets.syncSms = data.syncSms;
    if (data.syncSystem !== undefined) esp32State.googleSheets.syncSystem = data.syncSystem;
    if (data.syncNetwork !== undefined) esp32State.googleSheets.syncNetwork = data.syncNetwork;
    
    if (data.uptime !== undefined) {
      systemUptimeSeconds = data.uptime;
    }
    
    if (data.system) {
      esp32State.system.flashSize = data.system.flashSize || esp32State.system.flashSize;
      esp32State.system.freeHeap = data.system.freeHeap || esp32State.system.freeHeap;
      esp32State.system.chipRevision = data.system.chipRevision || esp32State.system.chipRevision;
      esp32State.system.sdkVersion = data.system.sdkVersion || esp32State.system.sdkVersion;
      esp32State.system.firmware = data.system.firmware || esp32State.system.firmware;
      esp32State.system.macSta = data.system.macSta || esp32State.system.macSta;
      esp32State.system.macAp = data.system.macAp || esp32State.system.macAp;
      esp32State.system.temperature = data.system.temperature || esp32State.system.temperature;
      esp32State.system.socModel = data.system.socModel || '';
      esp32State.system.coreFreq = data.system.coreFreq || '';
      esp32State.system.minHeap = data.system.minHeap || 0;
      esp32State.system.rtosTasks = data.system.rtosTasks || 0;
      esp32State.system.cpuUsage = '12%'; // Mock cpu usage as ESP32 doesn't naturally track overall CPU%
    }
    
    if (data.sd) {
      esp32State.sd = esp32State.sd || {};
      esp32State.sd.mounted = data.sd.mounted;
      if (data.sd.mounted) {
        esp32State.sd.size = (data.sd.totalBytes / (1024 * 1024 * 1024)).toFixed(1) + ' GB';
        esp32State.sd.used = (data.sd.usedBytes / (1024 * 1024 * 1024)).toFixed(1) + ' GB';
        esp32State.sd.free = ((data.sd.totalBytes - data.sd.usedBytes) / (1024 * 1024 * 1024)).toFixed(1) + ' GB';
        esp32State.sd.files = data.sd.fileCount || 0;
      }
    }
    
    esp32State.ap.status = (data.mode === 'AP') ? 'Running' : 'Stopped';
    // Use real AP SSID from data if available
    if (data.apSsid) esp32State.ap.ssid = data.apSsid;
    if (esp32State.ap.status === 'Stopped') {
      esp32State.ap.clients = [];
    } else {
      if (data.apClients > 0) {
        // Create dummy client objects to map the count accurately to UI
        esp32State.ap.clients = Array(data.apClients).fill({ ip: '192.168.4.x' });
      } else {
        esp32State.ap.clients = [];
      }
    }
    
    if (data.state === 'RINGING' || data.state === 'VALIDATING') {
      // Show incoming-call popup for a REAL call (only once per call)
      const incomingNum = data.callerNumber || '';
      if (incomingNum && incomingNum !== lastRingingNumber) {
        lastRingingNumber = incomingNum;
        if (data.testModeEnabled && data.state === 'RINGING') {
          showIncomingCallPopup(incomingNum);
        }
        addNotification('📞 Incoming Call', `Ringing from ${incomingNum}`, 'primary');
      }
      // Also update the live-call widget
      const liveCallPopup = document.getElementById('live-call-popup');
      const liveCallNum    = document.getElementById('live-call-number');
      const liveCallStatus = document.getElementById('live-call-status');
      const liveCallDtmf   = document.getElementById('live-call-dtmf');
      if (liveCallPopup) {
        liveCallPopup.classList.remove('d-none');
        if (liveCallNum)    liveCallNum.textContent    = incomingNum || 'Unknown';
        if (liveCallStatus) {
          liveCallStatus.textContent = data.state;
          if (data.state === 'VALIDATING') {
            liveCallStatus.className = 'text-info fw-bold';
          } else {
            liveCallStatus.className = 'text-warning fw-bold';
          }
        }
        if (liveCallDtmf)   liveCallDtmf.textContent   = data.dtmfBuffer || '-';
      }
    } else if (data.state === 'ACTIVE CALL' || data.state === 'COLLECTING DTMF') {
      // Call was answered — dismiss incoming popup, update live widget
      if (incomingCallActive) {
        dismissIncomingCall();
        addNotification('📲 Call Answered', `Active call with ${data.callerNumber || 'Unknown'}`, 'success');
      }
      const liveCallPopup  = document.getElementById('live-call-popup');
      const liveCallNum    = document.getElementById('live-call-number');
      const liveCallStatus = document.getElementById('live-call-status');
      const liveCallDtmf   = document.getElementById('live-call-dtmf');
      if (liveCallPopup) {
        liveCallPopup.classList.remove('d-none');
        if (liveCallNum)    liveCallNum.textContent    = data.callerNumber || 'Unknown';
        if (liveCallStatus) liveCallStatus.textContent = data.state;
        if (liveCallDtmf)   liveCallDtmf.textContent   = data.dtmfBuffer || '-';
      }
    } else {
      // IDLE / ENDED — dismiss popup and hide live widget
      if (incomingCallActive) dismissIncomingCall();
      lastRingingNumber = '';   // Reset for next call
      const liveCallPopup = document.getElementById('live-call-popup');
      if (liveCallPopup && !liveCallPopup.classList.contains('d-none')) {
        liveCallPopup.classList.add('d-none');
      }
    }

    try {
      const logsRes = await fetch('/get_calls');
      if (logsRes.ok) {
        const logsData = await logsRes.json();
        callsData = logsData.map((c, index) => {
          let number = c.phoneNumber || 'Unknown';
          let date = 'Local Session';
          let time = c.timeStr || '';
          if (time.includes('T')) {
            const parts = time.split('T');
            date = parts[0];
            time = parts[1].substring(0, 8);
          }
          let duration = 0;
          let status = c.callStatus || 'Completed';
          if (status === 'Answered') status = 'Completed';
          
          return {
            id: 1001 + index,
            number,
            date,
            time,
            duration,
            status
          };
        });
        callsData.sort((a, b) => b.id - a.id);
        filteredCalls = [...callsData];
      }
    } catch (logErr) {
      console.warn('Error fetching or parsing calls data:', logErr);
    }
    
    // Track ESP32 online/offline transitions for notification
    if (_esp32WasOnline !== true) {
      if (_esp32WasOnline === false) {
        addNotification('🟢 ESP32 Reconnected', 'Device is back online and responding.', 'success');
      }
      _esp32WasOnline = true;
    }

    renderAllViews();
    updateSidebarDots();
    applyLogsFilters();
  } catch (err) {
    console.warn('Error fetching real ESP32 data:', err);
    const wsIcon = document.getElementById('ws-icon');
    const wsText = document.getElementById('ws-text');
    if (wsIcon && wsText) {
      wsIcon.className = 'fa-solid fa-circle-nodes text-danger';
      wsText.textContent = 'Offline';
    }
    // Only notify once on going offline
    if (_esp32WasOnline === true) {
      _esp32WasOnline = false;
      addNotification('🔴 ESP32 Offline', 'Unable to reach device. Check connection.', 'danger');
    }
  }
}

function setupSocketIO() {
  if (typeof io === 'undefined') {
    console.warn('Socket.IO client library not loaded. Falling back to WebSocket Simulator.');
    setupWebSocketSimulator();
    return;
  }
  
  if (wsTimer) { clearInterval(wsTimer); wsTimer = null; }
  
  const wsPill = document.getElementById('ws-pill');
  if (wsPill) wsPill.classList.remove('d-none');
  
  socket = io(BACKEND_URL);
 
  socket.on('connect_error', () => {
    console.warn('⚠️ Socket.IO connection error. Falling back to WebSocket Simulator & Mock API.');
    socket.close();
    useMockApi = true;
    setupWebSocketSimulator();
  });
 
  socket.on('connect', () => {
    console.log('✅ Socket.IO connected to backend');
    const wsIcon = document.getElementById('ws-icon');
    const wsText = document.getElementById('ws-text');
    if (wsIcon && wsText) {
      wsIcon.className = 'fa-solid fa-circle-nodes text-success ws-glow';
      wsText.textContent = 'WS Active';
    }
    showToast('Real-time Connection', 'Successfully connected to ESP32 backend via Socket.IO', 'success');
    addNotification('System Connected', 'WebSocket connection established successfully.', 'success');
    loadInitialData();
  });
 
  socket.on('disconnect', () => {
    console.warn('⚠️ Socket.IO disconnected from backend');
    const wsIcon = document.getElementById('ws-icon');
    const wsText = document.getElementById('ws-text');
    if (wsIcon && wsText) {
      wsIcon.className = 'fa-solid fa-circle-nodes text-danger';
      wsText.textContent = 'WS Inactive';
    }
    addNotification('System Disconnected', 'WebSocket link to ESP32 was lost.', 'danger');
  });
 
  socket.on('dashboard:update', (data) => {
    console.log('Received dashboard:update:', data);
    esp32State.networkMode = data.mode || 'AP';
    esp32State.sta.status = data.wifiStatus || 'DISCONNECTED';
    esp32State.sta.ip = data.ipAddress || '—';
    esp32State.sim.rssi = parseInt(data.signalStrength) || -75;
    esp32State.connectivity.internetAvailable = data.wifiStatus === 'CONNECTED';
    if (data.uptime) {
      systemUptimeSeconds = parseUptimeToSeconds(data.uptime);
    }
    renderAllViews();
    updateSidebarDots();
  });
 
  socket.on('call:new', (call) => {
    console.log('Received call:new:', call);
    const exists = callsData.some(c => c.id === call.id);
    if (!exists) {
      callsData.push({
        id: call.id,
        number: call.callerNumber,
        date: (call.timestamp || '').split('T')[0],
        time: (call.timestamp || '').split('T')[1]?.substring(0, 8) || '',
        duration: call.duration || 0,
        status: call.status
      });
      applyLogsFilters();
      updateAnalyticsCharts();
      updateAllCallCounters();
    }
    addNotification('Incoming Call', `Ringing from ${call.callerNumber || 'Unknown'}...`, 'primary');
    // Show incoming call popup for real socket event (guard against duplicates)
    const incomingNum = call.callerNumber || '';
    if (incomingNum && incomingNum !== lastRingingNumber) {
      lastRingingNumber = incomingNum;
      showIncomingCallPopup(incomingNum);
    }
  });
 
  socket.on('call:end', (data) => {
    console.log('Received call:end:', data);
    const call = callsData.find(c => c.id === data.id);
    if (call) {
      call.duration = data.duration;
      call.status = 'Completed';
      applyLogsFilters();
      updateAnalyticsCharts();
      showToast('Call Completed', `Call with ${call.number} ended. Duration: ${data.duration}s`, 'info');
      addNotification('Call Ended', `Call with ${call.number} ended. Duration: ${data.duration}s`, 'info');
    }
  });
 
  socket.on('sms:new', (sms) => {
    console.log('Received sms:new:', sms);
    showToast('New SMS Received', `From: ${sms.sender}\nMessage: ${sms.message}`, 'info');
    addNotification('SMS Received', `From ${sms.sender}: "${sms.message}"`, 'warning');
  });
 
  socket.on('device:status', (deviceState) => {
    console.log('Received device:status:', deviceState);
    if (deviceState.apStatus) esp32State.ap.status = deviceState.apStatus;
    if (deviceState.staStatus) esp32State.sta.status = deviceState.staStatus;
    if (deviceState.ip) esp32State.sta.ip = deviceState.ip;
    renderAllViews();
    updateSidebarDots();
  });
 
  socket.on('signal:update', (data) => {
    console.log('Received signal:update:', data);
    if (data.rssi) {
      esp32State.sta.rssi = data.rssi;
      updateRssiTrendChart(data.rssi);
      renderAllViews();
    }
  });
}

let simulatorTimer = null;
function setupWebSocketSimulator() {
  if (simulatorTimer) clearInterval(simulatorTimer);
  console.log('[Simulator] WebSocket Simulator started in passive real-time mode.');
  
  // Emulate connection established
  setTimeout(() => {
    const wsIcon = document.getElementById('ws-icon');
    const wsText = document.getElementById('ws-text');
    if (wsIcon && wsText) {
      wsIcon.className = 'fa-solid fa-circle-nodes text-success ws-glow';
      wsText.textContent = 'ESP32 Real-Time';
    }
  }, 1000);
}



function renderStaDashboard() {
  const { ap, sta, connectivity, sim } = esp32State;
  const isAp  = ap.status === 'Running';
  const isSta = sta.status === 'CONNECTED';

  // 1. Device Status
  const staStatus      = document.getElementById('sta-status');
  const staStatusDesc  = document.getElementById('sta-status-desc');
  const staStatusTrend = document.getElementById('sta-status-trend');
  if (staStatus) {
    if (connectivity.esp32Reachable) {
      staStatus.textContent = 'ACTIVE';
      staStatus.className = 'metric-value text-success';
      if (staStatusDesc) staStatusDesc.textContent = 'System online & reachable';
      if (staStatusTrend) {
        staStatusTrend.innerHTML = '<i class="fa-solid fa-circle-check"></i> Healthy';
        staStatusTrend.className = 'metric-trend positive';
      }
    } else {
      staStatus.textContent = 'OFFLINE';
      staStatus.className = 'metric-value text-danger';
      if (staStatusDesc) staStatusDesc.textContent = 'No active connection';
      if (staStatusTrend) {
        staStatusTrend.innerHTML = '<i class="fa-solid fa-triangle-exclamation"></i> Disconnected';
        staStatusTrend.className = 'metric-trend negative text-danger';
      }
    }
  }

  // 2. Internet Status
  const staInternet      = document.getElementById('sta-internet');
  const staInternetDesc  = document.getElementById('sta-internet-desc');
  const staInternetTrend = document.getElementById('sta-internet-trend');
  if (staInternet) {
    if (connectivity.internetAvailable) {
      staInternet.textContent = 'ONLINE';
      staInternet.className = 'metric-value text-success';
      if (staInternetDesc) staInternetDesc.textContent = `Latency: ${connectivity.pingInternet} ms`;
      if (staInternetTrend) {
        staInternetTrend.innerHTML = '<i class="fa-solid fa-cloud text-success"></i> Connected';
        staInternetTrend.className = 'metric-trend positive';
      }
    } else {
      staInternet.textContent = 'OFFLINE';
      staInternet.className = 'metric-value text-danger';
      if (staInternetDesc) staInternetDesc.textContent = 'No WAN network link';
      if (staInternetTrend) {
        staInternetTrend.innerHTML = '<i class="fa-solid fa-cloud-slash text-danger"></i> Disconnected';
        staInternetTrend.className = 'metric-trend negative text-danger';
      }
    }
  }

  // 3. SIM Status
  const staSim      = document.getElementById('sta-sim');
  const staSimDesc  = document.getElementById('sta-sim-desc');
  const staSimTrend = document.getElementById('sta-sim-trend');
  let towerBars = 0;
  if (staSim) {
    if (sim.status === 'ACTIVE') {
      staSim.textContent = 'ACTIVE';
      staSim.className = 'metric-value text-success';
      if (staSimDesc) staSimDesc.textContent = `${sim.carrier} (${sim.rssi} dBm)`;
      if (staSimTrend) {
        staSimTrend.innerHTML = `<i class="fa-solid fa-tower-cell text-success"></i> Registered`;
        staSimTrend.className = 'metric-trend positive';
      }

      const dbm = sim.rssi;
      if (dbm > -70) towerBars = 4;
      else if (dbm > -85) towerBars = 3;
      else if (dbm > -98) towerBars = 2;
      else if (dbm > -110) towerBars = 1;
    } else {
      staSim.textContent = 'NO SIM';
      staSim.className = 'metric-value text-muted';
      if (staSimDesc) staSimDesc.textContent = 'Not inserted or locked';
      if (staSimTrend) {
        staSimTrend.innerHTML = '<i class="fa-solid fa-circle-exclamation text-warning"></i> Sim Error';
        staSimTrend.className = 'metric-trend neutral text-warning';
      }
    }
    updateMiniSignalBars('sta-card-tower-bars', towerBars);
  }

  // 4. WiFi Status
  const staWifiSsid  = document.getElementById('sta-wifi-ssid');
  const staWifiRssi  = document.getElementById('sta-wifi-rssi');
  const staWifiTrend = document.getElementById('sta-wifi-trend');
  let wifiBars = 0;
  if (staWifiSsid) {
    if (isSta) {
      staWifiSsid.textContent = sta.ssid;
      if (staWifiRssi) staWifiRssi.textContent = `RSSI: ${sta.rssi} dBm (${rssiLabel(sta.rssi)})`;
      if (staWifiTrend) {
        staWifiTrend.innerHTML = '<i class="fa-solid fa-circle-check text-success"></i> Connected';
        staWifiTrend.className = 'metric-trend positive';
      }

      const dbm = sta.rssi;
      if (dbm > -50) wifiBars = 4;
      else if (dbm > -70) wifiBars = 3;
      else if (dbm > -85) wifiBars = 2;
      else if (dbm > -95) wifiBars = 1;
    } else if (isAp) {
      staWifiSsid.textContent = 'AP active';
      if (staWifiRssi) staWifiRssi.textContent = `SSID: ${ap.ssid}`;
      if (staWifiTrend) {
        staWifiTrend.innerHTML = '<i class="fa-solid fa-tower-broadcast text-info"></i> Broadcasting';
        staWifiTrend.className = 'metric-trend positive text-info';
      }
    } else {
      staWifiSsid.textContent = 'DISCONNECTED';
      if (staWifiRssi) staWifiRssi.textContent = 'RSSI: — dBm';
      if (staWifiTrend) {
        staWifiTrend.innerHTML = '<i class="fa-solid fa-circle-xmark text-danger"></i> Disconnected';
        staWifiTrend.className = 'metric-trend negative text-danger';
      }
    }
    updateMiniSignalBars('sta-card-wifi-bars', wifiBars);
  }

  // 5. Router Gateway IP
  const gatewayIp = isSta ? sta.gateway : (isAp ? ap.gateway : '—');
  const ipEl = document.getElementById('sta-ip');
  if (ipEl) ipEl.textContent = gatewayIp;
  const ipDesc = document.getElementById('sta-ip-desc');
  if (ipDesc) ipDesc.textContent = isSta ? `Device IP: ${sta.ip}` : 'Default gateway address';

  // 6. Clients
  const clientCount = ap.clients.length;
  const clientsEl = document.getElementById('sta-clients');
  if (clientsEl) clientsEl.textContent = clientCount;
  const clientsBadgeEl = document.getElementById('sta-clients-badge');
  if (clientsBadgeEl) clientsBadgeEl.textContent = `${clientCount} online`;

  // Toggle visibility of AP Clients column card and DHCP Clients section
  const staClientsCol = document.getElementById('sta-card-clients-col');
  if (staClientsCol) staClientsCol.classList.toggle('d-none', !isAp);
  const dhcpClientsRow = document.getElementById('dash-dhcp-clients-row');
  if (dhcpClientsRow) dhcpClientsRow.classList.toggle('d-none', !isAp);

  // 7. Uptime
  const uptimeEl = document.getElementById('sta-uptime');
  if (uptimeEl) uptimeEl.textContent = formatUptimeShort(systemUptimeSeconds);

  // 8. Total calls & Call count badge
  updateAllCallCounters();

  // 9. DHCP client list
  renderStaClientList();
}

function renderStaClientList() {
  const listEl = document.getElementById('sta-client-list');
  if (!listEl) return;
  listEl.innerHTML = '';
  if (esp32State.ap.clients.length === 0) {
    listEl.innerHTML = '<p class="text-muted small text-center py-3 mb-0">No active AP clients connected</p>';
    return;
  }

  esp32State.ap.clients.forEach(c => {
    const ico = c.device === 'laptop' ? 'fa-laptop' : c.device === 'tablet' ? 'fa-tablet-screen-button' : 'fa-mobile-screen-button';
    listEl.insertAdjacentHTML('beforeend', `
      <div class="client-item">
        <div class="client-info">
          <div class="client-avatar"><i class="fa-solid ${ico}"></i></div>
          <div class="client-details">
            <div class="client-name">${c.name}</div>
            <div class="client-ip">${c.ip}</div>
          </div>
        </div>
        <div class="client-meta">
          <div>${c.mac}</div>
          <div>${c.rssi} dBm</div>
        </div>
      </div>
    `);
  });
}

// ============================================================
// GOOGLE SHEETS SYNC MODULE  (STA Mode Dashboard)
// ============================================================

const sheetsSync = {
  rowsUploaded: 0,
  errorsCount:  0,
  autoTimer:    null,
  isRunning:    false,

  /** Build payload from checked fields + current state */
  buildPayload() {
    const { sta, ap, connectivity, sim } = esp32State;
    const checked = id => document.getElementById(id)?.checked;
    const sheetName = document.getElementById('sta-sheet-name')?.value?.trim() || 'STA_Logs';
    const payload   = { sheet: sheetName };

    if (checked('sync-field-timestamp'))  payload.timestamp    = new Date().toISOString();
    if (checked('sync-field-device'))     payload.deviceStatus  = connectivity.esp32Reachable ? 'Online' : 'Offline';
    if (checked('sync-field-wifi'))       payload.wifiSSID      = sta.ssid;
    if (checked('sync-field-wifi'))       payload.wifiRSSI      = `${sta.rssi} dBm`;
    if (checked('sync-field-internet'))   payload.internet      = connectivity.internetAvailable ? 'Available' : 'Unavailable';
    if (checked('sync-field-sim'))        payload.simStatus     = sim.status;
    if (checked('sync-field-sim'))        payload.simCarrier    = sim.carrier;
    if (checked('sync-field-uptime'))     payload.uptime        = formatUptime(systemUptimeSeconds);
    if (checked('sync-field-calls'))      payload.totalCalls    = callsData.length;
    if (checked('sync-field-clients'))    payload.apClients     = ap.clients.length;
    if (checked('sync-field-ip'))         payload.espIP         = sta.ip;
    if (checked('sync-field-ip'))         payload.gateway       = sta.gateway;
    return payload;
  },

  /** Refresh the JSON preview pane */
  refreshPreview() {
    const el = document.getElementById('sta-sheets-payload-preview');
    if (!el) return;
    try {
      el.textContent = JSON.stringify(this.buildPayload(), null, 2);
    } catch (e) { el.textContent = '{}'; }
  },

  /** Update the status pill (connected | error | testing | idle) */
  setStatus(state, text) {
    const dot  = document.getElementById('sta-sheets-dot');
    const txt  = document.getElementById('sta-sheets-status-text');
    if (dot) dot.className = `sheets-sync-dot ${state}`;
    if (txt) txt.textContent = text;
  },

  /** Update the stats row */
  updateStats(httpCode) {
    setEl('sta-sheets-rows',      String(this.rowsUploaded));
    setEl('sta-sheets-errors',    String(this.errorsCount));
    setEl('sta-sheets-http',      httpCode || '—');
    const now = new Date();
    setEl('sta-sheets-last-push', now.toLocaleTimeString());
    const el = document.getElementById('sta-sheets-http');
    if (el) {
      el.className = `sheets-stat-value digital-font ${String(httpCode).startsWith('2') ? 'text-success' : 'text-danger'}`;
    }
  },

  /** Validate URL field */
  getUrl() {
    const url = document.getElementById('sta-script-url')?.value?.trim();
    if (!url) { showToast('Sheets Sync', 'Please enter a valid Apps Script URL first', 'warning'); return null; }
    try { new URL(url); } catch { showToast('Sheets Sync', 'Invalid URL format', 'danger'); return null; }
    localStorage.setItem('sheets_script_url', url);
    return url;
  },

  /** Test connection to Apps Script — real fetch, no mock */
  async testConnection() {
    const url = this.getUrl(); if (!url) return;
    this.setStatus('testing', 'Testing…');
    setLoading('btn-sta-sheets-test', true, '<i class="fa-solid fa-bolt text-warning me-1"></i>Test Connection');
    try {
      if (!url.includes('script.google.com')) throw new Error('URL must be a valid Google Apps Script URL');
      // Use no-cors mode — Apps Script returns opaque response (status 0) on success
      // If the URL is completely unreachable, this will throw a TypeError
      const pingUrl = url + '?action=ping';
      const res = await fetch(pingUrl, { mode: 'no-cors', cache: 'no-cache' });
      // no-cors opaque response = reachable (status 0 is expected)
      this.setStatus('connected', 'Connected ✓');
      showToast('Sheets Sync', '✅ Apps Script URL is reachable — connection healthy', 'success');
      this.updateStats('200');
      addNotification('Sheets Sync', 'Apps Script connection test passed.', 'success');
    } catch (err) {
      this.errorsCount++;
      this.setStatus('error', 'Connection Failed');
      showToast('Sheets Sync', `❌ Could not reach Apps Script: ${err.message || 'timeout'}`, 'danger');
      this.updateStats('ERR');
      addNotification('Sheets Sync Error', 'Connection test failed: ' + err.message, 'danger');
    } finally {
      setLoading('btn-sta-sheets-test', false, '<i class="fa-solid fa-bolt text-warning me-1"></i>Test Connection');
    }
  },

  /** Push one data row to Google Sheets — real POST, no mock */
  async pushNow() {
    if (this.isRunning) return;
    const url = this.getUrl(); if (!url) return;
    if (!url.includes('script.google.com')) {
      showToast('Sheets Sync', 'URL must be a valid Google Apps Script URL', 'danger');
      return;
    }

    this.isRunning = true;
    this.setStatus('testing', 'Pushing…');
    setLoading('btn-sta-sheets-push', true, '<i class="fa-solid fa-upload me-2"></i>Push Data to Sheet Now');

    const payload = this.buildPayload();
    try {
      // Real POST to Google Apps Script — must use no-cors due to CORS restrictions
      await fetch(url, {
        method: 'POST',
        mode: 'no-cors',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
      // no-cors POST returns opaque response with status 0 — if no error thrown, it was sent
      this.rowsUploaded++;
      this.setStatus('connected', 'Synced ✓');
      this.updateStats('200');
      this.refreshPreview();
      showToast('Sheets Sync', `📊 Row #${this.rowsUploaded} pushed to "${payload.sheet}"`, 'success');
      addNotification('Sheets Push', `Row pushed to "${payload.sheet}" successfully.`, 'success');
    } catch (err) {
      this.errorsCount++;
      this.setStatus('error', 'Push Failed');
      this.updateStats('ERR');
      showToast('Sheets Sync', `❌ Push failed: ${err.message}`, 'danger');
      addNotification('Sheets Push Error', 'Push failed: ' + err.message, 'danger');
    } finally {
      this.isRunning = false;
      setLoading('btn-sta-sheets-push', false, '<i class="fa-solid fa-upload me-2"></i>Push Data to Sheet Now');
    }
  },

  /** Start / stop auto-push interval */
  setAutoSync(enabled) {
    if (this.autoTimer) { clearInterval(this.autoTimer); this.autoTimer = null; }
    if (!enabled) { showToast('Auto Sync', 'Auto-sync disabled', 'info'); return; }

    const secs = parseInt(document.getElementById('sta-sync-interval')?.value || '60', 10);
    if (!secs) { showToast('Auto Sync', 'Set an interval other than "Manual only"', 'warning'); document.getElementById('sta-sheets-auto-sync').checked = false; return; }
    this.autoTimer = setInterval(() => this.pushNow(), secs * 1000);
    showToast('Auto Sync', `⏱ Auto-pushing every ${secs >= 60 ? secs/60 + ' min' : secs + 's'}`, 'success');
  },

  /** Restore previously saved URL from localStorage */
  restore() {
    const saved = localStorage.getItem('sheets_script_url');
    if (saved) {
      const el = document.getElementById('sta-script-url');
      if (el) el.value = saved;
    }
    this.refreshPreview();
  }
};

// Bind Sheets Sync buttons on DOM ready (called from bindEventHandlers scope)
(function bindSheetsSyncHandlers() {
  document.addEventListener('DOMContentLoaded', () => {
    sheetsSync.restore();

    bindClick('btn-sta-sheets-test',    () => sheetsSync.testConnection());
    bindClick('btn-sta-sheets-push',    () => sheetsSync.pushNow());
    bindClick('btn-sta-sheets-preview', () => sheetsSync.refreshPreview());

    // Checkbox change → refresh preview
    document.querySelectorAll('[id^="sync-field-"]').forEach(cb => {
      cb.addEventListener('change', () => sheetsSync.refreshPreview());
    });

    // Auto-sync toggle
    const autoToggle = document.getElementById('sta-sheets-auto-sync');
    if (autoToggle) {
      autoToggle.addEventListener('change', () => sheetsSync.setAutoSync(autoToggle.checked));
    }

    // URL field → save on blur
    const urlInput = document.getElementById('sta-script-url');
    if (urlInput) {
      urlInput.addEventListener('blur', () => {
        const v = urlInput.value.trim();
        if (v) localStorage.setItem('sheets_script_url', v);
        sheetsSync.refreshPreview();
      });
    }
  });
})();

// ============================================================
// EXTERNAL INTEGRATION FETCHING & TELECOM ANIMATIONS
// ============================================================

async function fetchExternalIntegrations() {
  // Telegram Bot Data Fetching
  try {
    const dashTelegramStatus = document.getElementById('dash-telegram-status');
    const dashTelegramBadge = document.getElementById('dash-telegram-badge');
    if (isRealESP32) {
      const res = await fetch('/api/telegram/status');
      if (res.ok) {
        const data = await res.json();
        if (dashTelegramStatus) dashTelegramStatus.textContent = data.messages > 0 ? `Processing (${data.messages})` : 'Listening';
        if (dashTelegramBadge) dashTelegramBadge.textContent = 'Active';
      }
    } else {
      // Mock logic
      if (dashTelegramStatus) dashTelegramStatus.textContent = 'Listening (Mock)';
      if (dashTelegramBadge) dashTelegramBadge.textContent = 'Active';
    }
  } catch (err) {
    console.error('Telegram fetch error', err);
  }

  // Google Sheets integration status from local state to prevent CORS issues
  try {
    const dashFirebaseStatus = document.getElementById('dash-firebase-status');
    const dashFirebaseBadge = document.getElementById('dash-firebase-badge');
    
    if (esp32State.googleSheets.status === 'CONNECTED') {
      if (dashFirebaseStatus) dashFirebaseStatus.textContent = 'Synced';
      if (dashFirebaseBadge) {
        dashFirebaseBadge.textContent = 'Live';
        dashFirebaseBadge.className = 'badge bg-success';
      }
    } else {
      const hasUrl = localStorage.getItem('sheets_script_url') || esp32State.googleSheets.scriptUrl;
      if (dashFirebaseStatus) {
        dashFirebaseStatus.textContent = hasUrl ? 'Offline' : 'Unconfigured';
      }
      if (dashFirebaseBadge) {
        dashFirebaseBadge.textContent = 'Offline';
        dashFirebaseBadge.className = 'badge bg-danger';
      }
    }
  } catch (err) {
    console.warn('Google Sheets status update failed', err);
  }
}

// Live Telecom Animations (Pulsing Nodes & Toasts)
function simulateTelecomActivity() {
  setInterval(() => {
    // Pulse ESP32 Core Node
    const espNode = document.getElementById('topo-node-esp32');
    if (espNode) {
      espNode.style.transform = 'scale(1.1)';
      espNode.style.boxShadow = '0 0 20px rgba(0, 255, 255, 0.6)';
      setTimeout(() => {
        espNode.style.transform = 'scale(1)';
        espNode.style.boxShadow = 'none';
      }, 300);
    }

    // Pulse Link Services Arrow
    const servicesArrow = document.querySelector('#topo-link-services .topo-link-arrow i');
    if (servicesArrow) {
      servicesArrow.classList.add('fa-beat');
      setTimeout(() => servicesArrow.classList.remove('fa-beat'), 800);
    }

    const sdArrow = document.querySelector('#topo-link-sd .topo-link-arrow i');
    if (sdArrow) {
      sdArrow.classList.add('fa-beat');
      setTimeout(() => sdArrow.classList.remove('fa-beat'), 800);
    }

    // Occasional simulated incoming call toast removed for live data
  }, 4000); // Check every 4s for activity
}

// ============================================================
// WEB SERIAL TERMINAL
// ============================================================
let serialInterval = null;

function initSerialTerminal() {
  const termOut = document.getElementById('terminal-output');
  const termIn = document.getElementById('terminal-input');
  const btnSend = document.getElementById('btn-term-send');
  const btnClear = document.getElementById('btn-term-clear');
  const autoscroll = document.getElementById('term-autoscroll');

  if (!termOut || !termIn || !btnSend) return;

  function appendToTerminal(text, type = 'rx') {
    const div = document.createElement('div');
    if (type === 'tx') {
      div.className = 'text-warning';
      div.textContent = `> ${text}`;
    } else if (type === 'err') {
      div.className = 'text-danger';
      div.textContent = `[ERROR] ${text}`;
    } else {
      div.className = 'text-success';
      div.textContent = text;
    }
    termOut.appendChild(div);
    if (autoscroll && autoscroll.checked) {
      termOut.scrollTop = termOut.scrollHeight;
    }
    // Also feed to the Hardware Debug console in real time
    if (typeof logToHardwareDebugConsole === 'function') {
      logToHardwareDebugConsole(text);
    }
  }

  async function pollSerial() {
    if (!isRealESP32) return;
    try {
      const res = await fetch('/serial_data');
      if (res.ok) {
        const text = await res.text();
        if (text && text.trim().length > 0) {
          const lines = text.split('\n');
          lines.forEach(line => {
            if (line.trim().length > 0) appendToTerminal(line, 'rx');
          });
        }
      }
    } catch (e) {
      // ignore network errors for polling
    }
  }

  async function sendAT() {
    const cmd = termIn.value.trim();
    if (!cmd) return;
    appendToTerminal(cmd, 'tx');
    termIn.value = '';
    
    if (!isRealESP32) {
      appendToTerminal("Simulated OK", 'rx');
      return;
    }

    try {
      const res = await fetch('/send_at', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: `cmd=${encodeURIComponent(cmd)}`
      });
      if (!res.ok) {
        appendToTerminal("Failed to send command", 'err');
      }
    } catch (e) {
      appendToTerminal(e.message, 'err');
    }
  }

  btnSend.addEventListener('click', sendAT);
  termIn.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') sendAT();
  });
  if (btnClear) {
    btnClear.addEventListener('click', () => {
      termOut.innerHTML = '';
    });
  }

  if (isRealESP32) {
    serialInterval = setInterval(pollSerial, 1500);
  }
}

// Start intervals on DOM loaded
document.addEventListener('DOMContentLoaded', () => {
  setInterval(fetchExternalIntegrations, 15000); // Fetch external data every 15s
  setTimeout(fetchExternalIntegrations, 2000);   // Initial fetch
  setTimeout(simulateTelecomActivity, 3000);     // Start telecom animations
  initSerialTerminal();
});

// ============================================================
// SD CARD AND IVR CONFIGURATOR HANDLERS
// ============================================================

function setupSDAndIVR() {
  const btnRefreshSD = document.getElementById('btn-refresh-sd');
  if (btnRefreshSD) btnRefreshSD.addEventListener('click', loadSDManagerData);

  const btnUploadAudio = document.getElementById('btn-upload-audio');
  if (btnUploadAudio) btnUploadAudio.addEventListener('click', uploadAudio);

  const btnSaveIVR = document.getElementById('btn-save-ivr');
  if (btnSaveIVR) btnSaveIVR.addEventListener('click', saveIVRConfig);

  const btnReloadIVR = document.getElementById('btn-reload-ivr');
  if (btnReloadIVR) btnReloadIVR.addEventListener('click', loadIVRConfig);

  const btnTestSD = document.getElementById('btn-test-sd');
  if (btnTestSD) btnTestSD.addEventListener('click', testTFCard);

  const btnWipeSD = document.getElementById('btn-wipe-sd');
  if (btnWipeSD) btnWipeSD.addEventListener('click', wipeTFCard);
}

async function loadSDManagerData() {
  if (currentSection !== 'sd-manager') return;
  
  try {
    const statusRes = isRealESP32 ? await (await fetch('/sd_status')).json() : await simulateApiCall('/sd_status');
    document.getElementById('sd-status-text').textContent = statusRes.found ? 'Mounted' : 'Not Found';
    if (statusRes.found) {
      document.getElementById('sd-used-space').textContent = (statusRes.used / (1024*1024)).toFixed(2) + ' MB';
      document.getElementById('sd-total-space').textContent = (statusRes.total / (1024*1024)).toFixed(2) + ' MB';
    } else {
      document.getElementById('sd-used-space').textContent = '—';
      document.getElementById('sd-total-space').textContent = '—';
    }

    const files = isRealESP32 ? await (await fetch('/sd_files')).json() : await simulateApiCall('/sd_files');
    const tbody = document.getElementById('sd-files-table');
    if (!files || files.length === 0) {
      tbody.innerHTML = '<tr><td colspan="3" class="text-center text-muted">No files found on TF card.</td></tr>';
    } else {
      tbody.innerHTML = '';
      files.forEach(f => {
        const isAudio = f.name.endsWith('.amr') || f.name.endsWith('.wav') || f.name.endsWith('.mp3');
        const icon = isAudio ? 'fa-file-audio text-info' : 'fa-file-lines text-warning';
        const tr = document.createElement('tr');
        tr.innerHTML = `
          <td><i class="fa-solid ${icon} me-2"></i> ${f.name}</td>
          <td>${(f.size / 1024).toFixed(1)} KB</td>
          <td class="text-end">
            <button class="btn btn-sm btn-glass-info me-1" onclick="previewFile('${f.name}')" title="Preview File"><i class="fa-solid fa-eye"></i></button>
            <button class="btn btn-sm btn-glass-danger" onclick="deleteAudio('${f.name}')" title="Delete File"><i class="fa-solid fa-trash"></i></button>
          </td>
        `;
        tbody.appendChild(tr);
      });
    }
  } catch(e) {
    console.error('Failed to load SD data:', e);
  }
}

async function testTFCard() {
  const btn = document.getElementById('btn-test-sd');
  const alertEl = document.getElementById('sd-test-alert');
  const msgEl = document.getElementById('sd-test-msg');
  
  if (btn) btn.disabled = true;
  if (alertEl) {
    alertEl.classList.remove('d-none', 'alert-success', 'alert-danger');
    alertEl.classList.add('alert-info');
  }
  if (msgEl) msgEl.textContent = 'Running diagnostics on TF Card...';
  
  try {
    if (isRealESP32) {
      const res = await fetch('/sd_test');
      if (!res.ok) throw new Error('HTTP ' + res.status);
      const data = await res.json();
      if (data.success) {
        if (alertEl) {
          alertEl.classList.remove('alert-info');
          alertEl.classList.add('alert-success');
        }
        if (msgEl) msgEl.textContent = `Success: ${data.message}`;
        showToast('TF Card Test', 'Read/Write diagnostic test passed!', 'success');
      } else {
        if (alertEl) {
          alertEl.classList.remove('alert-info');
          alertEl.classList.add('alert-danger');
        }
        if (msgEl) msgEl.textContent = `Failed: ${data.error}`;
        showToast('TF Card Test', `Diagnostic failed: ${data.error}`, 'danger');
      }
    } else {
      await new Promise(resolve => setTimeout(resolve, 1500));
      if (alertEl) {
        alertEl.classList.remove('alert-info');
        alertEl.classList.add('alert-success');
      }
      if (msgEl) msgEl.textContent = 'Success: [Mock] Read/Write test passed on simulated TF Card.';
      showToast('TF Card Test', '[Mock] Diagnostic test passed!', 'success');
    }
  } catch (e) {
    if (alertEl) {
      alertEl.classList.remove('alert-info');
      alertEl.classList.add('alert-danger');
    }
    if (msgEl) msgEl.textContent = `Error: ${e.message}`;
    showToast('TF Card Test', `Network error: ${e.message}`, 'danger');
  } finally {
    if (btn) btn.disabled = false;
    loadSDManagerData();
  }
}

async function wipeTFCard() {
  if (!confirm('⚠️ WARNING: This will delete ALL files stored in the root folder of the TF Card. This action CANNOT be undone. Are you sure you want to proceed?')) {
    return;
  }
  const btn = document.getElementById('btn-wipe-sd');
  if (btn) btn.disabled = true;
  
  try {
    if (isRealESP32) {
      const res = await fetch('/sd_wipe', { method: 'POST' });
      if (!res.ok) throw new Error('HTTP ' + res.status);
      const data = await res.json();
      if (data.success) {
        showToast('TF Card Wiped', data.message, 'success');
        addNotification('TF Card Wiped', data.message, 'warning');
      } else {
        showToast('TF Card Wipe Failed', data.error, 'danger');
      }
    } else {
      await new Promise(resolve => setTimeout(resolve, 1000));
      showToast('TF Card Wiped', '[Mock] Simulated TF Card wiped successfully.', 'success');
    }
  } catch (e) {
    showToast('TF Card Wipe Failed', 'Error: ' + e.message, 'danger');
  } finally {
    if (btn) btn.disabled = false;
    loadSDManagerData();
  }
}

async function previewFile(filename) {
  const modalEl = document.getElementById('file-preview-modal');
  const titleEl = document.getElementById('preview-filename-title');
  const contentEl = document.getElementById('file-preview-content');
  
  if (!modalEl || !titleEl || !contentEl) return;
  
  titleEl.textContent = filename;
  contentEl.textContent = 'Loading file content from TF card...';
  
  const modal = new bootstrap.Modal(modalEl);
  modal.show();
  
  try {
    const res = await fetch(`/read_file?file=${encodeURIComponent(filename)}`);
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const content = await res.text();
    contentEl.textContent = content || '(Empty File)';
  } catch (e) {
    contentEl.textContent = `Error loading file: ${e.message}`;
  }
}

window.previewFile = previewFile;

async function uploadAudio() {
  const fileInput = document.getElementById('audio-upload-file');
  if (!fileInput.files || fileInput.files.length === 0) {
    showToast('SD Manager', 'Please select a file to upload.', 'warning');
    return;
  }
  const file = fileInput.files[0];

  let targetPath = document.getElementById('audio-upload-path').value.trim();
  if (!targetPath) {
    targetPath = file.name;
  }
  if (!targetPath.startsWith('/')) {
    targetPath = '/' + targetPath;
  }

  const formData = new FormData();
  formData.append('file', file, targetPath);

  setLoading('btn-upload-audio', true, 'Uploading...');
  try {
    const res = await fetch('/upload_audio', {
      method: 'POST',
      body: formData
    });
    if (res.ok) {
      showToast('SD Manager', 'File uploaded successfully.', 'success');
      loadSDManagerData();
      fileInput.value = '';
      document.getElementById('audio-upload-path').value = '';
    } else {
      showToast('SD Manager', 'Upload failed.', 'danger');
    }
  } catch(e) {
    showToast('SD Manager', 'Network error during upload.', 'danger');
  } finally {
    setLoading('btn-upload-audio', false, '<i class="fa-solid fa-cloud-arrow-up me-2"></i>Upload');
  }
}

async function deleteAudio(filename) {
  if (!confirm(`Are you sure you want to delete ${filename}?`)) return;
  try {
    const formData = new URLSearchParams();
    formData.append('file', filename);
    const res = await fetch('/delete_audio', { method: 'POST', body: formData });
    if (res.ok) {
      showToast('SD Manager', 'File deleted.', 'success');
      loadSDManagerData();
    } else {
      showToast('SD Manager', 'Delete failed.', 'danger');
    }
  } catch(e) {
    showToast('SD Manager', 'Network error.', 'danger');
  }
}

async function loadIVRConfig() {
  try {
    const res = await simulateApiCall('/get_ivr_config');
    const ta = document.getElementById('ivr-config-json');
    if (ta && Object.keys(res).length > 0) {
      ta.value = JSON.stringify(res, null, 2);
    }
    showToast('IVR Config', 'Configuration loaded from device.', 'info');
  } catch(e) {
    console.error('Failed to load IVR Config:', e);
  }
}

async function saveIVRConfig() {
  const ta = document.getElementById('ivr-config-json');
  if (!ta) return;
  try {
    // Validate JSON
    JSON.parse(ta.value);
  } catch(e) {
    showToast('IVR Config', 'Invalid JSON syntax. Please fix before saving.', 'danger');
    return;
  }

  setLoading('btn-save-ivr', true, 'Saving...');
  try {
    if (isRealESP32) {
      const formData = new URLSearchParams();
      formData.append('config', ta.value);
      const res = await fetch('/save_ivr_config', { method: 'POST', body: formData });
      if (res.ok) {
        showToast('IVR Config', 'Configuration saved to SD card.', 'success');
      } else {
        showToast('IVR Config', 'Failed to save configuration.', 'danger');
      }
    } else {
      await new Promise(resolve => setTimeout(resolve, 800));
      showToast('IVR Config', '[Mock] Configuration saved successfully to simulated SD card.', 'success');
    }
  } catch(e) {
    showToast('IVR Config', 'Network error: ' + e.message, 'danger');
  } finally {
    setLoading('btn-save-ivr', false, '<i class="fa-solid fa-floppy-disk me-2"></i>Save Configuration');
  }
}

document.addEventListener('DOMContentLoaded', () => {
  setupSDAndIVR();
  
  // Tie load actions to navigation clicks
  const sdNav = document.getElementById('nav-sd-manager');
  if (sdNav) sdNav.addEventListener('click', () => setTimeout(loadSDManagerData, 200));

  const ivrNav = document.getElementById('nav-ivr-configurator');
  if (ivrNav) ivrNav.addEventListener('click', () => setTimeout(loadIVRConfig, 200));
});

// ============================================================
// GATEKEEPER, TELEGRAM & SYNC LOGIC
// ============================================================

document.addEventListener('DOMContentLoaded', () => {
  setupLoginGatekeeper();
  setupTelegramConfig();
  setupSyncConfig();
  if (isRealESP32) {
    fetchGlobalConfig();
  }
});

function setupLoginGatekeeper() {
  const overlay = document.getElementById('login-overlay');
  const btnLogin = document.getElementById('btn-login-submit');
  
  if (!overlay || !btnLogin) return;
  
  // Check local storage for token
  const token = localStorage.getItem('vvarma_auth_token') === 'AUTH_VALID' || localStorage.getItem('ivr_logged_in') === '1';
  if (token) {
    overlay.classList.add('d-none');
  } else {
    overlay.classList.remove('d-none');
  }
}

function setupTelegramConfig() {
  const btnSave = document.getElementById('btn-save-telegram');
  if (!btnSave) return;
  
  btnSave.addEventListener('click', async () => {
    const token = document.getElementById('tg-token').value.trim();
    const chatId = document.getElementById('tg-chat-id').value.trim();
    
    btnSave.disabled = true;
    btnSave.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Saving...';
    
    try {
      const formData = new URLSearchParams();
      formData.append('token', token);
      formData.append('chatId', chatId);
      
      const res = await fetch('/api/telegram-config', { method: 'POST', body: formData });
      if (res.ok) {
        showToast('Telegram', 'Configuration saved successfully', 'success');
      } else {
        showToast('Telegram', 'Failed to save configuration', 'danger');
      }
    } catch (e) {
      showToast('Error', 'Connection failed', 'danger');
    }
    btnSave.disabled = false;
    btnSave.innerHTML = 'Save Configuration';
  });
}

function setupSyncConfig() {
  const btnSave = document.getElementById('btn-save-sync-config');
  if (!btnSave) return;
  
  btnSave.addEventListener('click', async () => {
    const inc = document.getElementById('sync-incoming').checked;
    const out = document.getElementById('sync-outgoing').checked;
    const sms = document.getElementById('sync-sms').checked;
    const sys = document.getElementById('sync-system').checked;
    const net = document.getElementById('sync-network').checked;
    
    btnSave.disabled = true;
    btnSave.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Saving...';
    
    try {
      const formData = new URLSearchParams();
      formData.append('syncIncoming', inc);
      formData.append('syncOutgoing', out);
      formData.append('syncSms', sms);
      formData.append('syncSystem', sys);
      formData.append('syncNetwork', net);
      
      const res = await fetch('/api/sync-config', { method: 'POST', body: formData });
      if (res.ok) {
        showToast('Sync Config', 'Trigger settings saved', 'success');
      } else {
        showToast('Sync Config', 'Failed to save', 'danger');
      }
    } catch (e) {
      showToast('Error', 'Connection failed', 'danger');
    }
    btnSave.disabled = false;
    btnSave.innerHTML = '<i class="fa-solid fa-floppy-disk me-2"></i>Save Configuration';
  });
}

async function fetchGlobalConfig() {
  try {
    const res = await fetch('/api/get-config');
    if (res.ok) {
      const data = await res.json();
      const tgToken = document.getElementById('tg-token');
      const tgChat = document.getElementById('tg-chat-id');
      if (tgToken) tgToken.value = data.tgToken || '';
      if (tgChat) tgChat.value = data.tgChatId || '';
      
      const syncInc = document.getElementById('sync-incoming');
      const syncOut = document.getElementById('sync-outgoing');
      const syncSms = document.getElementById('sync-sms');
      const syncSys = document.getElementById('sync-system');
      const syncNet = document.getElementById('sync-network');
      
      if (syncInc) syncInc.checked = data.syncIncoming;
      if (syncOut) syncOut.checked = data.syncOutgoing;
      if (syncSms) syncSms.checked = data.syncSms;
      if (syncSys) syncSys.checked = data.syncSystem;
      if (syncNet) syncNet.checked = data.syncNetwork;
    }
  } catch(e) {
    console.error('Failed to fetch config', e);
  }
}

// ============================================================
// VARMA PANEL LOGIC
// ============================================================

document.addEventListener('DOMContentLoaded', () => {
  const btnReload = document.getElementById('btn-varma-reload');
  const btnClear  = document.getElementById('btn-varma-clear');
  const btnInit   = document.getElementById('btn-varma-init');

  if (btnReload) btnReload.addEventListener('click', () => fetchVarmaData());

  if (btnClear) {
    btnClear.addEventListener('click', () => {
      localStorage.removeItem('vvarma_sheets_cache');
      clearVarmaUI();
      showToast('VARMA Panel', '🗑️ Local cache cleared', 'warning');
    });
  }

  if (btnInit) {
    btnInit.addEventListener('click', async () => {
      const isConfigured = localStorage.getItem('sheets_script_url') || (typeof esp32State !== 'undefined' && esp32State.googleSheets?.sheetId);
      if (!isConfigured) { showToast('VARMA Panel', '⚠️ No Sheets URL configured', 'warning'); return; }
      btnInit.disabled = true;
      btnInit.innerHTML = '<i class="fa-solid fa-spinner fa-spin me-2"></i>Initializing...';
      try {
        if (isRealESP32) {
          const controller = new AbortController();
          const timeoutId = setTimeout(() => controller.abort(), 50000); // 50s timeout
          
          const res = await fetch('/init_varma_sheets', { signal: controller.signal });
          clearTimeout(timeoutId);
          if (!res.ok) throw new Error('HTTP ' + res.status);
          const result = await res.json();
          if (result.success) {
            showToast('VARMA Panel', '✅ Sheets initialized successfully', 'success');
            addNotification('Sheets Initialized', 'Google Sheets schema has been set up and is ready.', 'success');
            fetchVarmaData();
          } else {
            showToast('VARMA Panel', '❌ Init failed: ' + (result.message || 'Unknown error'), 'danger');
          }
        } else {
          // Mock mode
          await new Promise(resolve => setTimeout(resolve, 1500));
          showToast('VARMA Panel', '✅ [Mock] Sheets initialized successfully', 'success');
          addNotification('Sheets Initialized', '[Mock] Google Sheets schema has been set up and is ready.', 'success');
          fetchVarmaData();
        }
      } catch (e) {
        showToast('VARMA Panel', '❌ Network error during init: ' + e.message, 'danger');
      } finally {
        btnInit.disabled = false;
        btnInit.innerHTML = '<i class="fa-solid fa-database me-2"></i>Init Sheets';
      }
    });
  }

  // Load cached Sheets data if it's fresh and from a real source (not mock)
  const cached = localStorage.getItem('vvarma_sheets_cache');
  if (cached) {
    try {
      const data = JSON.parse(cached);
      // Invalidate mock-era cache: if it contains the hardcoded demo phone numbers, clear it
      const mockPhone = '+919876543210';
      const isMock = (data.registeredNumbers || []).some(r => r.number === mockPhone && r.name === 'GSV Support Line');
      // Also invalidate if cache is older than 30 minutes
      const cacheAge = data.fetchedAt ? (Date.now() - new Date(data.fetchedAt).getTime()) : Infinity;
      const isFresh = cacheAge < 30 * 60 * 1000;
      if (!isMock && isFresh) {
        populateVarmaPanel(data);
      } else {
        localStorage.removeItem('vvarma_sheets_cache');
      }
    } catch (e) {
      localStorage.removeItem('vvarma_sheets_cache');
    }
  }

  // Auto-fetch fresh data from Sheets only if a real script URL is configured
  const scriptUrlForCheck = localStorage.getItem('sheets_script_url') || '';
  const hasRealUrl = scriptUrlForCheck && scriptUrlForCheck.includes('script.google.com');
  if (hasRealUrl) {
    fetchVarmaData();
  } else if (isRealESP32) {
    // On real ESP32: wait for /status to populate scriptUrl (setupRealPoller handles it)
    // Don't auto-fetch now; user can click Reload after connectivity is established
    clearVarmaUI();
  } else {
    clearVarmaUI();
  }
});

function clearVarmaUI() {
  // Reset top cards
  ['vp-complaints-filed', 'vp-complaints-solved', 'vp-tech-messaged', 'vp-hc-messages', 'vp-cust-received-messages'].forEach(id => {
    const el = document.getElementById(id);
    if (el) el.innerText = '0';
  });

  // Reset grid counters
  ['vp-total-calls', 'vp-attended-calls', 'vp-missed-calls', 'vp-dialed-calls', 'vp-unknown-numbers', 'vp-known-numbers', 'vp-messages-sent', 'vp-messages-received'].forEach(id => {
    const el = document.getElementById(id);
    if (el) el.innerText = '0';
  });

  // Reset tables
  ['table-call-history', 'table-messages', 'table-unknown-numbers', 'table-registered-numbers', 'table-complaints', 'table-warranty-details'].forEach(tId => {
    const tbody = document.querySelector(`#${tId} tbody`);
    if (tbody) tbody.innerHTML = '<tr><td colspan="10" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr>';
  });
}

// No mock data — VARMA Panel loads exclusively from Google Sheets.
// Use the "Reload from Cloud" button or configure the Sheets Script URL in
// Device Mgmt → Google Sheets URL to enable live data.

async function fetchVarmaData() {
  if (window.varmaSyncInProgress) {
    console.warn('fetchVarmaData already in progress, skipping concurrent call.');
    return;
  }
  window.varmaSyncInProgress = true;

  const btnReload = document.getElementById('btn-varma-reload');
  if (btnReload) {
    btnReload.disabled = true;
    btnReload.innerHTML = '<i class="fa-solid fa-spinner fa-spin me-2"></i>Loading...';
  }

  const hasCache = localStorage.getItem('vvarma_sheets_cache') !== null;

  if (!hasCache) {
    // Wipe UI counters temporarily only if no cache exists
    ['vp-complaints-filed', 'vp-complaints-solved', 'vp-tech-messaged', 'vp-hc-messages', 'vp-cust-received-messages',
     'vp-total-calls', 'vp-attended-calls', 'vp-missed-calls', 'vp-dialed-calls', 'vp-unknown-numbers', 'vp-known-numbers', 'vp-messages-sent', 'vp-messages-received'].forEach(id => {
      const el = document.getElementById(id);
      if (el) el.innerText = '—';
    });

    ['table-call-history', 'table-messages', 'table-unknown-numbers', 'table-registered-numbers', 'table-complaints', 'table-warranty-details'].forEach(tId => {
      const tbody = document.querySelector(`#${tId} tbody`);
      if (tbody) tbody.innerHTML = '<tr><td colspan="10" class="text-center text-muted py-3"><i class="fa-solid fa-spinner fa-spin me-2"></i>Fetching from Cloud…</td></tr>';
    });
  }

  try {
    // Strict check: need a real Apps Script URL, not just any truthy value
    const configuredUrl = localStorage.getItem('sheets_script_url') || esp32State.googleSheets?.scriptUrl || '';
    const hasRealUrl = configuredUrl && configuredUrl.includes('script.google.com');
    if (!hasRealUrl && isRealESP32) {
      throw new Error('Google Sheets Script URL not configured. Go to Settings → Google Sheets to set it up.');
    }

    let data;
    if (isRealESP32) {
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), 55000); // 55s timeout
      
      const response = await fetch('/get_varma_data', { signal: controller.signal });
      clearTimeout(timeoutId);
      if (!response.ok) throw new Error(`HTTP ${response.status}`);
      data = await response.json();
    } else {
      // Mock mode
      await new Promise(resolve => setTimeout(resolve, 1500));
      data = {
        ok: true,
        spreadsheetId: 'MOCK_SPREADSHEET_ID_12345',
        fetchedAt: new Date().toISOString(),
        topCards: {
          complaintsFiled: 12,
          complaintsSolved: 8,
          techMessaged: 5,
          hcMessages: 140,
          custReceivedMessages: 95
        },
        gridCounters: {
          totalCalls: 154,
          attendedCalls: 120,
          missedCalls: 34,
          dialedCalls: 45,
          unknownCount: 22,
          knownNumbers: 89,
          messagesSent: 210,
          messagesReceived: 185
        },
        callHistory: [
          { Call_ID: "CALL-01", Number: "+919876543210", Date: "2026-06-15", Time: "10:15:30", Duration: "45", Status: "Answered", Type: "Incoming", DTMF_Input: "1" },
          { Call_ID: "CALL-02", Number: "+919998887770", Date: "2026-06-15", Time: "11:20:05", Duration: "0", Status: "Busy", Type: "Outgoing", DTMF_Input: "No Input" }
        ],
        messagesList: [
          { Message_ID: "MSG-01", Number: "+919876543210", Date: "2026-06-15", Time: "10:16:00", Message: "Warranty registered successfully", Direction: "Outgoing", Status: "Sent" }
        ],
        warrantyList: [
          { "Registration Date": "2026-06-15", "Warranty ID": "W-999", "Customer Name": "Rajesh Kumar", "Mobile Number": "+919876543210", "Product Model": "VV-100", "Status": "Active" }
        ],
        complaintsList: [
          { "Complaint ID": "C-001", "Customer Name": "Rajesh Kumar", "Mobile Number": "+919876543210", "Problem Description": "Motor not starting", "Status": "Open" }
        ],
        registeredMobiles: [
          { Phone: "+919876543210", Name: "Rajesh Kumar", Product: "VV-100", Serial: "SN-9988", Status: "Active" }
        ]
      };
    }

    if (data.ok === false) throw new Error(data.error || 'Apps Script error');

    if (data.spreadsheetId) {
      esp32State.googleSheets.sheetId = data.spreadsheetId;
      localStorage.setItem('vvarma_sheet_id', data.spreadsheetId);
    }

    localStorage.setItem('vvarma_sheets_cache', JSON.stringify(data));
    populateVarmaPanel(data);
    showToast('VARMA Panel', '✅ Data synced from Google Sheets', 'success');

  } catch (error) {
    console.warn('Fetch VARMA Data Error:', error);
    
    // Clear cache and UI indicators on failure
    localStorage.removeItem('vvarma_sheets_cache');
    clearVarmaUI();

    const configuredUrl = localStorage.getItem('sheets_script_url') || esp32State.googleSheets?.scriptUrl || '';
    const isConfiguredCheck = configuredUrl && configuredUrl.includes('script.google.com');
    if (!isConfiguredCheck) {
      showToast('VARMA Panel', '⚠️ Google Sheets Script URL not configured. Go to Settings → Google Sheets integration to set it up.', 'warning');
    } else {
      showToast('VARMA Panel', '❌ Cloud sync failed: ' + error.message, 'danger');
      addNotification('VARMA Panel Sync Failure', 'Sync failed: ' + error.message, 'danger');
    }
  } finally {
    window.varmaSyncInProgress = false;
    if (btnReload) {
      btnReload.disabled = false;
      btnReload.innerHTML = '<i class="fa-solid fa-cloud-arrow-down me-2"></i>Reload from Cloud';
    }
  }
}

function populateVarmaPanel(data) {
  // 1. Populate top cards
  const tc = data.topCards || {};
  document.getElementById('vp-complaints-filed').innerText     = tc.complaintsFiled      || 0;
  document.getElementById('vp-complaints-solved').innerText    = tc.complaintsSolved     || 0;
  document.getElementById('vp-tech-messaged').innerText        = tc.techMessaged         || 0;
  document.getElementById('vp-hc-messages').innerText          = tc.hcMessages           || 0;
  document.getElementById('vp-cust-received-messages').innerText = tc.custReceivedMessages || 0;

  // 2. Populate grid counters
  const gc = data.gridCounters || {};
  document.getElementById('vp-total-calls').innerText     = gc.totalCalls      || 0;
  document.getElementById('vp-attended-calls').innerText  = gc.attendedCalls   || 0;
  document.getElementById('vp-missed-calls').innerText    = gc.missedCalls     || 0;
  document.getElementById('vp-dialed-calls').innerText    = gc.dialedCalls     || 0;
  document.getElementById('vp-unknown-numbers').innerText = gc.unknownNumbers  || 0;
  document.getElementById('vp-known-numbers').innerText   = gc.knownNumbers    || 0;
  document.getElementById('vp-messages-sent').innerText   = gc.messagesSent    || 0;
  document.getElementById('vp-messages-received').innerText = gc.messagesReceived || 0;

  // Show fetch timestamp if available
  const tsEl = document.getElementById('vp-last-sync-time');
  if (tsEl && data.fetchedAt) {
    tsEl.textContent = 'Last synced: ' + new Date(data.fetchedAt).toLocaleString();
  }

  // Helper for populating tables
  const populateTable = (tableId, arr, columns) => {
    const tbody = document.querySelector(`#${tableId} tbody`);
    if (!tbody) return;
    tbody.innerHTML = '';
    if (!arr || !arr.length) {
      tbody.innerHTML = `<tr><td colspan="${columns.length}" class="text-center text-muted py-4">No data available</td></tr>`;
      return;
    }
    arr.forEach(rowObj => {
      const tr = document.createElement('tr');
      columns.forEach(col => {
        const td = document.createElement('td');
        td.innerText = rowObj[col] !== undefined ? rowObj[col] : '-';
        tr.appendChild(td);
      });
      tbody.appendChild(tr);
    });
  };

  // 3. Populate sub-tabs — use empty arrays if data missing (no mock fallback)
  populateTable('table-call-history',    data.callHistories    || [], ['date', 'number', 'type', 'status']);
  populateTable('table-messages',        data.messages         || [], ['date', 'number', 'message', 'status']);
  populateTable('table-unknown-numbers', data.unknownNumbers   || [], ['number', 'date', 'count']);
  populateTable('table-registered-numbers', data.registeredNumbers || [], ['name', 'number', 'product', 'serial', 'status']);
  populateTable('table-complaints',      data.complaints       || [], ['id', 'date', 'name', 'number', 'desc', 'status']);
  populateTable('table-warranty-details',data.warranties       || [], ['date', 'name', 'number', 'product', 'serial', 'expiry']);

  // Also update registeredCustomers for incoming call lookup
  if (data.registeredNumbers && data.registeredNumbers.length > 0) {
    registeredCustomers = data.registeredNumbers.map(r => ({
      phone:   r.number,
      name:    r.name,
      product: r.product,
      serial:  r.serial,
      status:  r.status
    }));
    sessionStorage.setItem('registered_customers', JSON.stringify(registeredCustomers));
  }
}

function getMockVarmaData() {
  return {
    ok: true,
    fetchedAt: new Date().toISOString(),
    sources: {
      ivrCallHistory:  'IVR_Calls',
      ivrMessages:     'IVR_Messages',
      warrantyRows:    3,
      complaintRows:   2,
      partyRows:       3
    },
    topCards: {
      complaintsFiled:      2,
      complaintsSolved:     1,
      techMessaged:         1,
      hcMessages:           3,
      custReceivedMessages: 3
    },
    gridCounters: {
      totalCalls:       5,
      attendedCalls:    3,
      missedCalls:      2,
      dialedCalls:      1,
      unknownNumbers:   1,
      knownNumbers:     3,
      messagesSent:     3,
      messagesReceived: 0
    },
    callHistories: [
      { date: '2026-06-15 10:12:34', number: '+919876543210', type: 'Incoming', status: 'Answered', name: 'GSV Support Line', dtmf: '1', duration: '45' },
      { date: '2026-06-15 11:45:12', number: '+919876543211', type: 'Incoming', status: 'Answered', name: 'Tech Dept Hotline', dtmf: '2', duration: '120' },
      { date: '2026-06-15 12:20:05', number: '+919876543212', type: 'Incoming', status: 'Missed', name: 'Sales Enquiries', dtmf: '', duration: '0' },
      { date: '2026-06-15 12:30:15', number: '+919884733449', type: 'Incoming', status: 'Rejected/Unregistered', name: 'Unknown', dtmf: '', duration: '0' },
      { date: '2026-06-15 13:05:40', number: '+919876543210', type: 'Outgoing', status: 'Dialing Callback', name: 'GSV Support Line', dtmf: 'CALLBACK', duration: '30' }
    ],
    messages: [
      { date: '2026-06-15 10:13:00', number: '+919876543210', message: 'Welcome to V-Varma IVR.', status: 'Sent' },
      { date: '2026-06-15 11:46:00', number: '+919876543211', message: 'Your support ticket has been created.', status: 'Sent' },
      { date: '2026-06-15 12:21:00', number: '+919876543212', message: 'Missed call alert: representative will call back.', status: 'Sent' }
    ],
    unknownNumbers: [
      { number: '+919884733449', date: '2026-06-15', count: 1 }
    ],
    registeredNumbers: [
      { name: 'GSV Support Line', number: '+919876543210', product: 'IVR Controller V3', serial: 'SN-VARMA-001', status: 'Active Warranty' },
      { name: 'Tech Dept Hotline', number: '+919876543211', product: 'IVR Controller V3', serial: 'SN-VARMA-002', status: 'Active Warranty' },
      { name: 'Sales Enquiries', number: '+919876543212', product: 'IVR Gateway Pro', serial: 'SN-VARMA-003', status: 'Expired Warranty' }
    ],
    complaints: [
      { id: 'CMP-1001', date: '2026-06-15', name: 'GSV Support Line', number: '+919876543210', desc: 'Device not connecting to WiFi', status: 'Solved' },
      { id: 'CMP-1002', date: '2026-06-15', name: 'Tech Dept Hotline', number: '+919876543211', desc: 'Incoming calls get dropped', status: 'Logged' }
    ],
    warranties: [
      { date: '2025-06-15', name: 'GSV Support Line', number: '+919876543210', product: 'IVR Controller V3', serial: 'SN-VARMA-001', expiry: '2026-06-15' },
      { date: '2025-06-15', name: 'Tech Dept Hotline', number: '+919876543211', product: 'IVR Controller V3', serial: 'SN-VARMA-002', expiry: '2026-06-15' }
    ]
  };
}

function logCallLocally(number, status, dtmf, duration, direction = 'Incoming') {
  const today = new Date();
  const dateStr = today.toISOString().split('T')[0];
  const timeStr = today.toTimeString().split(' ')[0];
  const newId = callsData.length > 0 ? Math.max(...callsData.map(c => c.id)) + 1 : 1001;
  
  const newCall = {
    id: newId,
    number: number,
    date: dateStr,
    time: timeStr,
    duration: parseInt(duration || 0),
    status: status,
    type: direction
  };
  
  callsData.unshift(newCall);
  if (callsData.length > 200) {
    callsData = callsData.slice(0, 200);
  }
  
  applyLogsFilters();
  updateAnalyticsCharts();
  updateAllCallCounters();
}

// ============================================================
// INCOMING CALL POPUP CONTROLLER
// ============================================================

/**
 * Normalise phone numbers for comparison:
 *   +91XXXXXXXXXX  →  91XXXXXXXXXX
 *   09XXXXXXXXX    →  9XXXXXXXXX   (strip leading 0)
 */
function normalisePhone(raw) {
  let n = String(raw || '').replace(/[\s\-().]/g, '');
  if (n.startsWith('+')) n = n.slice(1);
  if (n.startsWith('0'))  n = n.slice(1);
  return n;
}

function lookupRegisteredCustomer(phone) {
  const norm = normalisePhone(phone);
  return registeredCustomers.find(c => normalisePhone(c.phone) === norm) || null;
}

/** Show the incoming-call popup with optional customer info */
function showIncomingCallPopup(phone) {
  if (incomingCallActive) return;   // don't stack popups
  incomingCallActive = true;

  const customer = lookupRegisteredCustomer(phone);

  // Status badge
  const badge = customer
    ? `<span class="ic-badge ic-badge-registered"><i class="fa-solid fa-circle-check me-1"></i>Registered Customer</span>`
    : `<span class="ic-badge ic-badge-unknown"><i class="fa-solid fa-circle-question me-1"></i>Unknown Number</span>`;

  // Customer info block
  const infoBlock = customer ? `
    <div class="ic-customer-info">
      <div class="ic-info-row"><span class="ic-info-label">Name</span><span class="ic-info-val">${customer.name}</span></div>
      <div class="ic-info-row"><span class="ic-info-label">Product</span><span class="ic-info-val">${customer.product}</span></div>
      <div class="ic-info-row"><span class="ic-info-label">Serial</span><span class="ic-info-val">${customer.serial}</span></div>
      <div class="ic-info-row"><span class="ic-info-label">Warranty</span><span class="ic-info-val ${customer.status === 'Active Warranty' ? 'text-success' : 'text-danger'}">${customer.status}</span></div>
    </div>` : `
    <div class="ic-customer-info">
      <div class="text-muted small">No registration found for this number.<br>You can add them via Help Center.</div>
    </div>`;

  // Build popup
  const existing = document.getElementById('ic-popup');
  if (existing) existing.remove();

  const div = document.createElement('div');
  div.id = 'ic-popup';
  div.className = 'ic-popup ic-popup-show';
  div.innerHTML = `
    <div class="ic-popup-header">
      <div class="ic-ring-anim"><i class="fa-solid fa-phone-volume"></i></div>
      <div class="ic-header-text">
        <div class="ic-title">Incoming Call</div>
        <div class="ic-phone digital-font">${phone}</div>
        ${badge}
      </div>
      <button class="ic-close-btn" onclick="dismissIncomingCall()"><i class="fa-solid fa-xmark"></i></button>
    </div>
    ${infoBlock}
    <div class="ic-actions">
      <button class="ic-btn ic-btn-answer" onclick="icAnswer('${phone}')"><i class="fa-solid fa-phone me-2"></i>Answer</button>
      <button class="ic-btn ic-btn-decline" onclick="icDecline('${phone}')"><i class="fa-solid fa-phone-slash me-2"></i>Decline</button>
    </div>
    <div class="ic-extra-actions">
      <button class="ic-fwd-btn ic-sms-btn" onclick="icSendSMS('${phone}')"><i class="fa-solid fa-comment-sms me-1"></i>SMS — Customer Wants to Speak</button>
    </div>
    <div class="ic-timer-bar"><div class="ic-timer-fill" id="ic-timer-fill"></div></div>
  `;
  document.body.appendChild(div);

  // Auto-dismiss after 30 s with shrinking progress bar
  const fill = document.getElementById('ic-timer-fill');
  if (fill) {
    fill.style.width = '100%';
    fill.style.transition = 'width 30s linear';
    requestAnimationFrame(() => { if (fill) fill.style.width = '0%'; });
  }
  let remaining = 30;
  incomingCallTimer = setInterval(() => {
    remaining--;
    if (remaining <= 0) dismissIncomingCall();
  }, 1000);
}

function dismissIncomingCall() {
  if (incomingCallTimer) { clearInterval(incomingCallTimer); incomingCallTimer = null; }
  const popup = document.getElementById('ic-popup');
  if (popup) {
    popup.classList.remove('ic-popup-show');
    popup.classList.add('ic-popup-hide');
    setTimeout(() => { popup.remove(); }, 400);
  }
  incomingCallActive = false;
  lastRingingNumber  = '';   // Allow next real call to show popup
}

function icAnswer(phone) {
  showToast('Call Answered', `Connecting to ${phone}…`, 'success');
  fetch('/answer');
  dismissIncomingCall();
}

function icDecline(phone) {
  showToast('Call Declined', `Missed call from ${phone}`, 'warning');
  fetch('/hangup');
  dismissIncomingCall();
}

function icSendSMS(phone) {
  const msg = `Hello! A customer at ${phone} would like to speak with a representative. Please call back as soon as possible.`;
  showToast('SMS Sent', 'Help Center notified that customer wants to speak.', 'success');
  fetch(`/api/send_sms?number=${encodeURIComponent(phone)}&message=${encodeURIComponent(msg)}`, { method: 'POST' });
  dismissIncomingCall();
}

// ============================================================
// (scheduleNextIncomingCallDemo removed — popups are
//  triggered by real ESP32 events only via socket/poller)
// ============================================================

// ============================================================
// HELP CENTER NUMBERS MANAGER
// ============================================================

function saveHelpCenterNumbers() {
  localStorage.setItem('help_center_numbers', JSON.stringify(helpCenterNumbers));
}

async function fetchHelpCenterNumbers() {
  try {
    const url = localStorage.getItem('sheets_script_url') || (typeof esp32State !== 'undefined' && esp32State.googleSheets?.scriptUrl) || '';
    if (!url) return;
    const fetchUrl = `${url}?action=get_hc_numbers`;
    const res = await fetch(fetchUrl);
    const data = await res.json();
    if (Array.isArray(data)) {
      helpCenterNumbers = data.map(r => ({
        id: String(r.ID || r.id || ''),
        number: String(r.Number || r.number || ''),
        name: String(r.Name || r.name || ''),
        role: String(r.Role || r.role || ''),
        forwardEnabled: String(r.ForwardEnabled || r.forwardEnabled || '').trim().toLowerCase() === 'true',
        smsEnabled: String(r.SmsEnabled || r.smsEnabled || '').trim().toLowerCase() === 'true'
      }));
      renderHelpCenterNumbers();
      console.log(`[HelpCenter] Loaded ${helpCenterNumbers.length} numbers from Sheets`);
    }
  } catch (e) {
    console.warn('[HelpCenter] Could not load Help Center numbers:', e.message);
  }
}

function renderHelpCenterNumbers() {
  const tbody = document.getElementById('hc-numbers-tbody');
  if (!tbody) return;
  tbody.innerHTML = '';
  if (!helpCenterNumbers.length) {
    tbody.innerHTML = '<tr><td colspan="6" class="text-center text-muted py-4">No Help Center numbers added yet.</td></tr>';
    return;
  }
  helpCenterNumbers.forEach((h, idx) => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td class="digital-font text-info">${h.number}</td>
      <td>${h.name}</td>
      <td><span class="badge bg-secondary">${h.role}</span></td>
      <td><span class="badge ${h.forwardEnabled ? 'bg-success' : 'bg-dark text-muted'}">${h.forwardEnabled ? 'Yes' : 'No'}</span></td>
      <td><span class="badge ${h.smsEnabled ? 'bg-info text-dark' : 'bg-dark text-muted'}">${h.smsEnabled ? 'Yes' : 'No'}</span></td>
      <td>
        <button class="btn btn-sm btn-outline-warning btn-hc-edit me-1" data-index="${idx}" title="Edit"><i class="fa-solid fa-pencil"></i></button>
        <button class="btn btn-sm btn-outline-danger btn-hc-delete" data-index="${idx}" title="Delete"><i class="fa-solid fa-trash"></i></button>
      </td>`;
    tbody.appendChild(tr);
  });

  // Bind edit events dynamically to guarantee compatibility
  tbody.querySelectorAll('.btn-hc-edit').forEach(btn => {
    btn.addEventListener('click', (e) => {
      const idx = parseInt(btn.getAttribute('data-index'));
      editHelpCenterNumber(idx);
    });
  });

  // Bind delete events dynamically to guarantee compatibility
  tbody.querySelectorAll('.btn-hc-delete').forEach(btn => {
    btn.addEventListener('click', (e) => {
      const idx = parseInt(btn.getAttribute('data-index'));
      deleteHelpCenterNumber(idx);
    });
  });
}

window.editHelpCenterNumber = editHelpCenterNumber;
window.deleteHelpCenterNumber = deleteHelpCenterNumber;

async function addHelpCenterNumber() {
  const num  = (document.getElementById('hc-new-number')?.value || '').trim();
  const name = (document.getElementById('hc-new-name')?.value   || '').trim();
  const role = (document.getElementById('hc-new-role')?.value   || 'Support').trim();
  const fwd  = document.getElementById('hc-new-forward')?.checked ?? true;
  const sms  = document.getElementById('hc-new-sms')?.checked   ?? true;
  if (!num || !name) { showToast('Help Center', 'Please enter both number and name.', 'warning'); return; }
  
  const tempId = String(Date.now());
  showToast('Help Center', `Adding "${name}" to Google Sheets…`, 'info');
  
  helpCenterNumbers.push({ id: tempId, name, number: num, role, forwardEnabled: fwd, smsEnabled: sms });
  renderHelpCenterNumbers();

  try {
    const scriptUrl = localStorage.getItem('sheets_script_url') || (typeof esp32State !== 'undefined' && esp32State.googleSheets?.scriptUrl) || '';
    if (scriptUrl) {
      const params = new URLSearchParams();
      params.append('action', 'save_hc_number');
      params.append('id', tempId);
      params.append('number', num);
      params.append('name', name);
      params.append('role', role);
      params.append('forwardEnabled', fwd ? 'true' : 'false');
      params.append('smsEnabled', sms ? 'true' : 'false');
      
      const res = await fetch(`${scriptUrl}?${params.toString()}`);
      const resJson = await res.json();
      if (resJson.ok) {
        showToast('Help Center', `"${name}" added successfully to Sheets.`, 'success');
      } else {
        throw new Error(resJson.error || 'Server error');
      }
    } else {
      saveHelpCenterNumbers();
      showToast('Help Center', `"${name}" saved to local storage (No script URL).`, 'warning');
    }
  } catch(e) {
    console.error('[HelpCenter] Add error:', e);
    showToast('Help Center Error', 'Failed to save to Google Sheets', 'danger');
  }
  
  ['hc-new-number','hc-new-name','hc-new-role'].forEach(id => { const el = document.getElementById(id); if (el) el.value = ''; });
}

async function deleteHelpCenterNumber(idx) {
  const h = helpCenterNumbers[idx];
  if (!h) return;
  if (!confirm(`Remove "${h.name}" from Help Center?`)) return;
  
  showToast('Help Center', `Deleting "${h.name}" from Google Sheets…`, 'info');
  
  helpCenterNumbers.splice(idx, 1);
  renderHelpCenterNumbers();

  try {
    const scriptUrl = localStorage.getItem('sheets_script_url') || (typeof esp32State !== 'undefined' && esp32State.googleSheets?.scriptUrl) || '';
    if (scriptUrl && h.id) {
      const res = await fetch(`${scriptUrl}?action=delete_hc_number&id=${encodeURIComponent(h.id)}`);
      const resJson = await res.json();
      if (resJson.ok) {
        showToast('Help Center', 'Number removed from Sheets.', 'warning');
      } else {
        throw new Error(resJson.error || 'Server error');
      }
    } else {
      saveHelpCenterNumbers();
      showToast('Help Center', 'Number removed from local storage (No script URL).', 'warning');
    }
  } catch(e) {
    console.error('[HelpCenter] Delete error:', e);
    showToast('Help Center Error', 'Failed to delete from Google Sheets', 'danger');
  }
}

async function editHelpCenterNumber(idx) {
  const h = helpCenterNumbers[idx];
  if (!h) return;
  const newNum  = prompt('Phone number:', h.number); if (newNum === null) return;
  const newName = prompt('Display name:', h.name);   if (newName === null) return;
  const newRole = prompt('Role/Dept:', h.role);       if (newRole === null) return;
  
  const updated = { ...h, number: newNum.trim(), name: newName.trim(), role: newRole.trim() };
  helpCenterNumbers[idx] = updated;
  renderHelpCenterNumbers();
  
  showToast('Help Center', `Updating "${updated.name}" in Google Sheets…`, 'info');

  try {
    const scriptUrl = localStorage.getItem('sheets_script_url') || (typeof esp32State !== 'undefined' && esp32State.googleSheets?.scriptUrl) || '';
    if (scriptUrl && h.id) {
      const params = new URLSearchParams();
      params.append('action', 'save_hc_number');
      params.append('id', h.id);
      params.append('number', updated.number);
      params.append('name', updated.name);
      params.append('role', updated.role);
      params.append('forwardEnabled', updated.forwardEnabled ? 'true' : 'false');
      params.append('smsEnabled', updated.smsEnabled ? 'true' : 'false');

      const res = await fetch(`${scriptUrl}?${params.toString()}`);
      const resJson = await res.json();
      if (resJson.ok) {
        showToast('Help Center', 'Number updated in Sheets.', 'success');
      } else {
        throw new Error(resJson.error || 'Server error');
      }
    } else {
      saveHelpCenterNumbers();
      showToast('Help Center', 'Number updated locally (No script URL).', 'success');
    }
  } catch(e) {
    console.error('[HelpCenter] Edit error:', e);
    showToast('Help Center Error', 'Failed to update in Google Sheets', 'danger');
  }
}

function testCallTo(number, name) {
  showToast('Test Call', `Initiating hardware call to ${name || number}…`, 'info');
  fetch(`/api/dial?number=${encodeURIComponent(number)}`, { method: 'POST' })
    .then(res => res.json())
    .then(data => {
      if (data.status === 'success') {
        showToast('Test Call', 'Call dial sequence started.', 'success');
      } else {
        showToast('Test Call', 'Dial failed: ' + data.message, 'danger');
      }
    })
    .catch(err => {
      showToast('Test Call', 'Connection failed: ' + err.message, 'danger');
    });
}

// ============================================================
// SCADA SVG FLOW LINES
// ============================================================
function drawScadaLines() {
  const wrapper = document.querySelector('.topology-map-wrapper');
  if (!wrapper) return;
  
  let svg = document.getElementById('topo-svg-overlay');
  if (!svg) return;
  
  // Clear existing paths
  svg.innerHTML = '';
  
  const drawPath = (id1, id2, linkId) => {
    const el1 = document.getElementById(id1);
    const el2 = document.getElementById(id2);
    if (!el1 || !el2) return;
    
    // Find the icon circle inside the node for precise centering
    const icon1 = el1.querySelector('.topo-node-icon') || el1;
    const icon2 = el2.querySelector('.topo-node-icon') || el2;

    const r1 = icon1.getBoundingClientRect();
    const r2 = icon2.getBoundingClientRect();
    const svgR = svg.getBoundingClientRect();
    
    // Center coords relative to SVG
    const x1 = r1.left + r1.width/2 - svgR.left;
    const y1 = r1.top + r1.height/2 - svgR.top;
    const x2 = r2.left + r2.width/2 - svgR.left;
    const y2 = r2.top + r2.height/2 - svgR.top;
    
    // Cubic bezier for a smooth SCADA pipe curve
    const d = `M ${x1},${y1} C ${x1},${(y1+y2)/2} ${x2},${(y1+y2)/2} ${x2},${y2}`;
    
    // Animated flow path
    const path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
    path.setAttribute('d', d);
    path.setAttribute('id', linkId);
    path.setAttribute('class', 'topo-link scada-path-flow status-inactive'); 
    svg.appendChild(path);

    // Create warning hazard symbol group exactly at midpoint
    const xMid = (x1 + x2) / 2;
    const yMid = (y1 + y2) / 2;

    const warnGroup = document.createElementNS('http://www.w3.org/2000/svg', 'g');
    warnGroup.setAttribute('class', 'scada-warn-symbol');
    warnGroup.setAttribute('transform', `translate(${xMid}, ${yMid})`);

    const polygon = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
    polygon.setAttribute('points', '-12,8 0,-12 12,8');

    const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
    text.setAttribute('y', '4');
    text.setAttribute('text-anchor', 'middle');
    text.textContent = '!';

    warnGroup.appendChild(polygon);
    warnGroup.appendChild(text);
    svg.appendChild(warnGroup);
  };
  
  // Connections
  drawPath('topo-node-tower', 'topo-node-gsm', 'topo-link-tower-gsm');
  drawPath('topo-node-gsm', 'topo-node-esp32', 'topo-link-gsm-esp');
  drawPath('topo-node-router', 'topo-node-esp32', 'topo-link-sta-esp');
  drawPath('topo-node-ivr', 'topo-node-esp32', 'topo-link-ivr-esp');
  drawPath('topo-node-esp32', 'topo-node-sheets', 'topo-link-sheets-esp');
  drawPath('topo-node-esp32', 'topo-node-firebase', 'topo-link-firebase-esp');
  drawPath('topo-node-esp32', 'topo-node-telegram', 'topo-link-telegram-esp');
  drawPath('topo-node-clients', 'topo-node-esp32', 'topo-link-clients-esp');
  drawPath('topo-node-sd', 'topo-node-esp32', 'topo-link-sd-esp');
  
  // Reapply current statuses to the newly created SVG paths
  setTimeout(updateNetworkTopology, 50);
}

window.addEventListener('resize', () => {
  const topoLink = document.querySelector('.sidebar-link[data-target="system-mode"]');
  if (topoLink && topoLink.classList.contains('active')) {
    drawScadaLines();
  }
});

// Call once on init after a brief delay for rendering layout
setTimeout(drawScadaLines, 500);

function initGsmControls() {
  const switchCall = document.getElementById('switch-call-system');
  const switchSms = document.getElementById('switch-sms-system');
  const switchValidation = document.getElementById('switch-validation-system');
  const switchCustomIvr = document.getElementById('switch-custom-ivr');
  const cardCall = document.getElementById('card-call-system');
  const cardSms = document.getElementById('card-sms-system');
  const cardValidation = document.getElementById('card-validation-system');
  const cardCustomIvr = document.getElementById('card-custom-ivr');

  if (!switchCall || !switchSms) return;

  const updateCardUI = (card, active, type = 'system') => {
    if (!card) return;
    card.classList.toggle('system-disabled', !active);
    const statusText = card.querySelector('.control-status-text');
    if (statusText) {
      if (type === 'validation') {
        statusText.innerHTML = active 
          ? '<i class="fa-solid fa-circle-check"></i> VALIDATION ON' 
          : '<i class="fa-solid fa-circle-xmark"></i> VALIDATION OFF';
        statusText.className = active 
          ? 'control-status-text text-warning mt-2 small fw-bold digital-font' 
          : 'control-status-text text-muted mt-2 small fw-bold digital-font';
      } else if (type === 'ivr') {
        statusText.innerHTML = active 
          ? '<i class="fa-solid fa-circle-check"></i> IVR FLOW ON' 
          : '<i class="fa-solid fa-circle-xmark"></i> IVR FLOW OFF';
        statusText.className = active 
          ? 'control-status-text text-success mt-2 small fw-bold digital-font' 
          : 'control-status-text text-muted mt-2 small fw-bold digital-font';
      } else {
        statusText.innerHTML = active 
          ? '<i class="fa-solid fa-circle-check"></i> SYSTEM ACTIVE' 
          : '<i class="fa-solid fa-circle-xmark"></i> SYSTEM INACTIVE';
        statusText.className = active 
          ? 'control-status-text text-success mt-2 small fw-bold digital-font' 
          : 'control-status-text text-danger mt-2 small fw-bold digital-font';
      }
    }
  };

  let saveSettingsTimeout = null;
  const saveSettings = () => {
    const callActive = switchCall.checked;
    const smsActive = switchSms.checked;
    const validationActive = switchValidation ? switchValidation.checked : false;
    const ivrActive = switchCustomIvr ? switchCustomIvr.checked : false;
    
    updateCardUI(cardCall, callActive);
    updateCardUI(cardSms, smsActive);
    updateCardUI(cardValidation, validationActive, 'validation');
    updateCardUI(cardCustomIvr, ivrActive, 'ivr');

    if (saveSettingsTimeout) clearTimeout(saveSettingsTimeout);
    saveSettingsTimeout = setTimeout(async () => {
      try {
        const formData = new URLSearchParams();
        formData.append('callSystemActive', callActive ? 'true' : 'false');
        formData.append('smsSystemActive', smsActive ? 'true' : 'false');
        formData.append('callerValidationEnabled', validationActive ? 'true' : 'false');
        formData.append('customIvrEnabled', ivrActive ? 'true' : 'false');
        
        await fetch('/api/gsm-settings', {
          method: 'POST',
          body: formData
        });
        showToast('GSM Control', `Settings saved to device EEPROM.`, 'success');
      } catch (err) {
        console.error('Error saving GSM settings:', err);
      }
    }, 300);
  };

  switchCall.addEventListener('change', () => {
    switchCall.dataset.userToggled = 'true';
    saveSettings();
  });
  switchSms.addEventListener('change', () => {
    switchSms.dataset.userToggled = 'true';
    saveSettings();
  });
  if (switchValidation) {
    switchValidation.addEventListener('change', () => {
      switchValidation.dataset.userToggled = 'true';
      saveSettings();
    });
  }
  if (switchCustomIvr) {
    switchCustomIvr.addEventListener('change', () => {
      switchCustomIvr.dataset.userToggled = 'true';
      saveSettings();
    });
  }
}

// Bind Add button & GSM controls
document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('btn-hc-add')?.addEventListener('click', addHelpCenterNumber);
  initGsmControls();
});

// ============================================================
// SIMULATOR & TEST PANEL LOGIC
// ============================================================
// Expose globally for serial poller to feed logs
window.logToHardwareDebugConsole = function(msg) {
  const consoleLogs = document.getElementById('sim-console-logs');
  if (!consoleLogs) return;
  const now = new Date();
  const timeStr = now.toTimeString().split(' ')[0];
  const logEl = document.createElement('div');
  logEl.className = 'text-white-50';
  logEl.innerHTML = `<span class="text-info">[${timeStr}]</span> ${msg}`;
  consoleLogs.appendChild(logEl);
  consoleLogs.scrollTop = consoleLogs.scrollHeight;
};

function initSimulator() {
  const switchMode = document.getElementById('switch-test-mode');
  const switchAttendAll = document.getElementById('switch-test-attend-all');
  const switchCallback = document.getElementById('switch-test-callback');
  const switchSms = document.getElementById('switch-test-sms');
  const simNumInput = document.getElementById('test-sim-number');
  const autoReplyMsg = document.getElementById('test-auto-reply-msg');
  const keypadGrid = document.getElementById('sim-keypad-grid');
  const dialerScreen = document.getElementById('sim-dialer-screen');
  const dialerStatus = document.getElementById('sim-dialer-status');
  const btnDial = document.getElementById('btn-sim-dial');
  const btnAnswer = document.getElementById('btn-sim-answer');
  const btnHangup = document.getElementById('btn-sim-hangup');
  const btnClear = document.getElementById('btn-sim-clear');
  const btnClearLogs = document.getElementById('btn-sim-clear-logs');
  const consoleLogs = document.getElementById('sim-console-logs');
  const liveDtmfBuffer = document.getElementById('sim-live-dtmf-buffer');
  const liveDtmfDesc = document.getElementById('sim-live-dtmf-description');

  if (!switchMode) return;

  const loadConfig = async () => {
    try {
      const res = await fetch('/api/test-config');
      if (res.ok) {
        const data = await res.json();
        switchMode.checked = data.testModeEnabled;
        switchAttendAll.checked = data.testAutoAttend;
        if (switchCallback) switchCallback.checked = data.testAutoCallback;
        if (switchSms) switchSms.checked = data.testAutoSms;
        if (simNumInput) simNumInput.value = data.testPhoneNumber || '';
        if (autoReplyMsg) autoReplyMsg.value = data.testSmsTemplate || 'Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in';
      }
    } catch (err) {
      console.error('Error loading test config:', err);
    }
  };

  const saveConfig = async () => {
    try {
      const formData = new URLSearchParams();
      formData.append('testModeEnabled', switchMode.checked ? 'true' : 'false');
      formData.append('testAutoAttend', switchAttendAll.checked ? 'true' : 'false');
      formData.append('testAutoCallback', (switchCallback && switchCallback.checked) ? 'true' : 'false');
      formData.append('testAutoSms', (switchSms && switchSms.checked) ? 'true' : 'false');
      formData.append('testPhoneNumber', simNumInput ? simNumInput.value.trim() : '');
      formData.append('testSmsTemplate', autoReplyMsg ? autoReplyMsg.value : '');

      await fetch('/api/test-config', {
        method: 'POST',
        body: formData
      });
    } catch (err) {
      console.error('Error saving test config:', err);
    }
  };

  loadConfig();

  // Bind change listeners to auto-save config
  switchMode.addEventListener('change', () => {
    saveConfig();
  });
  switchAttendAll.addEventListener('change', saveConfig);
  if (switchCallback) switchCallback.addEventListener('change', saveConfig);
  if (switchSms) switchSms.addEventListener('change', saveConfig);
  if (simNumInput) simNumInput.addEventListener('change', saveConfig);
  if (autoReplyMsg) autoReplyMsg.addEventListener('change', saveConfig);

  // Keypad clicks
  if (keypadGrid) {
    keypadGrid.addEventListener('click', async (e) => {
      const btn = e.target.closest('.dialer-key');
      if (!btn) return;
      const key = btn.getAttribute('data-key');
      if (!key) return;

      btn.classList.add('btn-light');
      setTimeout(() => btn.classList.remove('btn-light'), 150);

      // Play local DTMF audio feedback
      await playDTMF(key);

      const state = dialerStatus ? dialerStatus.textContent : 'Ready';
      const isCallActive = (state !== 'Ready' && state !== 'IDLE' && state !== 'Call Ended' && state !== 'Rejected' && state !== '');

      if (isCallActive) {
        window.logToHardwareDebugConsole(`[DTMF] Clicked tone: <strong>${key}</strong> | Sending to hardware...`);
        fetch(`/api/dtmf?digit=${encodeURIComponent(key)}`, { method: 'POST' })
          .then(res => res.json())
          .then(d => {
            if (d.status !== 'success') {
              window.logToHardwareDebugConsole(`[DTMF ERROR] Failed to send DTMF: ${d.message}`);
            }
          })
          .catch(err => {
            window.logToHardwareDebugConsole(`[DTMF ERROR] Connection failed: ${err.message}`);
          });
      } else {
        if (dialerScreen) {
          dialerScreen.textContent += key;
          dialerScreen.scrollLeft = dialerScreen.scrollWidth;
        }
      }
    });
  }

  // Dial
  if (btnDial) {
    btnDial.addEventListener('click', () => {
      const num = dialerScreen.textContent.trim() || (simNumInput ? simNumInput.value.trim() : '');
      if (!num) {
        showToast('Hardware Debug', 'Please enter a number on the keypad or in the input.', 'warning');
        return;
      }
      window.logToHardwareDebugConsole(`[DIAL] Dialing outgoing call to: <strong>${num}</strong>...`);
      fetch(`/api/dial?number=${encodeURIComponent(num)}`, { method: 'POST' })
        .then(res => res.json())
        .then(data => {
          if (data.status === 'success') {
            showToast('Hardware Debug', 'Dialing sequence started.', 'success');
            window.logToHardwareDebugConsole(`[DIAL] Dialing initiated successfully.`);
          } else {
            showToast('Hardware Debug', 'Dial failed: ' + data.message, 'danger');
            window.logToHardwareDebugConsole(`[DIAL ERROR] ${data.message}`);
          }
        })
        .catch(e => {
          window.logToHardwareDebugConsole(`[DIAL ERROR] Connection error: ${e.message}`);
        });
    });
  }

  // Answer
  if (btnAnswer) {
    btnAnswer.addEventListener('click', () => {
      window.logToHardwareDebugConsole(`[CALL] Issuing ATA command (Answer call)...`);
      fetch('/answer')
        .then(res => res.json())
        .then(data => {
          if (data.status === 'success') {
            showToast('Hardware Debug', 'Call Answered.', 'success');
            window.logToHardwareDebugConsole(`[CALL] Call answered successfully.`);
          } else {
            showToast('Hardware Debug', 'Answer failed: ' + data.message, 'danger');
            window.logToHardwareDebugConsole(`[CALL ERROR] ${data.message}`);
          }
        })
        .catch(e => {
          window.logToHardwareDebugConsole(`[CALL ERROR] Connection error: ${e.message}`);
        });
    });
  }

  // Hangup
  if (btnHangup) {
    btnHangup.addEventListener('click', () => {
      window.logToHardwareDebugConsole(`[CALL] Issuing ATH command (Hang up)...`);
      fetch('/hangup')
        .then(res => res.json())
        .then(data => {
          if (data.status === 'success') {
            showToast('Hardware Debug', 'Call Terminated.', 'info');
            window.logToHardwareDebugConsole(`[CALL] Call hung up successfully.`);
          } else {
            showToast('Hardware Debug', 'Hang up failed: ' + data.message, 'danger');
            window.logToHardwareDebugConsole(`[CALL ERROR] ${data.message}`);
          }
        })
        .catch(e => {
          window.logToHardwareDebugConsole(`[CALL ERROR] Connection error: ${e.message}`);
        });
    });
  }

  // Clear Screen
  if (btnClear) {
    btnClear.addEventListener('click', () => {
      if (dialerScreen) dialerScreen.textContent = '';
      window.logToHardwareDebugConsole('Dialer display cleared.');
    });
  }

  // Clear Logs
  if (btnClearLogs) {
    btnClearLogs.addEventListener('click', () => {
      if (consoleLogs) consoleLogs.innerHTML = '<div class="text-muted">[00:00:00] Console cleared. Waiting for logs...</div>';
    });
  }

  // Real-time poller function specifically for updating the Dialer UI
  const pollDialerState = async () => {
    try {
      const res = await fetch('/status');
      if (res.ok) {
        const data = await res.json();
        
        const state = data.state || 'IDLE';
        if (dialerStatus) dialerStatus.textContent = (state === 'IDLE') ? 'Ready' : state;

        const isCallActive = (state !== 'IDLE' && state !== 'Call Ended' && state !== 'Rejected');
        
        if (isCallActive) {
          if (dialerScreen) dialerScreen.textContent = data.callerNumber || 'Unknown';
          if (liveDtmfBuffer) liveDtmfBuffer.textContent = data.dtmfBuffer || '-';
          if (liveDtmfDesc) liveDtmfDesc.textContent = `Active call session: ${data.callerNumber || 'Unknown'}`;
        } else {
          if (liveDtmfBuffer && liveDtmfBuffer.textContent !== '-') liveDtmfBuffer.textContent = '-';
          if (liveDtmfDesc && liveDtmfDesc.textContent !== 'No active simulated call session') {
            liveDtmfDesc.textContent = 'No active simulated call session';
          }
        }
      }
    } catch (e) {
      // ignore network errors
    }
  };

  // Poll state every 1.5 seconds
  setInterval(pollDialerState, 1500);
}

// Expose functions globally for dynamic HTML onclick events
window.editHelpCenterNumber = editHelpCenterNumber;
window.deleteHelpCenterNumber = deleteHelpCenterNumber;
window.testCallTo = testCallTo;
window.deleteAudio = deleteAudio;
window.icForwardTo = icForwardTo;
window.dismissIncomingCall = dismissIncomingCall;
window.icAnswer = icAnswer;
window.icDecline = icDecline;
window.icSendSMS = icSendSMS;

// Bind Simulator on DOMContentLoaded
document.addEventListener('DOMContentLoaded', () => {
  initSimulator();
});
