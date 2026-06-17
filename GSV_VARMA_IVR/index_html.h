#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>V-VARMA IVR | GSV Electrical Enterprises</title>
  <meta name="description" content="A premium Varma responsive IoT dashboard for monitoring and controlling an ESP32-based IVR (Interactive Voice Response) system.">
  <meta name="keywords" content="Varma, ESP32, IVR, IoT, Dashboard, Access Point, Station Mode, EEPROM, WiFi, SIM800L">
  <meta name="theme-color" content="#0a0602">

  <!-- Google Fonts -->
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;700&display=swap" rel="stylesheet">

  <!-- FontAwesome Icons -->
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">

  <!-- Bootstrap 5 CSS -->
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">

  <!-- Custom Stylesheet -->
  <style>
/*
 * ESP32 IVR System Dashboard Stylesheet
 * Theme: Varma Premium IoT Dark Dashboard — Glassmorphism + Orange/Brown Accents
 * Version: 2.0.0
 */

@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&family=Poppins:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;700&display=swap');

/* ============================================================
   CSS DESIGN TOKENS
 ============================================================ */
:root {
  /* Backgrounds */
  --bg-color:          #0d0d0d;
  --bg-gradient:       radial-gradient(ellipse at 15% 20%, #141008 0%, #0d0d0d 70%),
                       radial-gradient(ellipse at 85% 80%, #1a0f00 0%, transparent 60%);

  /* Glass cards */
  --glass-bg:          rgba(26, 16, 2, 0.65);
  --glass-bg-hover:    rgba(36, 22, 4, 0.80);
  --glass-border:      rgba(241, 137, 18, 0.08);
  --glass-border-hover: rgba(241, 137, 18, 0.35);

  /* Accents */
  --primary-accent:    #f18912;
  --secondary-accent:  #251103;
  --accent-gradient:   linear-gradient(135deg, #f18912 0%, #251103 100%);
  --accent-glow:       rgba(241, 137, 18, 0.30);

  /* Semantic colours */
  --color-success:     #2ecc71;
  --color-danger:      #ef4444;
  --color-warning:     #f97316;
  --color-info:        #3b82f6;
  --color-purple:      #a855f7;
  --color-orange:      #f18912;

  /* Text */
  --text-main:         #f8fafc;
  --text-muted:        #94a3b8;
  --text-dark:         #475569;

  /* Layout */
  --sidebar-width:     280px;
  --navbar-height:     72px;
  --content-padding:   2rem;
  --radius-card:       24px;
  --radius-btn:        16px;
  --radius-sm:         10px;

  /* Transition */
  --t-fast:            0.2s;
  --t-normal:          0.32s;
  --t-slow:            0.55s;
  --ease-out:          cubic-bezier(0.4, 0, 0.2, 1);

  /* ── esp32_v3.ino EXACT color aliases (for full compatibility) ── */
  --v-orange:          #f18912;
  --v-brown:           #251103;
  --v-red:             #ff4d4d;
  --v-green:           #2ecc71;
  --v-blue:            #1a2a6c;
  --bg-dark:           #0d0d0d;
  --card-bg:           #1a1008;
  --border-color:      rgba(241, 137, 18, 0.20);
  --header-gradient:   linear-gradient(135deg, #1e1e2d, #251103, #f18912);
}

/* ============================================================
   LIGHT THEME DESIGN TOKENS & OVERRIDES (V-VARMA ADMIN PANEL)
   ============================================================ */
body.light-theme {
  /* Backgrounds */
  --bg-color:          #f4f7fa;
  --bg-gradient:       radial-gradient(ellipse at 15% 20%, #f4f7fa 0%, #f4f7fa 70%),
                       radial-gradient(ellipse at 85% 80%, #ffffff 0%, transparent 60%);

  /* Glass cards */
  --glass-bg:          #ffffff;
  --glass-bg-hover:    #ffffff;
  --glass-border:      #eef0f7;
  --glass-border-hover: #f18912;

  /* Accents */
  --primary-accent:    #f18912;
  --secondary-accent:  #251103;
  --accent-gradient:   linear-gradient(135deg, #f18912 0%, #251103 100%);
  --accent-glow:       rgba(241, 137, 18, 0.15);

  /* Text */
  --text-main:         #1e1e2d;
  --text-muted:        #606366;
  --text-dark:         #a2a3b7;
}

/* Smooth transitions for theme toggle switching */
body, 
#sidebar, 
#top-navbar, 
.glass-card, 
.sidebar-link, 
.sidebar-link-icon, 
.status-pill, 
.nav-stat-chip, 
.metric-value, 
.metric-icon, 
.client-item, 
.info-row, 
.table-glass td, 
.table-glass th, 
.glass-input, 
.glass-select,
.input-group-text,
.glass-input-addon,
.glass-input-addon-btn,
.pagination-glass .page-item .page-link {
  transition: background-color var(--t-normal) ease,
              background var(--t-normal) ease,
              border-color var(--t-normal) ease,
              color var(--t-normal) ease,
              box-shadow var(--t-normal) ease,
              transform var(--t-normal) var(--ease-out);
}

body.light-theme #sidebar {
  background: #1e1e2d;
  border-right-color: rgba(255, 255, 255, 0.05);
}

body.light-theme .sidebar-section-label {
  color: #494b74;
}

body.light-theme .sidebar-link {
  color: #a2a3b7;
}

body.light-theme .sidebar-link-icon {
  background: rgba(255, 255, 255, 0.03);
  color: #a2a3b7;
}

body.light-theme .sidebar-link:hover {
  color: #fff;
  background: rgba(255, 255, 255, 0.05);
}

body.light-theme .sidebar-link:hover .sidebar-link-icon {
  background: rgba(241, 137, 18, 0.1);
  color: #f18912;
}

body.light-theme .sidebar-link.active {
  background: rgba(255, 255, 255, 0.08);
  border-left-color: #f18912;
  color: #fff;
}

body.light-theme .sidebar-link.active .sidebar-link-icon {
  background: var(--accent-gradient);
  color: #fff;
}

body.light-theme .sidebar-footer {
  border-top-color: rgba(255, 255, 255, 0.08);
}

body.light-theme .footer-chip-name {
  color: #a2a3b7;
}

body.light-theme #top-navbar {
  background: #ffffff;
  border-bottom-color: #eef0f7;
  box-shadow: 0 2px 15px rgba(0, 0, 0, 0.05);
}

body.light-theme .navbar-page-title {
  color: #1e1e2d;
}

body.light-theme #nav-uptime {
  color: #606366 !important;
}

body.light-theme .nav-stat-chip {
  background: #f4f7fa;
  border-color: #eef0f7;
}

body.light-theme .nav-stat-chip strong {
  color: #1e1e2d !important;
}

body.light-theme .status-pill {
  background: #f4f7fa;
  border-color: #eef0f7;
  color: #1e1e2d;
}

body.light-theme #esp32-status-text {
  color: #1e1e2d;
}

body.light-theme .section-title {
  color: #1e1e2d;
}

body.light-theme .card-section-title {
  color: #1e1e2d;
}

body.light-theme .widget-title {
  color: #1e1e2d;
}

body.light-theme .metric-value {
  color: #1e1e2d;
}

body.light-theme .client-details .client-name {
  color: #1e1e2d;
}

body.light-theme .info-row {
  border-bottom-color: #eef0f7;
}

body.light-theme .info-value {
  color: #1e1e2d !important;
}

body.light-theme .mini-signal-bars .mini-bar {
  background: rgba(0, 0, 0, 0.15);
}

body.light-theme .rssi-bar {
  background: rgba(0, 0, 0, 0.15);
}

body.light-theme .bg-particles .particle {
  opacity: 0.03;
}

body.light-theme .glass-input,
body.light-theme .glass-select {
  background: #ffffff;
  border-color: #d1d5db;
  color: #1e1e2d;
}

body.light-theme .glass-input:focus,
body.light-theme .glass-select:focus {
  background: #ffffff;
  border-color: var(--primary-accent);
  box-shadow: 0 0 0 3px rgba(241, 137, 18, 0.15);
  color: #1e1e2d;
}

body.light-theme .glass-input::placeholder {
  color: #a2a3b7;
}

body.light-theme .glass-input-addon {
  background: #f4f7fa;
  border-color: #d1d5db;
}

body.light-theme .input-group-text {
  background: #f4f7fa;
  border-color: #d1d5db;
  color: #1e1e2d;
}

body.light-theme .glass-input-addon-btn {
  background: #f4f7fa;
  border-color: #d1d5db;
}

body.light-theme .glass-input-addon-btn:hover {
  background: #eef2f7;
}

body.light-theme .table-glass th {
  background: #f4f7fa !important;
  color: #1e1e2d !important;
  border-bottom-color: #eef0f7 !important;
}

body.light-theme .table-glass td {
  background: transparent !important;
  border-bottom-color: #eef0f7 !important;
  color: #606366 !important;
}

body.light-theme .table-glass tbody tr:hover td {
  background: #f4f7fa !important;
  color: #1e1e2d !important;
}

body.light-theme .pagination-glass .page-item .page-link {
  background: #ffffff;
  border-color: #d1d5db;
  color: #606366;
}

body.light-theme .pagination-glass .page-item.active .page-link {
  background: var(--accent-gradient);
  color: #fff;
}

body.light-theme .client-item {
  background: #ffffff;
  border-color: #eef0f7;
}

body.light-theme .client-item:hover {
  background: #f4f7fa;
  border-color: var(--primary-accent);
}

body.light-theme .client-avatar {
  background: rgba(241, 137, 18, 0.05);
  border-color: rgba(241, 137, 18, 0.12);
}

body.light-theme .modal-content.glass-modal {
  background: #ffffff;
  border-color: #eef0f7;
  color: #1e1e2d;
}

body.light-theme .modal-header,
body.light-theme .modal-footer {
  border-color: #eef0f7;
}

body.light-theme .modal-body {
  color: #606366;
}

body.light-theme .btn-close-white-custom {
  filter: none;
}

body.light-theme .toast.glass-toast {
  background: #ffffff !important;
  border-color: #eef0f7 !important;
  color: #1e1e2d !important;
}

body.light-theme .toast-header {
  border-bottom-color: #eef0f7 !important;
  color: #1e1e2d !important;
}

body.light-theme .resource-gauge .gauge-ring {
  background: radial-gradient(circle, #fff 55%, transparent 56%) !important;
}

body.light-theme .gauge-ring span {
  color: #1e1e2d;
}

body.light-theme .gauge-title {
  color: #606366;
}

body.light-theme .btn-glass-secondary {
  background: #f4f7fa !important;
  border-color: #d1d5db !important;
  color: #1e1e2d !important;
}

body.light-theme .btn-glass-secondary:hover {
  background: #eef2f7 !important;
  color: #1e1e2d !important;
  border-color: #c5c9d0 !important;
}

body.light-theme .btn-glass-primary {
  background: var(--primary-accent) !important;
  border-color: var(--primary-accent) !important;
  color: #ffffff !important;
}

body.light-theme .btn-glass-primary:hover {
  background: #c0791a !important;
  border-color: #c0791a !important;
  color: #ffffff !important;
}


/* ============================================================
   RESET & BASE
============================================================ */
*, *::before, *::after {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

html {
  scroll-behavior: smooth;
  overflow-y: auto !important;
}

body {
  font-family: 'Inter', 'Poppins', system-ui, -apple-system, sans-serif;
  background: var(--bg-color);
  background-image: var(--bg-gradient);
  background-attachment: fixed;
  color: var(--text-main);
  min-height: 100vh;
  overflow-x: hidden;
  overflow-y: auto !important;
  line-height: 1.6;
}

/* Custom Scrollbar */
::-webkit-scrollbar { width: 5px; height: 5px; }
::-webkit-scrollbar-track { background: rgba(6, 9, 18, 0.6); }
::-webkit-scrollbar-thumb { background: rgba(243, 156, 18, 0.2); border-radius: 4px; }
::-webkit-scrollbar-thumb:hover { background: rgba(243, 156, 18, 0.4); }

.digital-font {
  font-family: 'JetBrains Mono', 'Courier New', monospace;
  font-weight: 500;
}

/* Make text-muted and form-labels extremely legible and bold on dark theme cards */
body .text-muted, 
body .text-muted.small,
body .glass-card .text-muted,
body .form-label.text-muted {
  color: #cbd5e1 !important; /* Bright, clear off-white/light-gray */
}
body .form-label,
body .glass-card .form-label {
  color: #f1f5f9 !important; /* Bold white for labels */
  font-weight: 700 !important; /* Bold */
}

body.light-theme .text-muted,
body.light-theme .text-muted.small,
body.light-theme .glass-card .text-muted,
body.light-theme .form-label.text-muted {
  color: #475569 !important; /* Standard dark gray for light theme */
}
body.light-theme .form-label,
body.light-theme .glass-card .form-label {
  color: #1e293b !important;
  font-weight: 700 !important;
}

/* ============================================================
   ANIMATED BACKGROUND PARTICLES
============================================================ */
.bg-particles {
  position: fixed;
  inset: 0;
  pointer-events: none;
  z-index: 0;
  overflow: hidden;
}

.particle {
  position: absolute;
  border-radius: 50%;
  opacity: 0.06;
  animation: float-particle linear infinite;
  background: var(--primary-accent);
}

.particle:nth-child(1) { width: 300px; height: 300px; top: -100px; left: 10%; animation-duration: 25s; animation-delay: 0s; }
.particle:nth-child(2) { width: 200px; height: 200px; top: 40%; right: 5%; animation-duration: 30s; animation-delay: -10s; background: var(--secondary-accent); }
.particle:nth-child(3) { width: 150px; height: 150px; bottom: 10%; left: 20%; animation-duration: 20s; animation-delay: -5s; }
.particle:nth-child(4) { width: 250px; height: 250px; top: 20%; right: 25%; animation-duration: 35s; animation-delay: -15s; opacity: 0.04; }
.particle:nth-child(5) { width: 100px; height: 100px; bottom: 30%; right: 15%; animation-duration: 22s; animation-delay: -8s; background: var(--color-purple); }
.particle:nth-child(6) { width: 180px; height: 180px; top: 60%; left: 5%; animation-duration: 28s; animation-delay: -3s; background: var(--secondary-accent); }

@keyframes float-particle {
  0%   { transform: translateY(0) scale(1); }
  33%  { transform: translateY(-60px) scale(1.05); }
  66%  { transform: translateY(-20px) scale(0.97); }
  100% { transform: translateY(0) scale(1); }
}

/* ============================================================
   GLASSMORPHISM CARD
============================================================ */
.glass-card {
  background: var(--glass-bg);
  backdrop-filter: blur(16px) saturate(180%);
  -webkit-backdrop-filter: blur(16px) saturate(180%);
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-card);
  padding: 1.5rem;
  transition: transform var(--t-normal) var(--ease-out),
              box-shadow var(--t-normal) var(--ease-out),
              border-color var(--t-normal) ease,
              background var(--t-normal) ease;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.30);
  position: relative;
  overflow: hidden;
}

.glass-card::before {
  content: '';
  position: absolute;
  top: 0; left: 0; right: 0;
  height: 1px;
  background: linear-gradient(90deg, transparent, rgba(243, 156, 18, 0.2), transparent);
  opacity: 0;
  transition: opacity var(--t-normal) ease;
}

.glass-card:hover {
  background: var(--glass-bg-hover);
  border-color: var(--glass-border-hover);
  transform: translateY(-3px);
  box-shadow: 0 16px 48px rgba(0, 0, 0, 0.35), 0 0 0 1px rgba(243, 156, 18, 0.06);
}

.glass-card:hover::before { opacity: 1; }

/* ============================================================
   APP LAYOUT
============================================================ */
#app-container {
  display: flex;
  min-height: 100vh;
  position: relative;
  z-index: 1;
}

/* ============================================================
   SIDEBAR
============================================================ */
#sidebar {
  width: var(--sidebar-width);
  background: rgba(6, 10, 20, 0.80);
  backdrop-filter: blur(24px);
  -webkit-backdrop-filter: blur(24px);
  border-right: 1px solid var(--glass-border);
  display: flex;
  flex-direction: column;
  position: fixed;
  top: 0; bottom: 0; left: 0;
  z-index: 1030;
  transition: transform var(--t-normal) var(--ease-out);
  overflow: hidden;
}

/* Sidebar Header / Brand */
.sidebar-header {
  height: var(--navbar-height);
  display: flex;
  align-items: center;
  padding: 0 1.25rem;
  border-bottom: 1px solid var(--glass-border);
  flex-shrink: 0;
}

.sidebar-brand {
  display: flex;
  align-items: center;
  gap: 0.85rem;
  text-decoration: none;
  cursor: pointer;
}

.brand-icon-wrap {
  width: 40px;
  height: 40px;
  border-radius: 10px;
  background: var(--accent-gradient);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.1rem;
  color: #fff;
  flex-shrink: 0;
  box-shadow: 0 4px 15px var(--accent-glow);
  animation: brand-pulse 3s ease-in-out infinite;
}

@keyframes brand-pulse {
  0%, 100% { box-shadow: 0 4px 15px var(--accent-glow); }
  50% { box-shadow: 0 4px 25px rgba(243, 156, 18, 0.5); }
}

.brand-text { display: flex; flex-direction: column; }
.brand-title {
  font-size: 1.05rem;
  font-weight: 700;
  letter-spacing: 0.5px;
  background: var(--accent-gradient);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  line-height: 1.2;
}
.brand-sub { font-size: 0.7rem; color: var(--text-dark); font-family: 'JetBrains Mono', monospace; }

/* Section Labels */
.sidebar-section-label {
  font-size: 0.65rem;
  font-weight: 700;
  letter-spacing: 1.5px;
  color: var(--text-dark);
  padding: 1rem 1.5rem 0.3rem;
  text-transform: uppercase;
}

/* Menu list */
.sidebar-menu {
  list-style: none;
  padding: 0 0.75rem;
}

.sidebar-item { margin-bottom: 2px; }

.sidebar-link {
  display: flex;
  align-items: center;
  gap: 0.85rem;
  padding: 0.72rem 1rem;
  color: var(--text-muted);
  text-decoration: none;
  border-radius: 11px;
  font-weight: 500;
  font-size: 0.9rem;
  transition: all var(--t-fast) ease;
  position: relative;
  overflow: hidden;
}

.sidebar-link-icon {
  width: 32px;
  height: 32px;
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 0.95rem;
  background: rgba(255, 255, 255, 0.04);
  flex-shrink: 0;
  transition: all var(--t-fast) ease;
}

.sidebar-link-text { flex: 1; }

.sidebar-link:hover {
  color: var(--text-main);
  background: rgba(255, 255, 255, 0.04);
}

.sidebar-link:hover .sidebar-link-icon {
  background: rgba(243, 156, 18, 0.12);
  color: var(--primary-accent);
}

.sidebar-link.active {
  color: #fff;
  background: linear-gradient(135deg, rgba(243, 156, 18, 0.15) 0%, rgba(79, 172, 254, 0.1) 100%);
  border: 1px solid rgba(243, 156, 18, 0.2);
}

.sidebar-link.active .sidebar-link-icon {
  background: var(--accent-gradient);
  color: #fff;
  box-shadow: 0 2px 10px var(--accent-glow);
}

/* Status dots on nav links */
.sidebar-status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--color-success);
  box-shadow: 0 0 6px var(--color-success);
  animation: pulse-dot 2s infinite;
}

.sidebar-status-dot.offline {
  background: var(--color-danger);
  box-shadow: 0 0 6px var(--color-danger);
}

@keyframes pulse-dot {
  0%, 100% { opacity: 1; transform: scale(1); }
  50% { opacity: 0.7; transform: scale(0.85); }
}

/* Count badge on sidebar */
.sidebar-count-badge {
  background: rgba(243, 156, 18, 0.15);
  color: var(--primary-accent);
  font-size: 0.7rem;
  font-weight: 700;
  padding: 0.15rem 0.55rem;
  border-radius: 30px;
  border: 1px solid rgba(243, 156, 18, 0.2);
  font-family: 'JetBrains Mono', monospace;
}

.pulse-badge {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--color-success);
  animation: pulse-dot 2s infinite;
}

/* Sidebar Footer */
.sidebar-footer {
  margin-top: auto;
  padding: 1rem 1.25rem;
  border-top: 1px solid var(--glass-border);
  flex-shrink: 0;
}

.sidebar-footer-device {
  display: flex;
  align-items: center;
  gap: 0.75rem;
}

.footer-chip-icon {
  width: 36px;
  height: 36px;
  border-radius: 10px;
  background: rgba(243, 156, 18, 0.1);
  color: var(--primary-accent);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
  flex-shrink: 0;
}

.footer-chip-details { flex: 1; overflow: hidden; }
.footer-chip-name { font-size: 0.8rem; font-weight: 600; color: var(--text-main); }
.footer-chip-mac {
  font-size: 0.65rem;
  color: var(--text-dark);
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.footer-online-dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
  background: var(--color-success);
  box-shadow: 0 0 8px var(--color-success);
  animation: pulseGreen 2s infinite;
  flex-shrink: 0;
}

/* ============================================================
   MAIN WRAPPER
============================================================ */
#main-wrapper {
  margin-left: var(--sidebar-width);
  flex: 1;
  min-width: 0;
  display: flex;
  flex-direction: column;
}

/* ============================================================
   TOP NAVBAR
============================================================ */
#top-navbar {
  height: var(--navbar-height);
  background: rgba(6, 9, 20, 0.50);
  backdrop-filter: blur(16px);
  -webkit-backdrop-filter: blur(16px);
  border-bottom: 1px solid var(--glass-border);
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 1.75rem;
  position: sticky;
  top: 0;
  z-index: 1020;
  gap: 0.75rem;
}

.navbar-left { display: flex; align-items: center; gap: 0.75rem; flex: 1; min-width: 0; }
.navbar-right { display: flex; align-items: center; gap: 0.75rem; flex-shrink: 1; min-width: 0; }

.sidebar-toggle {
  background: none;
  border: none;
  color: var(--text-main);
  font-size: 1.15rem;
  cursor: pointer;
  display: none;
  width: 38px; height: 38px;
  border-radius: 9px;
  align-items: center;
  justify-content: center;
  transition: background var(--t-fast);
  flex-shrink: 0;
}
.sidebar-toggle:hover { background: rgba(255, 255, 255, 0.06); }

.breadcrumb-wrap { display: flex; align-items: baseline; gap: 0.4rem; overflow: hidden; }

.navbar-page-title {
  font-size: 1rem;
  font-weight: 700;
  color: #fff;
  margin: 0;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.nav-stat-chip {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  background: rgba(255, 255, 255, 0.04);
  border: 1px solid var(--glass-border);
  border-radius: 30px;
  padding: 0.35rem 0.85rem;
  font-size: 0.82rem;
}

.mode-badge-text {
  font-weight: 700;
  color: var(--primary-accent);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.8rem;
}

/* Online status pill */
.status-pill {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  background: rgba(255, 255, 255, 0.04);
  border: 1px solid var(--glass-border);
  border-radius: 30px;
  padding: 0.35rem 0.85rem;
  font-size: 0.82rem;
  font-weight: 500;
  color: var(--text-main);
}

.status-dot {
  width: 8px; height: 8px;
  border-radius: 50%;
  flex-shrink: 0;
}

.status-dot.online {
  background: var(--color-success);
  box-shadow: 0 0 8px var(--color-success);
  animation: pulseGreen 2s infinite;
}

.status-dot.offline {
  background: var(--color-danger);
  box-shadow: 0 0 8px var(--color-danger);
  animation: pulseRed 2s infinite;
}

/* ============================================================
   CONTENT AREA
============================================================ */
.content-container {
  padding: var(--content-padding);
  flex: 1;
}

.page-section {
  display: none;
}

.page-section.active {
  display: block;
  animation: fadeInUp 0.4s var(--ease-out) forwards;
}

/* Section Headers */
.section-header {
  display: flex;
  align-items: flex-start;
  justify-content: space-between;
  flex-wrap: wrap;
  gap: 1rem;
}

.section-title {
  font-size: 1.5rem;
  font-weight: 800;
  color: #fff;
  letter-spacing: -0.3px;
}

/* Card Section Titles */
.card-section-title {
  font-size: 1rem;
  font-weight: 700;
  color: #fff;
  margin-bottom: 1.25rem;
}

/* Widget header */
.widget-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  flex-wrap: wrap;
  gap: 0.5rem;
}

.widget-title {
  font-size: 0.95rem;
  font-weight: 700;
  color: #fff;
  margin: 0;
}

/* ============================================================
   METRIC CARDS (Dashboard)
============================================================ */
.metric-card {
  padding: 1.25rem;
  display: flex;
  flex-direction: column;
  gap: 0.2rem;
}

.metric-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 0.5rem;
}

