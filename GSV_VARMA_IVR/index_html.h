#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>V-VARMA IVR Dashboard - Status & Diagnostics</title>
</head>
<body style="font-family: sans-serif; padding: 40px 20px; background-color: #0c0804; color: #f3e9dc; max-width: 600px; margin: 0 auto; line-height: 1.6;">
  <h2 style="color: #ff9f1c; margin-bottom: 20px; font-weight: 800; letter-spacing: 1px;">V-VARMA IVR DIAGNOSTICS</h2>
  
  <div style="background-color: rgba(230, 57, 70, 0.1); border: 2px solid #e63946; border-radius: 12px; padding: 20px; text-align: left; margin-bottom: 30px;">
    <h3 style="color: #e63946; margin-top: 0; font-size: 1.1rem; display: flex; align-items: center; gap: 8px;">
      ⚠️ Dashboard Serving Prevented
    </h3>
    <p style="margin: 8px 0 0 0; font-size: 0.95rem;">
      The device failed to serve the main dashboard page. This typically occurs because:
    </p>
    <ul style="margin: 10px 0 0 20px; padding: 0; font-size: 0.9rem;">
      <li style="margin-bottom: 6px;"><b>Active Call in Progress:</b> SD card reading is temporarily blocked during phone calls to guarantee clear audio streaming without stuttering. Wait for the call to end and refresh.</li>
      <li style="margin-bottom: 6px;"><b>Missing File:</b> The <code style="color: #ff9f1c;">index.html</code> file is not located at the root of your SD card. Copy it from the <code style="color: #ff9f1c;">voice_sd_card</code> directory.</li>
      <li style="margin-bottom: 6px;"><b>SD Card Offline:</b> The SD card is unmounted, faulty, or not inserted correctly.</li>
    </ul>
  </div>

  <div style="background-color: #1a1008; border: 1px solid #333; border-radius: 12px; padding: 20px; text-align: left;">
    <h3 style="color: #ff9f1c; margin-top: 0; font-size: 1.1rem;">🌐 STA & mDNS Troubleshooting</h3>
    <ul style="margin: 10px 0 0 20px; padding: 0; font-size: 0.9rem;">
      <li style="margin-bottom: 10px;">
        <b>Using Local IP Address:</b> If <code style="color: #ff9f1c;">http://vvarmaivr.local</code> fails to load, check your Wi-Fi router's connected client list or check the serial trace on startup to find the ESP32's assigned IP (e.g. <code style="color: #ff9f1c;">http://192.168.1.150/dashboard</code>).
      </li>
      <li style="margin-bottom: 10px;">
        <b>Windows Firewall Profile:</b> If using Windows, check that your connected WiFi network profile is set to <b>Private Network</b>. Windows blocks mDNS name resolution on "Public" profiles.
      </li>
      <li style="margin-bottom: 10px;">
        <b>Mobile Device & Cellular Data:</b> If accessing from iOS or Android, turn off cellular data (mobile internet) temporary so your browser queries the local WiFi network interface instead of mobile towers.
      </li>
      <li style="margin-bottom: 10px;">
        <b>Force Reboot:</b> You can factory-reset the WiFi credentials by holding the physical <b>BOOT (GPIO 0)</b> button on the board for 10 seconds to reopen the AP Setup Hotspot.
      </li>
    </ul>
  </div>

  <hr style="border: 0; border-top: 1px solid #333; margin: 30px 0;">
  <p style="font-size: 0.8rem; color: #888;">Device Version: v2.5.0-IVR | GSV Electrical Enterprises</p>
</body>
</html>
)rawliteral";

#endif
