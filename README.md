# GSV VARMA IVR — Project Guide

**One folder. Everything you need.**

---

## How to Start

### Web Dashboard
Double-click `START.bat` → browser opens at http://localhost:3000
> Login: `admin` / `GSVIVR001`

### ESP32 Firmware
Open Arduino IDE → File → Open → browse to:
```
GSV_VARMA_IVR\GSV_VARMA_IVR\GSV_VARMA_IVR.ino
```

---

## Folder Structure

```
GSV_VARMA_IVR/
│
├── START.bat                       ← Double-click to start dashboard
├── README.md                       ← This file
│
├── GSV_VARMA_IVR/                  ← ESP32 Arduino Sketch
│   ├── GSV_VARMA_IVR.ino           ← MAIN FIRMWARE (open in Arduino IDE)
│   ├── CallbackQueue.h             ← Call queue helper
│   ├── index_html.h                ← Dashboard HTML (stored on ESP32 flash)
│   └── wifi_config_html.h          ← AP Config Portal HTML
│
└── web/                            ← Web Dashboard
    ├── index.html                  ← Dashboard UI
    ├── style.css                   ← Orange/Brown premium theme
    ├── script.js                   ← Dashboard logic + DTMF dialer
    └── backend/                    ← Node.js API Server
        ├── server.js               ← Entry point (port 3000)
        ├── .env                    ← Configuration (Apps Script URL, API keys)
        ├── package.json
        ├── config/                 ← Server config
        ├── controllers/            ← API route handlers
        ├── routes/                 ← API route definitions
        ├── services/               ← Google Sheets + Apps Script logic
        ├── middleware/             ← Auth, rate limiting
        ├── socket/                 ← Real-time Socket.IO
        ├── utils/                  ← Helpers, logger, CSV export
        └── google-apps-script/
            └── Code.gs             ← Deploy this to Google Apps Script
```

---

## Hardware Pins

| Module | ESP32 Pin | Module Pin |
|--------|-----------|-----------|
| SIM800L GSM | GPIO16 (RX) | TX |
| SIM800L GSM | GPIO17 (TX) | RX |
| DFPlayer Mini MP3 | GPIO26 (RX) | TX |
| DFPlayer Mini MP3 | GPIO27 (TX) | RX (via 1kΩ) |
| LCD SDA | GPIO21 | SDA |
| LCD SCL | GPIO22 | SCL |
| Red LED | GPIO2 | + |
| Green LED | GPIO13 | + |
| Buzzer | GPIO25 | + |

> SIM800L needs **4.0V @ 2A** from a buck converter — NOT 5V directly.

---

## MP3 Voice Files (DFPlayer Mini SD Card)

| File | Voice Content |
|------|--------------|
| `0001.mp3` | Welcome greeting |
| `0002.mp3` | Menu: "Press 1, 2, 3..." |
| `0005.mp3` | "Thank you, Goodbye" |
| `0006.mp3` | "Invalid option, try again" |
| `0007.mp3` | "Connecting you now..." |

---

## Google Apps Script

Script ID: `1JY8HDpGbMYZ0Dw5lWRpXgKj4aeZBZeH7HSRyADqwr6uAc9lyT3hXFBwr`

The `Code.gs` file is in `web/backend/google-apps-script/Code.gs` — deploy it as a Web App with "Anyone" access.

---

## Default Credentials

| | Value |
|--|-------|
| AP WiFi | `VARMA IVR SYSTEM` / `GSVIVR001` |
| Web Login | `admin` / `GSVIVR001` |