.metric-label {
  font-size: 0.78rem;
  font-weight: 600;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.metric-icon {
  width: 38px; height: 38px;
  border-radius: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
  background: rgba(243, 156, 18, 0.1);
  color: var(--primary-accent);
  border: 1px solid rgba(243, 156, 18, 0.12);
  transition: all var(--t-normal) ease;
}

.glass-card:hover .metric-icon {
  background: var(--accent-gradient);
  color: #fff;
  border-color: transparent;
  transform: scale(1.1) rotate(6deg);
  box-shadow: 0 4px 12px var(--accent-glow);
}

/* Per-card accent colours */
[data-color="blue"] .metric-icon  { background:rgba(243, 156, 18, 0.10); color:#f39c12; border-color:rgba(243, 156, 18, 0.15); }
[data-color="green"] .metric-icon { background:rgba(0,230,118,0.10); color:#00e676; border-color:rgba(0,230,118,0.15); }
[data-color="cyan"] .metric-icon  { background:rgba(0,176,255,0.10); color:#00b0ff; border-color:rgba(0,176,255,0.15); }
[data-color="purple"] .metric-icon{ background:rgba(124,77,255,0.10); color:#7c4dff; border-color:rgba(124,77,255,0.15); }
[data-color="orange"] .metric-icon{ background:rgba(255,109,0,0.10); color:#ff6d00; border-color:rgba(255,109,0,0.15); }

.metric-value {
  font-size: 1.6rem;
  font-weight: 800;
  color: #fff;
  line-height: 1.1;
}

.metric-desc {
  font-size: 0.78rem;
  color: var(--text-muted);
  margin-top: 2px;
}

.metric-trend {
  font-size: 0.73rem;
  font-weight: 600;
  margin-top: 0.5rem;
  display: flex;
  align-items: center;
  gap: 0.3rem;
}

.metric-trend.positive { color: var(--color-success); }
.metric-trend.negative { color: var(--color-danger); }
.metric-trend.neutral  { color: var(--text-muted); }

/* ============================================================
   RSSI SIGNAL BARS
============================================================ */
.rssi-bars-wrap {
  display: flex;
  align-items: flex-end;
  gap: 3px;
  height: 22px;
}

.rssi-bar-col {
  display: flex;
  align-items: flex-end;
  height: 100%;
}

.rssi-bar-col:nth-child(1) .rssi-bar { height: 20%; }
.rssi-bar-col:nth-child(2) .rssi-bar { height: 40%; }
.rssi-bar-col:nth-child(3) .rssi-bar { height: 60%; }
.rssi-bar-col:nth-child(4) .rssi-bar { height: 80%; }
.rssi-bar-col:nth-child(5) .rssi-bar { height: 100%; }

.rssi-bar {
  width: 5px;
  background: rgba(255, 255, 255, 0.15);
  border-radius: 2px;
  transition: background 0.4s ease;
}

/* Signal quality classes */
.rssi-bars-wrap.signal-excellent .rssi-bar { background: var(--color-success); }
.rssi-bars-wrap.signal-good .rssi-bar-col:nth-child(-n+4) .rssi-bar { background: var(--color-success); }
.rssi-bars-wrap.signal-fair .rssi-bar-col:nth-child(-n+3) .rssi-bar { background: var(--color-warning); }
.rssi-bars-wrap.signal-weak .rssi-bar-col:nth-child(-n+2) .rssi-bar { background: var(--color-danger); }
.rssi-bars-wrap.signal-none .rssi-bar { background: rgba(255,255,255,0.1); }

/* ============================================================
   MINI INFO CARDS (Last Call)
============================================================ */
.info-mini-card {
  background: rgba(255, 255, 255, 0.025);
  border: 1px solid var(--glass-border);
  border-radius: 11px;
  padding: 0.75rem;
}

.info-mini-label {
  font-size: 0.72rem;
  color: var(--text-dark);
  text-transform: uppercase;
  letter-spacing: 0.5px;
  font-weight: 600;
  margin-bottom: 0.35rem;
}

.info-mini-value {
  font-size: 0.95rem;
  font-weight: 600;
  color: #fff;
}

/* ============================================================
   BADGE / PILL COMPONENTS
============================================================ */
.badge-pill-count {
  background: rgba(243, 156, 18, 0.12);
  color: var(--primary-accent);
  border: 1px solid rgba(243, 156, 18, 0.22);
  border-radius: 30px;
  padding: 0.2rem 0.7rem;
  font-size: 0.75rem;
  font-weight: 700;
}

/* Status Badges */
.status-badge {
  display: inline-flex;
  align-items: center;
  gap: 0.35rem;
  padding: 0.3rem 0.75rem;
  border-radius: 30px;
  font-size: 0.75rem;
  font-weight: 700;
  white-space: nowrap;
}

.status-badge-completed {
  background: rgba(0, 230, 118, 0.12);
  color: var(--color-success);
  border: 1px solid rgba(0, 230, 118, 0.25);
}

.status-badge-missed {
  background: rgba(255, 23, 68, 0.12);
  color: var(--color-danger);
  border: 1px solid rgba(255, 23, 68, 0.25);
}

.status-badge-busy {
  background: rgba(255, 145, 0, 0.12);
  color: var(--color-warning);
  border: 1px solid rgba(255, 145, 0, 0.25);
}

.status-badge-failed {
  background: rgba(148, 163, 184, 0.1);
  color: var(--text-muted);
  border: 1px solid rgba(148, 163, 184, 0.2);
}

/* ============================================================
   CLIENT LIST
============================================================ */
.client-list {
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
}

.client-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0.85rem 1rem;
  background: rgba(255, 255, 255, 0.025);
  border: 1px solid var(--glass-border);
  border-radius: 12px;
  transition: border-color var(--t-fast) ease, background var(--t-fast) ease;
}

.client-item:hover {
  border-color: rgba(243, 156, 18, 0.15);
  background: rgba(255, 255, 255, 0.04);
}

.client-info { display: flex; align-items: center; gap: 0.7rem; }

.client-avatar {
  width: 36px; height: 36px;
  border-radius: 9px;
  background: rgba(243, 156, 18, 0.1);
  color: var(--primary-accent);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 0.9rem;
  border: 1px solid rgba(243, 156, 18, 0.15);
}

.client-details .client-name { font-size: 0.9rem; font-weight: 600; color: #fff; }
.client-details .client-ip { font-size: 0.78rem; color: var(--text-muted); font-family: 'JetBrains Mono', monospace; }

.client-meta { text-align: right; font-size: 0.75rem; color: var(--text-muted); font-family: 'JetBrains Mono', monospace; }

/* ============================================================
   INFO ROWS (key-value pairs)
============================================================ */
.info-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.8rem 0;
  border-bottom: 1px solid var(--glass-border);
  flex-wrap: wrap;
  gap: 0.5rem;
}

.info-row:last-of-type { border-bottom: none; }

.info-label {
  color: var(--text-muted);
  font-size: 0.875rem;
  font-weight: 500;
  flex-shrink: 0;
}

.info-value {
  color: var(--text-main);
  font-size: 0.875rem;
  font-weight: 600;
  text-align: right;
}

/* ============================================================
   BUTTONS
============================================================ */
.btn-action-group {
  display: flex;
  gap: 0.75rem;
  flex-wrap: wrap;
}

.btn-glass-primary {
  background: var(--accent-gradient);
  border: none;
  color: #fff;
  font-weight: 700;
  border-radius: var(--radius-btn);
  padding: 0.6rem 1.4rem;
  font-size: 0.875rem;
  transition: all var(--t-fast) ease;
  box-shadow: 0 4px 15px rgba(243, 156, 18, 0.2);
  cursor: pointer;
}

.btn-glass-primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 24px rgba(243, 156, 18, 0.35);
  filter: brightness(1.08);
  color: #fff;
}

.btn-glass-primary:active { transform: translateY(0); box-shadow: 0 2px 8px rgba(243, 156, 18, 0.2); }

.btn-glass-secondary {
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid var(--glass-border);
  color: var(--text-main);
  font-weight: 600;
  border-radius: var(--radius-btn);
  padding: 0.6rem 1.4rem;
  font-size: 0.875rem;
  transition: all var(--t-fast) ease;
  cursor: pointer;
}

.btn-glass-secondary:hover {
  background: rgba(255, 255, 255, 0.1);
  color: #fff;
  border-color: rgba(255, 255, 255, 0.2);
  transform: translateY(-2px);
}

.btn-glass-danger {
  background: linear-gradient(135deg, #ff1744, #d50000);
  border: none;
  color: #fff;
  font-weight: 700;
  border-radius: var(--radius-btn);
  padding: 0.6rem 1.4rem;
  font-size: 0.875rem;
  transition: all var(--t-fast) ease;
  box-shadow: 0 4px 15px rgba(255, 23, 68, 0.2);
  cursor: pointer;
}

.btn-glass-danger:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 24px rgba(255, 23, 68, 0.35);
  filter: brightness(1.1);
  color: #fff;
}

.btn-glass-warning {
  background: linear-gradient(135deg, #ff9100, #ff6d00);
  border: none;
  color: #fff;
  font-weight: 700;
  border-radius: var(--radius-btn);
  padding: 0.6rem 1.4rem;
  font-size: 0.875rem;
  transition: all var(--t-fast) ease;
  box-shadow: 0 4px 15px rgba(255, 109, 0, 0.2);
  cursor: pointer;
}

.btn-glass-warning:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 24px rgba(255, 109, 0, 0.35);
  filter: brightness(1.1);
  color: #fff;
}

/* Disabled button state */
.btn-glass-primary:disabled,
.btn-glass-secondary:disabled,
.btn-glass-danger:disabled,
.btn-glass-warning:disabled {
  opacity: 0.45;
  cursor: not-allowed;
  transform: none !important;
  box-shadow: none !important;
  filter: none !important;
}

/* Small variant */
.btn-sm {
  padding: 0.4rem 0.9rem !important;
  font-size: 0.8rem !important;
  border-radius: 8px !important;
}

/* ============================================================
   FORM INPUTS
============================================================ */
.form-label {
  color: var(--text-muted);
  font-size: 0.85rem;
  font-weight: 600;
  margin-bottom: 0.4rem;
}

.glass-input, .glass-select {
  background: rgba(8, 14, 28, 0.65) !important;
  border: 1px solid var(--glass-border) !important;
  color: var(--text-main) !important;
  border-radius: var(--radius-sm) !important;
  padding: 0.6rem 1rem !important;
  font-size: 0.875rem;
  transition: all var(--t-fast) ease;
}

.glass-input:focus, .glass-select:focus {
  background: rgba(12, 20, 38, 0.85) !important;
  border-color: var(--primary-accent) !important;
  box-shadow: 0 0 0 3px rgba(243, 156, 18, 0.1) !important;
  color: #fff !important;
  outline: none;
}

.glass-input::placeholder { color: rgba(255, 255, 255, 0.45) !important; }
body.light-theme .glass-input::placeholder { color: rgba(0, 0, 0, 0.4) !important; }

.glass-input:-webkit-autofill {
  -webkit-text-fill-color: var(--text-main) !important;
  -webkit-box-shadow: 0 0 0 30px rgba(8, 14, 28, 0.95) inset !important;
}

/* Input with icon */
.input-with-icon { position: relative; }
.input-icon {
  position: absolute;
  left: 1rem;
  top: 50%;
  transform: translateY(-50%);
  color: var(--text-dark);
  font-size: 0.85rem;
  z-index: 2;
  pointer-events: none;
}

.glass-input-addon {
  background: rgba(8, 14, 28, 0.65) !important;
  border: 1px solid var(--glass-border) !important;
  color: var(--text-muted) !important;
  border-radius: 0 !important;
}

.glass-input-addon-btn {
  background: rgba(8, 14, 28, 0.65) !important;
  border: 1px solid var(--glass-border) !important;
  border-left: none !important;
  border-radius: 0 var(--radius-sm) var(--radius-sm) 0 !important;
  padding: 0.6rem 0.85rem;
}

.glass-input-addon-btn:hover { background: rgba(255,255,255,0.06) !important; }

/* ============================================================
   AP/STA MODE LIVE INDICATOR BADGES
============================================================ */
.ap-live-badge, .sta-live-badge {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.4rem 0.9rem;
  border-radius: 30px;
  font-size: 0.8rem;
  font-weight: 700;
  border: 1px solid rgba(0, 230, 118, 0.3);
  background: rgba(0, 230, 118, 0.08);
  color: var(--color-success);
}

.ap-live-dot, .sta-live-dot {
  width: 8px; height: 8px;
  border-radius: 50%;
  background: var(--color-success);
  box-shadow: 0 0 6px var(--color-success);
  animation: pulseGreen 2s infinite;
}

/* ============================================================
   CONNECTIVITY HEALTH CARDS
============================================================ */
.health-card {
  background: rgba(12, 20, 40, 0.5);
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-card);
  padding: 1.5rem 1rem;
  text-align: center;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.4rem;
  transition: all var(--t-normal) ease;
}

.health-card:hover { transform: translateY(-3px); }

.health-icon-ring {
  width: 70px; height: 70px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-bottom: 0.5rem;
  border: 2px solid currentColor;
  opacity: 0.9;
  transition: all var(--t-normal) ease;
}

.health-card:hover .health-icon-ring { opacity: 1; transform: scale(1.05); }

.health-status-icon { font-size: 1.75rem; }

.health-label { font-size: 0.9rem; font-weight: 700; }
.health-value { font-size: 0.78rem; opacity: 0.8; }

.health-ping-badge {
  font-size: 0.7rem;
  font-weight: 700;
  padding: 0.15rem 0.55rem;
  border-radius: 30px;
  background: currentColor;
  color: #fff !important;
  opacity: 0.25;
  margin-top: 0.25rem;
  font-family: 'JetBrains Mono', monospace;
}

.health-online {
  color: var(--color-success);
  border-color: rgba(0, 230, 118, 0.2);
}

.health-online .health-icon-ring { border-color: rgba(0, 230, 118, 0.4); background: rgba(0, 230, 118, 0.08); }

.health-offline {
  color: var(--color-danger);
  border-color: rgba(255, 23, 68, 0.2);
}

.health-offline .health-icon-ring { border-color: rgba(255, 23, 68, 0.4); background: rgba(255, 23, 68, 0.08); }

/* Countdown refresh bar */
.countdown-container {
  display: flex;
  align-items: center;
  gap: 0.6rem;
  font-size: 0.82rem;
}

.countdown-bar-outer {
  width: 64px; height: 4px;
  background: rgba(255, 255, 255, 0.08);
  border-radius: 2px;
  overflow: hidden;
}

.countdown-bar-inner {
  height: 100%;
  background: var(--accent-gradient);
  border-radius: 2px;
  transition: width 0.1s linear;
  transform-origin: left;
}

/* Diagnostics mini-cards */
.diag-metric-card {
  background: rgba(255, 255, 255, 0.025);
  border: 1px solid var(--glass-border);
  border-radius: 12px;
  padding: 1rem 1.25rem;
  display: flex;
  align-items: center;
  gap: 1rem;
}

.diag-metric-icon { font-size: 1.5rem; flex-shrink: 0; }

/* ============================================================
   EEPROM PAGE
============================================================ */
.memory-usage-wrap { }

.progress-glass {
  background: rgba(255, 255, 255, 0.06);
  border-radius: 30px;
  height: 8px;
  overflow: hidden;
}

.progress-glass-bar {
  background: var(--accent-gradient);
  height: 100%;
  border-radius: 30px;
  box-shadow: 0 0 10px rgba(243, 156, 18, 0.4);
  transition: width 0.6s ease;
}

.danger-zone-card { border-color: rgba(255, 23, 68, 0.2) !important; }
.danger-zone-card::before { background: linear-gradient(90deg, transparent, rgba(255, 23, 68, 0.2), transparent) !important; }

.danger-info-box {
  background: rgba(255, 23, 68, 0.05);
  border: 1px solid rgba(255, 23, 68, 0.12);
  border-radius: 12px;
  padding: 1rem;
}

/* ============================================================
   AP MODE STATUS BADGES
============================================================ */
.badge-status {
  display: inline-flex;
  align-items: center;
  gap: 0.4rem;
  padding: 0.35rem 0.85rem;
  border-radius: 30px;
  font-size: 0.8rem;
  font-weight: 700;
}

.badge-status-running {
  background: rgba(0, 230, 118, 0.12);
  color: var(--color-success);
  border: 1px solid rgba(0, 230, 118, 0.25);
}

.badge-status-stopped {
  background: rgba(255, 23, 68, 0.12);
  color: var(--color-danger);
  border: 1px solid rgba(255, 23, 68, 0.25);
}

/* Security notice block */
.security-notice {
  display: flex;
  align-items: center;
  gap: 1rem;
  background: rgba(255, 255, 255, 0.025);
  border: 1px solid var(--glass-border);
  border-radius: 12px;
  padding: 1rem 1.25rem;
}

/* ============================================================
   CALL HISTORY TABLE
============================================================ */
.table-responsive {
  border-radius: 12px;
  max-height: 400px;
  overflow-x: auto !important;
  overflow-y: auto !important;
  border: 1px solid var(--glass-border);
}

.table-responsive thead th {
  position: sticky;
  top: 0;
  z-index: 10;
  background-color: #0c0804 !important; /* Match the dark theme background */
}

.table-glass {
  --bs-table-bg: transparent !important;
  --bs-table-accent-bg: transparent !important;
  --bs-table-striped-bg: transparent !important;
  --bs-table-active-bg: transparent !important;
  --bs-table-hover-bg: transparent !important;
  background-color: transparent !important;
  background: transparent !important;
  color: var(--text-main) !important;
  margin-bottom: 0;
  width: 100%;
  border-collapse: collapse;
}

.table-glass th {
  background: rgba(255, 255, 255, 0.035) !important;
  color: var(--text-muted) !important;
  border-bottom: 1px solid var(--glass-border) !important;
  font-weight: 600;
  font-size: 0.8rem;
  padding: 0.9rem 1.2rem !important;
  text-transform: uppercase;
  letter-spacing: 0.4px;
  white-space: nowrap;
}

.table-glass td {
  background: transparent !important;
  border-bottom: 1px solid rgba(255, 255, 255, 0.04) !important;
  font-size: 0.87rem;
  padding: 0.8rem 1.2rem !important;
  vertical-align: middle;
  color: var(--text-muted) !important;
  transition: color var(--t-fast) ease, background var(--t-fast) ease;
}

.table-glass tr:last-child td { border-bottom: none !important; }

.table-glass tbody tr:hover td {
  background: rgba(243, 156, 18, 0.03) !important;
  color: #fff !important;
}

/* Pagination */
.pagination-glass .page-item .page-link {
  background: rgba(12, 20, 40, 0.5);
  border: 1px solid var(--glass-border);
  color: var(--text-muted);
  border-radius: 8px;
  margin: 0 2px;
  padding: 0.45rem 0.8rem;
  font-size: 0.85rem;
  transition: all var(--t-fast) ease;
}

.pagination-glass .page-item.active .page-link {
  background: var(--accent-gradient);
  color: #fff;
  border-color: transparent;
  box-shadow: 0 3px 10px var(--accent-glow);
}

.pagination-glass .page-item.disabled .page-link {
  background: rgba(255,255,255,0.01);
  color: var(--text-dark);
  border-color: rgba(255,255,255,0.03);
}

.pagination-glass .page-item:not(.active):not(.disabled) .page-link:hover {
  background: rgba(243, 156, 18, 0.08);
  color: var(--primary-accent);
  border-color: rgba(243, 156, 18, 0.2);
}

/* ============================================================
   ANALYTICS
============================================================ */
.analytics-stat-card {
  padding: 1.25rem;
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  gap: 0.2rem;
}

.analytics-stat-icon {
  width: 42px; height: 42px;
  border-radius: 11px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
  margin-bottom: 0.5rem;
}

.analytics-stat-value { font-size: 1.8rem; font-weight: 800; color: #fff; line-height: 1.1; }
.analytics-stat-label { font-size: 0.78rem; font-weight: 600; color: var(--text-muted); text-transform: uppercase; letter-spacing: 0.4px; }
.analytics-stat-sub { font-size: 0.75rem; color: var(--text-dark); margin-top: 3px; }

/* Charts */
.chart-title {
  font-size: 0.9rem;
  font-weight: 700;
  color: #fff;
  margin-bottom: 1rem;
}

.chart-container {
  position: relative;
  height: 240px;
  width: 100%;
}

/* Pie chart legend */
.pie-legend { display: flex; flex-direction: column; gap: 0.65rem; }

.pie-legend-item {
  display: flex;
  align-items: center;
  gap: 0.7rem;
  font-size: 0.85rem;
}

.pie-legend-dot { width: 12px; height: 12px; border-radius: 4px; flex-shrink: 0; }
.pie-legend-label { flex: 1; color: var(--text-muted); }
.pie-legend-val { font-family: 'JetBrains Mono', monospace; color: #fff; font-weight: 700; font-size: 0.82rem; }

/* ============================================================
   SYSTEM INFO - Resource Gauges
============================================================ */
.gauge-ring {
  --pct: 0;
  --color: #f39c12;
  width: 90px;
  height: 90px;
  border-radius: 50%;
  background: conic-gradient(var(--color) calc(var(--pct) * 1%), rgba(255,255,255,0.06) 0%);
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 0 auto;
  position: relative;
  box-shadow: 0 0 20px rgba(0, 0, 0, 0.4);
}

.gauge-ring::after {
  content: '';
  position: absolute;
  inset: 10px;
  border-radius: 50%;
  background: var(--glass-bg);
}

.gauge-label {
  position: relative;
  z-index: 1;
  font-size: 1rem;
  font-weight: 800;
  color: #fff;
  font-family: 'JetBrains Mono', monospace;
}

.gauge-title {
  font-size: 0.78rem;
  font-weight: 600;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

/* ============================================================
   TOAST
============================================================ */
.glass-toast {
  background: rgba(8, 14, 28, 0.92) !important;
  backdrop-filter: blur(20px);
  -webkit-backdrop-filter: blur(20px);
  border: 1px solid var(--glass-border) !important;
  border-radius: 14px !important;
  color: var(--text-main) !important;
  box-shadow: 0 12px 40px rgba(0,0,0,0.5) !important;
  min-width: 300px;
}

.glass-toast .toast-header {
  background: rgba(255,255,255,0.03) !important;
  color: var(--text-main) !important;
  border-bottom: 1px solid var(--glass-border) !important;
  border-radius: 14px 14px 0 0 !important;
}

.glass-toast .toast-body { color: var(--text-muted); font-size: 0.87rem; }

/* ============================================================
   MODAL
============================================================ */
.glass-modal {
  background: rgba(8, 14, 28, 0.95) !important;
  backdrop-filter: blur(30px);
  -webkit-backdrop-filter: blur(30px);
  border: 1px solid var(--glass-border) !important;
  border-radius: 22px !important;
  box-shadow: 0 20px 60px rgba(0,0,0,0.7) !important;
  color: var(--text-main) !important;
}

.modal-backdrop { backdrop-filter: blur(4px); }

.modal-danger-icon {
  width: 70px; height: 70px;
  border-radius: 50%;
  background: rgba(255, 23, 68, 0.12);
  border: 2px solid rgba(255, 23, 68, 0.25);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 2rem;
  color: var(--color-danger);
  margin: 0 auto 0.5rem;
}

/* ============================================================
   MOBILE SIDEBAR BACKDROP
============================================================ */
.sidebar-backdrop {
  position: fixed;
  inset: 0;
  background: rgba(0,0,0,0.65);
  backdrop-filter: blur(4px);
  z-index: 1025;
  display: none;
  opacity: 0;
  transition: opacity var(--t-normal) ease;
}

.sidebar-backdrop.show {
  display: block;
  opacity: 1;
}

/* ============================================================
   ANIMATIONS
============================================================ */
@keyframes fadeInUp {
  from { opacity: 0; transform: translateY(14px); }
  to   { opacity: 1; transform: translateY(0); }
}

@keyframes pulseGreen {
  0%   { box-shadow: 0 0 0 0 rgba(0, 230, 118, 0.7); }
  70%  { box-shadow: 0 0 0 6px rgba(0, 230, 118, 0); }
  100% { box-shadow: 0 0 0 0 rgba(0, 230, 118, 0); }
}

@keyframes pulseRed {
  0%   { box-shadow: 0 0 0 0 rgba(255, 23, 68, 0.7); }
  70%  { box-shadow: 0 0 0 6px rgba(255, 23, 68, 0); }
  100% { box-shadow: 0 0 0 0 rgba(255, 23, 68, 0); }
}

@keyframes spin {
  to { transform: rotate(360deg); }
}

/* Spinner */
.spinner-border-sm {
  display: inline-block;
  width: 0.85rem;
  height: 0.85rem;
  border: 0.15rem solid currentColor;
  border-right-color: transparent;
  border-radius: 50%;
  animation: spin 0.65s linear infinite;
}

.sheets-stat-value {
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--text-main);
}

.sheets-analytics-container {
  background: rgba(5,10,20,0.4) !important;
}

body.light-theme .sheets-analytics-container {
  background: rgba(255,255,255,0.7) !important;
}

/* ============================================================
   RESPONSIVE — TABLET (≤1024px)
============================================================ */
@media (max-width: 1024px) {
  :root { --sidebar-width: 240px; --content-padding: 1.5rem; }
  #top-navbar { padding: 0 1.5rem; }
}

/* ============================================================
   RESPONSIVE — MOBILE (≤768px)
============================================================ */
@media (max-width: 768px) {
  :root { --content-padding: 1rem; }

  #sidebar { transform: translateX(-100%); }
  #sidebar.show { transform: translateX(0); box-shadow: 10px 0 40px rgba(0,0,0,0.6); }

  #main-wrapper { margin-left: 0 !important; }
  .sidebar-toggle { display: flex !important; }

  #top-navbar { padding: 0 1rem; }

  .section-title { font-size: 1.2rem; }

  .btn-action-group { flex-direction: column; }
  .btn-action-group .btn-glass-primary,
  .btn-action-group .btn-glass-secondary,
  .btn-action-group .btn-glass-danger,
  .btn-action-group .btn-glass-warning { width: 100%; justify-content: center; }

  .metric-value { font-size: 1.3rem; }

  .chart-container { height: 200px; }

  .gauge-ring { width: 75px; height: 75px; }
  .gauge-label { font-size: 0.85rem; }
}

/* ============================================================
   RESPONSIVE — SMALL MOBILE (≤480px)
============================================================ */
@media (max-width: 480px) {
  .nav-stat-chip { display: none !important; }
  .analytics-stat-value { font-size: 1.4rem; }
}

/* ============================================================
   MINI SIGNAL BARS & INDICATORS
============================================================ */
.mini-signal-bars {
  display: inline-flex;
  align-items: flex-end;
  gap: 2px;
  height: 12px;
  width: 18px;
  vertical-align: middle;
}

.mini-bar {
  display: block;
  width: 3px;
  background: rgba(255, 255, 255, 0.15);
  border-radius: 1px;
  transition: background-color var(--t-fast) var(--ease-out), box-shadow var(--t-fast) var(--ease-out);
}

.mini-bar.bar-1 { height: 25%; }
.mini-bar.bar-2 { height: 50%; }
.mini-bar.bar-3 { height: 75%; }
.mini-bar.bar-4 { height: 100%; }

/* Default active color (cyan) */
.mini-bar.active {
  background: var(--primary-accent);
  box-shadow: 0 0 4px var(--primary-accent);
}

/* Tower bars SIM active color (blue) */
#nav-tower-bars .mini-bar.active,
#card-tower-bars .mini-bar.active {
  background: var(--secondary-accent);
  box-shadow: 0 0 4px var(--secondary-accent);
}

/* Header signal chips hover */
.nav-stat-chip {
  transition: background-color var(--t-fast) var(--ease-out);
}
.nav-stat-chip:hover {
  background: rgba(255, 255, 255, 0.05);
}

/* Console simulator scrollbar styling */
#sim-console-output::-webkit-scrollbar {
  width: 3px;
}
#sim-console-output::-webkit-scrollbar-track {
  background: rgba(0, 0, 0, 0.2);
}
#sim-console-output::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.15);
  border-radius: 2px;
}

/* ============================================================
   UPGRADES: ENHANCED INTERFACES STYLING
   ============================================================ */
.summary-banner-row {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  margin-bottom: 1.5rem;
  position: relative;
  z-index: 1;
}

.summary-banner-card {
  flex: 1 1 calc(10% - 0.5rem);
  min-width: 110px;
  padding: 0.6rem 0.8rem;
  border-radius: 12px;
  background: var(--glass-bg);
  border: 1px solid var(--glass-border);
  display: flex;
  flex-direction: column;
  gap: 0.15rem;
  transition: all var(--t-fast) ease;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.15);
}

.summary-banner-card:hover {
  background: var(--glass-bg-hover);
  border-color: var(--glass-border-hover);
  transform: translateY(-2px);
}

.summary-card-label {
  font-size: 0.62rem;
  font-weight: 700;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.4px;
}

.summary-card-value {
  font-size: 0.82rem;
  font-weight: 700;
  color: #fff;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

body.light-theme .summary-card-value {
  color: #0f172a;
}

.ws-glow {
  animation: ws-pulse 2s infinite alternate;
}

@keyframes ws-pulse {
  0% { text-shadow: 0 0 4px var(--color-success); filter: drop-shadow(0 0 1px var(--color-success)); }
  100% { text-shadow: 0 0 12px var(--color-success); filter: drop-shadow(0 0 4px var(--color-success)); }
}

/* Skeleton Loading Animation */
.skeleton-row {
  display: flex;
  align-items: center;
  gap: 1rem;
  padding: 0.85rem 1rem;
  border-bottom: 1px solid var(--glass-border);
}

.skeleton-block {
  height: 12px;
  border-radius: 4px;
  background: rgba(255, 255, 255, 0.05);
  position: relative;
  overflow: hidden;
}

body.light-theme .skeleton-block {
  background: rgba(0, 0, 0, 0.06);
}

.skeleton-block::after {
  content: '';
  position: absolute;
  inset: 0;
  background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.06), transparent);
  transform: translateX(-100%);
  animation: shimmer 1.5s infinite;
}

body.light-theme .skeleton-block::after {
  background: linear-gradient(90deg, transparent, rgba(0, 0, 0, 0.04), transparent);
}

@keyframes shimmer {
  100% { transform: translateX(100%); }
}

.wifi-network-row {
  cursor: pointer;
  transition: all var(--t-fast) ease;
}

.wifi-network-row:hover td {
  background: rgba(243, 156, 18, 0.05) !important;
  color: #fff !important;
}

body.light-theme .wifi-network-row:hover td {
  background: rgba(0, 98, 255, 0.05) !important;
  color: #0f172a !important;
}

.status-dot-sidebar {
  width: 6px;
  height: 6px;
  border-radius: 50%;
  margin-left: auto;
  flex-shrink: 0;
}

.status-dot-sidebar.connected {
  background-color: var(--color-success);
  box-shadow: 0 0 6px var(--color-success);
  animation: pulse-dot 2s infinite;
}

.status-dot-sidebar.warning {
  background-color: var(--color-warning);
  box-shadow: 0 0 6px var(--color-warning);
  animation: pulse-dot-orange 2s infinite;
}

.status-dot-sidebar.disconnected {
  background-color: var(--color-danger);
  box-shadow: 0 0 6px var(--color-danger);
  animation: pulse-dot-red 2s infinite;
}

@keyframes pulse-dot-orange {
  0%, 100% { opacity: 1; transform: scale(1); box-shadow: 0 0 6px var(--color-warning); }
  50% { opacity: 0.7; transform: scale(0.85); box-shadow: 0 0 2px var(--color-warning); }
}

@keyframes pulse-dot-red {
  0%, 100% { opacity: 1; transform: scale(1); box-shadow: 0 0 6px var(--color-danger); }
  50% { opacity: 0.7; transform: scale(0.85); box-shadow: 0 0 2px var(--color-danger); }
}

.speed-metric {
  font-family: 'JetBrains Mono', monospace;
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--primary-accent);
}

.analytics-mini-graph {
  height: 60px;
  width: 100%;
}

/* ============================================================
   OPERATION MODE & TOPOLOGY DIAGRAM STYLE
============================================================ */
.mode-option-card {
  display: block;
  padding: 1.15rem 1.25rem;
  background: rgba(255, 255, 255, 0.015);
  border: 1px solid var(--glass-border);
  border-radius: 12px;
  cursor: pointer;
  transition: all var(--t-fast) ease;
}

.mode-option-card:hover {
  background: rgba(255, 255, 255, 0.035);
  transform: translateY(-2px);
  border-color: rgba(255, 255, 255, 0.15);
}

.mode-option-card.active {
  background: rgba(243, 156, 18, 0.04);
  border-color: var(--primary-accent);
  box-shadow: 0 0 15px rgba(243, 156, 18, 0.12);
}

#label-mode-sta.active {
  background: rgba(0, 230, 118, 0.04);
  border-color: var(--color-success);
  box-shadow: 0 0 15px rgba(0, 230, 118, 0.12);
}

#label-mode-dual.active {
  background: rgba(255, 145, 0, 0.04);
  border-color: var(--color-warning);
  box-shadow: 0 0 15px rgba(255, 145, 0, 0.12);
}

.mode-option-card input[type="radio"] {
  display: none;
}

.option-icon {
  font-size: 1.25rem;
  width: 24px;
  display: inline-flex;
  justify-content: center;
}

/* ============================================================
   TOPOLOGY MAP STYLES
   ============================================================ */
.topology-map-wrapper {
  min-height: 480px;
}

.topology-diagram {
  max-width: 500px;
}

.topo-node {
  opacity: 0.45;
  transition: all var(--t-normal) ease;
}

.topo-node.active {
  opacity: 1;
}

.topo-node-icon {
  width: 46px;
  height: 46px;
  border-radius: 50%;
  background: rgba(255, 255, 255, 0.03);
  border: 2px solid rgba(255, 255, 255, 0.1);
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 0 auto;
  font-size: 1.3rem;
  color: var(--text-dark);
  transition: all var(--t-normal) ease;
}

/* Small nodes for Cloud Services */
.topo-node-small .topo-node-icon {
  width: 32px;
  height: 32px;
  font-size: 1rem;
}

/* Glowing active node icons */
#topo-node-tower.active .topo-node-icon {
  background: rgba(253, 126, 20, 0.1);
  border-color: #fd7e14;
  color: #fd7e14;
  box-shadow: 0 0 12px rgba(253, 126, 20, 0.3);
}

#topo-node-gsm.active .topo-node-icon {
  background: rgba(243, 156, 18, 0.1);
  border-color: var(--primary-accent);
  color: var(--primary-accent);
  box-shadow: 0 0 12px rgba(243, 156, 18, 0.3);
}

#topo-node-router.active .topo-node-icon {
  background: rgba(0, 230, 118, 0.1);
  border-color: var(--color-success);
  color: var(--color-success);
  box-shadow: 0 0 12px rgba(0, 230, 118, 0.3);
}

#topo-node-clients.active .topo-node-icon {
  background: rgba(243, 156, 18, 0.1);
  border-color: var(--primary-accent);
  color: var(--primary-accent);
  box-shadow: 0 0 12px rgba(243, 156, 18, 0.3);
}

#topo-node-esp32.active .topo-node-icon {
  background: rgba(0, 230, 118, 0.12);
  border-color: var(--color-success);
  color: var(--color-success);
  box-shadow: 0 0 15px rgba(0, 230, 118, 0.4);
}

#topo-node-ivr.active .topo-node-icon {
  background: rgba(0, 230, 118, 0.1);
  border-color: var(--color-success);
  color: var(--color-success);
  box-shadow: 0 0 12px rgba(0, 230, 118, 0.3);
}

#topo-node-sd.active .topo-node-icon {
  background: rgba(189, 195, 199, 0.1);
  border-color: #bdc3c7;
  color: #bdc3c7;
  box-shadow: 0 0 12px rgba(189, 195, 199, 0.3);
}

#topo-node-sheets.active .topo-node-icon {
  background: rgba(15, 157, 88, 0.1);
  border-color: #0f9d58;
  color: #0f9d58;
  box-shadow: 0 0 12px rgba(15, 157, 88, 0.3);
}

#topo-node-firebase.active .topo-node-icon {
  background: rgba(255, 202, 40, 0.1);
  border-color: #ffca28;
  color: #ffca28;
  box-shadow: 0 0 12px rgba(255, 202, 40, 0.3);
}

#topo-node-telegram.active .topo-node-icon {
  background: rgba(0, 136, 204, 0.1);
  border-color: #0088cc;
  color: #0088cc;
  box-shadow: 0 0 12px rgba(0, 136, 204, 0.3);
}

.topo-node-name {
  font-weight: 600;
  font-size: 0.78rem;
}

.topo-node-small .topo-node-name {
  font-size: 0.65rem;
}

/* Node Metrics Details */
.topo-metrics {
  background: rgba(0, 0, 0, 0.2);
  border-radius: 4px;
  padding: 4px 6px;
  font-family: monospace;
  font-size: 0.65rem;
  line-height: 1.2;
  text-align: left;
  border: 1px solid rgba(255, 255, 255, 0.05);
}
.topo-metric-hidden {
  display: none;
}
.topo-node:hover .topo-metric-hidden {
  display: block; /* Show hidden metrics like IMEI on hover */
}

/* Animations: Pulse & WiFi Waves */
.esp-pulse-ring {
  position: absolute;
  top: 50%; left: 50%;
  transform: translate(-50%, -50%);
  width: 100%; height: 100%;
  border-radius: 50%;
  border: 2px solid #00e5ff; /* Cyan heartbeat */
  animation: heartbeatPulse 1.5s cubic-bezier(0.2, 0.8, 0.2, 1) infinite;
  opacity: 0;
}
@keyframes heartbeatPulse {
  0% { transform: translate(-50%, -50%) scale(1); opacity: 0.8; }
  15% { transform: translate(-50%, -50%) scale(1.15); opacity: 0.4; }
  30% { transform: translate(-50%, -50%) scale(1); opacity: 0.8; }
  100% { transform: translate(-50%, -50%) scale(1.6); opacity: 0; }
}

.topo-wifi-wave {
  position: absolute;
  top: -10px;
  right: -10px;
  font-size: 0.8rem;
  color: var(--color-success);
  animation: wifiWave 1.5s infinite;
}
@keyframes wifiWave {
  0%, 100% { opacity: 0.4; }
  50% { opacity: 1; transform: scale(1.1); }
}

.audio-wave {
  position: absolute;
  top: -10px;
  right: -10px;
  font-size: 0.8rem;
  color: #00e5ff;
  animation: audioWave 0.8s infinite alternate;
}
@keyframes audioWave {
  0% { transform: scaleY(0.5); opacity: 0.5; }
  100% { transform: scaleY(1.5); opacity: 1; }
}

/* Connectors styles */
.topo-link-container {
  height: 40px;
}

/* SCADA SVG Path Styles */
.topo-node {
  z-index: 2;
  position: relative;
}
#topo-svg-overlay {
  z-index: 1;
  pointer-events: none;
}

/* Active flow path — no background, just the animated line */
.scada-path-flow {
  fill: none;
  stroke-width: 2.5;
  stroke-linecap: round;
  transition: stroke 0.35s ease, filter 0.35s ease, stroke-opacity 0.35s ease;
}

/* Connected = Animating GREEN flow based on connection */
.scada-path-flow.status-good {
  stroke: #00e676;
  stroke-opacity: 0.95;
  stroke-dasharray: 8 6;
  filter: drop-shadow(0 0 4px rgba(0,230,118,0.7));
  animation: scadaFlowFwd 0.8s linear infinite;
}
.scada-path-flow.status-static-white {
  stroke: #ffffff;
  stroke-opacity: 0.85;
  stroke-dasharray: none;
  filter: drop-shadow(0 0 3px rgba(255, 255, 255, 0.5));
  animation: none;
}
/* Weak Signal = Animating AMBER/YELLOW flow */
.scada-path-flow.status-weak {
  stroke: #ffb300;
  stroke-opacity: 0.95;
  stroke-dasharray: 8 6;
  filter: drop-shadow(0 0 4px rgba(255,179,0,0.7));
  animation: scadaFlowFwd 1.2s linear infinite;
}

