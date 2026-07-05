#ifndef WIFI_CONFIG_HTML_H
#define WIFI_CONFIG_HTML_H

const char wifi_config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>V-VARMA | IVR Setup Portal</title>
    <link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@300;400;600;700&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>
        :root {
            --v-orange: #f39c12;
            --v-brown: #8B4513;
            --v-green: #2ecc71;
            --bg-dark: #0d0d0d;
            --card-bg: #1a1a1a;
            --border-color: #333;
        }
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Space Grotesk', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
        body { background: var(--bg-dark); color: #eee; min-height: 100vh; padding: 20px; display: flex; align-items: center; justify-content: center; }
        .input-group-v { position: relative; margin-bottom: 1.2rem; display: flex; gap: 8px; }
        .input-group-p { position: relative; margin-bottom: 1.2rem; }
        .eye-icon { 
            position: absolute; right: 15px; top: 50%; transform: translateY(-50%); 
            color: #555; cursor: pointer; transition: 0.2s; z-index: 10;
        }
        .eye-icon:hover { color: var(--v-orange); }
        .container { max-width: 500px; width: 100%; margin: 0 auto; }
        .header { text-align: center; margin-bottom: 25px; }
        .v-shield-logo { width: 85px; height: 85px; color: var(--v-orange); margin: 0 auto 10px; filter: drop-shadow(0 0 15px rgba(243,156,18,0.4)); }
        .v-card {
            background: var(--card-bg); border: 2px solid var(--border-color);
            border-radius: 24px; padding: 2rem;
            border-left: 6px solid var(--v-brown);
            box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
        }
        .card-title { color: var(--v-orange); font-weight: 800; text-transform: uppercase; margin-bottom: 1.2rem; display: flex; align-items: center; gap: 10px; font-size: 0.9rem; }
        .input-label { display: block; font-size: 0.72rem; color: #888; margin-bottom: 0.4rem; text-transform: uppercase; letter-spacing: 1px; }
        .v-input {
            width: 100%; background: #0a0a0a; border: 2px solid var(--border-color);
            padding: 0.9rem 1rem; border-radius: 12px; color: white; outline: none; transition: 0.3s;
        }
        .v-input:focus { border-color: var(--v-orange); }
        .btn-v { 
            width: 100%; padding: 1rem; border-radius: 12px; font-weight: 800; text-transform: uppercase;
            border: none; cursor: pointer; transition: 0.3s; font-size: 0.85rem; display: flex; align-items: center; justify-content: center; gap: 8px;
        }
        .btn-v-orange { background: linear-gradient(135deg, var(--v-brown), var(--v-orange)); color: white; }
        .btn-v-blue { background: rgba(56, 189, 248, 0.15); border: 1px solid rgba(56, 189, 248, 0.3); color: #38bdf8; height: 50px; font-size: 0.75rem; white-space: nowrap; width: auto; padding: 0 15px; }
        .btn-v-blue:hover { background: rgba(56, 189, 248, 0.25); }
        .wifi-list { max-height: 180px; overflow-y: auto; background: #0a0a0a; border-radius: 12px; margin-bottom: 1.2rem; display: none; border: 1px solid #333; }
        .wifi-item { padding: 12px 15px; cursor: pointer; border-bottom: 1px solid #222; display: flex; justify-content: space-between; font-size: 0.82rem; }
        .wifi-item:hover { background: #111; color: var(--v-orange); }
        #status-msg { text-align: center; font-size: 0.85rem; margin-top: 1rem; font-weight: bold; }
        .mac-info { font-size: 0.72rem; color: #555; text-align: center; font-family: monospace; }
        .logo-title { letter-spacing: 3px; font-weight: 800; font-size: 1.7rem; color: #fff; margin-bottom: 0.2rem; }
        
        .connecting-view { display: none; text-align: center; }
        .pulse-ring {
            width: 70px; height: 70px; border-radius: 50%; background: rgba(243, 156, 18, 0.1);
            display: flex; align-items: center; justify-content: center; color: var(--v-orange);
            margin: 20px auto 25px auto; animation: pulse-ring-anim 2s infinite;
        }
        @keyframes pulse-ring-anim {
            0% { box-shadow: 0 0 0 0 rgba(243, 156, 18, 0.4); }
            70% { box-shadow: 0 0 0 16px rgba(243, 156, 18, 0); }
            100% { box-shadow: 0 0 0 0 rgba(243, 156, 18, 0); }
        }
    </style>
</head>
<body>
    <div class="container" id="portal-container">
        <div id="setup-form-view">
            <div class="header">
                <div class="v-shield-logo">
                    <svg viewBox="0 0 512 512" xmlns="http://www.w3.org/2000/svg" style="width:100%; height:100%;">
                        <path d="M466.5 83.7l-192-80c-11.8-4.9-25.2-4.9-37 0l-192 80C27.3 91.3 16 108.3 16 128c0 198.5 114.5 335.7 221.5 380.3 11.8 4.9 25.1 4.9 36.9 0C381.5 463.7 496 326.5 496 128c0-19.7-11.3-36.7-29.5-44.3z" fill="currentColor"/>
                        <path d="M160 160 C180 250 220 380 256 400 C300 350 340 220 360 160" fill="none" stroke="#0d0d0d" stroke-width="45" stroke-linecap="round" stroke-linejoin="round"/>
                    </svg>
                </div>
                <h1 class="logo-title">V-VARMA</h1>
                <div style="font-size: 0.78rem; color: var(--v-orange); letter-spacing: 2px; font-weight: 700; text-transform: uppercase;">IVR Setup mode</div>
                <div style="font-size: 0.82rem; color: #888; margin-top: 6px;">Hotspot Network: <strong style="color: #fff;">V VARMA IVR</strong></div>
            </div>

            <div class="v-card">
                <div class="card-title"><i class="fas fa-wifi"></i> 1. Network Topology</div>
                
                <label class="input-label">Select SSID</label>
                <div class="input-group-v">
                    <input type="text" id="ssid" class="v-input" placeholder="Select or type Wi-Fi SSID" required>
                    <button type="button" class="btn-v btn-v-blue" id="scan-btn" onclick="scanWiFi()">
                        <i class="fas fa-sync-alt" id="scan-icon"></i> SCAN
                    </button>
                </div>
                <div id="wifiList" class="wifi-list"></div>
                
                <label class="input-label">Wi-Fi Password</label>
                <div class="input-group-p" style="margin-bottom: 1.5rem;">
                    <input type="password" id="pass" class="v-input" placeholder="Network Security Key">
                    <i class="fa-solid fa-eye eye-icon" onclick="togglePass('pass', this)"></i>
                </div>

                <div class="card-title"><i class="fas fa-lock"></i> 2. Admin Credentials</div>
                
                <label class="input-label">Username</label>
                <input type="text" id="devUser" class="v-input" style="margin-bottom: 1.2rem;" value="admin" placeholder="Admin Username" required>
                
                <label class="input-label">Authorization Key</label>
                <div class="input-group-p" style="margin-bottom: 1.5rem;">
                    <input type="password" id="devPass" class="v-input" placeholder="Security key for dashboard login" required>
                    <i class="fa-solid fa-eye eye-icon" onclick="togglePass('devPass', this)"></i>
                </div>

                <div class="card-title"><i class="fas fa-cloud"></i> 3. Google Sheet Integration</div>
                <label class="input-label">Google Script ID</label>
                <input type="text" id="scriptId" class="v-input" style="margin-bottom: 1.8rem;" placeholder="AKfycb..." value="">
                
                <button class="btn-v btn-v-orange" onclick="save()"><i class="fas fa-save"></i> Save Configuration</button>
                <div id="status-msg"></div>
            </div>
        </div>

        <!-- CONNECTING VIEW -->
        <div id="connecting-view" class="connecting-view">
            <div class="pulse-ring">
                <svg viewBox="0 0 512 512" xmlns="http://www.w3.org/2000/svg" style="width:50%; height:50%;">
                    <path d="M466.5 83.7l-192-80c-11.8-4.9-25.2-4.9-37 0l-192 80C27.3 91.3 16 108.3 16 128c0 198.5 114.5 335.7 221.5 380.3 11.8 4.9 25.1 4.9 36.9 0C381.5 463.7 496 326.5 496 128c0-19.7-11.3-36.7-29.5-44.3z" fill="currentColor"/>
                </svg>
            </div>
            <h1 style="color: var(--v-orange); font-weight: 800; letter-spacing: 2px;">REBOOTING DEVICE</h1>
            <p style="margin: 1rem 0; color: #888; font-size: 0.95rem; line-height: 1.5;">The ESP32 is saving configuration parameters and rebooting to join the target Wi-Fi network.</p>
            <div style="background: rgba(255,255,255,0.03); border: 1px solid #333; padding: 15px; border-radius: 12px; font-family: monospace; font-size: 0.85rem; margin-top: 1.5rem; text-align: left; line-height: 1.6;">
                1. Stay connected to the <strong style="color: var(--v-orange);">V VARMA IVR</strong> hotspot and open <a href="http://192.168.4.1/dashboard" style="color: #38bdf8; text-decoration: underline;">http://192.168.4.1/dashboard</a> to find the router IP.<br>
                2. Or connect to your router Wi-Fi and open the IP address assigned to the ESP32 (e.g., http://&lt;router-ip&gt;/dashboard).<br>
                3. You can also check your router's client list or the serial monitor trace on boot to find the exact IP address.
            </div>
        </div>
    </div>

    <script>
        function togglePass(id, el) {
            const inp = document.getElementById(id);
            if (inp.type === 'password') {
                inp.type = 'text';
                el.classList.replace('fa-eye', 'fa-eye-slash');
            } else {
                inp.type = 'password';
                el.classList.replace('fa-eye-slash', 'fa-eye');
            }
        }

        async function scanWiFi() {
            const list = document.getElementById('wifiList');
            const icon = document.getElementById('scan-icon');
            list.style.display = 'block';
            list.innerHTML = '<div style="padding:15px; color:#888; text-align:center;"><i class="fas fa-spinner fa-spin"></i> Scanning...</div>';
            icon.classList.add('fa-spin');
            
            try {
                const res = await fetch('/scan_wifi');
                const nets = await res.json();
                if (nets.length === 0) {
                    list.innerHTML = '<div style="padding:15px; color:#888; text-align:center;">No networks found</div>';
                } else {
                    list.innerHTML = nets.map(n => `<div class="wifi-item" onclick="selectSSID('${n.ssid}')"><span>${n.ssid}</span><span>${n.rssi} dBm</span></div>`).join('');
                }
            } catch(e) { 
                list.innerHTML = '<div style="padding:15px; color:red; text-align:center;">Scan failed</div>'; 
            } finally {
                icon.classList.remove('fa-spin');
            }
        }

        function selectSSID(s) { 
            document.getElementById('ssid').value = s; 
            document.getElementById('wifiList').style.display = 'none'; 
        }

        async function save() {
            const s = document.getElementById('ssid').value;
            const p = document.getElementById('pass').value;
            const sid = document.getElementById('scriptId').value;
            const u = document.getElementById('devUser').value;
            const pw = document.getElementById('devPass').value;
            const msg = document.getElementById('status-msg');
            
            if (!s || !u || !pw) {
                msg.innerHTML = "<span style='color:red'>Please fill out all required fields.</span>";
                return;
            }
            
            msg.innerHTML = "Sending credentials to hardware...";
            msg.style.color = "var(--v-orange)";
            
            const data = new URLSearchParams();
            data.append('ssid', s); 
            data.append('pass', p); 
            data.append('scriptId', sid);
            data.append('adminUser', u); 
            data.append('adminPass', pw); 
            
            try {
                // First save Wi-Fi (without rebooting), then save system parameters and trigger reboot
                data.append('reboot', 'false');
                const res = await fetch('/save_net', { method: 'POST', body: data });
                const resSys = await fetch('/save_sys?script=' + encodeURIComponent(sid) + '&adminUser=' + encodeURIComponent(u) + '&adminPass=' + encodeURIComponent(pw) + '&reboot=true');
                
                if (res.ok && resSys.ok) {
                    msg.innerHTML = "<span style='color:var(--v-green)'>SUCCESS! Rebooting...</span>";
                    setTimeout(() => {
                        document.getElementById('setup-form-view').style.display = 'none';
                        document.getElementById('connecting-view').style.display = 'block';
                    }, 1000);
                } else {
                    msg.innerHTML = "<span style='color:red'>Hardware rejection error</span>";
                }
            } catch(e) { 
                msg.innerHTML = "<span style='color:red'>Network communication error</span>"; 
            }
        }
    </script>
</body>
</html>
)rawliteral";

#endif
