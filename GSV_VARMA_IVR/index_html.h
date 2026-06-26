#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>V-VARMA IVR Dashboard</title>
</head>
<body style="font-family: sans-serif; text-align: center; padding: 50px; background-color: #0c0804; color: #f3e9dc;">
  <h2 style="color: #ff9f1c;">V-VARMA IVR Dashboard</h2>
  <p style="color: #e63946; font-weight: bold;">Error: Dashboard file (index.html) not found on SD card!</p>
  <p>Please place the <b>index.html</b> file from the <b>voice_sd_card</b> folder onto the root of the SD card and restart the device.</p>
  <hr style="border: 0; border-top: 1px solid #444; margin: 30px 20%; width: 60%;">
  <p style="font-size: 0.85rem; color: #888;">Device Version: v1.3-IVR | GSV Electrical Enterprises</p>
</body>
</html>
)rawliteral";

#endif