/* Connected with ACTIVE SIGNAL FLOW (Transfer) = Faster Animating CYAN dash flow */
.scada-path-flow.status-transfer {
  stroke: #00e5ff;
  stroke-opacity: 1;
  stroke-dasharray: 6 4;
  filter: drop-shadow(0 0 6px rgba(0,229,255,0.95));
  animation: scadaFlowFwd 0.4s linear infinite;
}

/* Disconnected = RED pulsing — no dash flow, just solid/dashed pulse */
.scada-path-flow.status-error {
  stroke: #ff1744;
  stroke-opacity: 0.95;
  stroke-dasharray: none;
  filter: drop-shadow(0 0 5px rgba(255,23,68,0.85));
  animation: scadaErrorPulse 1.2s ease-in-out infinite;
}
.scada-path-flow.status-inactive {
  stroke: #ff1744;
  stroke-opacity: 0.7;
  stroke-dasharray: 4 4;
  filter: drop-shadow(0 0 3px rgba(255,23,68,0.5));
  animation: scadaErrorPulse 2.0s ease-in-out infinite;
}

@keyframes scadaFlowFwd {
  to { stroke-dashoffset: -14; }
}
@keyframes scadaErrorPulse {
  0%,100% { stroke-opacity: 0.25; }
  50%      { stroke-opacity: 1; }
}

/* Warning icon on disconnected links */
.scada-warn-symbol {
  display: none;
  pointer-events: none;
  user-select: none;
  animation: scadaWarnBlink 1s ease-in-out infinite;
}
.scada-warn-symbol polygon {
  fill: #ff1744;
  stroke: #ff1744;
  stroke-width: 2px;
  stroke-linejoin: round;
  filter: drop-shadow(0 0 4px rgba(255, 23, 68, 0.8));
}
.scada-warn-symbol text {
  fill: #080d1a;
  font-family: 'Inter', sans-serif;
  font-weight: 900;
  font-size: 11px;
}
.scada-path-flow.status-error + .scada-warn-symbol,
.scada-path-flow.status-inactive + .scada-warn-symbol {
  display: inline;
}

@keyframes scadaWarnBlink {
  0%,100% { opacity: 0.4; }
  50%      { opacity: 1; }
}

/* Warning badge on node icons when disconnected/inactive */
.topo-warn-badge {
  position: absolute;
  top: -6px;
  right: -6px;
  font-size: 0.75rem;
  color: #ff1744;
  background: #080d1a;
  border-radius: 50%;
  padding: 1px 3px;
  border: 1px solid rgba(255,23,68,0.4);
  box-shadow: 0 0 6px rgba(255,23,68,0.8);
  display: none;
  z-index: 10;
  pointer-events: none;
}
.topo-node:not(.active) .topo-warn-badge {
  display: inline-block;
  animation: warnBlink 1.2s ease-in-out infinite;
}
@keyframes warnBlink {
  0%, 100% { opacity: 0.3; transform: scale(0.9); }
  50% { opacity: 1; transform: scale(1.1); }
}

.topo-link {
  position: absolute;
  top: 0;
  bottom: 0;
  width: 2px;
  background: rgba(255, 255, 255, 0.1);
  overflow: hidden;
}

.topo-link-straight {
  left: 50%;
  transform: translateX(-50%);
}

.topo-link-angled-left {
  left: 30%;
  transform: rotate(30deg) scaleY(1.3);
  transform-origin: top center;
}

.topo-link-angled-right {
  right: 30%;
  transform: rotate(-30deg) scaleY(1.3);
  transform-origin: top center;
}

/* Flow Particles Animation */
.flow-particles {
  position: absolute;
  top: 0;
  left: -2px;
  width: 6px;
  height: 15px;
  border-radius: 4px;
  background: #fff;
  opacity: 0;
  filter: blur(1px) drop-shadow(0 0 5px #fff);
}

.topo-link.active .flow-particles {
  opacity: 1;
  animation: dataFlow 1.2s linear infinite;
}

/* Status Colors for Signal Paths */
.topo-link { width: 3px; }

.topo-link.status-good { background: rgba(0, 230, 118, 0.4); }
.topo-link.status-good .flow-particles { background: #00e676; box-shadow: 0 0 8px #00e676; }

.topo-link.status-transfer { background: rgba(0, 229, 255, 0.4); }
.topo-link.status-transfer .flow-particles { background: #00e5ff; box-shadow: 0 0 10px #00e5ff; }

.topo-link.status-weak { background: rgba(255, 179, 0, 0.4); }
.topo-link.status-weak .flow-particles { background: #ffb300; box-shadow: 0 0 8px #ffb300; }

.topo-link.status-warning { background: rgba(255, 145, 0, 0.5); animation: warningPulse 2s infinite; }
.topo-link.status-error { background: rgba(255, 23, 68, 0.5); animation: errorPulse 1s infinite; }
.topo-link.status-inactive { background: rgba(158, 158, 158, 0.3); }

/* Warning & Error Pulsing for Links */
@keyframes errorPulse {
  0% { background: rgba(255, 23, 68, 0.2); }
  50% { background: rgba(255, 23, 68, 0.8); }
  100% { background: rgba(255, 23, 68, 0.2); }
}
@keyframes warningPulse {
  0% { background: rgba(255, 145, 0, 0.2); }
  50% { background: rgba(255, 145, 0, 0.8); }
  100% { background: rgba(255, 145, 0, 0.2); }
}

@keyframes dataFlow {
  0% { transform: translateY(-20px); opacity: 0; }
  20% { opacity: 1; }
  80% { opacity: 1; }
  100% { transform: translateY(60px); opacity: 0; }
}

/* Mode loading overlay style */
.mode-overlay {
  position: absolute;
  top: 0; left: 0; right: 0; bottom: 0;
  z-index: 100;
  background: rgba(8, 13, 26, 0.75);
  backdrop-filter: blur(8px);
  border-radius: 16px;
  display: flex;
  align-items: center;
  justify-content: center;
  animation: fadeIn 0.2s ease-in-out;
}

/* Light Theme overrides */
body.light-theme .mode-option-card {
  background: rgba(0, 0, 0, 0.01);
  border-color: rgba(0, 0, 0, 0.08);
}
body.light-theme .mode-option-card:hover {
  background: rgba(0, 0, 0, 0.025);
  border-color: rgba(0, 0, 0, 0.15);
}
body.light-theme .mode-option-card.active {
  background: rgba(0, 98, 255, 0.03);
  border-color: #0062ff;
}
body.light-theme #label-mode-sta.active {
  background: rgba(0, 200, 83, 0.03);
  border-color: #00c853;
}
body.light-theme #label-mode-dual.active {
  background: rgba(255, 109, 0, 0.03);
  border-color: #ff6d00;
}
body.light-theme .topo-node-icon {
  background: rgba(0, 0, 0, 0.02);
  border-color: rgba(0, 0, 0, 0.1);
}
body.light-theme .mode-overlay {
  background: rgba(255, 255, 255, 0.8);
}

/* ============================================================
   GOOGLE SHEETS SYNC CARD  (STA Mode Dashboard)
   ============================================================ */

/* Card shell */
.sheets-sync-card {
  border-color: rgba(0, 230, 118, 0.18) !important;
  background: linear-gradient(135deg,
    rgba(0, 230, 118, 0.03) 0%,
    rgba(243, 156, 18, 0.03) 100%);
}

/* Icon wrap */
.sheets-sync-icon-wrap {
  width: 44px;
  height: 44px;
  border-radius: 12px;
  background: rgba(0, 230, 118, 0.12);
  border: 1px solid rgba(0, 230, 118, 0.25);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.3rem;
  flex-shrink: 0;
}

/* Status pill */
.sheets-sync-status-pill {
  display: inline-flex;
  align-items: center;
  gap: 7px;
  padding: 5px 14px;
  border-radius: 20px;
  font-size: 0.75rem;
  font-weight: 600;
  letter-spacing: 0.03em;
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid rgba(255, 255, 255, 0.1);
  white-space: nowrap;
  flex-shrink: 0;
}

.sheets-sync-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: #555;
  display: inline-block;
}
.sheets-sync-dot.connected  { background: #00e676; box-shadow: 0 0 6px #00e676; animation: pulse-dot 1.5s infinite; }
.sheets-sync-dot.error      { background: #ff5252; box-shadow: 0 0 6px #ff5252; }
.sheets-sync-dot.testing    { background: #ffd740; box-shadow: 0 0 6px #ffd740; animation: pulse-dot 0.8s infinite; }

@keyframes pulse-dot {
  0%, 100% { opacity: 1; }
  50%       { opacity: 0.3; }
}

/* Stats grid */
.sheets-stats-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 1px;
  border-radius: 12px;
  overflow: hidden;
  border: 1px solid rgba(255,255,255,0.06);
}
.sheets-stat-item {
  display: flex;
  flex-direction: column;
  gap: 4px;
  padding: 14px 16px;
  background: rgba(255,255,255,0.03);
}
.sheets-stat-item:nth-child(1) { border-right: 1px solid rgba(255,255,255,0.05); border-bottom: 1px solid rgba(255,255,255,0.05); }
.sheets-stat-item:nth-child(2) { border-bottom: 1px solid rgba(255,255,255,0.05); }
.sheets-stat-item:nth-child(3) { border-right: 1px solid rgba(255,255,255,0.05); }
.sheets-stat-label {
  font-size: 0.7rem;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.06em;
  font-weight: 600;
}
.sheets-stat-value {
  font-size: 1.05rem;
  font-weight: 700;
  color: var(--text-primary);
}

/* JSON preview pane */
.sheets-payload-preview {
  background: rgba(0,0,0,0.35);
  border: 1px solid rgba(255,255,255,0.07);
  border-radius: 10px;
  padding: 14px;
  display: flex;
  flex-direction: column;
  min-height: 140px;
}
.sheets-json-preview {
  font-size: 0.7rem;
  color: #a8e6cf;
  margin: 0;
  white-space: pre-wrap;
  word-break: break-all;
  flex: 1;
  overflow: auto;
  line-height: 1.6;
}

/* Glass inputs for this card */
.glass-input {
  background: rgba(255,255,255,0.04) !important;
  border: 1px solid rgba(255,255,255,0.1) !important;
  color: #fff !important;
  border-radius: 8px !important;
  font-size: 0.83rem !important;
  transition: border-color 0.25s, box-shadow 0.25s;
}
.glass-input:focus {
  background: rgba(255,255,255,0.07) !important;
  border-color: var(--accent-primary) !important;
  box-shadow: 0 0 0 3px rgba(243, 156, 18, 0.12) !important;
  outline: none;
}
.glass-input::placeholder { color: rgba(255,255,255,0.28) !important; }

.glass-select {
  background: rgba(255,255,255,0.04) !important;
  border: 1px solid rgba(255,255,255,0.1) !important;
  color: #fff !important;
  border-radius: 8px !important;
  font-size: 0.83rem !important;
  transition: border-color 0.25s;
}
.glass-select:focus {
  border-color: var(--accent-primary) !important;
  box-shadow: 0 0 0 3px rgba(243, 156, 18, 0.12) !important;
  outline: none;
}
.glass-select option { background: #1a1a2e; color: #fff; }

/* Checkbox accent */
.sheets-sync-card .form-check-input:checked {
  background-color: var(--accent-primary);
  border-color: var(--accent-primary);
}

/* Light-theme overrides */
body.light-theme .sheets-sync-card {
  border-color: rgba(0, 200, 83, 0.25) !important;
  background: linear-gradient(135deg,
    rgba(0, 230, 118, 0.03) 0%,
    rgba(0, 150, 200, 0.03) 100%);
}
body.light-theme .sheets-sync-icon-wrap {
  background: rgba(0, 200, 83, 0.1);
  border-color: rgba(0, 200, 83, 0.2);
}
body.light-theme .sheets-sync-status-pill {
  background: rgba(0,0,0,0.04);
  border-color: rgba(0,0,0,0.1);
  color: #333;
}
body.light-theme .sheets-payload-preview {
  background: rgba(0,0,0,0.04);
  border-color: rgba(0,0,0,0.08);
}
body.light-theme .sheets-json-preview { color: #1b7a45; }
body.light-theme .sheets-stat-item    { background: rgba(0,0,0,0.02); }
body.light-theme .glass-input  { background: #fff !important; border-color: #dee2e6 !important; color: #212529 !important; }
body.light-theme .glass-input::placeholder { color: #aaa !important; }
body.light-theme .glass-select { background: #fff !important; border-color: #dee2e6 !important; color: #212529 !important; }
body.light-theme .glass-select option { background: #fff; color: #212529; }


/* ============================================================
   FIRST-RUN SETUP WIZARD  (.fro-*)
   ============================================================ */

/* Full-screen overlay */
.fro-overlay {
  position: fixed;
  inset: 0;
  z-index: 9999;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 20px;
  background: radial-gradient(ellipse at 30% 20%, rgba(243, 156, 18, 0.12) 0%, transparent 60%),
              radial-gradient(ellipse at 70% 80%, rgba(79,172,254,0.1) 0%, transparent 60%),
              #080d1a;
  overflow-y: auto;
}
.fro-overlay.fro-hidden {
  display: none;
}

/* Background orbs */
.fro-bg-orbs { position: fixed; inset: 0; pointer-events: none; overflow: hidden; }
.fro-orb {
  position: absolute;
  border-radius: 50%;
  filter: blur(60px);
  animation: fro-orb-drift 12s ease-in-out infinite alternate;
}
.fro-orb.orb-1 { width: 400px; height: 400px; background: rgba(243, 156, 18, 0.07); top: -100px; left: -100px; }
.fro-orb.orb-2 { width: 350px; height: 350px; background: rgba(79,172,254,0.07); bottom: -80px; right: -80px; animation-delay: 4s; }
.fro-orb.orb-3 { width: 250px; height: 250px; background: rgba(0,230,118,0.05); top: 50%; left: 50%; transform: translate(-50%, -50%); animation-delay: 8s; }
@keyframes fro-orb-drift {
  from { transform: translateY(0) scale(1); }
  to   { transform: translateY(-30px) scale(1.1); }
}

/* Step tracker bar */
.fro-step-bar {
  display: flex;
  align-items: center;
  gap: 0;
  margin-bottom: 32px;
  position: relative;
  z-index: 1;
}
.fro-step-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 5px;
  position: relative;
}
.fro-step-item span {
  width: 36px;
  height: 36px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-weight: 700;
  font-size: 0.85rem;
  background: rgba(255,255,255,0.07);
  border: 2px solid rgba(255,255,255,0.12);
  color: rgba(255,255,255,0.4);
  transition: all 0.4s ease;
}
.fro-step-item small {
  font-size: 0.65rem;
  color: rgba(255,255,255,0.3);
  letter-spacing: 0.04em;
  transition: color 0.4s;
  white-space: nowrap;
}
.fro-step-item.active span {
  background: linear-gradient(135deg, #f39c12, #4facfe);
  border-color: #f39c12;
  color: #080d1a;
  box-shadow: 0 0 16px rgba(243, 156, 18, 0.5);
}
.fro-step-item.active small { color: #f39c12; }
.fro-step-item.done span {
  background: rgba(0,230,118,0.15);
  border-color: #00e676;
  color: #00e676;
}
.fro-step-item.done small { color: #00e676; }
.fro-step-line {
  width: 48px;
  height: 2px;
  background: rgba(255,255,255,0.08);
  margin-bottom: 22px;
  transition: background 0.4s;
  flex-shrink: 0;
}
.fro-step-line.done { background: linear-gradient(90deg, #f39c12, #00e676); }

/* Card */
.fro-card {
  background: rgba(255,255,255,0.04);
  backdrop-filter: blur(20px);
  -webkit-backdrop-filter: blur(20px);
  border: 1px solid rgba(255,255,255,0.09);
  border-radius: 24px;
  padding: 40px;
  width: 100%;
  max-width: 560px;
  text-align: center;
  position: relative;
  z-index: 1;
  animation: fro-card-in 0.45s cubic-bezier(0.34, 1.56, 0.64, 1) both;
}
@keyframes fro-card-in {
  from { opacity: 0; transform: translateY(24px) scale(0.96); }
  to   { opacity: 1; transform: translateY(0) scale(1); }
}

/* Icon ring */
.fro-icon-ring {
  width: 72px;
  height: 72px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.8rem;
  margin: 0 auto 20px;
  position: relative;
  z-index: 1;
}
.fro-icon-ring.blue { background: rgba(243, 156, 18, 0.12); border: 2px solid rgba(243, 156, 18, 0.3); color: #f39c12; }
.fro-icon-ring.green { background: rgba(0,230,118,0.12); border: 2px solid rgba(0,230,118,0.3); color: #00e676; }
.fro-icon-ring.cyan  { background: rgba(79,172,254,0.12); border: 2px solid rgba(79,172,254,0.3); color: #4facfe; }

/* Titles */
.fro-title {
  font-size: 1.6rem;
  font-weight: 800;
  color: #fff;
  margin: 0 0 6px;
  letter-spacing: -0.02em;
}
.fro-subtitle {
  color: rgba(255,255,255,0.45);
  font-size: 0.9rem;
  margin-bottom: 24px;
}

/* Info block (step 1) */
.fro-info-block {
  text-align: left;
  background: rgba(255,255,255,0.03);
  border: 1px solid rgba(255,255,255,0.07);
  border-radius: 14px;
  padding: 18px;
  margin-bottom: 24px;
  display: flex;
  flex-direction: column;
  gap: 16px;
}
.fro-info-row {
  display: flex;
  gap: 14px;
  align-items: flex-start;
}
.fro-info-icon {
  width: 32px;
  height: 32px;
  border-radius: 8px;
  background: rgba(255,255,255,0.05);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 0.9rem;
  flex-shrink: 0;
  margin-top: 2px;
}
.fro-code {
  background: rgba(243, 156, 18, 0.1);
  color: #f39c12;
  border-radius: 4px;
  padding: 1px 6px;
  font-size: 0.8rem;
  font-family: 'JetBrains Mono', monospace;
}

/* Form fields */
.fro-field {
  text-align: left;
  margin-bottom: 16px;
  position: relative;
}
.fro-label {
  display: block;
  font-size: 0.8rem;
  font-weight: 600;
  color: rgba(255,255,255,0.7);
  margin-bottom: 7px;
  letter-spacing: 0.02em;
}
.fro-input-wrap {
  position: relative;
  display: flex;
}
.fro-input {
  flex: 1;
  background: rgba(255,255,255,0.05);
  border: 1px solid rgba(255,255,255,0.12);
  border-radius: 10px;
  padding: 12px 46px 12px 14px;
  color: #fff;
  font-size: 0.9rem;
  transition: border-color 0.25s, box-shadow 0.25s;
  font-family: 'Inter', sans-serif;
  outline: none;
  width: 100%;
}
.fro-input:focus {
  border-color: #f39c12;
  box-shadow: 0 0 0 3px rgba(243, 156, 18, 0.12);
}
.fro-input::placeholder { color: rgba(255,255,255,0.25); }
.fro-input.fro-error { border-color: #ff5252; box-shadow: 0 0 0 3px rgba(255,82,82,0.12); }
.fro-scan-btn, .fro-eye-btn {
  position: absolute;
  right: 0;
  top: 0;
  bottom: 0;
  width: 42px;
  border: none;
  border-radius: 0 10px 10px 0;
  background: rgba(243, 156, 18, 0.1);
  color: #f39c12;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: background 0.2s;
  font-size: 0.85rem;
}
.fro-scan-btn:hover, .fro-eye-btn:hover { background: rgba(243, 156, 18, 0.2); }

/* Network dropdown */
.fro-networks-dropdown {
  position: absolute;
  top: calc(100% + 4px);
  left: 0;
  right: 0;
  background: #0f172a;
  border: 1px solid rgba(255,255,255,0.1);
  border-radius: 10px;
  z-index: 100;
  overflow: hidden;
  box-shadow: 0 8px 30px rgba(0,0,0,0.5);
}
.fro-network-item {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 10px 14px;
  cursor: pointer;
  border-bottom: 1px solid rgba(255,255,255,0.04);
  transition: background 0.15s;
}
.fro-network-item:last-child { border-bottom: none; }
.fro-network-item:hover { background: rgba(243, 156, 18, 0.07); }
.fro-network-ssid { color: #fff; font-size: 0.85rem; font-weight: 500; }
.fro-network-meta { color: rgba(255,255,255,0.4); font-size: 0.7rem; margin-left: auto; display: flex; gap: 8px; align-items: center; }

/* Password strength */
.fro-strength-bar-wrap {
  display: flex;
  align-items: center;
  gap: 10px;
  margin-bottom: 12px;
}
.fro-strength-bar {
  flex: 1;
  height: 4px;
  background: rgba(255,255,255,0.08);
  border-radius: 4px;
  overflow: hidden;
}
.fro-strength-fill {
  height: 100%;
  width: 0%;
  border-radius: 4px;
  transition: width 0.35s ease, background 0.35s;
  background: #ff5252;
}
.fro-strength-label {
  font-size: 0.7rem;
  color: rgba(255,255,255,0.35);
  white-space: nowrap;
  min-width: 90px;
  text-align: right;
}

/* Security note */
.fro-security-note {
  font-size: 0.75rem;
  color: rgba(255,255,255,0.35);
  background: rgba(0,230,118,0.04);
  border: 1px solid rgba(0,230,118,0.1);
  border-radius: 8px;
  padding: 8px 12px;
  text-align: left;
  margin-bottom: 22px;
}

/* Connecting animation */
.fro-connecting-anim {
  position: relative;
  width: 100px;
  height: 100px;
  margin: 0 auto;
}
.fro-pulse-ring {
  position: absolute;
  inset: 0;
  border-radius: 50%;
  border: 2px solid rgba(79,172,254,0.4);
  animation: fro-pulse 2s ease-out infinite;
}
.fro-pulse-ring.delay-1 { animation-delay: 0.6s; }
.fro-pulse-ring.delay-2 { animation-delay: 1.2s; }
@keyframes fro-pulse {
  0%   { transform: scale(0.8); opacity: 1; }
  100% { transform: scale(2.2); opacity: 0; }
}
.fro-spin-wrap {
  position: absolute;
  inset: 14px;
  margin: 0;
  width: auto;
  height: auto;
}

/* Connect log */
.fro-connect-log {
  background: rgba(0,0,0,0.3);
  border: 1px solid rgba(255,255,255,0.06);
  border-radius: 10px;
  padding: 14px;
  text-align: left;
  min-height: 80px;
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.72rem;
  color: #a8e6cf;
  display: flex;
  flex-direction: column;
  gap: 4px;
  margin-top: 20px;
}
.fro-log-line { display: flex; gap: 8px; }
.fro-log-time { color: rgba(255,255,255,0.25); }
.fro-log-ok   { color: #00e676; }
.fro-log-warn { color: #ffd740; }
.fro-log-err  { color: #ff5252; }

/* Success grid */
.fro-success-pop {
  animation: fro-pop 0.5s cubic-bezier(0.34, 1.56, 0.64, 1) both 0.1s;
}
@keyframes fro-pop {
  from { transform: scale(0.5); opacity: 0; }
  to   { transform: scale(1);   opacity: 1; }
}
.fro-success-grid {
  background: rgba(255,255,255,0.03);
  border: 1px solid rgba(255,255,255,0.07);
  border-radius: 14px;
  overflow: hidden;
  text-align: left;
  margin-bottom: 20px;
}
.fro-success-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 18px;
  border-bottom: 1px solid rgba(255,255,255,0.05);
}
.fro-success-row:last-child { border-bottom: none; }

/* Reconnect notice */
.fro-reconnect-notice {
  display: flex;
  gap: 12px;
  align-items: flex-start;
  background: rgba(255,164,0,0.06);
  border: 1px solid rgba(255,164,0,0.15);
  border-radius: 12px;
  padding: 14px 16px;
  text-align: left;
  margin-bottom: 24px;
}

/* Buttons */
.fro-actions {
  display: flex;
  flex-direction: column;
  gap: 10px;
}
.fro-btn-primary {
  width: 100%;
  padding: 14px;
  border-radius: 12px;
  border: none;
  background: linear-gradient(135deg, #f39c12, #4facfe);
  color: #080d1a;
  font-size: 0.95rem;
  font-weight: 700;
  cursor: pointer;
  transition: opacity 0.2s, transform 0.15s, box-shadow 0.2s;
  letter-spacing: 0.01em;
  box-shadow: 0 4px 20px rgba(243, 156, 18, 0.3);
}
.fro-btn-primary:hover { opacity: 0.9; transform: translateY(-1px); box-shadow: 0 6px 28px rgba(243, 156, 18, 0.4); }
.fro-btn-primary:active { transform: translateY(0); }
.fro-btn-primary:disabled { opacity: 0.45; cursor: not-allowed; transform: none; }
.fro-btn-ghost {
  width: 100%;
  padding: 11px;
  border-radius: 12px;
  border: 1px solid rgba(255,255,255,0.1);
  background: transparent;
  color: rgba(255,255,255,0.45);
  font-size: 0.85rem;
  cursor: pointer;
  transition: color 0.2s, border-color 0.2s;
}
.fro-btn-ghost:hover { color: #fff; border-color: rgba(255,255,255,0.25); }

/* Error shake */
@keyframes fro-shake {
  0%, 100% { transform: translateX(0); }
  20%, 60%  { transform: translateX(-8px); }
  40%, 80%  { transform: translateX(8px); }
}
.fro-shake { animation: fro-shake 0.4s ease; }

/* Responsive */
@media (max-width: 576px) {
  .fro-card { padding: 28px 20px; }
  .fro-step-line { width: 28px; }
  .fro-title { font-size: 1.3rem; }
  .fro-step-item small { display: none; }
}

/* ============================================================
   LIGHT-THEME OVERRIDES FOR FIRST-RUN SETUP WIZARD
   ============================================================ */
body.light-theme .fro-overlay {
  background: radial-gradient(ellipse at 30% 20%, rgba(0, 86, 204, 0.08) 0%, transparent 60%),
              radial-gradient(ellipse at 70% 80%, rgba(0, 145, 234, 0.06) 0%, transparent 60%),
              #f1f5f9;
}
body.light-theme .fro-card {
  background: rgba(255, 255, 255, 0.85);
  border-color: rgba(0, 0, 0, 0.08);
  box-shadow: 0 10px 40px rgba(0, 0, 0, 0.06);
}
body.light-theme .fro-title {
  color: #0f172a;
}
body.light-theme .fro-subtitle {
  color: #64748b;
}
body.light-theme .fro-label {
  color: #334155;
}
body.light-theme .fro-input {
  background: #fff;
  border-color: #dee2e6;
  color: #212529;
}
body.light-theme .fro-input:focus {
  border-color: #0056cc;
  box-shadow: 0 0 0 3px rgba(0, 86, 204, 0.12);
}
body.light-theme .fro-input::placeholder {
  color: #999;
}
body.light-theme .fro-strength-bar {
  background: rgba(0, 0, 0, 0.08);
}
body.light-theme .fro-strength-label {
  color: #64748b;
}
body.light-theme .fro-security-note {
  color: #1b7a45;
  background: rgba(0, 230, 118, 0.06);
  border-color: rgba(0, 230, 118, 0.15);
}
body.light-theme .fro-btn-ghost {
  border-color: rgba(0, 0, 0, 0.15);
  color: #475569;
}
body.light-theme .fro-btn-ghost:hover {
  color: #0f172a;
  border-color: rgba(0, 0, 0, 0.3);
  background: rgba(0, 0, 0, 0.03);
}
body.light-theme .fro-info-block {
  background: rgba(0, 0, 0, 0.02);
  border-color: rgba(0, 0, 0, 0.06);
}
body.light-theme .fro-info-icon {
  background: rgba(0, 0, 0, 0.04);
}
body.light-theme .fro-networks-dropdown {
  background: #ffffff;
  border-color: #dee2e6;
  box-shadow: 0 8px 30px rgba(0, 0, 0, 0.08);
}
body.light-theme .fro-network-item {
  border-bottom-color: rgba(0, 0, 0, 0.05);
}
body.light-theme .fro-network-item:hover {
  background: rgba(0, 86, 204, 0.05);
}
body.light-theme .fro-network-ssid {
  color: #0f172a;
}
body.light-theme .fro-network-meta {
  color: #64748b;
}
body.light-theme .fro-success-grid {
  background: rgba(0, 0, 0, 0.02);
  border-color: rgba(0, 0, 0, 0.06);
}
body.light-theme .fro-success-row {
  border-bottom-color: rgba(0, 0, 0, 0.04);
}
body.light-theme .fro-reconnect-notice {
  background: rgba(255, 164, 0, 0.06);
  border-color: rgba(255, 164, 0, 0.2);
  color: #b26a00;
}
body.light-theme .fro-connect-log {
  background: rgba(0, 0, 0, 0.04);
  border-color: rgba(0, 0, 0, 0.08);
  color: #0d5c3a;
}
body.light-theme .fro-log-time {
  color: rgba(0, 0, 0, 0.45);
}
body.light-theme .fro-step-item span {
  background: rgba(0, 0, 0, 0.04);
  border-color: rgba(0, 0, 0, 0.1);
  color: rgba(0, 0, 0, 0.4);
}
body.light-theme .fro-step-item small {
  color: rgba(0, 0, 0, 0.4);
}
body.light-theme .fro-step-item.active span {
  background: linear-gradient(135deg, #0056cc, #0091ea);
  border-color: #0056cc;
  color: #fff;
  box-shadow: 0 0 16px rgba(0, 86, 204, 0.3);
}
body.light-theme .fro-step-item.active small {
  color: #0056cc;
}
body.light-theme .fro-step-item.done span {
  background: rgba(0, 230, 118, 0.1);
  border-color: #00e676;
  color: #00c853;
}
body.light-theme .fro-step-item.done small {
  color: #00c853;
}
body.light-theme .fro-step-line {
  background: rgba(0, 0, 0, 0.08);
}
body.light-theme .fro-step-line.done {
  background: linear-gradient(90deg, #0056cc, #00e676);
}
body.light-theme .fro-icon-ring.blue {
  background: rgba(0, 86, 204, 0.08);
  border-color: rgba(0, 86, 204, 0.25);
  color: #0056cc;
}
body.light-theme .fro-icon-ring.green {
  background: rgba(25, 135, 84, 0.08);
  border-color: rgba(25, 135, 84, 0.25);
  color: #198754;
}
body.light-theme .fro-icon-ring.cyan {
  background: rgba(0, 145, 234, 0.08);
  border-color: rgba(0, 145, 234, 0.25);
  color: #0091ea;
}
body.light-theme .fro-code {
  background: rgba(0, 86, 204, 0.06);
  color: #0056cc;
}

/* Force dark/light colors inside setup wizard text utility overrides */
body.light-theme .fro-card .text-white {
  color: #0f172a !important;
}
body.light-theme .fro-card .text-muted {
  color: #64748b !important;
}
body.light-theme .fro-card .text-info {
  color: #0056cc !important;
}
body.light-theme .fro-card .text-success {
  color: #198754 !important;
}
body.light-theme .fro-card .text-warning {
  color: #b25e00 !important;
}
body.light-theme .fro-card .text-danger {
  color: #dc3545 !important;
}

/* General text visibility overrides for Light Theme */
body.light-theme .glass-card .text-white:not(.badge) {
  color: var(--text-main) !important;
}
body.light-theme #top-navbar .text-white {
  color: var(--text-main) !important;
}

/* ============================================================
   VARMA AUTHORIZATION & KEYPAD STYLES
============================================================ */

/* Hand-Drawn V-Shield Logo & Animations */
.v-shield-svg {
  display: inline-block;
  vertical-align: middle;
  width: 100%;
  height: 100%;
}

.v-shield-logo-big {
  width: 130px;
  height: 130px;
  color: var(--primary-accent);
  margin: 0 auto 1.5rem auto;
  filter: drop-shadow(0 0 15px rgba(243, 156, 18, 0.4));
  animation: shieldPulse 2.5s infinite ease-in-out;
}

@keyframes shieldPulse {
  0%, 100% { transform: scale(1); }
  50% { transform: scale(1.05); }
}

/* Custom Login Overlay */
#login-overlay {
  position: fixed;
  inset: 0;
  background: rgba(8, 8, 8, 0.96);
  z-index: 10000;
  display: flex;
  align-items: center;
  justify-content: center;
  backdrop-filter: blur(20px);
  -webkit-backdrop-filter: blur(20px);
  transition: opacity 0.4s ease, visibility 0.4s ease;
}

#login-overlay.d-none {
  opacity: 0;
  visibility: hidden;
  display: none !important;
}

.login-card {
  background: var(--glass-bg);
  border: 2px solid var(--primary-accent);
  border-radius: 28px;
  padding: 3rem;
  width: 90%;
  max-width: 420px;
  text-align: center;
  box-shadow: 0 10px 40px rgba(243, 156, 18, 0.15);
  backdrop-filter: blur(16px);
  -webkit-backdrop-filter: blur(16px);
}

.login-subtext {
  font-size: 0.95rem;
  color: #cbd5e1 !important;
  font-weight: 700 !important;
  text-transform: none;
  margin-bottom: 2rem;
}

.input-group-p {
  position: relative;
  margin-bottom: 1rem;
}

.v-input {
  width: 100%;
  background: rgba(0, 0, 0, 0.4);
  border: 1px solid var(--glass-border);
  padding: 0.95rem 1rem;
  border-radius: 12px;
  color: var(--text-main);
  outline: none;
  transition: border-color 0.25s ease, box-shadow 0.25s ease;
}

.v-input:focus {
  border-color: var(--primary-accent);
  box-shadow: 0 0 0 2px rgba(243, 156, 18, 0.2);
}

.eye-icon {
  position: absolute;
  right: 15px;
  top: 50%;
  transform: translateY(-50%);
  color: var(--text-muted);
  cursor: pointer;
  transition: color 0.2s;
  z-index: 10;
}

.eye-icon:hover {
  color: var(--primary-accent);
}

/* Dialer Keypad Modal/Visual Grid */
.keypad-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 15px;
  max-width: 260px;
  margin: 1.5rem auto;
}

.dialer-key {
  background: rgba(0, 0, 0, 0.3);
  border: 2px solid var(--glass-border);
  border-radius: 50%;
  width: 65px;
  height: 65px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  font-weight: 700;
  font-size: 1.4rem;
  color: var(--text-main);
  cursor: pointer;
  transition: all 0.15s ease;
  user-select: none;
  margin: 0 auto;
}

.dialer-key small {
  font-size: 0.65rem;
  color: var(--text-muted);
  font-weight: 400;
  margin-top: -3px;
  text-transform: uppercase;
}

.dialer-key:hover {
  border-color: var(--primary-accent);
  color: var(--primary-accent);
  background: rgba(243, 156, 18, 0.05);
}

.dialer-key:active, .dialer-key.active {
  background: var(--primary-accent);
  color: #000 !important;
  border-color: var(--primary-accent);
  box-shadow: 0 0 15px var(--accent-glow);
  transform: scale(0.92);
}

.dialer-key:active small, .dialer-key.active small {
  color: #000;
}

/* Premium V-Button */
.btn-v {
  padding: 0.75rem 1.4rem;
  border-radius: 12px;
  font-weight: 700;
  text-transform: uppercase;
  transition: all 0.25s ease;
  border: none;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  font-size: 0.85rem;
}

.btn-v-orange {
  background: var(--accent-gradient);
  color: #fff !important;
  box-shadow: 0 4px 15px rgba(243, 156, 18, 0.2);
}

.btn-v-orange:hover {
  filter: brightness(1.1);
  box-shadow: 0 4px 20px rgba(243, 156, 18, 0.35);
}

/* ============================================================
   FUTURISTIC HUD CONTROL CENTER REDESIGN OVERRIDES
   ============================================================ */

/* 1. Base Container Adjustments for Floating Layout */
#sidebar {
  top: 1.25rem !important;
  bottom: 1.25rem !important;
  left: 1.25rem !important;
  height: calc(100vh - 2.5rem) !important;
  border-radius: 24px !important;
  background: rgba(6, 10, 20, 0.75) !important;
  border: 1px solid rgba(243, 156, 18, 0.08) !important;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.5), 0 0 20px rgba(243, 156, 18, 0.04) !important;
  overflow-y: auto !important;
  scrollbar-width: none; /* Firefox */
  z-index: 1040 !important;
}

#sidebar::-webkit-scrollbar {
  display: none; /* Chrome/Safari */
}

#main-wrapper {
  margin-left: calc(var(--sidebar-width) + 1.25rem) !important;
  padding: 1.25rem 1.25rem 1.25rem 0 !important;
  flex: 1;
  min-width: 0;
  display: flex;
  flex-direction: column;
}

/* 2. Top Navigation Bar (Floating Glass Panel) */
#top-navbar {
  background: rgba(26, 16, 2, 0.50) !important;
  border: 1px solid rgba(243, 156, 18, 0.08) !important;
  border-radius: 20px !important;
  margin-bottom: 1.25rem !important;
  padding: 0 1.5rem !important;
  height: var(--navbar-height) !important;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3), 0 0 15px rgba(243, 156, 18, 0.02) !important;
  position: sticky !important;
  top: 1.25rem !important;
  z-index: 1020 !important;
}

/* Global HUD Search Bar */
.nav-search-bar {
  background: rgba(0, 0, 0, 0.3) !important;
  border: 1px solid rgba(243, 156, 18, 0.12) !important;
  border-radius: 12px !important;
  padding: 0.45rem 1rem !important;
  display: flex;
  align-items: center;
  gap: 0.5rem;
  transition: all 0.3s ease;
}
.nav-search-bar:focus-within {
  border-color: rgba(243, 156, 18, 0.4) !important;
  box-shadow: 0 0 12px rgba(243, 156, 18, 0.2) !important;
  background: rgba(0, 0, 0, 0.5) !important;
}
.nav-search-bar .search-icon {
  color: var(--primary-accent) !important;
  font-size: 0.85rem;
}
.nav-search-bar .search-input {
  background: none !important;
  border: none !important;
  color: #fff !important;
  outline: none !important;
  font-size: 0.85rem;
  width: 120px;
  transition: width 0.3s ease;
}
.nav-search-bar .search-input:focus {
  width: 180px;
}

/* User Profile Card */
.nav-profile-card {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  background: rgba(255, 255, 255, 0.02) !important;
  border: 1px solid rgba(255, 255, 255, 0.05) !important;
  border-radius: 14px !important;
  padding: 0.35rem 0.75rem !important;
  cursor: pointer;
  transition: all 0.25s ease;
}
.nav-profile-card:hover {
  background: rgba(255, 255, 255, 0.06) !important;
  border-color: rgba(243, 156, 18, 0.2) !important;
}
.profile-avatar {
  width: 32px;
  height: 32px;
  border-radius: 10px;
  object-fit: cover;
  border: 1.5px solid rgba(243, 156, 18, 0.25) !important;
}
.profile-meta {
  display: flex;
  flex-direction: column;
  line-height: 1.2;
}
.profile-name {
  font-size: 0.8rem;
  font-weight: 600;
  color: #fff;
}
.profile-role {
  font-size: 0.65rem;
  color: var(--text-muted);
}

/* Nav Icons settings/bell */
.nav-icon-btn {
  width: 38px;
  height: 38px;
  border-radius: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(255, 255, 255, 0.02) !important;
  border: 1px solid rgba(255, 255, 255, 0.05) !important;
  color: var(--text-muted) !important;
  cursor: pointer;
  position: relative;
  transition: all 0.25s ease;
}
.nav-icon-btn:hover {
  color: var(--primary-accent) !important;
  background: rgba(243, 156, 18, 0.08) !important;
  border-color: rgba(243, 156, 18, 0.2) !important;
  transform: translateY(-1px);
}
.notification-badge {
  position: absolute;
  top: -4px;
  right: -4px;
  background: var(--color-danger) !important;
  color: #fff !important;
  font-size: 0.65rem;
  font-weight: 700;
  padding: 1px 4px;
  border-radius: 6px;
  border: 1.5px solid #0d0d0d !important;
  box-shadow: 0 0 8px rgba(239, 68, 68, 0.4);
}

/* 3. Sidebar Health Widget & Wave animation */
.sidebar-health-card {
  background: rgba(16, 185, 129, 0.02) !important;
  border: 1px solid rgba(16, 185, 129, 0.08) !important;
  border-radius: 16px !important;
  padding: 0.75rem 1rem !important;
  margin: 1rem 0.75rem !important;
}
.health-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 0.4rem;
}
.health-label {
  font-size: 0.65rem;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.8px;
}
.health-status-badge {
  background: rgba(16, 185, 129, 0.12) !important;
  color: #2ecc71 !important;
  font-size: 0.65rem;
  font-weight: 700;
  padding: 1px 6px;
  border-radius: 8px;
  border: 1px solid rgba(16, 185, 129, 0.15) !important;
}
.health-wave-container {
  height: 20px;
  overflow: hidden;
  position: relative;
  border-radius: 6px;
}
.health-wave-animate {
  width: 200%;
  height: 100%;
  position: absolute;
  left: 0;
  animation: healthWaveMove 4s linear infinite;
}
@keyframes healthWaveMove {
  0% { transform: translateX(0); }
  100% { transform: translateX(-50%); }
}

/* 4. Active Sidebar State with neon glow */
.sidebar-link.active {
  background: linear-gradient(135deg, rgba(243, 156, 18, 0.12) 0%, rgba(139, 69, 19, 0.12) 100%) !important;
  border: 1px solid rgba(243, 156, 18, 0.3) !important;
  box-shadow: 0 0 15px rgba(243, 156, 18, 0.15) !important;
  color: #fff !important;
  animation: sidebarActiveBorderPulse 4s infinite alternate;
}
.sidebar-link.active .sidebar-link-icon {
  background: linear-gradient(135deg, #f39c12 0%, #8B4513 100%) !important;
  color: #fff !important;
  box-shadow: 0 0 10px rgba(243, 156, 18, 0.5) !important;
  transform: scale(1.05);
}
@keyframes sidebarActiveBorderPulse {
  0% { border-color: rgba(243, 156, 18, 0.2); box-shadow: 0 0 8px rgba(243, 156, 18, 0.1); }
  100% { border-color: rgba(243, 156, 18, 0.45); box-shadow: 0 0 18px rgba(243, 156, 18, 0.25); }
}

/* 5. Metric Cards Layout & HUD Visualizer Elements */
.metric-card {
  display: flex !important;
  flex-direction: row !important;
  justify-content: space-between !important;
  align-items: center !important;
  min-height: 155px !important;
  padding: 1.25rem !important;
  position: relative !important;
  overflow: hidden !important;
}

.metric-content-left {
  flex: 1;
  display: flex;
  flex-direction: column;
  z-index: 2;
  min-width: 0;
}

.metric-label {
  font-family: 'Poppins', sans-serif !important;
  font-size: 0.75rem !important;
  text-transform: uppercase !important;
  letter-spacing: 0.8px !important;
  margin-bottom: 0.5rem !important;
}

.metric-value {
  font-size: 1.5rem !important;
  font-weight: 700 !important;
  line-height: 1.2 !important;
  margin-bottom: 0.35rem !important;
  letter-spacing: -0.5px !important;
}

.hud-action-link {
  font-size: 0.7rem !important;
  color: rgba(243, 156, 18, 0.7) !important;
  text-decoration: none !important;
  display: inline-flex;
  align-items: center;
  gap: 4px;
  margin-top: 0.5rem;
  transition: all 0.2s ease;
  font-weight: 600;
}
.hud-action-link:hover {
  color: #fff !important;
  text-shadow: 0 0 8px rgba(243, 156, 18, 0.5);
}
.hud-action-link .small-arrow {
  transition: transform 0.2s ease;
}
.hud-action-link:hover .small-arrow {
  transform: translateX(3px);
}

.hud-visualizer {
  width: 72px !important;
  height: 72px !important;
  position: relative !important;
  display: flex !important;
  align-items: center !important;
  justify-content: center !important;
  flex-shrink: 0 !important;
  margin-left: 0.75rem !important;
  z-index: 2;
}

.hud-visualizer svg {
  width: 100% !important;
  height: 100% !important;
  max-height: 72px !important;
  max-width: 72px !important;
}

.hud-icon {
  position: absolute;
  font-size: 1.25rem;
  color: var(--primary-accent);
  opacity: 0.85;
  text-shadow: 0 0 8px var(--accent-glow);
}

/* Radar Sweep (Device Status) */
.device-radar {
  border: 1px solid rgba(243, 156, 18, 0.1);
  border-radius: 50%;
}
.radar-circle {
  position: absolute;
  border: 1px solid rgba(243, 156, 18, 0.25);
  border-radius: 50%;
  width: 100%;
  height: 100%;
  animation: radarSweepCircle 3s infinite linear;
  opacity: 0;
}
.circle-2 { animation-delay: 1s; }
.circle-3 { animation-delay: 2s; }
.radar-sweep {
  position: absolute;
  width: 100%;
  height: 100%;
  background: conic-gradient(from 0deg, rgba(243, 156, 18, 0.15) 0%, transparent 60%);
  border-radius: 50%;
  animation: radarSweepRotate 4s linear infinite;
}
@keyframes radarSweepCircle {
  0% { transform: scale(0.1); opacity: 0.8; }
  100% { transform: scale(1.1); opacity: 0; }
}
@keyframes radarSweepRotate {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* Rotating Hologram Globe (Internet Status) */
.globe-visualizer svg {
  animation: globeRotate 16s linear infinite;
  filter: drop-shadow(0 0 4px rgba(243, 156, 18, 0.2));
}
@keyframes globeRotate {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* Floating Golden Microchip (SIM Status) */
.sim-visualizer svg {
  animation: simFloat 3.5s ease-in-out infinite alternate;
  filter: drop-shadow(0 0 6px rgba(249, 115, 22, 0.25));
}
@keyframes simFloat {
  0% { transform: translateY(2px) rotate(0deg); }
  100% { transform: translateY(-4px) rotate(3deg); }
}

/* Concentric WiFi Waves (WiFi Status) */
.wifi-waves .wifi-wave {
  position: absolute;
  border: 1.5px solid rgba(243, 156, 18, 0.25);
  border-radius: 50%;
  animation: wifiPulsate 2s infinite ease-out;
  opacity: 0;
}
.wifi-waves .wave-1 { width: 32px; height: 32px; animation-delay: 0s; }
.wifi-waves .wave-2 { width: 52px; height: 52px; animation-delay: 0.6s; }
.wifi-waves .wave-3 { width: 72px; height: 72px; animation-delay: 1.2s; }
@keyframes wifiPulsate {
  0% { transform: scale(0.6); opacity: 0.8; }
  100% { transform: scale(1.15); opacity: 0; }
}

/* Node connections (Router Gateway IP) */
.routing-nodes .nodes-svg circle {
  animation: nodeGlow 2.5s ease-in-out infinite alternate;
}
.routing-nodes .nodes-svg circle:nth-child(4) { animation-delay: 0.5s; }
.routing-nodes .nodes-svg circle:nth-child(5) { animation-delay: 1.2s; }
@keyframes nodeGlow {
  0% { r: 6px; stroke-width: 1.5px; filter: drop-shadow(0 0 2px var(--primary-accent)); }
  100% { r: 8px; stroke-width: 2.5px; filter: drop-shadow(0 0 8px var(--primary-accent)); }
}

/* Triangle Mesh node glow (Total Calls) */
.calls-nodes svg polygon {
  animation: polygonGlow 3s ease-in-out infinite alternate;
}
.calls-nodes svg circle {
  animation: callsCircleGlow 2s ease-in-out infinite alternate;
}
@keyframes polygonGlow {
  0% { fill: rgba(168, 85, 247, 0.02); stroke: rgba(168, 85, 247, 0.25); }
  100% { fill: rgba(168, 85, 247, 0.08); stroke: rgba(168, 85, 247, 0.7); }
}
@keyframes callsCircleGlow {
  0% { filter: drop-shadow(0 0 1px rgba(168, 85, 247, 0.3)); }
  100% { filter: drop-shadow(0 0 5px rgba(168, 85, 247, 0.8)); }
}

/* Transceiver signal tower broadcast (AP Clients) */
.ap-broadcast .broadcast-wave {
  transform-origin: 50px 40px;
  animation: broadcastWaveSignal 1.8s infinite linear;
  opacity: 0;
}
.ap-broadcast .wave-1 { animation-delay: 0s; }
.ap-broadcast .wave-2 { animation-delay: 0.6s; }
.ap-broadcast .wave-3 { animation-delay: 1.2s; }
@keyframes broadcastWaveSignal {
  0% { opacity: 0; transform: scale(0.6); }
  50% { opacity: 0.8; }
  100% { opacity: 0; transform: scale(1.15); }
}

/* Analog moving clock hands (System Uptime) */
.uptime-clock .hour-hand {
  transform-origin: 50px 50px;
  animation: clockRotateAnim 24s linear infinite;
}
.uptime-clock .minute-hand {
  transform-origin: 50px 50px;
  animation: clockRotateAnim 2s linear infinite;
}
@keyframes clockRotateAnim {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* 6. Modern Glass-Card Border overlays & Glows */
.glass-card::after {
  content: '';
  position: absolute;
  inset: 0;
  border-radius: var(--radius-card);
  padding: 1px;
  background: linear-gradient(135deg, rgba(243, 156, 18, 0.1), rgba(139, 69, 19, 0.05));
  -webkit-mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
  -webkit-mask-composite: xor;
  mask-composite: exclude;
  pointer-events: none;
  z-index: 1;
}

.glass-card[data-color="blue"]::after { background: linear-gradient(135deg, rgba(243, 156, 18, 0.15), transparent); }
.glass-card[data-color="green"]::after { background: linear-gradient(135deg, rgba(16, 185, 129, 0.15), transparent); }
.glass-card[data-color="orange"]::after { background: linear-gradient(135deg, rgba(249, 115, 22, 0.15), transparent); }
.glass-card[data-color="cyan"]::after { background: linear-gradient(135deg, rgba(6, 182, 212, 0.15), transparent); }
.glass-card[data-color="purple"]::after { background: linear-gradient(135deg, rgba(168, 85, 247, 0.15), transparent); }

.glass-card:hover::after {
  background: linear-gradient(135deg, rgba(243, 156, 18, 0.4), rgba(139, 69, 19, 0.25)) !important;
}

/* 7. Tables & Lists Clean Redesign */
.table-glass,
.table-glass thead,
.table-glass tbody,
.table-glass tr {
  --bs-table-bg: transparent !important;
  --bs-table-accent-bg: transparent !important;
  --bs-table-striped-bg: transparent !important;
  --bs-table-active-bg: transparent !important;
  --bs-table-hover-bg: transparent !important;
  background-color: transparent !important;
  background: transparent !important;
  border-collapse: separate !important;
  border-spacing: 0 6px !important;
}
.table-glass th {
  font-family: 'Poppins', sans-serif !important;
  font-size: 0.75rem !important;
  letter-spacing: 0.8px !important;
  text-transform: uppercase !important;
  padding: 1rem !important;
  border: none !important;
  background: rgba(255, 255, 255, 0.035) !important;
  color: var(--text-muted) !important;
}
.table-glass td {
  padding: 1rem !important;
  border: none !important;
  background: rgba(255, 255, 255, 0.01) !important;
  transition: all 0.25s ease !important;
  color: var(--text-muted) !important;
}
.table-glass tbody tr td:first-child {
  border-top-left-radius: 12px !important;
  border-bottom-left-radius: 12px !important;
}
.table-glass tbody tr td:last-child {
  border-top-right-radius: 12px !important;
  border-bottom-right-radius: 12px !important;
}
.table-glass tbody tr:hover td {
  background: rgba(243, 156, 18, 0.04) !important;
  color: #fff !important;
}
.table-glass tbody tr {
  transition: all 0.25s ease !important;
}
.table-glass tbody tr:hover {
  transform: translateY(-1px);
  box-shadow: 0 4px 15px rgba(243, 156, 18, 0.05) !important;
}

/* Custom badges */
.badge-pill-count {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.72rem;
  background: rgba(243, 156, 18, 0.1);
  color: var(--primary-accent);
  border: 1px solid rgba(243, 156, 18, 0.18);
  padding: 2px 8px;
  border-radius: 20px;
}

/* Active WS Pill glow */
.ws-glow {
  animation: wsGlowPulse 2s infinite alternate;
}
@keyframes wsGlowPulse {
  0% { filter: drop-shadow(0 0 2px rgba(16, 185, 129, 0.4)); opacity: 0.8; }
  100% { filter: drop-shadow(0 0 8px rgba(16, 185, 129, 0.95)); opacity: 1; }
}

/* Responsive adjustments */
@media (max-width: 1040px) {
  #sidebar {
    transform: translateX(-110%);
    top: 0 !important;
    bottom: 0 !important;
    left: 0 !important;
    height: 100vh !important;
    border-radius: 0 !important;
  }
  #sidebar.show {
    transform: translateX(0);
  }
  #main-wrapper {
    margin-left: 0 !important;
    padding: 1rem !important;
  }
  #top-navbar {
    top: 0 !important;
    border-radius: 16px !important;
  }
}

/* ============================================================
   LIGHT THEME — COMPREHENSIVE CONTRAST FIXES
   Ensures all text is readable on white/light backgrounds
   ============================================================ */
body.light-theme {
  color: #1a1a1a !important;
}

body.light-theme body,
body.light-theme #main-content,
body.light-theme .content-panel {
  color: #1a1a1a;
}

/* Card & glass panel text */
body.light-theme .glass-card,
body.light-theme .metric-card,
body.light-theme .stat-card {
  color: #1a1a1a !important;
  background: rgba(255,255,255,0.85) !important;
  border-color: rgba(192,121,26,0.30) !important;
}

body.light-theme .login-card {
  background: rgba(255, 255, 255, 0.9) !important;
  border-color: #c0791a !important;
  box-shadow: 0 10px 40px rgba(192, 121, 26, 0.15) !important;
}

body.light-theme .v-input {
  background: rgba(255, 255, 255, 0.95) !important;
  color: #0f172a !important;
  border-color: rgba(192, 121, 26, 0.4) !important;
}

body.light-theme .v-input::placeholder {
  color: #64748b !important;
}

body.light-theme .eye-icon {
  color: #475569 !important;
}
body.light-theme .login-subtext {
  color: #1e293b !important;
  font-weight: 700 !important;
}

body.light-theme .health-card {
  background: rgba(255, 255, 255, 0.8) !important;
  border-color: rgba(192, 121, 26, 0.25) !important;
}

body.light-theme .health-online {
  color: #059669 !important; /* Darker green */
  border-color: rgba(5, 150, 105, 0.3) !important;
}

body.light-theme .health-offline {
  color: #dc2626 !important; /* Darker red */
  border-color: rgba(220, 38, 38, 0.3) !important;
}

body.light-theme .health-ping-badge {
  color: currentColor !important;
  opacity: 0.8 !important;
  background: rgba(0, 0, 0, 0.05) !important;
}

body.light-theme .sheets-analytics-container {
  background: rgba(0, 0, 0, 0.05) !important;
}

body.light-theme .sim-terminal-container {
  background: #1e293b !important;
}

body.light-theme .sim-terminal-container .text-secondary {
  color: #94a3b8 !important;
}

body.light-theme .sim-terminal-input {
  background: #0f172a !important;
  color: #f8fafc !important;
  border-color: #334155 !important;
}

body.light-theme .sim-terminal-input::placeholder {
  color: #64748b !important;
}

body.light-theme .topology-map-wrapper {
  background: rgba(0, 0, 0, 0.05) !important;
}

body.light-theme .topology-map-wrapper .text-white {
  color: #0f172a !important;
}

body.light-theme .topo-node-sub {
  color: #475569 !important;
}

body.light-theme .badge.bg-secondary {
  background: #64748b !important;
  color: #ffffff !important;
}

body.light-theme .sheets-analytics-title {
  color: #0f172a !important;
}

/* Headings & titles */
body.light-theme h1,
body.light-theme h2,
body.light-theme h3,
body.light-theme h4,
body.light-theme h5,
body.light-theme h6,
body.light-theme .section-title,
body.light-theme .card-title,
body.light-theme .panel-title {
  color: #111111 !important;
}

/* Muted text — must be dark enough on white */
body.light-theme .text-muted,
body.light-theme .text-secondary,
body.light-theme small,
body.light-theme .small {
  color: #555555 !important;
}

/* Metric values in analytics/dashboard */
body.light-theme .metric-value,
body.light-theme .stat-value,
body.light-theme .kpi-value {
  color: #111111 !important;
}

body.light-theme .metric-label,
body.light-theme .stat-label,
body.light-theme .kpi-label {
  color: #444444 !important;
}

/* Primary accent text on light bg */
body.light-theme .text-accent,
body.light-theme .text-primary-accent {
  color: #c0791a !important;
}

/* Tables — must have visible borders and readable text */
body.light-theme .table-glass,
body.light-theme .table-glass table {
  --bs-table-bg: transparent !important;
  --bs-table-accent-bg: transparent !important;
  --bs-table-striped-bg: transparent !important;
  --bs-table-active-bg: transparent !important;
  --bs-table-hover-bg: transparent !important;
  background-color: transparent !important;
  color: #1a1a1a !important;
}
body.light-theme .table-glass th {
  color: #111111 !important;
  background: rgba(192,121,26,0.10) !important;
  border-bottom: 2px solid rgba(192,121,26,0.30) !important;
}
body.light-theme .table-glass td {
  color: #222222 !important;
  border-bottom: 1px solid rgba(0,0,0,0.08) !important;
}
body.light-theme .table-glass tr:hover td {
  background: rgba(192,121,26,0.05) !important;
}

/* Sidebar on light mode */
body.light-theme #sidebar {
  background: rgba(255,255,255,0.94) !important;
  border-right: 1px solid rgba(0,0,0,0.10) !important;
}
body.light-theme .sidebar-link {
  color: #333333 !important;
}
body.light-theme .sidebar-link:hover,
body.light-theme .sidebar-link.active {
  color: #c0791a !important;
  background: rgba(192,121,26,0.10) !important;
}
body.light-theme .sidebar-section-label {
  color: #777777 !important;
}

/* Navbar */
body.light-theme #top-navbar {
  background: rgba(255,255,255,0.92) !important;
  color: #111111 !important;
}
body.light-theme .nav-stat-chip {
  color: #222222 !important;
  background: rgba(0,0,0,0.05) !important;
}
body.light-theme .nav-stat-chip i {
  color: #007bff !important;
}
body.light-theme .nav-search-bar {
  background: rgba(0,0,0,0.05) !important;
  border-color: rgba(0,0,0,0.1) !important;
}
body.light-theme .nav-search-bar .search-input {
  color: #222 !important;
}
body.light-theme .nav-search-bar .search-icon {
  color: #555 !important;
}
body.light-theme .nav-search-bar .search-input::placeholder {
  color: #888 !important;
}
body.light-theme .nav-icon-btn {
  color: #444 !important;
  background: rgba(0,0,0,0.05) !important;
}
body.light-theme .nav-icon-btn:hover {
  background: rgba(0,0,0,0.1) !important;
}
body.light-theme .mini-bar {
  background: #aaa !important;
}
body.light-theme .mini-bar.active {
  background: #007bff !important;
  box-shadow: none !important;
}

/* Status badges / pills */
body.light-theme .status-pill,
body.light-theme .badge {
  color: #111111 !important;
}
body.light-theme .badge.bg-success  { background: #27ae60 !important; color: #fff !important; }
body.light-theme .badge.bg-danger   { background: #c0392b !important; color: #fff !important; }
body.light-theme .badge.bg-warning  { background: #e67e22 !important; color: #fff !important; }
body.light-theme .badge.bg-info     { background: #2980b9 !important; color: #fff !important; }

/* Chart legend and axis labels */
body.light-theme .chartjs-legend,
body.light-theme canvas + div {
  color: #222222 !important;
}

/* Form inputs on light theme */
body.light-theme .glass-input,
body.light-theme .glass-select,
body.light-theme .form-control,
body.light-theme .form-select {
  background: rgba(255,255,255,0.80) !important;
  color: #111111 !important;
  border-color: rgba(192,121,26,0.40) !important;
}
body.light-theme .glass-input::placeholder { color: #888888 !important; }
body.light-theme .input-group-text,
body.light-theme .glass-input-addon {
  background: rgba(192,121,26,0.08) !important;
  color: #444444 !important;
  border-color: rgba(192,121,26,0.30) !important;
}

/* Info rows (label/value pairs) */
body.light-theme .info-row .info-label { color: #555555 !important; }
body.light-theme .info-row .info-value { color: #111111 !important; }

/* Topology Map Light Theme */
body.light-theme .topo-node-name {
  color: #333 !important;
}
body.light-theme .topo-metrics {
  background: rgba(0, 0, 0, 0.05) !important;
  border-color: rgba(0, 0, 0, 0.1) !important;
  color: #444 !important;
}
body.light-theme .cloud-label {
  color: #333 !important;
}
body.light-theme .topo-title {
  color: #111 !important;
}

body.light-theme .topo-link {
  background: rgba(0, 0, 0, 0.08) !important;
}

body.light-theme .topo-link.status-good { background: rgba(0, 200, 83, 0.6) !important; }
body.light-theme .topo-link.status-good .flow-particles { background: #009624 !important; box-shadow: none !important; filter: none !important; }

body.light-theme .topo-link.status-weak { background: rgba(243, 156, 18, 0.6) !important; }
body.light-theme .topo-link.status-weak .flow-particles { background: #d35400 !important; box-shadow: none !important; filter: none !important; }

body.light-theme .topo-link.status-bad { background: rgba(213, 0, 0, 0.6) !important; }

/* Modal on light theme */
body.light-theme .glass-modal,
body.light-theme .modal-content {
  background: rgba(255,255,255,0.96) !important;
  color: #1a1a1a !important;
}
body.light-theme .modal-header,
body.light-theme .modal-footer {
  border-color: rgba(192,121,26,0.20) !important;
}
body.light-theme .modal-title,
body.light-theme .modal-body p,
body.light-theme .modal-body label {
  color: #111111 !important;
}

/* Toast notifications on light */
body.light-theme .toast { background: rgba(255,255,255,0.96) !important; color: #111111 !important; }

/* ============================================================
   DTMF DIALER KEY PRESSED ANIMATION
   ============================================================ */
.keypad-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 14px;
  max-width: 280px;
  margin: 15px auto;
}

.dialer-key {
  background: rgba(255, 255, 255, 0.03);
  border: 1px solid rgba(255, 255, 255, 0.08);
  border-radius: 50%;
  width: 62px;
  height: 62px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  color: var(--text-light, #f8fafc);
  font-size: 1.4rem;
  font-weight: 700;
  line-height: 1.1;
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
  cursor: pointer;
  transition: transform 0.12s ease, background 0.12s ease, box-shadow 0.12s ease;
  user-select: none;
}

.dialer-key small {
  font-size: 0.58rem;
  color: var(--text-muted, #8899b5);
  font-weight: 400;
  text-transform: uppercase;
  margin-top: -2px;
  letter-spacing: 0.05em;
}

body.light-theme .dialer-key {
  background: rgba(0, 0, 0, 0.03) !important;
  border-color: rgba(0, 0, 0, 0.1) !important;
  color: #1e293b !important;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.05) !important;
}

body.light-theme .dialer-key small {
  color: #64748b !important;
}

.dialer-key:hover {
  transform: scale(1.08);
  background: rgba(243,156,18,0.18) !important;
  box-shadow: 0 0 12px rgba(243,156,18,0.40);
}

.dialer-key:active,
.dialer-key-pressed {
  transform: scale(0.92) !important;
  background: rgba(243,156,18,0.40) !important;
  box-shadow: 0 0 18px rgba(243,156,18,0.70) !important;
  color: #fff !important;
  transition: transform 0.06s ease, background 0.06s ease !important;
}

@keyframes dialerKeyFlash {
  0%   { background: rgba(243,156,18,0.60); transform: scale(0.90); }
  50%  { background: rgba(243,156,18,0.20); transform: scale(1.05); }
  100% { background: transparent;           transform: scale(1.00); }
}
.dialer-key-pressed {
  animation: dialerKeyFlash 0.18s ease forwards;
}

/* ============================================================
   GRADIENT PATTERN FIX — Analytics chart tooltips in dark
   ============================================================ */
.chartjs-tooltip {
  background: rgba(26,16,2,0.92) !important;
  border: 1px solid rgba(243,156,18,0.40) !important;
  color: #f8fafc !important;
  border-radius: 10px;
  padding: 8px 14px;
}
body.light-theme .chartjs-tooltip {
  background: rgba(255,255,255,0.96) !important;
  border-color: rgba(192,121,26,0.40) !important;
  color: #111111 !important;
}

/* ============================================================
   ULTRA-WIDE SCREEN (>=1750px) NAVBAR RESPONSIVENESS
   ============================================================ */
.nav-chip-uw {
  display: none !important;
}
.nav-label-uw {
  display: none !important;
}

@media (min-width: 1750px) {
  .nav-chip-uw {
    display: flex !important;
  }
  .nav-label-uw {
    display: inline !important;
  }
}

/* ============================================================
   INCOMING CALL POPUP
============================================================ */
.ic-popup {
  position: fixed;
  bottom: 1.5rem;
  right: 1.5rem;
  width: 360px;
  z-index: 9999;
  background: rgba(15, 10, 2, 0.92);
  backdrop-filter: blur(24px) saturate(200%);
  -webkit-backdrop-filter: blur(24px) saturate(200%);
  border: 1px solid rgba(243, 156, 18, 0.30);
  border-radius: 20px;
  box-shadow: 0 20px 60px rgba(0,0,0,0.6), 0 0 0 1px rgba(243,156,18,0.08), 0 0 30px rgba(243,156,18,0.06);
  overflow: hidden;
  transition: transform 0.35s cubic-bezier(0.34, 1.56, 0.64, 1), opacity 0.35s ease;
}

.ic-popup-show {
  opacity: 1;
  transform: translateY(0) scale(1);
  animation: ic-slide-in 0.4s cubic-bezier(0.34, 1.56, 0.64, 1) both;
}

.ic-popup-hide {
  opacity: 0;
  transform: translateY(20px) scale(0.95);
}

@keyframes ic-slide-in {
  from { opacity: 0; transform: translateY(60px) scale(0.9); }
  to   { opacity: 1; transform: translateY(0) scale(1); }
}

/* Header row */
.ic-popup-header {
  display: flex;
  align-items: center;
  gap: 0.85rem;
  padding: 1rem 1rem 0.75rem;
  border-bottom: 1px solid rgba(243, 156, 18, 0.12);
}

/* Ringing icon animation */
.ic-ring-anim {
  width: 48px;
  height: 48px;
  border-radius: 50%;
  background: linear-gradient(135deg, #22c55e, #16a34a);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.2rem;
  color: #fff;
  flex-shrink: 0;
  animation: ic-ring 0.6s ease-in-out infinite alternate;
  box-shadow: 0 0 0 0 rgba(34, 197, 94, 0.5);
}

@keyframes ic-ring {
  from { transform: rotate(-15deg); box-shadow: 0 0 0 0 rgba(34, 197, 94, 0.4); }
  to   { transform: rotate(15deg);  box-shadow: 0 0 0 10px rgba(34, 197, 94, 0); }
}

.ic-header-text { flex: 1; }
.ic-title  { font-size: 0.72rem; color: var(--text-muted); text-transform: uppercase; letter-spacing: 1px; }
.ic-phone  { font-size: 1.05rem; font-weight: 700; color: #fff; margin: 0.1rem 0; }

.ic-badge {
  display: inline-block;
  font-size: 0.68rem;
  font-weight: 600;
  padding: 0.15rem 0.55rem;
  border-radius: 30px;
}
.ic-badge-registered {
  background: rgba(34, 197, 94, 0.15);
  color: #4ade80;
  border: 1px solid rgba(34, 197, 94, 0.25);
}
.ic-badge-unknown {
  background: rgba(249, 115, 22, 0.12);
  color: #fb923c;
  border: 1px solid rgba(249, 115, 22, 0.25);
}

.ic-close-btn {
  background: rgba(255,255,255,0.05);
  border: 1px solid rgba(255,255,255,0.08);
  color: var(--text-muted);
  border-radius: 8px;
  width: 30px; height: 30px;
  display: flex; align-items: center; justify-content: center;
  cursor: pointer;
  font-size: 0.85rem;
  flex-shrink: 0;
  transition: all 0.2s;
}
.ic-close-btn:hover { background: rgba(239,68,68,0.15); color: #f87171; border-color: rgba(239,68,68,0.25); }

/* Customer info */
.ic-customer-info {
  padding: 0.75rem 1rem;
  border-bottom: 1px solid rgba(255,255,255,0.05);
}
.ic-info-row {
  display: flex;
  justify-content: space-between;
  font-size: 0.8rem;
  padding: 0.18rem 0;
}
.ic-info-label { color: var(--text-muted); }
.ic-info-val   { color: #fff; font-weight: 500; text-align: right; }

/* Answer / Decline */
.ic-actions {
  display: flex;
  gap: 0.65rem;
  padding: 0.75rem 1rem 0.5rem;
}
.ic-btn {
  flex: 1;
  border: none;
  border-radius: 12px;
  font-size: 0.85rem;
  font-weight: 600;
  padding: 0.6rem;
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex; align-items: center; justify-content: center;
}
.ic-btn-answer  { background: linear-gradient(135deg, #22c55e, #16a34a); color: #fff; }
.ic-btn-answer:hover  { opacity: 0.88; transform: scale(0.97); }
.ic-btn-decline { background: linear-gradient(135deg, #ef4444, #dc2626); color: #fff; }
.ic-btn-decline:hover { opacity: 0.88; transform: scale(0.97); }

/* Extra actions (forward / SMS) */
.ic-extra-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 0.4rem;
  padding: 0.25rem 1rem 0.75rem;
}
.ic-extra-label {
  width: 100%;
  font-size: 0.68rem;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.8px;
  margin-bottom: 0.1rem;
}
.ic-fwd-btn {
  font-size: 0.78rem;
  font-weight: 600;
  padding: 0.4rem 0.75rem;
  border-radius: 10px;
  cursor: pointer;
  border: 1px solid rgba(59, 130, 246, 0.3);
  background: rgba(59, 130, 246, 0.1);
  color: #93c5fd;
  transition: all 0.2s;
}
.ic-fwd-btn:hover { background: rgba(59, 130, 246, 0.22); border-color: rgba(59, 130, 246, 0.5); }
.ic-sms-btn {
  border-color: rgba(168, 85, 247, 0.3) !important;
  background: rgba(168, 85, 247, 0.1) !important;
  color: #c4b5fd !important;
}
.ic-sms-btn:hover { background: rgba(168, 85, 247, 0.22) !important; }

/* Countdown bar */
.ic-timer-bar {
  height: 3px;
  background: rgba(255,255,255,0.06);
}
.ic-timer-fill {
  height: 100%;
  width: 100%;
  background: linear-gradient(90deg, #f59e0b, #ef4444);
  border-radius: 0 0 2px 2px;
}

/* ============================================================
   HELP CENTER MANAGER (inside VARMA Panel)
============================================================ */
.hc-add-form {
  display: grid;
  grid-template-columns: 1fr 1fr auto auto;
  gap: 0.5rem;
  align-items: center;
  margin-bottom: 1.25rem;
  padding: 1rem;
  background: rgba(255,255,255,0.03);
  border-radius: 12px;
  border: 1px solid rgba(255,255,255,0.06);
}

.hc-add-form .hc-check-col {
  display: flex;
  gap: 1rem;
  align-items: center;
}

@media (max-width: 768px) {
  .ic-popup { width: calc(100vw - 2rem); right: 1rem; bottom: 1rem; }
  .hc-add-form { grid-template-columns: 1fr; }
}

/* GSM System Control Widget Panel */
.gsm-control-card {
  padding: 24px;
  background: linear-gradient(135deg, rgba(255, 255, 255, 0.03) 0%, rgba(255, 255, 255, 0.01) 100%);
  border: 1px solid rgba(255, 255, 255, 0.08) !important;
  transition: all 0.3s ease;
  border-radius: 16px;
}
.gsm-control-card:hover {
  border-color: rgba(255, 255, 255, 0.15) !important;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
  transform: translateY(-2px);
}
.large-switch .form-check-input {
  width: 55px;
  height: 28px;
  cursor: pointer;
  background-color: rgba(255, 255, 255, 0.12);
  border-color: rgba(255, 255, 255, 0.2);
}
.large-switch .form-check-input:checked {
  background-color: #00e676;
  border-color: #00e676;
  box-shadow: 0 0 12px rgba(0, 230, 118, 0.4);
}
/* Inactive states */
.gsm-control-card.system-disabled {
  border-color: rgba(255, 23, 68, 0.2) !important;
  background: linear-gradient(135deg, rgba(255, 23, 68, 0.02) 0%, transparent 100%);
}
.gsm-control-card.system-disabled .control-status-text {
  color: #ff1744 !important;
}
.gsm-control-card.system-disabled .large-switch .form-check-input:not(:checked) {
  background-color: rgba(255, 23, 68, 0.15);
  border-color: rgba(255, 23, 68, 0.25);
}

/* ============================================================
   NOTIFICATION DROPDOWN & REAL-TIME LOGS
   ============================================================ */
.dropdown-menu-custom {
  position: absolute;
  top: 100%;
  right: 0;
  width: 300px;
  z-index: 1050;
  margin-top: 10px;
  padding: 12px;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.6);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  transition: opacity 0.2s ease, transform 0.2s ease;
  transform-origin: top right;
}
.dropdown-menu-custom.d-none {
  display: none !important;
}
.notification-item {
  padding: 8px 10px;
  border-radius: 6px;
  margin-bottom: 6px;
  background: rgba(255, 255, 255, 0.02);
  border-left: 3px solid var(--primary-accent);
  transition: background 0.2s;
  text-align: left;
}
.notification-item:hover {
  background: rgba(255, 255, 255, 0.05);
}
.notification-item.type-success { border-left-color: var(--color-success); }
.notification-item.type-danger { border-left-color: var(--color-danger); }
.notification-item.type-warning { border-left-color: var(--color-orange); }
.notification-item.type-primary { border-left-color: var(--primary-accent); }
.notification-item.type-info { border-left-color: var(--color-cyan); }

.notification-item-title {
  font-size: 0.78rem;
  font-weight: 700;
  color: #fff;
  margin-bottom: 2px;
}
.notification-item-desc {
  font-size: 0.72rem;
  color: var(--text-muted);
  line-height: 1.2;
}
.notification-item-time {
  font-size: 0.65rem;
  color: var(--text-muted);
  text-align: right;
  margin-top: 4px;
}

/* ============================================================
   LIGHT THEME OVERRIDES FOR TEST PANEL & SIMULATOR
   ============================================================ */
body.light-theme #test-panel .glass-card {
  background: #ffffff !important;
  border-color: #eef0f7 !important;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.04) !important;
}
body.light-theme #test-panel .text-white,
body.light-theme #test-panel .dialer-key {
  color: #1e1e2d !important;
}
body.light-theme #test-panel input.form-control,
body.light-theme #test-panel textarea.form-control {
  background-color: #ffffff !important;
  color: #1e1e2d !important;
  border-color: #d1d5db !important;
}
body.light-theme #test-panel input.form-control:focus,
body.light-theme #test-panel textarea.form-control:focus {
  background-color: #ffffff !important;
  color: #1e1e2d !important;
  border-color: var(--primary-accent) !important;
  box-shadow: 0 0 0 0.2rem rgba(241, 137, 18, 0.15) !important;
}
body.light-theme #test-panel .dialer-display-wrap,
body.light-theme #test-panel .bg-black {
  background-color: #f8fafc !important;
  border-color: #e2e8f0 !important;
}
body.light-theme #sim-dialer-screen,
body.light-theme #sim-live-dtmf-buffer {
  color: #0f172a !important;
}
body.light-theme #sim-dialer-status,
body.light-theme #sim-live-dtmf-description {
  color: #64748b !important;
}
body.light-theme #sim-console-logs {
  background-color: #f8fafc !important;
  color: #0f172a !important;
  border-color: #e2e8f0 !important;
}
body.light-theme #sim-console-logs .text-muted {
  color: #64748b !important;
}

body.light-theme .text-white:not(.btn):not(.btn *):not(.badge):not(.badge *):not([class*="btn-"]):not([class*="btn-"] *) {
  color: #1e1e2d !important;
}

/* Make tables scrollable vertically when overflowed */
.table-responsive {
  max-height: 400px;
  overflow-y: auto !important;
}


</style>
</head>
<body>
  <script>
    if (localStorage.getItem('theme') === 'light') {
      document.body.classList.add('light-theme');
    }
  </script>

  <!-- Animated Background Particles -->
  <div class="bg-particles" aria-hidden="true">
    <div class="particle"></div>
    <div class="particle"></div>
    <div class="particle"></div>
    <div class="particle"></div>
    <div class="particle"></div>
    <div class="particle"></div>
  </div>

  <!-- ============================================================
       VARMA SECURITY GATEKEEPER LOGIN PORTAL
  ============================================================ -->
  <div id="login-overlay" class="d-none" aria-modal="true" role="dialog">
    <div class="login-card">
      <div class="v-shield-logo-big">
        <svg viewBox="0 0 512 512" xmlns="http://www.w3.org/2000/svg" class="v-shield-svg">
          <path d="M466.5 83.7l-192-80c-11.8-4.9-25.2-4.9-37 0l-192 80C27.3 91.3 16 108.3 16 128c0 198.5 114.5 335.7 221.5 380.3 11.8 4.9 25.1 4.9 36.9 0C381.5 463.7 496 326.5 496 128c0-19.7-11.3-36.7-29.5-44.3z" fill="currentColor"/>
          <path d="M160 160 C180 250 220 380 256 400 C300 350 340 220 360 160" fill="none" stroke="#1a1008" stroke-width="45" stroke-linecap="round" stroke-linejoin="round"/>
        </svg>
      </div>
      <h1 class="fw-bold text-white mb-1" style="letter-spacing: 4px; font-size: 1.8rem;">V-VARMA</h1>
      <div class="login-subtext">IVR ADMIN PANEL STA MODE Login</div>

      <div class="input-group-p" style="margin-bottom: 1rem;">
        <input type="text" class="v-input" id="admin-user" placeholder="Username" autocomplete="username">
      </div>
      <div class="input-group-p">
        <input type="password" class="v-input" id="admin-pass" placeholder="Authorization Key" autocomplete="current-password">
        <span class="eye-icon" id="btn-toggle-login-pass">
          <i class="fa-solid fa-eye" id="login-pass-eye-icon"></i>
        </span>
      </div>
      
      <div class="input-group-p" style="display: flex; align-items: center; gap: 10px; margin-top: -0.5rem; margin-bottom: 1.5rem; text-align: left;">
        <input type="checkbox" id="admin-remember" style="width: 18px; height: 18px; cursor: pointer; accent-color: var(--primary-accent);" checked>
        <label for="admin-remember" style="font-size: 0.82rem; color: var(--text-muted); cursor: pointer; user-select: none;">Stay logged in (365 days)</label>
      </div>
      
      <button class="btn-v btn-v-orange w-100" id="btn-login-submit">Grant Access</button>
    </div>
  </div>

  <!-- Mobile Sidebar Backdrop -->
  <div class="sidebar-backdrop" id="sidebar-backdrop"></div>

  <!-- ============================================================
       FIRST-RUN SETUP WIZARD
       (Shown only when no WiFi credentials are stored in localStorage)
  ============================================================ -->
  <div id="first-run-overlay" class="fro-overlay fro-hidden" aria-modal="true" role="dialog" aria-labelledby="fro-title">
    <div class="fro-bg-orbs" aria-hidden="true">
      <div class="fro-orb orb-1"></div>
      <div class="fro-orb orb-2"></div>
      <div class="fro-orb orb-3"></div>
    </div>

    <!-- Step tracker bar -->
    <div class="fro-step-bar">
      <div class="fro-step-item active" id="fro-bar-1"><span>1</span><small>Welcome</small></div>
      <div class="fro-step-line" id="fro-line-1-2"></div>
      <div class="fro-step-item" id="fro-bar-2"><span>2</span><small>WiFi Setup</small></div>
      <div class="fro-step-line" id="fro-line-2-3"></div>
      <div class="fro-step-item" id="fro-bar-3"><span>3</span><small>Connecting</small></div>
      <div class="fro-step-line" id="fro-line-3-4"></div>
      <div class="fro-step-item" id="fro-bar-4"><span>4</span><small>Done!</small></div>
    </div>

    <!-- ─── STEP 1: Welcome ─── -->
    <div class="fro-card" id="fro-step-1">
      <div class="fro-icon-ring blue">
        <i class="fa-solid fa-microchip"></i>
      </div>
      <h1 class="fro-title" id="fro-title">Welcome to ESP32 IVR</h1>
      <p class="fro-subtitle">First-Time Setup Wizard</p>
      <div class="fro-info-block">
        <div class="fro-info-row">
          <span class="fro-info-icon text-info"><i class="fa-solid fa-tower-broadcast"></i></span>
          <div>
            <strong class="text-white">You're connected via AP Mode</strong>
            <div class="text-muted small mt-1">Your ESP32 is currently acting as a WiFi hotspot (<code class="fro-code">ESP32_IVR_AP</code>). In the next step you'll enter your home/office router credentials so the device can connect to the internet.</div>
          </div>
        </div>
        <div class="fro-info-row">
          <span class="fro-info-icon text-warning"><i class="fa-solid fa-rotate"></i></span>
          <div>
            <strong class="text-white">What happens next?</strong>
            <div class="text-muted small mt-1">After you save, the ESP32 stores your credentials in EEPROM and switches to <strong class="text-success">Station (STA) Mode</strong>. You'll then connect to your router and access the full dashboard.</div>
          </div>
        </div>
      </div>
      <div class="fro-actions">
        <button class="fro-btn-primary" id="fro-btn-start">
          <i class="fa-solid fa-arrow-right me-2"></i>Begin Setup
        </button>
        <button class="fro-btn-ghost" id="fro-btn-skip">
          Skip — I'll set up later
        </button>
      </div>
    </div>

    <!-- ─── STEP 2: WiFi Credentials ─── -->
    <div class="fro-card d-none" id="fro-step-2">
      <div class="fro-icon-ring green">
        <i class="fa-solid fa-wifi"></i>
      </div>
      <h2 class="fro-title">Enter Your WiFi Details</h2>
      <p class="fro-subtitle">These will be saved to the ESP32's flash memory (EEPROM)</p>

      <!-- SSID picker -->
      <div class="fro-field">
        <label for="fro-ssid" class="fro-label"><i class="fa-solid fa-signal me-1 text-info"></i>WiFi Network (SSID)</label>
        <div class="fro-input-wrap">
          <input type="text" class="fro-input" id="fro-ssid" placeholder="Your home/office router name" autocomplete="off" spellcheck="false">
          <button class="fro-scan-btn" id="fro-btn-scan" title="Scan nearby networks">
            <i class="fa-solid fa-magnifying-glass"></i>
          </button>
        </div>
        <!-- Nearby networks dropdown -->
        <div class="fro-networks-dropdown d-none" id="fro-networks-list"></div>
      </div>

      <div class="fro-field">
        <label for="fro-password" class="fro-label"><i class="fa-solid fa-lock me-1 text-warning"></i>WiFi Password</label>
        <div class="fro-input-wrap">
          <input type="password" class="fro-input" id="fro-password" placeholder="Enter router password" autocomplete="new-password">
          <button class="fro-eye-btn" id="fro-btn-eye" title="Show/hide password" type="button">
            <i class="fa-solid fa-eye" id="fro-eye-icon"></i>
          </button>
        </div>
      </div>

      <div class="fro-strength-bar-wrap" id="fro-strength-wrap">
        <div class="fro-strength-bar"><div class="fro-strength-fill" id="fro-strength-fill"></div></div>
        <span class="fro-strength-label" id="fro-strength-label">Password strength</span>
      </div>

      <div class="fro-security-note">
        <i class="fa-solid fa-shield-halved text-success me-1"></i>
        Credentials are stored encrypted in EEPROM and never transmitted externally.
      </div>

      <div class="fro-actions">
        <button class="fro-btn-ghost" id="fro-btn-back-1">
          <i class="fa-solid fa-arrow-left me-1"></i>Back
        </button>
        <button class="fro-btn-primary" id="fro-btn-save-wifi">
          <i class="fa-solid fa-floppy-disk me-2"></i>Save &amp; Connect
        </button>
      </div>
    </div>

    <!-- ─── STEP 3: Connecting Animation ─── -->
    <div class="fro-card d-none" id="fro-step-3">
      <div class="fro-connecting-anim">
        <div class="fro-pulse-ring"></div>
        <div class="fro-pulse-ring delay-1"></div>
        <div class="fro-pulse-ring delay-2"></div>
        <div class="fro-icon-ring cyan fro-spin-wrap">
          <i class="fa-solid fa-rotate fa-spin"></i>
        </div>
      </div>
      <h2 class="fro-title mt-4">Connecting ESP32…</h2>
      <p class="fro-subtitle" id="fro-connect-status">Saving credentials to EEPROM…</p>

      <div class="fro-connect-log" id="fro-connect-log">
        <!-- Injected dynamically -->
      </div>
    </div>

    <!-- ─── STEP 4: Success ─── -->
    <div class="fro-card d-none" id="fro-step-4">
      <div class="fro-icon-ring green fro-success-pop">
        <i class="fa-solid fa-circle-check"></i>
      </div>
      <h2 class="fro-title text-success">Connected!</h2>
      <p class="fro-subtitle">ESP32 has joined your WiFi network</p>

      <div class="fro-success-grid">
        <div class="fro-success-row">
          <span class="text-muted small">Network</span>
          <span class="text-white digital-font" id="fro-result-ssid">—</span>
        </div>
        <div class="fro-success-row">
          <span class="text-muted small">Assigned IP</span>
          <span class="text-info digital-font" id="fro-result-ip">—</span>
        </div>
        <div class="fro-success-row">
          <span class="text-muted small">Mode</span>
          <span class="text-success fw-bold">Station (STA)</span>
        </div>
        <div class="fro-success-row">
          <span class="text-muted small">EEPROM</span>
          <span class="text-success">Saved ✓</span>
        </div>
      </div>

      <div class="fro-reconnect-notice">
        <i class="fa-solid fa-triangle-exclamation text-warning me-2"></i>
        <div>
          <strong class="text-white small">Reconnect your device</strong>
          <div class="text-muted" style="font-size:0.78rem;">The ESP32 AP is shutting down. Connect your device to your router WiFi (<strong id="fro-result-ssid-2" class="text-info">—</strong>) to continue using the dashboard.</div>
        </div>
      </div>

      <div class="fro-actions">
        <button class="fro-btn-primary" id="fro-btn-go-sta">
          <i class="fa-solid fa-arrow-right me-2"></i>Go to STA Dashboard
        </button>
      </div>
    </div>
  </div>

  <div id="app-container">

    <!-- ============================================================
         LEFT SIDEBAR NAVIGATION
    ============================================================ -->
    <aside id="sidebar">
      <div class="sidebar-header">
        <a href="#" class="sidebar-brand" id="brand-link" data-target="dashboard">
          <div class="brand-icon-wrap" style="color: var(--primary-accent); background: transparent; border: none; box-shadow: none; padding: 2px;">
            <svg viewBox="0 0 512 512" xmlns="http://www.w3.org/2000/svg" class="v-shield-svg">
              <path d="M466.5 83.7l-192-80c-11.8-4.9-25.2-4.9-37 0l-192 80C27.3 91.3 16 108.3 16 128c0 198.5 114.5 335.7 221.5 380.3 11.8 4.9 25.1 4.9 36.9 0C381.5 463.7 496 326.5 496 128c0-19.7-11.3-36.7-29.5-44.3z" fill="currentColor"/>
              <path d="M160 160 C180 250 220 380 256 400 C300 350 340 220 360 160" fill="none" stroke="var(--bg-color)" stroke-width="45" stroke-linecap="round" stroke-linejoin="round"/>
            </svg>
          </div>
          <div class="brand-text">
            <span class="brand-title">V-VARMA IVR</span>
            <span class="brand-sub">v2.3.0</span>
          </div>
        </a>
      </div>

      <div class="sidebar-section-label">MONITORING</div>
      <ul class="sidebar-menu">
        <li class="sidebar-item">
          <a href="#" class="sidebar-link active" data-target="dashboard" id="nav-dashboard">
            <span class="sidebar-link-icon"><i class="fa-solid fa-gauge-high"></i></span>
            <span class="sidebar-link-text">Dashboard</span>
            <span class="sidebar-link-badge pulse-badge" id="nav-badge-dashboard"></span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="analytics" id="nav-analytics">
            <span class="sidebar-link-icon"><i class="fa-solid fa-chart-pie"></i></span>
            <span class="sidebar-link-text">Analytics</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="varma-panel" id="nav-varma-panel">
            <span class="sidebar-link-icon"><i class="fa-solid fa-layer-group text-primary"></i></span>
            <span class="sidebar-link-text">VARMA Panel</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="call-history" id="nav-call-history">
            <span class="sidebar-link-icon"><i class="fa-solid fa-phone-volume"></i></span>
            <span class="sidebar-link-text">Call History</span>
            <span class="sidebar-count-badge d-none" id="nav-call-count">0</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="sd-manager" id="nav-sd-manager">
            <span class="sidebar-link-icon"><i class="fa-solid fa-sd-card"></i></span>
            <span class="sidebar-link-text">SD Card Manager</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="ivr-configurator" id="nav-ivr-configurator">
            <span class="sidebar-link-icon"><i class="fa-solid fa-list-ul"></i></span>
            <span class="sidebar-link-text">IVR Configurator</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="message-menu" id="nav-message-menu">
            <span class="sidebar-link-icon"><i class="fa-solid fa-message"></i></span>
            <span class="sidebar-link-text">Message Menu</span>
          </a>
        </li>
      </ul>

      <div class="sidebar-section-label">NETWORK</div>
      <ul class="sidebar-menu">
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="system-mode" id="nav-system-mode">
            <span class="sidebar-link-icon"><i class="fa-solid fa-network-wired"></i></span>
            <span class="sidebar-link-text">Network Topology</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="connectivity" id="nav-connectivity">
            <span class="sidebar-link-icon"><i class="fa-solid fa-globe"></i></span>
            <span class="sidebar-link-text">Connectivity</span>
          </a>
        </li>
      </ul>

      <div class="sidebar-section-label">SYSTEM</div>
      <ul class="sidebar-menu">
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="eeprom-mgmt" id="nav-eeprom">
            <span class="sidebar-link-icon"><i class="fa-solid fa-server"></i></span>
            <span class="sidebar-link-text">Device Mgmt</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="serial-terminal" id="nav-serial-terminal">
            <span class="sidebar-link-icon"><i class="fa-solid fa-terminal text-info"></i></span>
            <span class="sidebar-link-text">Serial Terminal</span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="test-panel" id="nav-test-panel">
            <span class="sidebar-link-icon"><i class="fa-solid fa-screwdriver-wrench text-warning"></i></span>
            <span class="sidebar-link-text">Hardware Debug</span>
          </a>
        </li>
      </ul>

      <div class="sidebar-section-label">INTEGRATIONS</div>
      <ul class="sidebar-menu">
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="google-sheets" id="nav-google-sheets">
            <span class="sidebar-link-icon"><i class="fa-solid fa-file-excel text-success"></i></span>
            <span class="sidebar-link-text">Google Sheets</span>
            <span class="status-dot-sidebar disconnected" id="nav-sheets-dot"></span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="firebase" id="nav-firebase">
            <span class="sidebar-link-icon"><i class="fa-solid fa-fire text-warning"></i></span>
            <span class="sidebar-link-text">Firebase Connect</span>
            <span class="status-dot-sidebar warning" id="nav-firebase-dot"></span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" data-target="telegram" id="nav-telegram">
            <span class="sidebar-link-icon"><i class="fa-solid fa-paper-plane text-info"></i></span>
            <span class="sidebar-link-text">Telegram Alerts</span>
            <span class="status-dot-sidebar disconnected" id="nav-telegram-dot"></span>
          </a>
        </li>
        <li class="sidebar-item">
          <a href="#" class="sidebar-link" id="nav-logout">
            <span class="sidebar-link-icon"><i class="fa-solid fa-right-from-bracket text-danger"></i></span>
            <span class="sidebar-link-text text-danger">Log Out</span>
          </a>
        </li>
      </ul>

      <!-- System Health Widget -->
      <div class="sidebar-health-card">
        <div class="health-header">
          <span class="health-label">System Health</span>
          <span class="health-status-badge">Excellent</span>
        </div>
        <div class="health-wave-container">
          <div class="health-wave-animate">
            <svg viewBox="0 0 200 30" preserveAspectRatio="none">
              <path d="M0,15 Q25,5 50,15 T100,15 T150,15 T200,15 L200,30 L0,30 Z" fill="rgba(16, 185, 129, 0.05)" stroke="#2ecc71" stroke-width="2" />
            </svg>
          </div>
        </div>
      </div>

      <div class="sidebar-footer">
        <div class="sidebar-footer-device">
          <div class="footer-chip-icon"><i class="fa-solid fa-microchip"></i></div>
          <div class="footer-chip-details">
            <div class="footer-chip-name">V-VARMA IVR</div>
            <div class="footer-chip-mac digital-font" id="sidebar-mac">—</div>
          </div>
          <div class="footer-online-dot" id="sidebar-online-dot"></div>
        </div>
      </div>
    </aside>

    <!-- ============================================================
         MAIN CONTENT WRAPPER
    ============================================================ -->
    <div id="main-wrapper">
      <!-- Top Navigation Bar -->
      <header id="top-navbar">
        <div class="navbar-left">
          <button class="sidebar-toggle" id="sidebar-toggle-btn" aria-label="Toggle Sidebar">
            <i class="fa-solid fa-bars"></i>
          </button>
          <div class="breadcrumb-wrap d-none d-md-inline-block">
            <span class="breadcrumb-prefix text-muted small">V-VARMA IVR /</span>
            <h1 class="navbar-page-title" id="current-page-title">Dashboard</h1>
          </div>
          <!-- Global HUD Search Bar -->
          <div class="nav-search-bar ms-4 d-none d-lg-flex">
            <i class="fa-solid fa-magnifying-glass search-icon" id="nav-search-icon" style="cursor:pointer;"></i>
            <input type="text" placeholder="Search anything..." class="search-input" id="nav-search-input">
          </div>
        </div>

        <div class="navbar-right">
          <!-- Live Uptime Counter -->
          <div class="nav-stat-chip nav-chip-uw">
            <i class="fa-regular fa-clock text-info"></i>
            <span class="text-muted small">Uptime:</span>
            <strong id="nav-uptime" class="digital-font text-white">0d 00h 00m 00s</strong>
          </div>

          <!-- WiFi Signal Indicator -->
          <div class="nav-stat-chip d-none d-xl-flex" id="nav-wifi-indicator" title="WiFi Signal">
            <i class="fa-solid fa-wifi me-1" id="nav-wifi-icon"></i>
            <span class="text-muted small me-1 nav-label-uw">WiFi:</span>
            <div class="mini-signal-bars" id="nav-wifi-bars">
              <span class="mini-bar bar-1"></span>
              <span class="mini-bar bar-2"></span>
              <span class="mini-bar bar-3"></span>
              <span class="mini-bar bar-4"></span>
            </div>
          </div>

          <!-- GSM/Tower Signal Indicator -->
          <div class="nav-stat-chip d-none d-xl-flex" id="nav-tower-indicator" title="GSM Tower Signal">
            <i class="fa-solid fa-tower-cell me-1" id="nav-tower-icon"></i>
            <span class="text-muted small me-1 nav-label-uw">SIM:</span>
            <div class="mini-signal-bars" id="nav-tower-bars">
              <span class="mini-bar bar-1"></span>
              <span class="mini-bar bar-2"></span>
              <span class="mini-bar bar-3"></span>
              <span class="mini-bar bar-4"></span>
            </div>
          </div>

          <!-- WS Status Indicator -->
          <div class="status-pill d-none d-sm-flex" id="ws-pill" title="WebSocket Live Feed">
            <i class="fa-solid fa-circle-nodes text-success ws-glow" id="ws-icon"></i>
            <span id="ws-text" class="small nav-label-uw">WS Active</span>
          </div>

          <!-- Database Status Indicator -->
          <div class="status-pill d-none d-sm-flex" id="db-pill" title="Database Connection">
            <i class="fa-solid fa-database text-danger" id="db-icon"></i>
            <span id="db-text" class="small nav-label-uw">DB Offline</span>
          </div>

          <!-- Theme Toggle -->
          <button class="status-pill border-0" id="theme-toggle-btn" aria-label="Toggle Light/Dark Theme" style="cursor: pointer;">
            <i class="fa-solid fa-moon text-info" id="theme-toggle-icon"></i>
            <span id="theme-toggle-text" class="small nav-label-uw">Dark Mode</span>
          </button>

          <!-- ESP32 Online Indicator -->
          <div class="status-pill" id="esp32-pill">
            <span class="status-dot offline" id="esp32-status-indicator"></span>
            <span id="esp32-status-text" class="small nav-label-uw">ESP32 Online</span>
          </div>

          <!-- Divider -->
          <span class="nav-divider mx-2 d-none d-md-inline"></span>

          <!-- Notification Bell -->
          <div class="nav-icon-btn position-relative d-none d-md-flex" title="Notifications" id="notification-bell-btn" style="cursor: pointer;">
            <i class="fa-solid fa-bell"></i>
            <span class="notification-badge d-none" id="notification-count">0</span>
            
            <!-- Notification Dropdown Menu -->
            <div class="dropdown-menu-custom glass-card d-none" id="notification-dropdown">
              <div class="dropdown-header d-flex justify-content-between align-items-center border-bottom border-secondary pb-2 mb-2">
                <span class="fw-bold text-white small"><i class="fa-solid fa-bell me-1 text-primary"></i> Notifications</span>
                <div class="d-flex gap-2 align-items-center">
                  <span class="text-muted" style="font-size:0.68rem;" id="notif-read-hint"></span>
                  <button class="btn btn-link btn-xs text-danger p-0 text-decoration-none small" id="btn-clear-notifications" style="font-size: 0.72rem;">Clear All</button>
                </div>
              </div>
              <div class="dropdown-body" id="notification-items-container" style="max-height: 250px; overflow-y: auto;">
                <p class="text-muted small text-center py-3 m-0">No new notifications</p>
              </div>
            </div>
          </div>

          <!-- Settings Cog -->
          <div class="nav-icon-btn d-none d-md-flex" title="System Settings" id="nav-settings-btn" style="cursor: pointer;">
            <i class="fa-solid fa-cog"></i>
          </div>
        </div>
      </header>

      <!-- Main Content Container -->
      <main class="content-container" id="main-content">

        <!-- ============================================================
             1. DASHBOARD PAGE
        ============================================================ -->
        <section id="dashboard" class="page-section active">

          <!-- Section Header -->
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">System Dashboard</h2>
              <p class="text-muted small mt-1 mb-0">Real-time overview of your ESP32 IVR system</p>
            </div>
            <div class="d-flex gap-2">
              <button class="btn btn-glass-primary btn-sm" id="btn-test-keypad-modal" data-bs-toggle="modal" data-bs-target="#dialer-test-modal">
                <i class="fa-solid fa-phone me-1"></i> Test Keypad
              </button>
              <button class="btn btn-glass-secondary btn-sm" id="btn-dash-refresh">
                <i class="fa-solid fa-rotate me-1"></i> Refresh
              </button>
            </div>
          </div>

          <!-- Global Dashboard Summary Banner (Shown on select pages) -->
          <div class="summary-banner-row d-none" id="global-summary-banner">
            <!-- Device Status -->
            <div class="summary-banner-card">
              <span class="summary-card-label">Device</span>
              <span class="summary-card-value text-danger" id="sum-device">Offline</span>
            </div>
            <!-- SIM Status -->
            <div class="summary-banner-card">
              <span class="summary-card-label">SIM</span>
              <span class="summary-card-value" id="sum-sim">—</span>
            </div>
            <!-- GSM Signal -->
            <div class="summary-banner-card">
              <span class="summary-card-label">GSM Signal</span>
              <span class="summary-card-value text-primary" id="sum-gsm">—</span>
            </div>
            <!-- WiFi Status -->
            <div class="summary-banner-card">
              <span class="summary-card-label">WiFi Link</span>
              <span class="summary-card-value" id="sum-wifi">—</span>
            </div>
            <!-- Internet Status -->
            <div class="summary-banner-card">
              <span class="summary-card-label">Internet</span>
              <span class="summary-card-value text-danger" id="sum-internet">Offline</span>
            </div>
            <!-- System Uptime -->
            <div class="summary-banner-card">
              <span class="summary-card-label">Uptime</span>
              <span class="summary-card-value digital-font" id="sum-uptime">00:00:00</span>
            </div>
            <!-- Total Calls -->
            <div class="summary-banner-card">
              <span class="summary-card-label">Total Calls</span>
              <span class="summary-card-value digital-font" id="sum-calls">0</span>
            </div>
            <!-- AP Clients -->
            <div class="summary-banner-card">
              <span class="summary-card-label">AP Clients</span>
              <span class="summary-card-value digital-font" id="sum-ap-clients">0</span>
            </div>
            <!-- ESP32 IP -->
            <div class="summary-banner-card">
              <span class="summary-card-label">IP Address</span>
              <span class="summary-card-value digital-font" id="sum-ip">—</span>
            </div>
            <!-- Last Sync -->
            <div class="summary-banner-card">
              <span class="summary-card-label">Last Sync</span>
              <span class="summary-card-value digital-font" id="sum-sync">—</span>
            </div>
          </div>


          <!-- Primary Metric Cards Row 1 -->
          <div class="row g-3 mb-3">
            <!-- Device Status -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="blue">
                <div class="metric-content-left">
                  <span class="metric-label">Device Status</span>
                  <div class="metric-value text-danger" id="dash-status">OFFLINE</div>
                  <div class="metric-desc" id="dash-status-desc">No active connection</div>
                  <div class="metric-trend positive" id="dash-status-trend"><i class="fa-solid fa-circle-check"></i> Healthy</div>
                  <a href="#" class="hud-action-link" data-target="eeprom-mgmt">View Details <i class="fa-solid fa-arrow-right-long small-arrow"></i></a>
                </div>
                <div class="hud-visualizer device-radar">
                  <div class="radar-circle circle-1"></div>
                  <div class="radar-circle circle-2"></div>
                  <div class="radar-circle circle-3"></div>
                  <div class="radar-sweep"></div>
                  <i class="fa-solid fa-microchip hud-icon"></i>
                </div>
              </div>
            </div>

            <!-- Internet Status -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="green">
                <div class="metric-content-left">
                  <span class="metric-label">Internet Status</span>
                  <div class="metric-value text-danger" id="dash-internet">OFFLINE</div>
                  <div class="metric-desc" id="dash-internet-desc">No WAN connectivity</div>
                  <div class="metric-trend neutral" id="dash-internet-trend"><i class="fa-solid fa-cloud-slash"></i> Offline</div>
                  <a href="#" class="hud-action-link" data-target="connectivity">Connection Stable</a>
                </div>
                <div class="hud-visualizer globe-visualizer">
                  <svg viewBox="0 0 100 100" class="globe-svg">
                    <circle cx="50" cy="50" r="40" stroke="var(--primary-accent)" stroke-width="1" fill="none" stroke-dasharray="4 4"/>
                    <ellipse cx="50" cy="50" rx="40" ry="12" stroke="var(--primary-accent)" stroke-width="1" fill="none"/>
                    <ellipse cx="50" cy="50" rx="12" ry="40" stroke="var(--primary-accent)" stroke-width="1" fill="none"/>
                    <line x1="50" y1="10" x2="50" y2="90" stroke="var(--primary-accent)" stroke-width="1"/>
                    <line x1="10" y1="50" x2="90" y2="50" stroke="var(--primary-accent)" stroke-width="1"/>
                  </svg>
                  <i class="fa-solid fa-globe hud-icon"></i>
                </div>
              </div>
            </div>

            <!-- SIM Status -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="orange">
                <div class="metric-content-left">
                  <span class="metric-label">SIM Status</span>
                  <div class="metric-value text-muted" id="dash-sim">NO SIM</div>
                  <div class="metric-desc" id="dash-sim-desc">Not inserted</div>
                  <div class="metric-trend neutral" id="dash-sim-trend"><i class="fa-solid fa-sim-card"></i> Ready</div>
                  <div class="mini-signal-bars mt-2" id="card-tower-bars">
                    <span class="mini-bar bar-1"></span>
                    <span class="mini-bar bar-2"></span>
                    <span class="mini-bar bar-3"></span>
                    <span class="mini-bar bar-4"></span>
                  </div>
                </div>
                <div class="hud-visualizer sim-visualizer">
                  <svg viewBox="0 0 60 80" class="sim-card-svg">
                    <path d="M5,5 L40,5 L55,20 L55,75 L5,75 Z" fill="rgba(243, 156, 18, 0.05)" stroke="var(--color-orange)" stroke-width="2"/>
                    <rect x="15" y="30" width="30" height="25" rx="3" fill="none" stroke="var(--color-orange)" stroke-width="1.5"/>
                    <line x1="30" y1="30" x2="30" y2="55" stroke="var(--color-orange)" stroke-width="1.5"/>
                    <line x1="15" y1="42.5" x2="45" y2="42.5" stroke="var(--color-orange)" stroke-width="1.5"/>
                  </svg>
                </div>
              </div>
            </div>

            <!-- WiFi Status -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="cyan">
                <div class="metric-content-left">
                  <span class="metric-label">WiFi Status</span>
                  <div class="metric-value" id="dash-wifi-ssid" style="font-size:1.1rem; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; max-width: 130px;">—</div>
                  <div class="metric-desc" id="dash-wifi-rssi">RSSI: — dBm</div>
                  <div class="metric-trend positive" id="dash-wifi-trend"><i class="fa-solid fa-wifi"></i> Connected</div>
                  <div class="mini-signal-bars mt-2" id="card-wifi-bars">
                    <span class="mini-bar bar-1"></span>
                    <span class="mini-bar bar-2"></span>
                    <span class="mini-bar bar-3"></span>
                    <span class="mini-bar bar-4"></span>
                  </div>
                </div>
                <div class="hud-visualizer wifi-waves">
                  <div class="wifi-wave wave-1"></div>
                  <div class="wifi-wave wave-2"></div>
                  <div class="wifi-wave wave-3"></div>
                  <i class="fa-solid fa-wifi hud-icon"></i>
                </div>
              </div>
            </div>
          </div>

          <!-- Primary Metric Cards Row 2 -->
          <div class="row g-3 mb-4">
            <!-- Router Gateway IP -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="blue">
                <div class="metric-content-left">
                  <span class="metric-label">Router Gateway IP</span>
                  <div class="metric-value digital-font" id="dash-ip" style="font-size:1rem;">—</div>
                  <div class="metric-desc" id="dash-ip-desc">Default gateway address</div>
                  <div class="metric-trend neutral"><i class="fa-solid fa-globe"></i> IPv4</div>
                </div>
                <div class="hud-visualizer routing-nodes">
                  <svg viewBox="0 0 100 100" class="nodes-svg">
                    <line x1="50" y1="20" x2="25" y2="70" stroke="var(--primary-accent)" stroke-width="1.5"/>
                    <line x1="50" y1="20" x2="75" y2="70" stroke="var(--primary-accent)" stroke-width="1.5"/>
                    <line x1="25" y1="70" x2="75" y2="70" stroke="var(--primary-accent)" stroke-width="1.5"/>
                    <circle cx="50" cy="20" r="8" fill="var(--bg-color)" stroke="var(--primary-accent)" stroke-width="2"/>
                    <circle cx="25" cy="70" r="8" fill="var(--bg-color)" stroke="var(--primary-accent)" stroke-width="2"/>
                    <circle cx="75" cy="70" r="8" fill="var(--bg-color)" stroke="var(--primary-accent)" stroke-width="2"/>
                  </svg>
                </div>
              </div>
            </div>

            <!-- Total Calls (Moved from Row 1) -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="purple">
                <div class="metric-content-left">
                  <span class="metric-label">Total Calls</span>
                  <div class="metric-value digital-font" id="dash-total-calls">0</div>
                  <div class="metric-desc" id="dash-total-calls-desc">Session logs</div>
                  <div class="metric-trend neutral"><i class="fa-solid fa-minus"></i> Active</div>
                </div>
                <div class="hud-visualizer calls-nodes">
                  <svg viewBox="0 0 100 100" class="nodes-svg">
                    <polygon points="50,15 80,75 20,75" fill="rgba(168, 85, 247, 0.05)" stroke="var(--color-purple)" stroke-width="2"/>
                    <line x1="50" y1="15" x2="50" y2="50" stroke="var(--color-purple)" stroke-width="1.5" stroke-dasharray="3 3"/>
                    <circle cx="50" cy="15" r="6" fill="var(--bg-color)" stroke="var(--color-purple)" stroke-width="2"/>
                    <circle cx="80" cy="75" r="6" fill="var(--bg-color)" stroke="var(--color-purple)" stroke-width="2"/>
                    <circle cx="20" cy="75" r="6" fill="var(--bg-color)" stroke="var(--color-purple)" stroke-width="2"/>
                    <circle cx="50" cy="50" r="4" fill="var(--color-purple)"/>
                  </svg>
                </div>
              </div>
            </div>

            <!-- Connected Clients -->
            <div class="col-6 col-sm-6 col-xl-3" id="dash-card-clients-col">
              <div class="glass-card metric-card" data-color="orange">
                <div class="metric-content-left">
                  <span class="metric-label">AP Clients</span>
                  <div class="metric-value digital-font" id="dash-clients">0</div>
                  <div class="metric-desc">Connected to ESP32 AP</div>
                  <div class="metric-trend neutral"><i class="fa-solid fa-circle"></i> Max 4</div>
                </div>
                <div class="hud-visualizer ap-broadcast">
                  <svg viewBox="0 0 100 100" class="broadcast-svg">
                    <line x1="50" y1="80" x2="50" y2="40" stroke="var(--color-orange)" stroke-width="3"/>
                    <polygon points="45,80 55,80 50,40" fill="none" stroke="var(--color-orange)" stroke-width="1.5"/>
                    <path d="M40,30 A15,15 0 0,1 60,30" fill="none" stroke="var(--color-orange)" stroke-width="1.5" class="broadcast-wave wave-1"/>
                    <path d="M30,20 A30,30 0 0,1 70,20" fill="none" stroke="var(--color-orange)" stroke-width="1.5" class="broadcast-wave wave-2"/>
                    <path d="M20,10 A45,45 0 0,1 80,10" fill="none" stroke="var(--color-orange)" stroke-width="1.5" class="broadcast-wave wave-3"/>
                    <circle cx="50" cy="40" r="5" fill="var(--color-orange)"/>
                  </svg>
                </div>
              </div>
            </div>

            <!-- System Uptime -->
            <div class="col-6 col-sm-6 col-xl-3">
              <div class="glass-card metric-card" data-color="cyan">
                <div class="metric-content-left">
                  <span class="metric-label">Uptime</span>
                  <div class="metric-value digital-font" id="dash-uptime" style="font-size:1rem;">00h 00m 00s</div>
                  <div class="metric-desc">Since last power cycle</div>
                  <div class="metric-trend positive"><i class="fa-solid fa-bolt"></i> Running</div>
                </div>
                <div class="hud-visualizer uptime-clock">
                  <svg viewBox="0 0 100 100" class="clock-svg">
                    <circle cx="50" cy="50" r="40" fill="none" stroke="var(--primary-accent)" stroke-width="2"/>
                    <line x1="50" y1="50" x2="50" y2="22" stroke="var(--primary-accent)" stroke-width="2.5" stroke-linecap="round" class="clock-hand hour-hand"/>
                    <line x1="50" y1="50" x2="72" y2="50" stroke="var(--primary-accent)" stroke-width="1.5" stroke-linecap="round" class="clock-hand minute-hand"/>
                    <circle cx="50" cy="50" r="4" fill="var(--primary-accent)"/>
                  </svg>
                </div>
              </div>
            </div>
          </div>

          <!-- GSM Control Panel (Call, SMS, Caller Validation & Custom IVR Control) -->
          <div class="row g-3 mb-4">
            <div class="col-12 col-sm-6 col-lg-3">
              <div class="glass-card gsm-control-card" id="card-call-system">
                <div class="d-flex align-items-center justify-content-between">
                  <div class="control-details">
                    <h5 class="fw-bold text-white mb-2"><i class="fa-solid fa-phone-volume text-success me-2"></i>Call Control System</h5>
                    <p class="text-muted small mb-0">Enable or disable incoming/outgoing cellular calls</p>
                  </div>
                  <div class="form-check form-switch large-switch">
                    <input class="form-check-input" type="checkbox" id="switch-call-system" checked>
                  </div>
                </div>
                <div class="control-status-text text-success mt-2 small fw-bold digital-font"><i class="fa-solid fa-circle-check"></i> SYSTEM ACTIVE</div>
              </div>
            </div>

            <div class="col-12 col-sm-6 col-lg-3">
              <div class="glass-card gsm-control-card" id="card-sms-system">
                <div class="d-flex align-items-center justify-content-between">
                  <div class="control-details">
                    <h5 class="fw-bold text-white mb-2"><i class="fa-solid fa-comment-sms text-info me-2"></i>SMS Alert System</h5>
                    <p class="text-muted small mb-0">Enable or disable auto-responses and alert text messages</p>
                  </div>
                  <div class="form-check form-switch large-switch">
                    <input class="form-check-input" type="checkbox" id="switch-sms-system" checked>
                  </div>
                </div>
                <div class="control-status-text text-success mt-2 small fw-bold digital-font"><i class="fa-solid fa-circle-check"></i> SYSTEM ACTIVE</div>
              </div>
            </div>

            <div class="col-12 col-sm-6 col-lg-3">
              <div class="glass-card gsm-control-card" id="card-validation-system">
                <div class="d-flex align-items-center justify-content-between">
                  <div class="control-details">
                    <h5 class="fw-bold text-white mb-2"><i class="fa-solid fa-user-shield text-warning me-2"></i>Caller Validation</h5>
                    <p class="text-muted small mb-0">Validate caller via Google Sheets (otherwise auto-answers all)</p>
                  </div>
                  <div class="form-check form-switch large-switch">
                    <input class="form-check-input" type="checkbox" id="switch-validation-system">
                  </div>
                </div>
                <div class="control-status-text text-muted mt-2 small fw-bold digital-font"><i class="fa-solid fa-circle-xmark"></i> VALIDATION OFF</div>
              </div>
            </div>

            <div class="col-12 col-sm-6 col-lg-3">
              <div class="glass-card gsm-control-card" id="card-custom-ivr">
                <div class="d-flex align-items-center justify-content-between">
                  <div class="control-details">
                    <h5 class="fw-bold text-white mb-2"><i class="fa-solid fa-list-check text-danger me-2"></i>Custom IVR Flow</h5>
                    <p class="text-muted small mb-0">Enable custom real-time 20-case IVR menus and actions</p>
                  </div>
                  <div class="form-check form-switch large-switch">
                    <input class="form-check-input" type="checkbox" id="switch-custom-ivr">
                  </div>
                </div>
                <div class="control-status-text text-muted mt-2 small fw-bold digital-font"><i class="fa-solid fa-circle-xmark"></i> IVR FLOW OFF</div>
              </div>
            </div>
          </div>

          <!-- Dashboard Lower Widgets -->
          <div class="row g-3">
            <!-- Firebase Sync Status -->
            <div class="col-md-6">
              <div class="glass-card">
                <div class="widget-header mb-3">
                  <h5 class="widget-title"><i class="fa-solid fa-database text-warning me-2"></i>Firebase Sync</h5>
                  <span class="badge bg-success" id="dash-firebase-badge">Live</span>
                </div>
                <div class="d-flex align-items-center justify-content-between">
                  <div class="metric-content-left">
                    <span class="metric-label">Status</span>
                    <div class="metric-value" id="dash-firebase-status" style="font-size:1.1rem;">Connected</div>
                    <div class="metric-desc" id="dash-firebase-desc">Syncing real-time records</div>
                  </div>
                  <div class="hud-visualizer">
                    <i class="fa-solid fa-cloud-arrow-up text-warning fa-2x fa-bounce" style="--fa-animation-duration: 3s;"></i>
                  </div>
                </div>
              </div>
            </div>

            <!-- Telegram Bot Status -->
            <div class="col-md-6">
              <div class="glass-card">
                <div class="widget-header mb-3">
                  <h5 class="widget-title"><i class="fa-brands fa-telegram text-info me-2"></i>Telegram Bot</h5>
                  <span class="badge bg-success" id="dash-telegram-badge">Active</span>
                </div>
                <div class="d-flex align-items-center justify-content-between">
                  <div class="metric-content-left">
                    <span class="metric-label">Status</span>
                    <div class="metric-value" id="dash-telegram-status" style="font-size:1.1rem;">Listening</div>
                    <div class="metric-desc" id="dash-telegram-desc">Processing incoming commands</div>
                  </div>
                  <div class="hud-visualizer">
                    <i class="fa-brands fa-telegram text-info fa-2x fa-fade" style="--fa-animation-duration: 2s;"></i>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             VARMA PANEL PAGE
        ============================================================ -->
        <section id="varma-panel" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">VARMA Panel</h2>
              <p class="text-muted small mt-1 mb-0">Live Cloud Synchronization from Google Sheets</p>
              <span id="vp-last-sync-time" class="text-muted" style="font-size:0.72rem;"></span>
            </div>
            <div class="d-flex gap-2 align-items-center flex-wrap">
              <button id="btn-varma-init" class="btn btn-glass-secondary" title="First-time: Create all required sheets and columns">
                <i class="fa-solid fa-database me-2"></i>Init Sheets
              </button>
              <button id="btn-varma-reload" class="btn btn-glass-primary">
                <i class="fa-solid fa-cloud-arrow-down me-2"></i>Reload from Cloud
              </button>
              <button id="btn-varma-clear" class="btn btn-glass-secondary">
                <i class="fa-solid fa-trash-can me-2"></i>Clear Cache
              </button>
            </div>
          </div>

          <!-- Top Metric Cards (5 items) -->
          <div class="d-flex flex-wrap gap-3 mb-4 justify-content-between">
            <div class="glass-card stat-card flex-grow-1" style="min-width: 200px;">
              <div class="stat-icon text-primary"><i class="fa-solid fa-file-circle-plus"></i></div>
              <div class="stat-label">Complaints Filed</div>
              <h3 class="stat-value" id="vp-complaints-filed">0</h3>
            </div>
            <div class="glass-card stat-card flex-grow-1" style="min-width: 200px;">
              <div class="stat-icon text-success"><i class="fa-solid fa-file-circle-check"></i></div>
              <div class="stat-label">Complaints Solved</div>
              <h3 class="stat-value" id="vp-complaints-solved">0</h3>
            </div>
            <div class="glass-card stat-card flex-grow-1" style="min-width: 200px;">
              <div class="stat-icon text-warning"><i class="fa-solid fa-user-tag"></i></div>
              <div class="stat-label">Tech Messaged</div>
              <h3 class="stat-value" id="vp-tech-messaged">0</h3>
            </div>
            <div class="glass-card stat-card flex-grow-1" style="min-width: 200px;">
              <div class="stat-icon text-info"><i class="fa-solid fa-comments"></i></div>
              <div class="stat-label">HC Messages</div>
              <h3 class="stat-value" id="vp-hc-messages">0</h3>
            </div>
            <div class="glass-card stat-card flex-grow-1" style="min-width: 200px;">
              <div class="stat-icon text-secondary"><i class="fa-solid fa-envelope-open-text"></i></div>
              <div class="stat-label">Customer Received Msg</div>
              <h3 class="stat-value" id="vp-cust-received-messages">0</h3>
            </div>
          </div>

          <h4 class="text-white mb-3 small-title"><i class="fa-solid fa-square-poll-vertical text-primary me-2"></i>Call & Message Metrics</h4>

          <!-- Grid Counters (8 items) -->
          <div class="row g-3 mb-4">
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-primary"><i class="fa-solid fa-phone-volume"></i></div>
                <div class="stat-label">Total Calls</div>
                <h3 class="stat-value" id="vp-total-calls">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-success"><i class="fa-solid fa-phone-slash fa-flip-horizontal"></i></div>
                <div class="stat-label">Attended Calls</div>
                <h3 class="stat-value" id="vp-attended-calls">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-danger"><i class="fa-solid fa-phone-slash"></i></div>
                <div class="stat-label">Missed Calls</div>
                <h3 class="stat-value" id="vp-missed-calls">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-info"><i class="fa-solid fa-phone-flip"></i></div>
                <div class="stat-label">Dialed Calls</div>
                <h3 class="stat-value" id="vp-dialed-calls">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-warning"><i class="fa-solid fa-question"></i></div>
                <div class="stat-label">Unknown Numbers</div>
                <h3 class="stat-value" id="vp-unknown-numbers">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-purple"><i class="fa-solid fa-address-book"></i></div>
                <div class="stat-label">Known Numbers</div>
                <h3 class="stat-value" id="vp-known-numbers">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-success"><i class="fa-solid fa-paper-plane"></i></div>
                <div class="stat-label">Messages Sent</div>
                <h3 class="stat-value" id="vp-messages-sent">0</h3>
              </div>
            </div>
            <div class="col-xl-3 col-md-6 col-6">
              <div class="glass-card stat-card">
                <div class="stat-icon text-secondary"><i class="fa-solid fa-envelope"></i></div>
                <div class="stat-label">Messages Received</div>
                <h3 class="stat-value" id="vp-messages-received">0</h3>
              </div>
            </div>
          </div>

          <!-- Horizontal sub-menu / data tabs -->
          <div class="glass-card">
            <ul class="nav nav-pills mb-3 custom-pills flex-wrap" id="varma-tabs" role="tablist">
              <li class="nav-item" role="presentation">
                <button class="nav-link active" id="tab-call-history-btn" data-bs-toggle="pill" data-bs-target="#pane-call-history" type="button" role="tab"><i class="fa-solid fa-phone me-1"></i>Call History</button>
              </li>
              <li class="nav-item" role="presentation">
                <button class="nav-link" id="tab-messages-btn" data-bs-toggle="pill" data-bs-target="#pane-messages" type="button" role="tab"><i class="fa-solid fa-envelope me-1"></i>Messages</button>
              </li>
              <li class="nav-item" role="presentation">
                <button class="nav-link" id="tab-unknown-numbers-btn" data-bs-toggle="pill" data-bs-target="#pane-unknown-numbers" type="button" role="tab"><i class="fa-solid fa-user-secret me-1"></i>Unknown Numbers</button>
              </li>
              <li class="nav-item" role="presentation">
                <button class="nav-link" id="tab-registered-numbers-btn" data-bs-toggle="pill" data-bs-target="#pane-registered-numbers" type="button" role="tab"><i class="fa-solid fa-address-card me-1"></i>Registered Numbers</button>
              </li>
              <li class="nav-item" role="presentation">
                <button class="nav-link" id="tab-complaints-btn" data-bs-toggle="pill" data-bs-target="#pane-complaints" type="button" role="tab"><i class="fa-solid fa-triangle-exclamation me-1"></i>Complaints</button>
              </li>
              <li class="nav-item" role="presentation">
                <button class="nav-link" id="tab-warranty-details-btn" data-bs-toggle="pill" data-bs-target="#pane-warranty-details" type="button" role="tab"><i class="fa-solid fa-shield-halved me-1"></i>Warranty Details</button>
              </li>
              <li class="nav-item" role="presentation">
                <button class="nav-link" id="tab-hcnum-btn" data-bs-toggle="pill" data-bs-target="#tab-hcnum" type="button" role="tab"><i class="fa-solid fa-phone-volume me-1"></i>HC Numbers</button>
              </li>
            </ul>

            <div class="tab-content" id="varma-tabsContent">
              <!-- Call History Tab -->
              <div class="tab-pane fade show active" id="pane-call-history" role="tabpanel">
                <div class="table-responsive">
                  <table class="table table-glass" id="table-call-history">
                    <thead><tr><th>Date & Time</th><th>Phone Number</th><th>Type</th><th>Status</th></tr></thead>
                    <tbody><tr><td colspan="4" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr></tbody>
                  </table>
                </div>
              </div>

              <!-- Messages Tab -->
              <div class="tab-pane fade" id="pane-messages" role="tabpanel">
                <div class="table-responsive">
                  <table class="table table-glass" id="table-messages">
                    <thead><tr><th>Date & Time</th><th>Phone Number</th><th>Message</th><th>Status</th></tr></thead>
                    <tbody><tr><td colspan="4" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr></tbody>
                  </table>
                </div>
              </div>

              <!-- Unknown Numbers Tab -->
              <div class="tab-pane fade" id="pane-unknown-numbers" role="tabpanel">
                <div class="table-responsive">
                  <table class="table table-glass" id="table-unknown-numbers">
                    <thead><tr><th>Phone Number</th><th>Last Contact Date</th><th>Calls Count</th></tr></thead>
                    <tbody><tr><td colspan="3" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr></tbody>
                  </table>
                </div>
              </div>

              <!-- Registered Numbers Tab -->
              <div class="tab-pane fade" id="pane-registered-numbers" role="tabpanel">
                <div class="table-responsive">
                  <table class="table table-glass" id="table-registered-numbers">
                    <thead><tr><th>Name</th><th>Phone Number</th><th>Product Name</th><th>Serial Number</th><th>Warranty Status</th></tr></thead>
                    <tbody><tr><td colspan="5" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr></tbody>
                  </table>
                </div>
              </div>

              <!-- Complaints Tab -->
              <div class="tab-pane fade" id="pane-complaints" role="tabpanel">
                <div class="table-responsive">
                  <table class="table table-glass" id="table-complaints">
                    <thead><tr><th>Complaint ID</th><th>Date Filed</th><th>Customer Name</th><th>Phone Number</th><th>Description</th><th>Status</th></tr></thead>
                    <tbody><tr><td colspan="6" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr></tbody>
                  </table>
                </div>
              </div>

              <!-- Warranty Details Tab -->
              <div class="tab-pane fade" id="pane-warranty-details" role="tabpanel">
                <div class="table-responsive">
                  <table class="table table-glass" id="table-warranty-details">
                    <thead><tr><th>Date Registered</th><th>Customer Name</th><th>Phone Number</th><th>Product Name</th><th>Serial Number</th><th>Warranty Expiry</th></tr></thead>
                    <tbody><tr><td colspan="6" class="text-center text-muted py-4">Click "Reload from Cloud" to fetch data</td></tr></tbody>
                  </table>
                </div>
              </div>

              <!-- Help Center Numbers Manager Tab -->
              <div class="tab-pane fade" id="tab-hcnum" role="tabpanel">
                <p class="text-muted small mb-3">
                  <i class="fa-solid fa-circle-info me-1 text-info"></i>
                  These numbers appear as <strong>Forward</strong> options in the incoming-call popup.
                  Enable <strong>SMS</strong> to send an instant alert when a customer wants to speak.
                </p>
                <!-- Add form -->
                <div class="hc-add-form">
                  <input type="tel"  id="hc-new-number"  class="form-control glass-input" placeholder="+91 Phone Number">
                  <input type="text" id="hc-new-name"    class="form-control glass-input" placeholder="Name / Department">
                  <input type="text" id="hc-new-role"    class="form-control glass-input" placeholder="Role (e.g. L1)" style="max-width:130px;">
                  <div class="hc-check-col">
                    <div class="form-check form-switch mb-0">
                      <input class="form-check-input" type="checkbox" id="hc-new-forward" checked>
                      <label class="form-check-label small text-muted" for="hc-new-forward">Forward</label>
                    </div>
                    <div class="form-check form-switch mb-0">
                      <input class="form-check-input" type="checkbox" id="hc-new-sms" checked>
                      <label class="form-check-label small text-muted" for="hc-new-sms">SMS</label>
                    </div>
                  </div>
                  <button id="btn-hc-add" class="btn btn-glass-primary">
                    <i class="fa-solid fa-plus me-1"></i>Add Number
                  </button>
                </div>
                <!-- Numbers table -->
                <div class="table-responsive">
                  <table class="table table-glass" id="hc-numbers-table">
                    <thead>
                      <tr>
                        <th>Phone Number</th>
                        <th>Name</th>
                        <th>Role</th>
                        <th>Forward</th>
                        <th>SMS Alert</th>
                        <th style="width:130px;">Actions</th>
                      </tr>
                    </thead>
                    <tbody id="hc-numbers-tbody">
                      <tr><td colspan="6" class="text-center text-muted py-4"><i class="fa-solid fa-spinner fa-spin me-2"></i>Loading…</td></tr>
                    </tbody>
                  </table>
                </div>
              </div>
            </div><!-- /.tab-content -->
          </div><!-- /.glass-card -->
        </section><!-- /#varma-panel -->

        <!-- ============================================================
             1.5 SYSTEM MODE PAGE
        ============================================================ -->
        <section id="system-mode" class="page-section" style="position: relative;">
          <!-- Section Header -->
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Network Topology</h2>
              <p class="text-muted small mt-1 mb-0">Visual representation of wireless data flow for the IVR device</p>
            </div>
          </div>

          <div class="row g-4">
            <!-- Dynamic Topology Panel -->
            <div class="col-lg-12">
              <div class="glass-card h-100 d-flex flex-column">
                <h5 class="card-section-title"><i class="fa-solid fa-network-wired text-info me-2"></i>Network Topology Map</h5>
                <p class="text-muted small mb-4">Visual representation of wireless data flow based on the active mode.</p>

                <!-- Topology Container -->
                <div class="topology-map-wrapper position-relative flex-grow-1 d-flex flex-column justify-content-center align-items-center p-3 py-4 bg-dark rounded border-0">
                  <svg id="topo-svg-overlay" style="position:absolute; top:0; left:0; width:100%; height:100%; pointer-events:none; z-index:0;"></svg>
                  
                  <!-- Diagram Node Layout -->
                  <div class="topology-diagram w-100 d-flex flex-column align-items-center" style="gap: 1.5rem;">
                    
                    <!-- LEVEL 1: Cellular Tower -->
                    <div class="d-flex justify-content-center w-100 px-4">
                      <div class="topo-node text-center active" id="topo-node-tower">
                        <div class="topo-node-icon"><i class="fa-solid fa-tower-cell"></i></div>
                        <div class="topo-node-name text-white mt-1">Cell Tower</div>
                        <div class="topo-node-sub text-muted digital-font" id="topo-tower-network">Searching...</div>
                      </div>
                    </div>

                    <!-- Flow from Tower to GSM -->
                    <div class="d-flex justify-content-center w-100 position-relative topo-link-container">
                      <div class="topo-link" id="old-link-tower-gsm" style="display:none;"></div>
                    </div>

                    <!-- LEVEL 2: Gateways (GSM, Router, AP Clients) -->
                    <div class="d-flex justify-content-between w-100 px-2" style="margin-top: -10px;">
                      <!-- GSM Module -->
                      <div class="topo-node text-center" id="topo-node-gsm">
                        <div class="topo-node-icon position-relative">
                          <i class="fa-solid fa-sim-card"></i>
                          <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                        </div>
                        <div class="topo-node-name text-white mt-1">GSM Modem</div>
                        <div class="topo-metrics mt-1 text-start">
                          <div class="topo-metric"><span class="text-muted">Carrier:</span> <span id="topo-gsm-carrier" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Network:</span> <span id="topo-gsm-network" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Signal:</span> <span id="topo-gsm-signal" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Status:</span> <span id="topo-gsm-reg" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">SIM:</span> <span id="topo-gsm-sim" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">IMEI:</span> <span id="topo-gsm-imei" class="text-white">--</span></div>
                        </div>
                      </div>

                      <!-- External Router (STA source) -->
                      <div class="topo-node text-center" id="topo-node-router">
                        <div class="topo-node-icon position-relative">
                          <i class="fa-solid fa-network-wired"></i>
                          <i class="fa-solid fa-wifi topo-wifi-wave d-none" id="topo-router-wave"></i>
                          <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                        </div>
                        <div class="topo-node-name text-white mt-1">Local Router</div>
                        <div class="topo-metrics mt-1 text-start">
                          <div class="topo-metric"><span class="text-muted">SSID:</span> <span id="topo-sta-ssid" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Local IP:</span> <span id="topo-sta-ip" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Gateway:</span> <span id="topo-sta-gw" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Signal:</span> <span id="topo-sta-rssi" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Quality:</span> <span id="topo-sta-quality" class="text-white">--</span></div>
                        </div>
                      </div>

                      <!-- AP Clients -->
                      <div class="topo-node text-center" id="topo-node-clients">
                        <div class="topo-node-icon position-relative">
                          <i class="fa-solid fa-mobile-screen"></i>
                          <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                        </div>
                        <div class="topo-node-name text-white mt-1">AP Clients</div>
                        <div class="topo-metrics mt-1 text-start">
                          <div class="topo-metric"><span class="text-muted">Connected:</span> <span id="topo-ap-clients" class="text-white">0</span></div>
                          <div class="topo-metric"><span class="text-muted">Client IPs:</span> <span id="topo-ap-ips" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Traffic:</span> <span id="topo-ap-traffic" class="text-white">Idle</span></div>
                        </div>
                      </div>
                    </div>

                    <!-- Flow Arrows to ESP32 -->
                    <div class="d-flex justify-content-center w-100 position-relative topo-link-container">
                      <div class="topo-link" id="old-link-gsm-esp" style="display:none;"></div>
                      <div class="topo-link" id="old-link-sta-esp" style="display:none;"></div>
                      <div class="topo-link" id="old-link-ap-esp" style="display:none;"></div>
                    </div>

                    <!-- LEVEL 3: ESP32 SoC -->
                    <div class="topo-node main-node text-center" id="topo-node-esp32" style="margin-top: -10px;">
                      <div class="topo-node-icon position-relative">
                        <i class="fa-solid fa-microchip text-info"></i>
                        <div class="esp-pulse-ring d-none" id="topo-esp-pulse"></div>
                      </div>
                      <div class="topo-node-name text-white font-bold mt-1" style="font-size: 0.9rem;">ESP32 Core</div>
                      <div class="topo-node-sub text-success digital-font fw-bold mt-1 mb-1" id="topo-node-esp32-mode" style="font-size:0.75rem; letter-spacing: 1px;">STA MODE</div>
                      <div class="topo-metrics mt-1 text-start">
                        <div class="topo-metric"><span class="text-muted">CPU Act:</span> <span id="topo-esp-cpu" class="text-white">--</span></div>
                        <div class="topo-metric"><span class="text-muted">Free Heap:</span> <span id="topo-esp-heap" class="text-white">--</span></div>
                        <div class="topo-metric"><span class="text-muted">Uptime:</span> <span id="topo-esp-uptime" class="text-white">--</span></div>
                      </div>
                    </div>

                    <!-- Flow Arrow Down to Services -->
                    <div class="d-flex justify-content-center w-100 position-relative topo-link-container">
                      <div class="topo-link" id="old-link-ivr-esp" style="display:none;"></div>
                      <div class="topo-link" id="old-link-sd-esp" style="display:none;"></div>
                      <div class="topo-link" id="old-link-cloud-esp" style="display:none;"></div>
                    </div>

                    <!-- LEVEL 4: Local Services/SD/WAN -->
                    <div class="d-flex justify-content-between align-items-start w-100 px-1" style="margin-top: -10px;">
                      <!-- Direct IVR Server -->
                      <div class="topo-node text-center active" id="topo-node-ivr">
                        <div class="topo-node-icon text-success position-relative">
                          <i class="fa-solid fa-phone-volume"></i>
                          <i class="fa-solid fa-waveform audio-wave d-none" id="topo-ivr-wave"></i>
                        </div>
                        <div class="topo-node-name text-white mt-1">IVR System</div>
                        <div class="topo-metrics mt-1 text-start">
                          <div class="topo-metric"><span class="text-muted">Status:</span> <span id="topo-ivr-status" class="text-white">Idle</span></div>
                          <div class="topo-metric"><span class="text-muted">Caller:</span> <span id="topo-ivr-caller" class="text-white">--</span></div>
                          <div class="topo-metric"><span class="text-muted">Calls:</span> <span id="topo-ivr-calls" class="text-white">0</span></div>
                        </div>
                      </div>

                      <!-- SD Card Node -->
                      <div class="topo-node text-center" id="topo-node-sd">
                        <div class="topo-node-icon position-relative">
                          <i class="fa-solid fa-sd-card"></i>
                          <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                        </div>
                        <div class="topo-node-name text-white mt-1">Memory Card</div>
                        <div class="topo-metrics mt-1 text-start" id="topo-sd-metrics">
                           <div class="topo-metric"><span class="text-muted">Status:</span> <span id="topo-sd-status" class="text-white">Mounted</span></div>
                           <div class="topo-metric"><span class="text-muted">Size:</span> <span id="topo-sd-size" class="text-white">16 GB</span></div>
                           <div class="topo-metric"><span class="text-muted">Used:</span> <span id="topo-sd-used" class="text-white">--</span></div>
                           <div class="topo-metric"><span class="text-muted">Free:</span> <span id="topo-sd-free" class="text-white">--</span></div>
                           <div class="topo-metric"><span class="text-muted">Files:</span> <span id="topo-sd-files" class="text-white">--</span></div>
                        </div>
                        <div class="topo-node-sub text-danger digital-font mt-1 fw-bold d-none" id="topo-sd-missing">Not Inserted</div>
                      </div>
                      
                      <!-- Cloud Services Group -->
                      <div class="d-flex flex-column align-items-center" id="topo-cloud-group">
                         <div class="topo-node-name text-white text-center mb-2 border-bottom border-secondary pb-1" style="font-size: 0.75rem; width: 100%;">Cloud Services</div>
                         <div class="d-flex gap-2">
                            <!-- Sheets -->
                            <div class="topo-node topo-node-small text-center" id="topo-node-sheets">
                               <div class="topo-node-icon position-relative">
                                  <i class="fa-solid fa-file-excel"></i>
                                  <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                               </div>
                               <div class="topo-node-name text-white mt-1">Sheets</div>
                            </div>
                            <!-- Firebase -->
                            <div class="topo-node topo-node-small text-center" id="topo-node-firebase">
                               <div class="topo-node-icon position-relative">
                                  <i class="fa-solid fa-database"></i>
                                  <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                               </div>
                               <div class="topo-node-name text-white mt-1">Firebase</div>
                            </div>
                            <!-- Telegram -->
                            <div class="topo-node topo-node-small text-center" id="topo-node-telegram">
                               <div class="topo-node-icon position-relative">
                                  <i class="fa-brands fa-telegram"></i>
                                  <i class="fa-solid fa-triangle-exclamation topo-warn-badge"></i>
                               </div>
                               <div class="topo-node-name text-white mt-1">Telegram</div>
                            </div>
                         </div>
                      </div>
                    </div>

                  </div>

                </div>
              </div>
            </div>
          </div>
        </section>


        <!-- ============================================================
             4. CONNECTIVITY PAGE
        ============================================================ -->
        <section id="connectivity" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Network Connectivity</h2>
            </div>
          </div>

          <!-- Health Status Cards -->
          <div class="row g-3 mb-4">
            <div class="col-sm-6 col-xl-3">
              <div class="health-card health-offline" id="health-wifi">
                <div class="health-icon-ring">
                  <div class="health-status-icon"><i class="fa-solid fa-wifi"></i></div>
                </div>
                <div class="health-label">WiFi Connected</div>
                <div class="health-value" id="health-wifi-desc">Disconnected</div>
                <div class="health-ping-badge">SSID Active</div>
              </div>
            </div>
            <div class="col-sm-6 col-xl-3">
              <div class="health-card health-offline" id="health-internet">
                <div class="health-icon-ring">
                  <div class="health-status-icon"><i class="fa-solid fa-earth-americas"></i></div>
                </div>
                <div class="health-label">Internet Available</div>
                <div class="health-value" id="health-internet-desc">Internet Offline</div>
                <div class="health-ping-badge" id="health-internet-ping">—</div>
              </div>
            </div>
            <div class="col-sm-6 col-xl-3">
              <div class="health-card health-offline" id="health-ap">
                <div class="health-icon-ring">
                  <div class="health-status-icon"><i class="fa-solid fa-tower-broadcast"></i></div>
                </div>
                <div class="health-label">AP Mode Status</div>
                <div class="health-value" id="health-ap-desc">Host AP Offline</div>
                <div class="health-ping-badge">WPA2-PSK</div>
              </div>
            </div>
            <div class="col-sm-6 col-xl-3">
              <div class="health-card health-offline" id="health-esp32">
                <div class="health-icon-ring">
                  <div class="health-status-icon"><i class="fa-solid fa-microchip"></i></div>
                </div>
                <div class="health-label">ESP32 Reachable</div>
                <div class="health-value" id="health-esp32-desc">Unreachable</div>
                <div class="health-ping-badge" id="health-esp32-ping">—</div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             TEST PANEL PAGE
        ============================================================ -->
        <section id="test-panel" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Hardware Debug & Test Panel</h2>
              <p class="text-muted small mt-1 mb-0">Run real call operations, capture DTMF tones, and verify auto-response routing rules</p>
            </div>
          </div>

          <div class="row g-4">
            <!-- Left: Settings & Dialer -->
            <div class="col-xl-6">
              <div class="glass-card mb-4">
                <h5 class="card-section-title"><i class="fa-solid fa-screwdriver-wrench text-warning me-2"></i>Hardware Debug Rules</h5>
                <div class="d-flex flex-column gap-3">
                  <!-- Test Mode Toggle -->
                  <div class="form-check form-switch form-switch-p">
                    <input class="form-check-input" type="checkbox" id="switch-test-mode">
                    <label class="form-check-label text-white" for="switch-test-mode">Enable Hardware Debug Rules</label>
                  </div>
                  <!-- Attend Call Toggle -->
                  <div class="form-check form-switch form-switch-p">
                    <input class="form-check-input" type="checkbox" id="switch-test-attend-all">
                    <label class="form-check-label text-white" for="switch-test-attend-all">Auto-Attend Call (Real-Time)</label>
                  </div>
                  <!-- Auto-Callback Toggle -->
                  <div class="form-check form-switch form-switch-p">
                    <input class="form-check-input" type="checkbox" id="switch-test-callback">
                    <label class="form-check-label text-white" for="switch-test-callback">Auto-Callback on Missed Call</label>
                  </div>
                  <!-- Send Confirmation SMS Toggle -->
                  <div class="form-check form-switch form-switch-p">
                    <input class="form-check-input" type="checkbox" id="switch-test-sms">
                    <label class="form-check-label text-white" for="switch-test-sms">Send Confirmation SMS</label>
                  </div>
                  <!-- Custom simulation number -->
                  <div class="mb-3">
                    <label for="test-sim-number" class="form-label text-muted small">Debug Target Mobile Number</label>
                    <input type="text" class="form-control glass-input" id="test-sim-number" value="" placeholder="e.g. 9092610415">
                  </div>
                  <!-- Auto-Reply Message Config -->
                  <div class="mb-2">
                    <label for="test-auto-reply-msg" class="form-label text-muted small">SMS Auto-Reply Template</label>
                    <textarea class="form-control glass-input" id="test-auto-reply-msg" rows="3">Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in</textarea>
                  </div>
                </div>
              </div>

              <!-- Simulator Keypad (Dialer) -->
              <div class="glass-card">
                <h5 class="card-section-title"><i class="fa-solid fa-mobile-screen text-info me-2"></i>Virtual Mobile Dialer</h5>
                <div class="d-flex flex-column align-items-center">
                  <!-- Dialer Display -->
                  <div class="dialer-display-wrap w-100 mb-4 bg-black p-3 rounded text-end position-relative" style="border: 1px solid rgba(255,255,255,0.1);">
                    <div class="text-muted small position-absolute top-0 start-0 m-2" id="sim-dialer-status">Ready</div>
                    <div class="fs-3 text-success fw-bold digital-font px-2" id="sim-dialer-screen" style="min-height: 2.2rem; overflow-x: auto; white-space: nowrap;"></div>
                  </div>
                  <!-- Keypad Grid -->
                  <div class="keypad-grid mb-4" id="sim-keypad-grid" style="display: grid; grid-template-columns: repeat(3, 1fr); gap: 1rem; max-width: 280px; width: 100%;">
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="1">1</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="2">2</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="3">3</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="4">4</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="5">5</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="6">6</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="7">7</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="8">8</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="9">9</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="*">*</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="0">0</button>
                    <button class="btn btn-glass-secondary py-3 fs-5 dialer-key text-white" data-key="#">#</button>
                  </div>
                  <!-- Actions -->
                  <div class="d-flex gap-2 w-100 justify-content-center" style="max-width: 320px;">
                    <button class="btn btn-success py-3 px-3 rounded-circle flex-grow-1" id="btn-sim-dial" title="Dial"><i class="fa-solid fa-phone fs-5 text-white"></i></button>
                    <button class="btn btn-primary py-3 px-3 rounded-circle flex-grow-1" id="btn-sim-answer" title="Answer Incoming"><i class="fa-solid fa-phone-flip fs-5 text-white"></i></button>
                    <button class="btn btn-danger py-3 px-3 rounded-circle flex-grow-1" id="btn-sim-hangup" title="Hang Up"><i class="fa-solid fa-phone-slash fs-5 text-white"></i></button>
                    <button class="btn btn-secondary py-3 px-3 rounded-circle flex-grow-1" id="btn-sim-clear" title="Clear Display"><i class="fa-solid fa-delete-left fs-5 text-white"></i></button>
                  </div>
                </div>
              </div>
            </div>

            <!-- Right: Real-time logs and DTMF Box -->
            <div class="col-xl-6">
              <!-- DTMF Entry monitor -->
              <div class="glass-card mb-4">
                <h5 class="card-section-title"><i class="fa-solid fa-list-ol text-success me-2"></i>DTMF Tone Stream</h5>
                <p class="text-muted small">Displays DTMF digit selection as they are clicked on the dialer during a call:</p>
                <div class="bg-black p-3 rounded text-center my-3" style="border: 1px solid rgba(255,255,255,0.1);">
                  <span class="fs-2 text-warning fw-bold digital-font" id="sim-live-dtmf-buffer">-</span>
                </div>
                <div class="text-muted small text-center" id="sim-live-dtmf-description">No active call session</div>
              </div>

              <!-- Message simulator logs -->
              <div class="glass-card">
                <h5 class="card-section-title"><i class="fa-solid fa-comments text-info me-2"></i>Call & Messages Console</h5>
                <div class="sim-console bg-black rounded p-3 text-start mb-3" id="sim-console-logs" style="height: 320px; overflow-y: auto; font-family: 'JetBrains Mono', monospace; font-size: 0.82rem; line-height: 1.5; border: 1px solid rgba(255,255,255,0.1);">
                  <div class="text-muted">[00:00:00] Console initialized. Default toggle is OFF.</div>
                </div>
                <button class="btn btn-glass-secondary btn-sm" id="btn-sim-clear-logs">Clear Console Logs</button>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             WEB SERIAL TERMINAL PAGE
        ============================================================ -->
        <section id="serial-terminal" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Web Serial Terminal</h2>
              <p class="text-muted small mt-1 mb-0">Live AT Command interface for debugging GSM module and ESP32 logs</p>
            </div>
          </div>

          <div class="row g-4 mb-4">
            <div class="col-12">
              <div class="glass-card h-100 d-flex flex-column">
                <div class="d-flex justify-content-between align-items-center mb-3">
                  <h5 class="card-section-title mb-0"><i class="fa-solid fa-terminal text-info me-2"></i>Serial Console</h5>
                  <div class="d-flex gap-3 align-items-center">
                    <div class="form-check form-switch">
                      <input class="form-check-input bg-success border-success" type="checkbox" id="term-autoscroll" checked>
                      <label class="form-check-label text-white small" for="term-autoscroll">Auto-Scroll</label>
                    </div>
                    <button class="btn btn-outline-danger btn-sm" id="btn-term-clear">
                      <i class="fa-solid fa-trash-can"></i> Clear
                    </button>
                  </div>
                </div>
                
                <div id="terminal-output" class="flex-grow-1 border border-secondary rounded bg-dark p-3 mb-3 overflow-y-auto" style="min-height: 400px; max-height: 600px; background: rgba(5, 10, 20, 0.9) !important; font-family: monospace; font-size: 0.85rem; line-height: 1.4; color: #00ff00;">
                  <!-- Logs go here -->
                  <div class="text-muted">Serial Terminal Ready. Waiting for data...</div>
                </div>

                <div class="input-group mt-auto">
                  <span class="input-group-text glass-input-addon text-info fw-bold">&gt;</span>
                  <input type="text" class="form-control glass-input font-monospace" id="terminal-input" placeholder="Type AT command (e.g., AT+CSQ) and press Enter...">
                  <button class="btn btn-primary" type="button" id="btn-term-send">Send</button>
                </div>
              </div>
            </div>
          </div>
        </section>
        <!-- ============================================================
             5. DEVICE MANAGEMENT PAGE
        ============================================================ -->
        <section id="eeprom-mgmt" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Device Management</h2>
              <p class="text-muted small mt-1 mb-0">Hardware specs, firmware details, runtime metrics &amp; factory reset</p>
            </div>
          </div>

          <!-- System Info Row -->
          <div class="row g-4 mb-4">
            <!-- Hardware Spec Card -->
            <div class="col-lg-6">
              <div class="glass-card h-100">
                <h5 class="card-section-title"><i class="fa-solid fa-microchip text-info me-2"></i>ESP32 Hardware Specs</h5>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">SoC Model</span>
                  <span class="info-value text-white fw-bold" id="sys-soc-model">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">Core Frequency</span>
                  <span class="info-value text-white digital-font fw-bold" id="sys-core-freq">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label">Flash ROM Size</span>
                  <span class="info-value text-white digital-font" id="sys-flash-size">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label">Chip Revision</span>
                  <span class="info-value text-white digital-font" id="sys-chip-revision">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label">STA MAC Address</span>
                  <span class="info-value text-info digital-font" id="sys-mac-sta">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label">AP MAC Address</span>
                  <span class="info-value text-info digital-font" id="sys-mac-ap">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">PSRAM</span>
                  <span class="info-value text-white fw-bold" id="sys-psram" style="opacity: 0.85;">Not Mounted</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">Bluetooth</span>
                  <span class="info-value text-white fw-bold" id="sys-bluetooth" style="opacity: 0.85;">BT + BLE Disabled</span>
                </div>
              </div>
            </div>

            <!-- Firmware & Runtime Card -->
            <div class="col-lg-6">
              <div class="glass-card h-100">
                <h5 class="card-section-title"><i class="fa-solid fa-terminal text-info me-2"></i>Firmware &amp; Runtime</h5>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">Firmware Version</span>
                  <span class="info-value text-white digital-font fw-bold" id="sys-firmware-version">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">ESP-IDF SDK</span>
                  <span class="info-value text-white fw-bold" id="sys-sdk-version">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label">Free Heap Memory</span>
                  <span class="info-value text-white digital-font" id="sys-free-heap">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">Min Free Heap Ever</span>
                  <span class="info-value text-white digital-font fw-bold" id="sys-min-heap">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">CPU Temperature</span>
                  <span class="info-value text-warning digital-font fw-bold" id="sys-temp">— °C</span>
                </div>
                <div class="info-row">
                  <span class="info-label fw-bold text-white">Active RTOS Tasks</span>
                  <span class="info-value text-white digital-font fw-bold" id="sys-rtos-tasks">—</span>
                </div>
                <div class="info-row">
                  <span class="info-label">System Uptime</span>
                  <span class="info-value text-white digital-font" id="sys-uptime-long">0 days, 0 hours, 0 minutes, 0 seconds</span>
                </div>
                <div class="info-row">
                  <span class="info-label">IVR Build Target</span>
                  <span class="info-value text-success">ESP32 Production</span>
                </div>
              </div>
            </div>

          </div>

          <!-- WiFi Router Settings -->
          <div class="row g-4 mb-4">
            <!-- WiFi Settings -->
            <div class="col-lg-12">
              <div class="glass-card d-flex flex-column">
                <h5 class="card-section-title"><i class="fa-solid fa-wifi text-info me-2"></i>WiFi Router Settings</h5>
                <p class="text-white small" style="opacity: 0.75;">Configure the ESP32 to connect to a local WiFi router in Station (STA) mode. The device will restart after saving to apply connection parameters.</p>
                
                <div class="row g-3">
                  <div class="col-md-4">
                    <div class="mb-2 text-white small">
                      <strong>Current Status:</strong> <span id="wifi-current-status" class="badge bg-secondary">Checking...</span>
                    </div>
                    <div class="mb-2 text-white small">
                      <strong>Connected SSID:</strong> <span id="wifi-current-ssid" class="text-info fw-bold">Not Connected</span>
                    </div>
                  </div>
                  
                  <div class="col-md-4">
                    <label class="form-label text-white small fw-bold">Select WiFi Network</label>
                    <div class="d-flex">
                      <select id="wifi-select-ssid" class="form-select glass-select flex-grow-1">
                        <option value="">-- Click Scan to find networks --</option>
                      </select>
                      <button class="btn btn-glass-secondary ms-2" id="btn-scan-wifi" type="button">
                        <i class="fa-solid fa-sync"></i> Scan
                      </button>
                    </div>
                  </div>
                  
                  <div class="col-md-4">
                    <label class="form-label text-white small fw-bold">WiFi Password</label>
                    <input type="password" id="wifi-input-pass" class="form-control glass-input" placeholder="Enter WiFi Password">
                  </div>
                </div>

                <div class="btn-action-group mt-3">
                  <button class="btn btn-glass-success flex-grow-1" id="btn-save-wifi">
                    <i class="fa-solid fa-save me-2"></i>Save &amp; Reboot
                  </button>
                </div>
              </div>
            </div>
          </div>

          <!-- AP & Admin Settings Split -->
          <div class="row g-4 mb-4">
            <!-- Device Credentials Card -->
            <div class="col-lg-6">
              <div class="glass-card h-100 d-flex flex-column">
                <h5 class="card-section-title"><i class="fa-solid fa-tower-broadcast text-info me-2"></i>Device Credentials (AP Mode)</h5>
                <p class="text-white small" style="opacity: 0.75;">Modify the Access Point broadcast name/SSID and password of the device. Default SSID: <strong>vvarmaivr</strong>, Password: <strong>GSVIVR001</strong>.</p>
                
                <div class="row g-3 mb-4">
                  <div class="col-sm-6">
                    <label class="form-label text-white small fw-bold">Device SSID</label>
                    <input type="text" id="ap-input-ssid" class="form-control glass-input" placeholder="vvarmaivr">
                  </div>
                  <div class="col-sm-6">
                    <label class="form-label text-white small fw-bold">Device Password</label>
                    <input type="password" id="ap-input-pass" class="form-control glass-input" placeholder="Min 8 characters">
                  </div>
                </div>

                <div class="btn-action-group mt-auto d-flex gap-2">
                  <button class="btn btn-glass-success flex-grow-1" id="btn-save-ap-mode">
                    <i class="fa-solid fa-save me-2"></i>Save &amp; Reboot
                  </button>
                  <button class="btn btn-glass-secondary" id="btn-reset-ap-mode" title="Reset to default AP credentials">
                    <i class="fa-solid fa-undo me-2"></i>Reset
                  </button>
                </div>
              </div>
            </div>

            <!-- Device Admin Credentials Card -->
            <div class="col-lg-6">
              <div class="glass-card h-100 d-flex flex-column">
                <h5 class="card-section-title"><i class="fa-solid fa-user-shield text-info me-2"></i>Device Admin Credentials</h5>
                <p class="text-white small" style="opacity: 0.75;">Modify the web portal administrative user login credentials. Default Username: <strong>admin</strong>, Password: <strong>GSVIVR001</strong>.</p>
                
                <div class="row g-3 mb-4">
                  <div class="col-sm-6">
                    <label class="form-label text-white small fw-bold">Admin Username</label>
                    <input type="text" id="admin-input-user" class="form-control glass-input" placeholder="admin">
                  </div>
                  <div class="col-sm-6">
                    <label class="form-label text-white small fw-bold">Admin Password</label>
                    <input type="password" id="admin-input-pass" class="form-control glass-input" placeholder="Enter new password">
                  </div>
                </div>

                <div class="btn-action-group mt-auto d-flex gap-2">
                  <button class="btn btn-glass-success flex-grow-1" id="btn-save-admin-cred">
                    <i class="fa-solid fa-save me-2"></i>Save &amp; Reboot
                  </button>
                  <button class="btn btn-glass-secondary" id="btn-reset-admin-cred" title="Reset to default admin username and password">
                    <i class="fa-solid fa-undo me-2"></i>Reset
                  </button>
                </div>
              </div>
            </div>
          </div>

          <!-- Factory Reset -->
          <div class="row g-4">
            <div class="col-lg-6">
              <div class="glass-card danger-zone-card d-flex flex-column">
                <h5 class="card-section-title text-danger"><i class="fa-solid fa-triangle-exclamation me-2"></i>Factory Reset</h5>
                <p class="text-white small" style="opacity: 0.75;">Permanently erases all stored configuration from the ESP32 flash memory. The device will reboot into AP Mode and require full re-configuration.</p>

                <div class="danger-info-box mb-4">
                  <h6 class="text-white small fw-bold mb-2"><i class="fa-solid fa-exclamation-circle text-danger me-2"></i>What gets erased:</h6>
                  <ul class="text-white small mb-0 ps-3" style="opacity: 0.85;">
                    <li>Stored WiFi SSID &amp; password</li>
                    <li>Custom IVR greeting configurations</li>
                    <li>Phone routing index tables</li>
                    <li>Client session records</li>
                  </ul>
                </div>

                <div class="btn-action-group mt-auto">
                  <button class="btn btn-glass-danger flex-grow-1" id="btn-factory-reset">
                    <i class="fa-solid fa-skull-crossbones me-2"></i>Factory Reset
                  </button>
                </div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             6. CALL HISTORY PAGE
        ============================================================ -->
        <section id="call-history" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Call History Logs</h2>
              <p class="text-muted small mt-1 mb-0">IVR system call records with search, filter, sort &amp; export</p>
            </div>
            <div class="d-flex gap-2 flex-wrap">
              <button class="btn btn-glass-secondary btn-sm" id="btn-refresh-logs">
                <i class="fa-solid fa-sync me-1"></i>Refresh
              </button>
              <button class="btn btn-glass-secondary btn-sm" id="btn-export-pdf">
                <i class="fa-solid fa-file-pdf me-1 text-danger"></i>Export PDF (A4)
              </button>
              <button class="btn btn-glass-danger btn-sm" id="btn-clear-logs">
                <i class="fa-solid fa-trash-can me-1"></i>Clear Logs
              </button>
            </div>
          </div>

          <div class="glass-card">
            <!-- Table Filters -->
            <div class="row g-3 mb-4">
              <div class="col-md-5">
                <div class="input-with-icon">
                  <i class="fa-solid fa-magnifying-glass input-icon"></i>
                  <input type="text" class="form-control glass-input ps-5" id="log-search-input" placeholder="Search caller, Call ID...">
                </div>
              </div>
              <div class="col-md-4">
                <select class="form-select glass-select" id="log-filter-status">
                  <option value="ALL">All Call Statuses</option>
                  <option value="Completed">Completed</option>
                  <option value="Missed">Missed</option>
                  <option value="Busy">Busy</option>
                  <option value="Failed">Failed</option>
                </select>
              </div>
              <div class="col-md-3">
                <select class="form-select glass-select" id="log-sort-by">
                  <option value="id-desc">Newest First</option>
                  <option value="id-asc">Oldest First</option>
                  <option value="duration-desc">Duration (High)</option>
                  <option value="duration-asc">Duration (Low)</option>
                </select>
              </div>
            </div>

            <!-- Data Table -->
            <div class="table-responsive">
              <table class="table table-glass" id="calls-table">
                <thead>
                  <tr>
                    <th><i class="fa-solid fa-hashtag me-1 text-muted"></i>Call ID</th>
                    <th><i class="fa-solid fa-phone me-1 text-muted"></i>Caller Number</th>
                    <th><i class="fa-regular fa-calendar me-1 text-muted"></i>Date</th>
                    <th><i class="fa-regular fa-clock me-1 text-muted"></i>Time</th>
                    <th><i class="fa-solid fa-stopwatch me-1 text-muted"></i>Duration</th>
                    <th><i class="fa-solid fa-tag me-1 text-muted"></i>Status</th>
                  </tr>
                </thead>
                <tbody id="calls-table-body">
                  <!-- Dynamically populated -->
                </tbody>
              </table>
            </div>

            <!-- Pagination -->
            <div class="d-flex align-items-center justify-content-between mt-4 flex-wrap gap-3">
              <div class="text-muted small" id="pagination-info-text">No call entries yet</div>
              <nav aria-label="Call logs navigation">
                <ul class="pagination pagination-glass m-0" id="logs-pagination"></ul>
              </nav>
            </div>
          </div>
        </section>

        <!-- ============================================================
             SD CARD MANAGER PAGE
        ============================================================ -->
        <section id="sd-manager" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title">SD Card Manager</h2>
              <p class="text-muted small m-0">Manage audio files stored on the local memory card.</p>
            </div>
            <div class="d-flex align-items-center gap-2 mt-3 mt-md-0 flex-wrap">
              <button class="btn btn-glass-primary" id="btn-test-sd"><i class="fa-solid fa-vial me-2"></i>Test TF Card</button>
              <button class="btn btn-glass-danger" id="btn-wipe-sd"><i class="fa-solid fa-trash-can me-2"></i>Wipe TF Card</button>
              <button class="btn btn-glass-info" id="btn-refresh-sd"><i class="fa-solid fa-rotate-right me-2"></i>Refresh Files</button>
            </div>
          </div>
          
          <div id="sd-test-alert" class="alert alert-info d-none mt-2 mb-4" role="alert">
            <i class="fa-solid fa-circle-info me-2"></i><span id="sd-test-msg">Running diagnostics...</span>
          </div>
          
          <div class="row g-4 mb-4">
            <div class="col-md-6 col-lg-4">
              <div class="glass-card text-center h-100">
                <div class="mb-2"><i class="fa-solid fa-sd-card text-primary fa-2x"></i></div>
                <h5 class="fw-bold mb-1">SD Status</h5>
                <h3 class="text-white mb-0" id="sd-status-text">Checking...</h3>
              </div>
            </div>
            <div class="col-md-6 col-lg-4">
              <div class="glass-card text-center h-100">
                <div class="mb-2"><i class="fa-solid fa-hard-drive text-success fa-2x"></i></div>
                <h5 class="fw-bold mb-1">Used Space</h5>
                <h3 class="text-white mb-0" id="sd-used-space">0 MB</h3>
              </div>
            </div>
            <div class="col-md-6 col-lg-4">
              <div class="glass-card text-center h-100">
                <div class="mb-2"><i class="fa-solid fa-database text-info fa-2x"></i></div>
                <h5 class="fw-bold mb-1">Total Space</h5>
                <h3 class="text-white mb-0" id="sd-total-space">0 MB</h3>
              </div>
            </div>
          </div>

          <div class="glass-card">
            <h5 class="card-section-title"><i class="fa-solid fa-file-audio text-info me-2"></i>Upload Audio & Files (.amr/.wav/.mp3/.json)</h5>
            <div class="row g-3 align-items-center mb-4">
              <div class="col-md-5">
                <label for="audio-upload-path" class="form-label text-muted fs-7 mb-1">Target Path on Card (e.g. /01/001.mp3 or leave empty for root)</label>
                <input type="text" class="form-control glass-input" id="audio-upload-path" placeholder="e.g. /01/001.mp3 or /ivr_menu.json">
              </div>
              <div class="col-md-7">
                <label for="audio-upload-file" class="form-label text-muted fs-7 mb-1">Select File</label>
                <div class="input-group">
                  <input type="file" class="form-control glass-input" id="audio-upload-file" accept=".amr,.wav,.mp3,.json">
                  <button class="btn btn-glass-primary" type="button" id="btn-upload-audio"><i class="fa-solid fa-cloud-arrow-up me-2"></i>Upload</button>
                </div>
              </div>
            </div>

            <h5 class="card-section-title mt-5"><i class="fa-solid fa-folder-open text-primary me-2"></i>Files on SD Card</h5>
            <div class="table-responsive mt-3">
              <table class="table table-glass align-middle">
                <thead>
                  <tr>
                    <th>Filename</th>
                    <th>Size</th>
                    <th class="text-end">Actions</th>
                  </tr>
                </thead>
                <tbody id="sd-files-table">
                  <tr><td colspan="3" class="text-center text-muted">No files found or SD not ready.</td></tr>
                </tbody>
              </table>
            </div>
          </div>
        </section>

        <!-- ============================================================
             IVR CONFIGURATOR PAGE
        ============================================================ -->
        <section id="ivr-configurator" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title">IVR Menu Configurator</h2>
              <p class="text-muted small m-0">Design local Interactive Voice Response flow rules.</p>
            </div>
            <div class="d-flex align-items-center gap-3 mt-3 mt-md-0">
              <button class="btn btn-glass-primary" id="btn-save-ivr"><i class="fa-solid fa-floppy-disk me-2"></i>Save Configuration</button>
            </div>
          </div>
          
          <div class="glass-card">
            <h5 class="card-section-title"><i class="fa-solid fa-code text-warning me-2"></i>JSON Configuration</h5>
            <p class="text-muted small mb-3">Define the node flow. Use file names stored on the SD Card. This allows offline playback if cloud is unavailable.</p>
            <div class="mb-3">
              <textarea id="ivr-config-json" class="form-control glass-input" rows="15" style="font-family: monospace;" spellcheck="false">{
  "nodes": {
    "start": {
      "audio": "/t-001.mp3",
      "options": {
        "1": "menu_tamil",
        "2": "menu_english",
        "3": "menu_hindi"
      }
    },
    "menu_english": {
      "audio": "/E_1001.mp3",
      "options": {
        "1": "english_sales",
        "2": "english_support"
      }
    },
    "menu_hindi": {
      "audio": "/h_001.mp3",
      "options": {
        "1": "hindi_sales",
        "2": "hindi_support"
      }
    },
    "menu_tamil": {
      "audio": "/t-002.mp3",
      "options": {
        "1": "tamil_sales",
        "2": "tamil_support"
      }
    }
  }
}</textarea>
            </div>
            <button class="btn btn-glass-secondary" id="btn-reload-ivr"><i class="fa-solid fa-rotate-left me-2"></i>Reload from Device</button>
          </div>
        </section>

        <!-- ============================================================
             MESSAGE MENU (KEYWORD & REPLIES)
        ============================================================ -->
        <section id="message-menu" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Message Menu Settings</h2>
              <p class="text-muted small mt-1 mb-0">Configure auto-responses for incoming SMS queries by keyword</p>
            </div>
            <div class="d-flex gap-2">
              <button class="btn btn-glass-secondary btn-sm" id="btn-add-keyword">
                <i class="fa-solid fa-plus me-1 text-info"></i>Add Rule
              </button>
            </div>
          </div>

          <div class="row g-4">
            <!-- Table & Rules Config -->
            <div class="col-lg-8">
              <div class="glass-card h-100">
                <div class="d-flex align-items-center justify-content-between mb-3 flex-wrap gap-2">
                  <h5 class="widget-title mb-0"><i class="fa-solid fa-gears text-info me-2"></i>Auto-Reply Keyword Rules</h5>
                  <div class="form-check form-switch custom-switch">
                    <input class="form-check-input" type="checkbox" role="switch" id="switch-auto-reply-enable" checked>
                    <label class="form-check-label text-muted small ms-1" for="switch-auto-reply-enable">Auto-Reply Enabled</label>
                  </div>
                </div>

                <div class="table-responsive">
                  <table class="table table-glass text-start align-middle" id="rules-table">
                    <thead>
                      <tr>
                        <th><i class="fa-solid fa-key me-1 text-muted"></i>Keyword</th>
                        <th><i class="fa-solid fa-reply me-1 text-muted"></i>Auto-Response Reply Message</th>
                        <th style="width: 100px;"><i class="fa-solid fa-toggle-on me-1 text-muted"></i>Status</th>
                        <th style="width: 120px;" class="text-end"><i class="fa-solid fa-screwdriver-wrench me-1 text-muted"></i>Actions</th>
                      </tr>
                    </thead>
                    <tbody id="rules-table-body">
                      <!-- Dynamically rendered via JS -->
                    </tbody>
                  </table>
                </div>
              </div>
            </div>

            <!-- Auto-Reply Simulator/Console -->
            <div class="col-lg-4">
              <div class="glass-card h-100 d-flex flex-column">
                <h5 class="widget-title mb-3"><i class="fa-solid fa-terminal text-info me-2"></i>SMS Auto-Reply Simulator</h5>
                <p class="text-muted small mb-3">Test how the ESP32 responds to keyword queries from clients. Type a keyword below to simulate.</p>
                
                <div class="flex-grow-1 border-0 rounded bg-dark p-3 mb-3 d-flex flex-column justify-content-between sim-terminal-container" style="min-height: 200px; font-family: monospace;">
                  <div id="sim-console-output" class="text-muted small overflow-y-auto" style="max-height: 180px;">
                    <div class="text-secondary">[SIMULATOR INITIALIZED]</div>
                    <div class="text-secondary">Ready to test auto-reply keywords...</div>
                  </div>
                </div>

                <div class="input-group">
                  <input type="text" class="form-control glass-input sim-terminal-input" id="sim-keyword-input" placeholder="Type keyword (e.g. STATUS)...">
                  <button class="btn btn-primary" type="button" id="btn-sim-test">Send</button>
                </div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             7. ANALYTICS PAGE
        ============================================================ -->
        <section id="analytics" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Analytics &amp; Insights</h2>
              <p class="text-muted small mt-1 mb-0">IVR call traffic trends and performance statistics</p>
            </div>
          </div>

          <!-- Stats Summary Cards -->
          <div class="row g-3 mb-4">
            <div class="col-6 col-xl-3">
              <div class="glass-card analytics-stat-card">
                <div class="analytics-stat-icon" style="background:rgba(243, 156, 18, 0.12);color:#f39c12;">
                  <i class="fa-solid fa-phone-volume"></i>
                </div>
                <div class="analytics-stat-value digital-font" id="stat-total-calls">0</div>
                <div class="analytics-stat-label">Total Calls</div>
                <div class="analytics-stat-sub text-muted"><i class="fa-solid fa-minus me-1"></i>No data yet</div>
              </div>
            </div>
            <div class="col-6 col-xl-3">
              <div class="glass-card analytics-stat-card">
                <div class="analytics-stat-icon" style="background:rgba(0,230,118,0.12);color:#2ecc71;">
                  <i class="fa-solid fa-circle-check"></i>
                </div>
                <div class="analytics-stat-value digital-font text-success" id="stat-completed-calls">0</div>
                <div class="analytics-stat-label">Completed</div>
                <div class="analytics-stat-sub text-muted">Rate: <span class="text-white fw-semibold" id="stat-completed-pct">0.0%</span></div>
              </div>
            </div>
            <div class="col-6 col-xl-3">
              <div class="glass-card analytics-stat-card">
                <div class="analytics-stat-icon" style="background:rgba(255,23,68,0.12);color:#ff1744;">
                  <i class="fa-solid fa-phone-missed"></i>
                </div>
                <div class="analytics-stat-value digital-font text-danger" id="stat-missed-calls">0</div>
                <div class="analytics-stat-label">Missed / Failed</div>
                <div class="analytics-stat-sub text-muted">Rate: <span class="text-white fw-semibold" id="stat-missed-pct">0.0%</span></div>
              </div>
            </div>
            <div class="col-6 col-xl-3">
              <div class="glass-card analytics-stat-card">
                <div class="analytics-stat-icon" style="background:rgba(0,176,255,0.12);color:#e67e22;">
                  <i class="fa-solid fa-stopwatch"></i>
                </div>
                <div class="analytics-stat-value digital-font text-info" id="stat-avg-duration">0s</div>
                <div class="analytics-stat-label">Avg Duration</div>
                <div class="analytics-stat-sub text-muted">Total: <span class="text-white fw-semibold">0m</span></div>
              </div>
            </div>
          </div>

          <!-- Chart Grid -->
          <div class="row g-4">
            <div class="col-lg-6 col-xl-4">
              <div class="glass-card">
                <h6 class="chart-title"><i class="fa-solid fa-chart-bar text-info me-2"></i>Daily Calls (Today)</h6>
                <div class="chart-container">
                  <canvas id="chart-daily"></canvas>
                </div>
              </div>
            </div>
            <div class="col-lg-6 col-xl-4">
              <div class="glass-card">
                <h6 class="chart-title"><i class="fa-solid fa-chart-line text-info me-2"></i>Weekly Traffic</h6>
                <div class="chart-container">
                  <canvas id="chart-weekly"></canvas>
                </div>
              </div>
            </div>
            <div class="col-lg-6 col-xl-4">
              <div class="glass-card">
                <h6 class="chart-title"><i class="fa-solid fa-chart-area text-info me-2"></i>Monthly Trend</h6>
                <div class="chart-container">
                  <canvas id="chart-monthly"></canvas>
                </div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             8. GOOGLE SHEETS INTEGRATION
        ============================================================ -->
        <section id="google-sheets" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Google Sheets Integration</h2>
              <p class="text-muted small mt-1 mb-0">Export system events, calls, and SMS logs directly to Google Sheets</p>
            </div>
          </div>

          <div class="row g-4">
            <!-- Connection Settings -->
            <div class="col-lg-7">
              <div class="glass-card">
                <h5 class="card-section-title"><i class="fa-solid fa-link text-success me-2"></i>Google Sheets Settings</h5>
                <form id="google-sheets-form" onsubmit="return false;">
                  <div class="mb-3">
                    <label for="sheets-id" class="form-label text-muted small">Google Sheet ID</label>
                    <input type="text" class="form-control glass-input" id="sheets-id" placeholder="Enter spreadsheet ID from URL" required>
                  </div>
                  <div class="mb-3 d-none">
                    <label for="sheets-name" class="form-label text-muted small">Sheet Name / Tab Name</label>
                    <input type="text" class="form-control glass-input" id="sheets-name" placeholder="e.g. Call_Logs" value="IVR" required>
                  </div>
                  <div class="mb-4">
                    <label for="sheets-script-url" class="form-label text-muted small">Google Apps Script URL</label>
                    <input type="url" class="form-control glass-input" id="sheets-script-url" placeholder="https://script.google.com/macros/s/.../exec" required>
                  </div>

                  <h6 class="text-white small mb-3">Sync Event Triggers</h6>
                  <div class="row g-2 mb-4">
                    <div class="col-sm-6">
                      <div class="form-check form-switch">
                        <input class="form-check-input" type="checkbox" id="sync-incoming" checked>
                        <label class="form-check-label text-muted small" for="sync-incoming">Incoming Call Logs</label>
                      </div>
                    </div>
                    <div class="col-sm-6">
                      <div class="form-check form-switch">
                        <input class="form-check-input" type="checkbox" id="sync-outgoing" checked>
                        <label class="form-check-label text-muted small" for="sync-outgoing">Outgoing Call Logs</label>
                      </div>
                    </div>
                    <div class="col-sm-6">
                      <div class="form-check form-switch">
                        <input class="form-check-input" type="checkbox" id="sync-sms" checked>
                        <label class="form-check-label text-muted small" for="sync-sms">Auto-Reply SMS Logs</label>
                      </div>
                    </div>
                    <div class="col-sm-6">
                      <div class="form-check form-switch">
                        <input class="form-check-input" type="checkbox" id="sync-system">
                        <label class="form-check-label text-muted small" for="sync-system">System Events</label>
                      </div>
                    </div>
                    <div class="col-sm-6">
                      <div class="form-check form-switch">
                        <input class="form-check-input" type="checkbox" id="sync-network">
                        <label class="form-check-label text-muted small" for="sync-network">Network Events</label>
                      </div>
                    </div>
                  </div>

                  <div class="btn-action-group">
                    <button type="submit" class="btn btn-glass-primary flex-grow-1" id="btn-save-sheets-config">
                      <i class="fa-solid fa-floppy-disk me-2"></i>Save Configuration
                    </button>
                    <button type="button" class="btn btn-glass-secondary flex-grow-1" id="btn-test-sheets-conn">
                      <i class="fa-solid fa-bolt me-2 text-warning"></i>Test Connection
                    </button>
                  </div>
                </form>
              </div>
            </div>

            <!-- Sheets Sync Status -->
            <div class="col-lg-5">
              <div class="glass-card h-100 d-flex flex-column justify-content-between">
                <div>
                  <h5 class="card-section-title"><i class="fa-solid fa-signal text-success me-2"></i>Sync Status Overview</h5>
                  <div class="info-row">
                    <span class="info-label">Sync Connection</span>
                    <span class="info-value text-danger" id="sheets-sync-status">DISCONNECTED</span>
                  </div>
                  <div class="info-row">
                    <span class="info-label">Last Successful Upload</span>
                    <span class="info-value text-white digital-font" id="sheets-last-sync-time">—</span>
                  </div>
                  <div class="info-row">
                    <span class="info-label">Total Records Uploaded</span>
                    <span class="info-value text-white digital-font" id="sheets-records-count">0</span>
                  </div>
                  <div class="info-row">
                    <span class="info-label">Logged Errors / Failures</span>
                    <span class="info-value text-success digital-font" id="sheets-errors-count">0</span>
                  </div>
                </div>

                <div class="p-3 rounded bg-dark border-0 mt-4 sheets-analytics-container">
                  <h6 class="text-white small fw-bold mb-2 sheets-analytics-title"><i class="fa-solid fa-chart-line text-success me-2"></i>Sheets Analytics</h6>
                  <div class="d-flex justify-content-between text-muted small py-1 border-bottom border-secondary-subtle">
                    <span>Logs Today:</span>
                    <strong class="text-white" id="sheets-today-count">0</strong>
                  </div>
                  <div class="d-flex justify-content-between text-muted small py-1 border-bottom border-secondary-subtle">
                    <span>SMS Count:</span>
                    <strong class="text-white" id="sheets-sms-count">0</strong>
                  </div>
                  <div class="d-flex justify-content-between text-muted small py-1">
                    <span>Last Update:</span>
                    <strong class="text-white" id="sheets-last-update-date">—</strong>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             9. FIREBASE CONNECTION PLACEHOLDER
        ============================================================ -->
        <section id="firebase" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Firebase Integration Settings</h2>
              <p class="text-muted small mt-1 mb-0">Synchronize real-time configurations using Firebase Realtime Database</p>
            </div>
          </div>
          <div class="row g-4">
            <div class="col-lg-6">
              <div class="glass-card">
                <h5 class="card-section-title"><i class="fa-solid fa-fire text-warning me-2"></i>Firebase Configuration</h5>
                <div class="mb-3">
                  <label class="form-label text-muted small">Firebase Database URL</label>
                  <input type="text" class="form-control glass-input" value="https://esp32-ivr-system-default-rtdb.firebaseio.com" disabled>
                </div>
                <div class="mb-3">
                  <label class="form-label text-muted small">Web API Key</label>
                  <input type="password" class="form-control glass-input" value="AIzaSyA184f9E8F2_D0WDQ6" disabled>
                </div>
                <div class="mb-4">
                  <label class="form-label text-muted small">Auth Domain</label>
                  <input type="text" class="form-control glass-input" value="esp32-ivr-system.firebaseapp.com" disabled>
                </div>
                <button class="btn btn-glass-secondary" disabled>Save Configuration</button>
              </div>
            </div>
            <div class="col-lg-6">
              <div class="glass-card h-100 d-flex flex-column justify-content-between">
                <div>
                  <h5 class="card-section-title"><i class="fa-solid fa-triangle-exclamation text-warning me-2"></i>Integration Details</h5>
                  <p class="text-muted small">Firebase Realtime Database syncs parameters like WiFi settings, SIM carrier data, and keyword replies in real-time between the ESP32 and this browser dashboard.</p>
                  <p class="text-muted small">Currently active in read-only diagnostic simulation mode.</p>
                </div>
                <div class="p-3 rounded bg-dark border-0" style="background: rgba(255,145,0,0.04) !important; border: 1px solid rgba(255,145,0,0.1) !important;">
                  <span class="text-warning small fw-bold"><i class="fa-solid fa-check-circle me-1"></i> RTDB Connected</span>
                  <p class="text-muted small m-0 mt-1">Real-time sync listeners are monitoring database write operations.</p>
                </div>
              </div>
            </div>
          </div>
        </section>

        <!-- ============================================================
             10. TELEGRAM ALERTS PLACEHOLDER
        ============================================================ -->
        <section id="telegram" class="page-section">
          <div class="section-header mb-4">
            <div>
              <h2 class="section-title mb-0">Telegram Alerts Notification Settings</h2>
              <p class="text-muted small mt-1 mb-0">Send instant text alerts to your Telegram chat channels for system events</p>
            </div>
          </div>
          <div class="row g-4">
            <div class="col-lg-6">
              <div class="glass-card">
                <h5 class="card-section-title"><i class="fa-solid fa-paper-plane text-info me-2"></i>Telegram Bot Config</h5>
                <div class="mb-3">
                  <label class="form-label text-muted small">Telegram Bot API Token</label>
                  <input type="text" id="tg-token" class="form-control glass-input" placeholder="Enter Bot API Token">
                </div>
                <div class="mb-4">
                  <label class="form-label text-muted small">Target Chat ID</label>
                  <input type="text" id="tg-chat-id" class="form-control glass-input" placeholder="Enter Chat ID">
                </div>
                <button class="btn btn-glass-secondary" id="btn-save-telegram">Save Configuration</button>
              </div>
            </div>
            <div class="col-lg-6">
              <div class="glass-card h-100 d-flex flex-column justify-content-between">
                <div>
                  <h5 class="card-section-title"><i class="fa-solid fa-info-circle text-info me-2"></i>Alert Settings</h5>
                  <p class="text-muted small">Incorporate Telegram bots to transmit system health indicators, notification logs, and unauthorized client attempts directly to administrative channels.</p>
                </div>
                <div class="p-3 rounded bg-dark border-0" style="background: rgba(0,176,255,0.04) !important; border: 1px solid rgba(0,176,255,0.1) !important;">
                  <span class="text-info small fw-bold"><i class="fa-solid fa-info-circle me-1"></i> Bot Diagnostic Status</span>
                  <p class="text-muted small m-0 mt-1">Chat bot notification webhook endpoints verified.</p>
                </div>
              </div>
            </div>
          </div>
        </section>

      </main>
    </div>
  </div>

  <!-- ============================================================
       TOAST NOTIFICATION SYSTEM
  ============================================================ -->
  <div class="toast-container position-fixed bottom-0 end-0 p-3" style="z-index:1200;">
    <div id="action-toast" class="toast glass-toast" role="alert" aria-live="assertive" aria-atomic="true" data-bs-delay="4500">
      <div class="toast-header">
        <i class="fa-solid fa-bell text-info me-2" id="toast-icon"></i>
        <strong class="me-auto" id="toast-title">System Alert</strong>
        <small class="text-muted" id="toast-time">Just now</small>
        <button type="button" class="btn-close btn-close-white ms-2" data-bs-dismiss="toast" aria-label="Close"></button>
      </div>
      <div class="toast-body" id="toast-body-text">Operation completed successfully.</div>
    </div>
  </div>

  <!-- ============================================================
       LIVE CALL POPUP MONITOR
  ============================================================ -->
  <div id="live-call-popup" class="position-fixed top-0 start-50 translate-middle-x mt-3 d-none" style="z-index: 1500; min-width: 320px;">
    <div class="card glass-card border-warning shadow-lg">
      <div class="card-header bg-warning text-dark fw-bold d-flex align-items-center">
        <i class="fa-solid fa-phone-volume fa-shake me-2" id="live-call-icon"></i>
        <span id="live-call-title">Incoming Call...</span>
      </div>
      <div class="card-body bg-dark text-white text-center">
        <h3 id="live-call-number" class="text-info mb-3">Fetching...</h3>
        <div class="d-flex justify-content-between text-muted small px-2">
          <span>Status:</span>
          <span id="live-call-status" class="text-warning fw-bold">RINGING</span>
        </div>
        <hr class="border-secondary my-2">
        <div class="mt-2 text-start px-2">
          <span class="text-muted small">Live Keypad Entry:</span>
          <div id="live-call-dtmf" class="fs-4 text-success fw-bold font-monospace mt-1" style="min-height: 35px; letter-spacing: 4px;">-</div>
        </div>
      </div>
    </div>
  </div>

  <!-- ============================================================
       EEPROM CLEAR CONFIRMATION MODAL
  ============================================================ -->
  <div class="modal fade" id="eeprom-confirm-modal" tabindex="-1" aria-labelledby="eepromModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered">
      <div class="modal-content glass-modal">
        <div class="modal-header border-0 pb-0">
          <div class="modal-danger-icon">
            <i class="fa-solid fa-triangle-exclamation"></i>
          </div>
        </div>
        <div class="modal-body text-center">
          <h5 class="modal-title text-white mb-2" id="eepromModalLabel">Erase System Memory?</h5>
          <p class="text-muted small m-0">Are you sure you want to erase all stored credentials?</p>
          <p class="text-muted small mt-2 mb-0">This clears saved WiFi SSIDs, passwords, custom call settings, and config cycles from the ESP32 EEPROM partition. The device will reboot in AP Mode.</p>
        </div>
        <div class="modal-footer border-0 justify-content-center gap-3">
          <button type="button" class="btn btn-glass-secondary" data-bs-dismiss="modal">Cancel</button>
          <button type="button" class="btn btn-glass-danger" id="btn-modal-confirm-clear">
            <i class="fa-solid fa-eraser me-2"></i>Confirm Erase
          </button>
        </div>
      </div>
    </div>
  </div>

  <!-- ============================================================
       LOGOUT CONFIRMATION MODAL
  ============================================================ -->
  <div class="modal fade" id="logout-confirm-modal" tabindex="-1" aria-labelledby="logoutModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered">
      <div class="modal-content glass-modal">
        <div class="modal-header border-0 pb-0">
          <div class="modal-danger-icon">
            <i class="fa-solid fa-right-from-bracket"></i>
          </div>
        </div>
        <div class="modal-body text-center">
          <h5 class="modal-title text-white mb-2" id="logoutModalLabel">Confirm Log Out</h5>
          <p class="text-muted small m-0">Are you sure you want to log out of the admin panel?</p>
        </div>
        <div class="modal-footer border-0 justify-content-center gap-3 pb-4">
          <button type="button" class="btn btn-glass-secondary" data-bs-dismiss="modal">Cancel</button>
          <button type="button" class="btn btn-glass-danger" id="btn-modal-confirm-logout">
            <i class="fa-solid fa-right-from-bracket me-2"></i>Log Out
          </button>
        </div>
      </div>
    </div>
  </div>

  <!-- ============================================================
       KEYWORD RULE EDIT MODAL
  ============================================================ -->
  <div class="modal fade" id="rule-modal" tabindex="-1" aria-labelledby="ruleModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered">
      <div class="modal-content glass-modal">
        <div class="modal-header border-0 pb-0">
          <h5 class="modal-title text-white" id="ruleModalLabel">Add Keyword Rule</h5>
          <button type="button" class="btn-close btn-close-white" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body">
          <form id="rule-form">
            <input type="hidden" id="rule-id-hidden">
            <div class="mb-3">
              <label for="rule-keyword" class="form-label text-muted small">Keyword (Case-Insensitive)</label>
              <input type="text" class="form-control bg-dark border-secondary text-white" id="rule-keyword" required placeholder="e.g. REBOOT">
            </div>
            <div class="mb-3">
              <label for="rule-reply" class="form-label text-muted small">Response Reply Message</label>
              <textarea class="form-control bg-dark border-secondary text-white" id="rule-reply" rows="3" required placeholder="Type the automated response message here..."></textarea>
            </div>
            <div class="mb-3 form-check form-switch">
              <input class="form-check-input" type="checkbox" id="rule-active" checked>
              <label class="form-check-label text-muted small ms-1" for="rule-active">Active</label>
            </div>
          </form>
        </div>
        <div class="modal-footer border-0">
          <button type="button" class="btn btn-glass-secondary" data-bs-dismiss="modal">Cancel</button>
          <button type="button" class="btn btn-primary" id="btn-save-rule">Save Rule</button>
        </div>
      </div>
    </div>
  </div>

  <!-- ============================================================
       DIALER TEST KEYPAD MODAL
  ============================================================ -->
  <div class="modal fade" id="dialer-test-modal" tabindex="-1" aria-labelledby="dialerModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered">
      <div class="modal-content glass-modal" style="max-width: 360px; margin: 0 auto;">
        <div class="modal-header border-0 pb-0">
          <h5 class="modal-title text-white" id="dialerModalLabel">
            <i class="fa-solid fa-phone-volume text-warning me-2"></i>DTMF Dialer Test
          </h5>
          <button type="button" class="btn-close btn-close-white" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body text-center">
          <p class="text-muted small">Input a phone number and test DTMF dialer operations. Click keys below to transmit tones.</p>
          
          <!-- Dialer Screen/Display -->
          <div class="p-3 bg-dark rounded mb-3 border border-secondary-subtle" style="background: rgba(0,0,0,0.5) !important;">
            <div id="dialer-screen" class="digital-font text-warning fs-3 text-end overflow-hidden" style="height: 38px; line-height: 38px; white-space: nowrap; text-overflow: ellipsis; letter-spacing: 2px;">
              
            </div>
            <div class="d-flex justify-content-between text-muted small mt-1" style="font-size:0.75rem;">
              <span>Tones Mode: SIM800L</span>
              <span id="dialer-status" class="text-success">Ready</span>
            </div>
          </div>

          <!-- Keypad Grid -->
          <div class="keypad-grid">
            <button class="dialer-key" data-key="1">1<small>o_o</small></button>
            <button class="dialer-key" data-key="2">2<small>abc</small></button>
            <button class="dialer-key" data-key="3">3<small>def</small></button>
            <button class="dialer-key" data-key="4">4<small>ghi</small></button>
            <button class="dialer-key" data-key="5">5<small>jkl</small></button>
            <button class="dialer-key" data-key="6">6<small>mno</small></button>
            <button class="dialer-key" data-key="7">7<small>pqrs</small></button>
            <button class="dialer-key" data-key="8">8<small>tuv</small></button>
            <button class="dialer-key" data-key="9">9<small>wxyz</small></button>
            <button class="dialer-key" data-key="*">*<small>tone</small></button>
            <button class="dialer-key" data-key="0">0<small>+</small></button>
            <button class="dialer-key" data-key="#">#<small>send</small></button>
          </div>
        </div>
        <div class="modal-footer border-0 pt-0 justify-content-center gap-2">
          <button type="button" class="btn btn-glass-secondary btn-sm" id="btn-dialer-clear">Clear Screen</button>
          <button type="button" class="btn btn-glass-primary btn-sm" id="btn-dialer-trigger-call">Place Call</button>
        </div>
      </div>
    </div>
  </div>

  <!-- ============================================================
       SD FILE PREVIEWER MODAL
  ============================================================ -->
  <div class="modal fade" id="file-preview-modal" tabindex="-1" aria-labelledby="filePreviewModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered modal-lg">
      <div class="modal-content glass-modal">
        <div class="modal-header border-0 pb-0">
          <h5 class="modal-title text-white" id="filePreviewModalLabel">
            <i class="fa-solid fa-file-invoice text-info me-2"></i>File Preview: <span id="preview-filename-title"></span>
          </h5>
          <button type="button" class="btn-close btn-close-white" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body">
          <div class="p-3 bg-dark rounded border border-secondary-subtle" style="max-height: 450px; overflow-y: auto;">
            <pre id="file-preview-content" class="text-white m-0" style="font-family: 'JetBrains Mono', monospace; font-size: 0.85rem; white-space: pre-wrap; word-break: break-all;"></pre>
          </div>
        </div>
        <div class="modal-footer border-0">
          <button type="button" class="btn btn-glass-secondary" data-bs-dismiss="modal">Close</button>
        </div>
      </div>
    </div>
  </div>

  <!-- Bootstrap 5 Bundle JS -->
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/js/bootstrap.bundle.min.js"></script>
  <!-- Chart.js -->
  <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>
  <!-- jsPDF & jsPDF-AutoTable for A4 PDF Export -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf-autotable/3.8.2/jspdf.plugin.autotable.min.js"></script>
  <!-- Socket.IO Client -->
  <script src="https://cdn.socket.io/4.7.4/socket.io.min.js"></script>
  <!-- Dashboard Script -->
  <script>
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

</script>

</body>
</html>

)rawliteral";

#endif
