/**
 * V-VARMA ESP32 SIM800L IVR MANAGEMENT SYSTEM
 *
 * Hardware Connections:
 * ─── SIM800L GSM Module ───────────────────────────────────────────
 * GPIO16  (Serial2 RX)  →  SIM800L TX
 * GPIO17  (Serial2 TX)  →  SIM800L RX
 * SIM800L VCC           →  4.0 V Buck Converter
 * Common GND            →  ESP32 GND, Buck GND, SIM800L GND
 *
 * ─── LED Indicators & Buzzer ─────────────────────────────────────
 * GPIO2   →  Red  LED   (error / ringing)
 * GPIO13  →  Green LED  (active / connected)
 * GPIO25  →  Buzzer     (tone alerts)
 *
 * ─── MP3-TF-16P DFPlayer (UART Serial1 remapped) ─────────────────
 * GPIO26  (Serial1 TX)  →  MP3-TF-16P RX
 * GPIO27  (Serial1 RX)  →  MP3-TF-16P TX
 * 5 V                   →  MP3-TF-16P VCC
 * GND                   →  MP3-TF-16P GND
 *
 * SD-card folder structure for MP3 module:
 *   /01/001.mp3  — Boot jingle
 *   /01/002.mp3  — Incoming ring
 *   /01/003.mp3  — Call answered
 *   /01/004.mp3  — Call ended
 *   /01/005.mp3  — IVR welcome message
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <NetBIOS.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>            // [OTA] ESP32 built-in OTA flash library
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include "CallbackQueue.h"
#include "index_html.h"
#include "wifi_config_html.h"

// ==================== DEFINITIONS ====================
#define FIRMWARE_VERSION "v1.1-IVR"
#define DEVICE_NAME "vvarmaivr"
#define AP_SSID DEVICE_NAME
#define AP_PASS "GSVIVR001"

#define GSM_RX_PIN 16
#define GSM_TX_PIN 17
#define GSM_BAUD 9600

// ─── Indicators ────────────────────────────────────────────────────────────────
#define LED_PIN        2   // Red  LED — ringing / error
#define LED_GREEN     13   // Green LED — active / connected / WiFi blink
#define BUZZER_PIN    25   // Piezo buzzer
#define RESET_BUTTON_PIN 0 // Boot / factory-reset button

// ─── MP3-TF-16P (DFPlayer Mini compatible) ────────────────────────────────────
// Uses HardwareSerial 1 remapped to free GPIO26/27
#define MP3_TX_PIN    26   // ESP32 TX → module RX
#define MP3_RX_PIN    27   // ESP32 RX ← module TX
#define MP3_BAUD    9600

// DFPlayer command bytes
#define MP3_START   0x7E
#define MP3_VER     0xFF
#define MP3_LEN     0x06
#define MP3_END     0xEF
#define MP3_CMD_PLAY_FOLDER   0x0F
#define MP3_CMD_VOLUME        0x06
#define MP3_CMD_STOP          0x16
#define MP3_CMD_RESET         0x0C

// ==================== GLOBALS ====================
unsigned long resetBtnStartTime = 0;
bool resetBtnPressed = false;

// HardwareSerial instances:
//   Serial2 -> GSM module  (GPIO16 RX, GPIO17 TX)
//   Serial1 -> MP3 player  (GPIO27 RX, GPIO26 TX)
HardwareSerial mp3Serial(1);   // UART1 remapped to GPIO26/27

// ==================== MP3 PLAYER (MP3-TF-16P / DFPlayer) ====================
// Forward declarations
void mp3SendCommand(uint8_t cmd, uint8_t param1, uint8_t param2);
void mp3SetVolume(uint8_t vol);
void mp3Play(uint8_t folder, uint8_t track);
void mp3Stop();
void mp3PlayAsync(uint8_t folder, uint8_t track);

/**
 * Build and send a DFPlayer Mini 10-byte serial command.
 * Packet: 7E FF 06 CMD FB p1 p2 CS_H CS_L EF
 */
void mp3SendCommand(uint8_t cmd, uint8_t param1, uint8_t param2) {
  uint8_t buf[10];
  buf[0] = MP3_START;
  buf[1] = MP3_VER;
  buf[2] = MP3_LEN;
  buf[3] = cmd;
  buf[4] = 0x00;   // no ACK
  buf[5] = param1;
  buf[6] = param2;
  uint16_t cs = -(buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6]);
  buf[7] = (cs >> 8) & 0xFF;
  buf[8] =  cs       & 0xFF;
  buf[9] = MP3_END;
  mp3Serial.write(buf, sizeof(buf));
  delay(30);
}

/** Initialize the MP3-TF-16P on GPIO26(TX)/GPIO27(RX). */
void mp3Init() {
  mp3Serial.begin(MP3_BAUD, SERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN);
  delay(800);
  mp3SendCommand(MP3_CMD_RESET, 0x00, 0x00);
  delay(1000);
  mp3SetVolume(25);   // Default volume 0-30
  Serial.println("[MP3] MP3-TF-16P ready on GPIO26/27");
}

/** Set volume (0=mute, 30=max). */
void mp3SetVolume(uint8_t vol) {
  if (vol > 30) vol = 30;
  mp3SendCommand(MP3_CMD_VOLUME, 0x00, vol);
}

/** Play track from numbered folder. mp3Play(1,2) plays /01/002.mp3 */
void mp3Play(uint8_t folder, uint8_t track) {
  mp3SendCommand(MP3_CMD_PLAY_FOLDER, folder, track);
}

/** Stop current track. */
void mp3Stop() {
  mp3SendCommand(MP3_CMD_STOP, 0x00, 0x00);
}

/** Non-blocking play (DFPlayer is always async). */
void mp3PlayAsync(uint8_t folder, uint8_t track) {
  mp3Play(folder, track);
}

CallbackQueue callbackQueue;
Preferences preferences;
DNSServer dnsServer;
WebServer server(80);

// NVS Settings
String savedSSID = "";
String savedPass = "";
String scriptId = "1JY8HDpGbMYZ0Dw5lWRpXgKj4aeZBZeH7HSRyADqwr6uAc9lyT3hXFBwr";
String deviceUser = "admin";
String devicePass = "GSVIVR001"; // Fallback default
String myMac = "";
String localIpStr = "0.0.0.0";

// Telegram Settings
String tgToken = "";
String tgChatId = "";

// Sync Triggers
bool syncIncoming = true;
bool syncOutgoing = true;
bool syncSms = true;
bool syncSystem = false;
bool syncNetwork = false;
bool callSystemActive = true;
bool smsSystemActive = true;

// System States
enum SystemState {
  SYSTEM_BOOT,
  SYSTEM_WIFI_CONFIG,
  SYSTEM_RUNNING
};
SystemState currentSystemState = SYSTEM_BOOT;

enum CallState {
  STATE_IDLE,
  STATE_VALIDATING_CALLER,
  STATE_RINGING,
  STATE_ACTIVE_CALL,
  STATE_COLLECTING_DTMF,
  STATE_ENDED,
  STATE_CALLBACK_DIALING,
  STATE_VALIDATING_SECOND_CALL
};
CallState currentCallState = STATE_IDLE;

// IVR Variables
String callerNumber = "";
String dtmfBuffer = "";
String lastDtmfDigit = "";
unsigned long callStartMillis = 0;
unsigned long lastDtmfMillis = 0;

// Caller Validation
volatile int validationResult = -1; // -1: pending, 0: invalid, 1: valid
String validatingNumber = "";
unsigned long ringingStartMillis = 0;
unsigned long lastActivityTime = 0;

// SIM800L Status variables
String gsmSignal = "0";
String gsmOperator = "Searching...";
String gsmBattery = "0.0V";
String gsmSimStatus = "Checking...";
String gsmRegStatus = "0";
bool callWaitingActivated = false;
int sheetsSyncCount = 0;
int sheetsSyncErrors = 0;
String sheetsLastSyncTime = "—";

// SMS & LED Indicator global variables
bool receivingSmsBody = false;
String incomingSmsSender = "";
int smsFlashCounter = 0;
unsigned long lastLedUpdate = 0;
int ledBlinkCounter = 0;


// Buffers & Logs
String webSerialLog = "";
#define MAX_LOGS 50
struct CallRecord {
  String timeStr;
  String phoneNumber;
  String dtmfData;
  String callStatus;
};
CallRecord callLogs[MAX_LOGS];
int logCount = 0;
int logInsertIndex = 0;

// Synchronization Tasks Struct
struct SyncTaskData {
  String timeStr;
  String phoneNumber;
  String dtmfData;
  String callStatus;
  String direction;
};

// State engine tracking variables
bool inCall = false;
unsigned long callStartTime = 0;
String currentCaller = "";
String currentDTMF = "";
bool isRegisteredCaller = false;

// Hardware status flags
bool gsmReady = false;
bool simReady = false;
bool netReady = false;
int signalStrength = 0;

// SD Card State
bool sdCardFound = false;
uint64_t sdCardUsedBytes = 0;
uint64_t sdCardTotalBytes = 0;

// Scheduler & Time variables
unsigned long gsmQueryTimer = 0;
int gsmQueryStep = 0;
bool serverStarted = false;

// ==================== OTA UPDATE GLOBALS ====================
// [OTA] Firmware URL — stored in NVS so it survives reboots.
// Set via the dashboard OTA Settings tab or paste directly:
// ─────────────────────────────────────────────────────────────
String otaFirmwareUrl = "";      // PASTE_FIRMWARE_URL_HERE (loaded from NVS at boot)
// ─────────────────────────────────────────────────────────────
String otaStatus      = "idle";  // [OTA] idle | starting | downloading | flashing | success | failed
bool   otaInProgress  = false;   // [OTA] Guard flag — prevents concurrent OTA attempts

// Forward declarations
void parseGsmResponse(String line);
void beep(int freq, int duration);
void setupWebServer();
bool isAuth();
void sendLargePage(const char* data);
void changeState(CallState newState);



// ==================== SERIAL LOGGER ====================
void webLog(String msg) {
  Serial.println("[LOG] " + msg);
  webSerialLog += msg + "\n";
  if (webSerialLog.length() > 2048) {
    webSerialLog = webSerialLog.substring(1024); // Truncate old logs if buffer exceeds 2KB
  }
}

// ==================== TELEGRAM ALERTS ====================
void sendTelegramAlert(String message) {
  if (tgToken.length() == 0 || tgChatId.length() == 0) return;
  if (WiFi.status() != WL_CONNECTED) return;
  
  webLog("Sending Telegram alert: " + message);
  WiFiClientSecure client;
  client.setInsecure(); // Bypass SSL validation for Telegram
  
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + tgToken + "/sendMessage";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  String payload = "{\"chat_id\":\"" + tgChatId + "\",\"text\":\"" + message + "\"}";
  int httpCode = http.POST(payload);
  
  if (httpCode > 0) {
    webLog("Telegram alert sent (HTTP " + String(httpCode) + ")");
  } else {
    webLog("Failed to send Telegram alert (HTTP " + String(httpCode) + ")");
  }
  http.end();
}

// ==================== TONE BEEP & LEDS ====================
void beep(int freq, int duration) {
  tone(BUZZER_PIN, freq, duration);
}

void playBootSound() {
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  beep(1047, 100); delay(120);
  beep(1318, 100); delay(120);
  beep(1568, 150); delay(170);
  mp3Play(1, 1);                 // /01/001.mp3 — boot jingle
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void playSuccessSound() {
  digitalWrite(LED_GREEN, HIGH);
  beep(1500, 150); delay(200);
  beep(2000, 150); delay(200);
  digitalWrite(LED_GREEN, LOW);
}

void playErrorSound() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    beep(400, 200); delay(250);
    digitalWrite(LED_PIN, LOW);
  }
}


// ==================== WIFI AP/STA LOGIC ====================
void mDNS_begin() {
  bool staActive = (WiFi.status() == WL_CONNECTED);
  wifi_mode_t wmode = WiFi.getMode();
  bool apActive = (WiFi.softAPIP() != IPAddress(0, 0, 0, 0) &&
                   (wmode == WIFI_AP || wmode == WIFI_AP_STA));
  if (!staActive && !apActive) return;

  MDNS.end();
  if (MDNS.begin(DEVICE_NAME)) {
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "id", myMac);
    MDNS.addServiceTxt("http", "tcp", "ver", FIRMWARE_VERSION);
    webLog("mDNS responder active at http://" + String(DEVICE_NAME) + ".local");
  } else {
    webLog("CRITICAL: mDNS startup failed");
  }

  if (staActive) {
    NBNS.begin(DEVICE_NAME);
    webLog("NetBIOS responder active at http://" + String(DEVICE_NAME));
  }
}

void startAPMode() {
  currentSystemState = SYSTEM_WIFI_CONFIG;
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(1000);
  IPAddress apIP = WiFi.softAPIP();
  localIpStr = apIP.toString();

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);
  
  setupWebServer();
  mDNS_begin();
  webLog("AP Mode started. SSID: " + String(AP_SSID) + " | IP: " + localIpStr);
}

void factoryReset() {
  webLog("Factory Reset triggered. Clearing NVS memory...");
  playErrorSound();   // Audio alert instead of LCD
  preferences.begin("wifi-config", false);
  preferences.clear();
  preferences.end();
  
  delay(2000);
  ESP.restart();
}

// ==================== OTA UPDATE (OVER-THE-AIR) ====================
// [OTA] FreeRTOS task — downloads firmware from OTA_FIRMWARE_URL and flashes it.
// Pinned to Core 1 with 16 KB stack, same pattern as other HTTP tasks in this project.
void otaTask(void* pvParameters) {
  webLog("[OTA] Download started from configured URL...");
  otaStatus = "downloading";

  String url = otaFirmwareUrl; // [OTA] URL loaded from NVS or set via dashboard

  // [OTA] Support both plain HTTP and HTTPS firmware servers
  WiFiClient        plainClient;
  WiFiClientSecure  secureClient;
  secureClient.setInsecure(); // [OTA] Bypass SSL cert check — acceptable for firmware hosted on trusted internal server

  HTTPClient http;
  bool begun = url.startsWith("https://")
                 ? http.begin(secureClient, url)
                 : http.begin(plainClient,  url);

  if (!begun) {
    webLog("[OTA] Failed to connect to firmware URL.");
    otaStatus = "failed";
    otaInProgress = false;
    vTaskDelete(NULL);
    return;
  }

  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  webLog("[OTA] Server response: HTTP " + String(httpCode));

  if (httpCode != HTTP_CODE_OK) {
    webLog("[OTA] Download failed. Aborting update.");
    http.end();
    otaStatus = "failed";
    otaInProgress = false;
    vTaskDelete(NULL);
    return;
  }

  int contentLen = http.getSize();
  webLog("[OTA] Firmware size: " + String(contentLen) + " bytes");

  if (contentLen <= 0) {
    webLog("[OTA] Server returned no content-length. Aborting.");
    http.end();
    otaStatus = "failed";
    otaInProgress = false;
    vTaskDelete(NULL);
    return;
  }

  // [OTA] Begin OTA partition write — Update.begin() reserves flash space
  if (!Update.begin(contentLen)) {
    webLog("[OTA] Not enough flash space. Error code: " + String(Update.getError()));
    http.end();
    otaStatus = "failed";
    otaInProgress = false;
    vTaskDelete(NULL);
    return;
  }

  otaStatus = "flashing";
  webLog("[OTA] Writing firmware to flash...");

  // [OTA] Stream firmware bytes directly from HTTP response into flash
  WiFiClient* stream  = http.getStreamPtr();
  size_t      written = Update.writeStream(*stream);

  webLog("[OTA] Bytes written: " + String(written) + " / " + String(contentLen));

  if (!Update.end()) {
    webLog("[OTA] Update.end() failed. Error code: " + String(Update.getError()));
    http.end();
    otaStatus = "failed";
    otaInProgress = false;
    vTaskDelete(NULL);
    return;
  }

  if (!Update.isFinished()) {
    webLog("[OTA] Update did not finish cleanly. Error code: " + String(Update.getError()));
    http.end();
    otaStatus = "failed";
    otaInProgress = false;
    vTaskDelete(NULL);
    return;
  }

  http.end();
  otaStatus = "success";
  webLog("[OTA] Firmware flashed successfully! Rebooting in 3 seconds...");
  // [OTA] Brief delay so the /ota_status endpoint can report "success" before reboot
  delay(3000);
  ESP.restart();
  vTaskDelete(NULL);
}

// [OTA] Called by the web route handler to validate conditions and spawn otaTask
void triggerOtaUpdate() {
  if (otaInProgress) {
    webLog("[OTA] Update already in progress — ignoring duplicate trigger.");
    return;
  }
  if (otaFirmwareUrl.length() == 0) {
    webLog("[OTA] No firmware URL set. Configure it in the dashboard OTA Settings tab.");
    return;
  }
  if (WiFi.status() != WL_CONNECTED) {
    webLog("[OTA] WiFi not connected. Cannot start OTA.");
    return;
  }
  otaInProgress = true;
  otaStatus     = "starting";
  webLog("[OTA] Spawning OTA task on Core 1...");
  // [OTA] 16 KB stack — required for TLS handshake + Update write buffer
  xTaskCreatePinnedToCore(otaTask, "ota_task", 16384, NULL, 1, NULL, 1);
}
// ==================== END OTA UPDATE ====================

// ==================== CALLER VALIDATION SYNC ====================
struct ValidateTaskData {
  String phoneNumber;
};

void validateTask(void *pvParameters) {
  ValidateTaskData *data = (ValidateTaskData *)pvParameters;
  if (data == NULL) {
    validationResult = 0;
    vTaskDelete(NULL);
    return;
  }
  
  if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Validating caller ID via Google Sheets: " + data->phoneNumber);
    WiFiClientSecure client;
    client.setInsecure(); // Bypass SSL verification
    
    HTTPClient http;
    // URL Encoding + as %2B in case there is a +
    String encPhone = data->phoneNumber;
    encPhone.replace("+", "%2B");
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=validateUser&phone=" + encPhone;
    
    if (http.begin(client, url)) {
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        webLog("[HTTP Task] Validation Response: " + payload);
        if (payload.indexOf("\"registered\":true") != -1 || payload.indexOf("\"registered\": true") != -1) {
          validationResult = 1;
        } else {
          validationResult = 0;
        }
      } else {
        webLog("[HTTP Task] Validation Failed. HTTP Code: " + String(httpCode));
        validationResult = 0;
      }
      http.end();
    } else {
      webLog("[HTTP Task] Validation connection failed.");
      validationResult = 0;
    }
  } else {
    webLog("[HTTP Task] No WiFi or script ID. Assuming invalid caller.");
    validationResult = 0;
  }
  delete data;
  vTaskDelete(NULL);
}

// ==================== SMS SENDING & LOGGING ====================
struct SyncSmsTaskData {
  String timeStr;
  String phoneNumber;
  String message;
  String direction;
  String status;
};

void syncSmsTask(void *pvParameters) {
  SyncSmsTaskData *data = (SyncSmsTaskData *)pvParameters;
  if (data == NULL) {
    vTaskDelete(NULL);
    return;
  }

  if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Syncing SMS to Google Sheets script: " + scriptId);
    WiFiClientSecure client;
    client.setInsecure();
    
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec";
    
    if (http.begin(client, url)) {
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      http.addHeader("Content-Type", "application/json");
      
      StaticJsonDocument<300> doc;
      doc["phoneNumber"] = data->phoneNumber;
      doc["message"] = data->message;
      doc["direction"] = data->direction;
      doc["status"] = data->status;
      doc["mac"] = myMac;
      doc["user"] = deviceUser;
      doc["pass"] = devicePass;
      
      String jsonStr;
      serializeJson(doc, jsonStr);
      
      int httpCode = http.POST(jsonStr);
      if (httpCode == 200 || httpCode == 302) {
        webLog("[HTTP Task] SMS Sync status code: " + String(httpCode));
      } else {
        webLog("[HTTP Task] SMS Sync POST failure: " + (httpCode > 0 ? String(httpCode) : http.errorToString(httpCode)));
      }
      http.end();
    }
  }
  delete data;
  vTaskDelete(NULL);
}

void triggerSmsSync(String timeStr, String phone, String msg, String direction, String status) {
  if (currentSystemState == SYSTEM_WIFI_CONFIG) return;
  SyncSmsTaskData *data = new SyncSmsTaskData();
  if (data != NULL) {
    data->timeStr = timeStr;
    data->phoneNumber = phone;
    data->message = msg;
    data->direction = direction;
    data->status = status;
    xTaskCreatePinnedToCore(syncSmsTask, "sms_sync_task", 16384, (void *)data, 1, NULL, 1);
  }
}

void addSmsLog(String phone, String msg, String direction, String status) {
  unsigned long uptimeSecs = millis() / 1000;
  unsigned long hrs = uptimeSecs / 3600;
  unsigned long mins = (uptimeSecs % 3600) / 60;
  unsigned long secs = uptimeSecs % 60;
  
  char timeBuf[16];
  snprintf(timeBuf, sizeof(timeBuf), "%02duh %02dum %02dus", (int)hrs, (int)mins, (int)secs);
  
  if (syncSms) {
    sendTelegramAlert("SMS [" + direction + "]: " + phone + " - Message: " + msg);
  }
  
  triggerSmsSync(String(timeBuf), phone, msg, direction, status);
}

void sendSMS(String number, String message) {
  if (number == "") return;
  webLog("Sending SMS to " + number + ": " + message);
  Serial2.println("AT+CMGF=1"); // Set text mode
  delay(150);
  Serial2.print("AT+CMGS=\"");
  Serial2.print(number);
  Serial2.println("\"");
  delay(250);
  Serial2.print(message);
  delay(150);
  Serial2.write(26); // Ctrl+Z key code
  delay(200);
  smsFlashCounter = 20; // 2 seconds of fast alternate blinking
  webLog("SMS sent command issued.");
}

// ==================== GOOGLE SHEET BACKGROUND SYNC ====================
void syncTask(void *pvParameters) {
  SyncTaskData *data = (SyncTaskData *)pvParameters;
  if (data == NULL) {
    vTaskDelete(NULL);
    return;
  }

  if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Syncing event to Google Sheets script: " + scriptId);
    WiFiClientSecure client;
    client.setInsecure(); // Bypass SSL verification
    
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec";
    
    if (http.begin(client, url)) {
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      http.addHeader("Content-Type", "application/json");
      
      StaticJsonDocument<300> doc;
      doc["timeStr"] = data->timeStr;
      doc["phoneNumber"] = data->phoneNumber;
      doc["dtmfData"] = data->dtmfData;
      doc["callStatus"] = data->callStatus;
      doc["direction"] = data->direction;
      doc["mac"] = myMac;
      doc["user"] = deviceUser;
      doc["pass"] = devicePass;
      
      String jsonStr;
      serializeJson(doc, jsonStr);
      
      int httpCode = http.POST(jsonStr);
      if (httpCode == 200 || httpCode == 302) {
        webLog("[HTTP Task] Sync status code: " + String(httpCode));
        sheetsSyncCount++;
        sheetsLastSyncTime = data->timeStr;
      } else {
        webLog("[HTTP Task] Sync POST failure: " + (httpCode > 0 ? String(httpCode) : http.errorToString(httpCode)));
        sheetsSyncErrors++;
      }
      http.end();
    } else {
      webLog("[HTTP Task] Unable to resolve endpoint URL");
    }
  } else {
    webLog("[HTTP Task] Skip sync: Wi-Fi not connected or Script ID not set.");
  }
  
  delete data;
  vTaskDelete(NULL);
}

void triggerDirectSync(String timeStr, String phone, String dtmf, String status, String direction) {
  if (currentSystemState == SYSTEM_WIFI_CONFIG) {
    webLog("[HTTP Sync] Skipping: Device is in AP Setup mode");
    return;
  }
  
  SyncTaskData *data = new SyncTaskData();
  if (data != NULL) {
    data->timeStr = timeStr;
    data->phoneNumber = phone;
    data->dtmfData = dtmf;
    data->callStatus = status;
    data->direction = direction;
    
    // Allocate 16KB stack on core 1 to handle HTTPS handshake processes
    BaseType_t res = xTaskCreatePinnedToCore(syncTask, "g_sync_task", 16384, (void *)data, 1, NULL, 1);
    if (res != pdPASS) {
      webLog("[HTTP Sync] Failed to allocate thread task");
      delete data;
    }
  }
}

void addCallLog(String phone, String dtmf, String status) {
  unsigned long uptimeSecs = millis() / 1000;
  unsigned long hrs = uptimeSecs / 3600;
  unsigned long mins = (uptimeSecs % 3600) / 60;
  unsigned long secs = uptimeSecs % 60;
  
  char timeBuf[16];
  snprintf(timeBuf, sizeof(timeBuf), "%02duh %02dum %02dus", (int)hrs, (int)mins, (int)secs);
  
  callLogs[logInsertIndex].timeStr = String(timeBuf);
  callLogs[logInsertIndex].phoneNumber = phone.length() > 0 ? phone : "Unknown";
  callLogs[logInsertIndex].dtmfData = dtmf.length() > 0 ? dtmf : "No Input";
  callLogs[logInsertIndex].callStatus = status;
  
  String timeStr = callLogs[logInsertIndex].timeStr;
  String phoneNumber = callLogs[logInsertIndex].phoneNumber;
  String dtmfData = callLogs[logInsertIndex].dtmfData;
  String callStatus = callLogs[logInsertIndex].callStatus;
  
  logInsertIndex = (logInsertIndex + 1) % MAX_LOGS;
  if (logCount < MAX_LOGS) {
    logCount++;
  }
  
  if (syncIncoming && (status == "Missed" || status == "Answered" || status == "Rejected" || status == "Rejected/Unregistered" || status == "Timeout Hangup" || status == "Terminated")) {
    sendTelegramAlert("Call Log: " + phone + " - Status: " + status + " - DTMF: " + dtmfData);
  }
  if (syncOutgoing && (status == "Dialing Callback")) {
    sendTelegramAlert("Outgoing Call: " + phone + " - Status: " + status);
  }

  webLog("Local call logged for caller: " + phoneNumber + " [" + callStatus + "]");
  
  String direction = (callStatus == "Dialing Callback") ? "Outgoing" : "Incoming";
  triggerDirectSync(timeStr, phoneNumber, dtmfData, callStatus, direction);
}

// ==================== STATE MANAGEMENT ====================
void changeState(CallState newState) {
  CallState oldState = currentCallState;
  currentCallState = newState;
  webLog("[STATE CHANGE] New State: " + String((int)newState));

  switch(newState) {
    case STATE_IDLE:
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED_GREEN, LOW);
      mp3Stop();                     // Stop any playing track
      break;

    case STATE_VALIDATING_CALLER:
      // Red LED blink pattern while we query Google Sheets
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(LED_GREEN, LOW);
      break;

    case STATE_RINGING:
      digitalWrite(LED_PIN, HIGH);   // Red LED on — incoming call
      digitalWrite(LED_GREEN, LOW);
      beep(1000, 300);               // Short buzzer alert
      mp3PlayAsync(1, 2);            // /01/002.mp3 — ring tone
      break;

    case STATE_ACTIVE_CALL:
    case STATE_COLLECTING_DTMF:
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED_GREEN, HIGH); // Green LED on — call active
      beep(1500, 100);
      mp3PlayAsync(1, 3);            // /01/003.mp3 — "call answered" jingle
      break;

    case STATE_ENDED:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_PIN, HIGH);
      mp3PlayAsync(1, 4);            // /01/004.mp3 — "call ended" tone
      beep(800, 200);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      
      // Follow-up SMS check
      if (smsSystemActive && (oldState == STATE_ACTIVE_CALL || oldState == STATE_COLLECTING_DTMF) && dtmfBuffer.length() == 0) {
        String msg = "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in";
        sendSMS(callerNumber, msg);
        addSmsLog(callerNumber, msg, "Outgoing", "Sent");
      }
      
      changeState(STATE_IDLE);
      break;

    case STATE_CALLBACK_DIALING:
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED_GREEN, HIGH);
      beep(1200, 200);
      break;

    default:
      break;
  }
}

String formatNumberForDialing(String num) {
  num.trim();
  if (num.length() == 0) return "";
  if (num.startsWith("+")) return num;
  if (num.length() == 12 && num.startsWith("91")) return "+" + num;
  if (num.length() == 10) return "+91" + num;
  if (num.length() == 11 && num.startsWith("0")) return "+91" + num.substring(1);
  if (num.length() >= 10) return "+" + num;
  return num;
}

// ==================== AT COMMAND EXECUTION ====================
void sendAT(String command, unsigned long waitMs = 100) {
  webLog("GSM TX: " + command);
  Serial2.println(command);
  delay(waitMs);
}

bool sendATCommand(String cmd, String expectedResponse, unsigned long timeoutMs) {
  while(Serial2.available() > 0) { Serial2.read(); } // Clear buffer
  
  if (cmd.length() > 0) {
    webLog("GSM TX: " + cmd);
    Serial2.println(cmd);
  }
  
  unsigned long start = millis();
  String totalResponse = "";
  String currentLine = "";
  
  while (millis() - start < timeoutMs) {
    server.handleClient(); // Keep local webserver alive during delay
    
    while (Serial2.available() > 0) {
      char c = Serial2.read();
      totalResponse += c;
      
      if (c == '\n') {
        currentLine.trim();
        if (currentLine.length() > 0) {
          parseGsmResponse(currentLine);
        }
        currentLine = "";
      } else if (c != '\r') {
        currentLine += c;
      }
      
      if (totalResponse.indexOf(expectedResponse) != -1) {
        delay(50);
        while(Serial2.available() > 0) { 
          char nextC = (char)Serial2.read();
          totalResponse += nextC;
          if (nextC == '\n') {
            currentLine.trim();
            if (currentLine.length() > 0) parseGsmResponse(currentLine);
            currentLine = "";
          } else if (nextC != '\r') {
            currentLine += nextC;
          }
        }
        if (currentLine.length() > 0) {
           currentLine.trim();
           parseGsmResponse(currentLine);
        }
        return true;
      }
      
      if (totalResponse.indexOf("ERROR") != -1 || totalResponse.indexOf("+CMS ERROR") != -1) {
        return false;
      }
      }
    delay(10);
  }
  return false;
}

// ==================== AT RESPONSE PARSING ====================
void parseGsmResponse(String line) {
  line.trim();
  if (line.length() == 0) return;
  webLog("GSM RX: " + line);

  // If we are waiting for the SMS message body
  if (receivingSmsBody) {
    receivingSmsBody = false;
    String sender = incomingSmsSender;
    incomingSmsSender = "";
    webLog("Incoming SMS message content parsed: " + line);
    addSmsLog(sender, line, "Incoming", "Received");
    smsFlashCounter = 20; // 2 seconds of fast alternate blinking
    return;
  }

  // Incoming call parsed
  if (line.startsWith("+CLIP:") || line.startsWith("+CCWA:")) {
    int firstQuote = line.indexOf('"');
    int secondQuote = line.indexOf('"', firstQuote + 1);
    if (firstQuote != -1 && secondQuote != -1) {
      String tempNumber = line.substring(firstQuote + 1, secondQuote);
      tempNumber = formatNumberForDialing(tempNumber);
      webLog("Caller ID parsed: " + tempNumber);
      
      if (!callSystemActive) {
        webLog("Call System is disabled. Rejecting call from: " + tempNumber);
        Serial2.println("ATH");
        addCallLog(tempNumber, "", "Auto-Rejected (Call Control Off)");
        return;
      }
      
      if (currentCallState == STATE_IDLE) {
        dtmfBuffer = "";
        lastDtmfDigit = "";
        callerNumber = tempNumber;
        ringingStartMillis = millis();
        validationResult = -1;
        validatingNumber = tempNumber;
        changeState(STATE_VALIDATING_CALLER);
        ValidateTaskData* vData = new ValidateTaskData{tempNumber};
        xTaskCreatePinnedToCore(validateTask, "validate_task", 8192, (void*)vData, 1, NULL, 1);
      } else {
        webLog("Line busy. Validating secondary caller: " + tempNumber);
        validationResult = -1;
        validatingNumber = tempNumber;
        changeState(STATE_VALIDATING_SECOND_CALL);
        ValidateTaskData* vData = new ValidateTaskData{tempNumber};
        xTaskCreatePinnedToCore(validateTask, "validate_task", 8192, (void*)vData, 1, NULL, 1);
      }
    }
  }
  
  // DTMF Tone captured
  else if (line.startsWith("+DTMF:")) {
    int colon = line.indexOf(':');
    if (colon != -1) {
      String key = line.substring(colon + 1);
      key.trim();
      lastDtmfDigit = key;
      dtmfBuffer += key;
      lastDtmfMillis = millis();
      webLog("DTMF Tone: " + key + " | Buffer: " + dtmfBuffer);
      changeState(STATE_COLLECTING_DTMF);
    }
  }
  
  // Remote Hang Up
  else if (line.equals("NO CARRIER") || line.equals("BUSY") || line.equals("NO ANSWER") || line.equals("NO DIALTONE")) {
    webLog("Remote call terminated.");
    if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF) {
      addCallLog(callerNumber, dtmfBuffer, "Answered");
    } else if (currentCallState == STATE_RINGING) {
      addCallLog(callerNumber, "", "Missed");
    }
    changeState(STATE_ENDED);
  }
  
  // CSQ signal strength parser
  else if (line.startsWith("+CSQ:")) {
    gsmSignal = line.substring(5);
    gsmSignal.trim();
  }
  
  // Operator network parser
  else if (line.startsWith("+COPS:")) {
    int startQ = line.indexOf('"');
    int endQ = line.indexOf('"', startQ + 1);
    if (startQ != -1 && endQ != -1) {
      gsmOperator = line.substring(startQ + 1, endQ);
    } else {
      gsmOperator = "Searching...";
    }
  }
  
  // Battery status parser
  else if (line.startsWith("+CBC:")) {
    int comma = line.lastIndexOf(',');
    if (comma != -1) {
      float v = line.substring(comma + 1).toFloat() / 1000.0;
      gsmBattery = String(v, 2) + " V";
    }
  }
  
  // Registration status parser
  else if (line.startsWith("+CREG:")) {
    int comma = line.indexOf(',');
    String stat = "";
    if (comma != -1) {
      stat = line.substring(comma + 1);
    } else {
      stat = line.substring(6);
    }
    stat.trim();
    gsmRegStatus = stat;
    
    if ((gsmRegStatus == "1" || gsmRegStatus == "5") && !callWaitingActivated) {
      webLog("Tower registered. Enabling Call Waiting, Text Mode & SMS Routing.");
      Serial2.println("AT+CCWA=1,1,1");
      delay(100);
      Serial2.println("AT+CMGF=1");
      delay(100);
      Serial2.println("AT+CNMI=2,2,0,0,0");
      callWaitingActivated = true;
    }
  }
  
  // SIM status
  else if (line.startsWith("+CPIN:")) {
    gsmSimStatus = line.substring(6);
    gsmSimStatus.trim();
  }
  
  // Incoming SMS notification parsed
  else if (line.startsWith("+CMT:")) {
    int firstQuote = line.indexOf('"');
    int secondQuote = line.indexOf('"', firstQuote + 1);
    if (firstQuote != -1 && secondQuote != -1) {
      incomingSmsSender = line.substring(firstQuote + 1, secondQuote);
      incomingSmsSender = formatNumberForDialing(incomingSmsSender);
      receivingSmsBody = true;
      webLog("SMS CMT Header parsed. Sender: " + incomingSmsSender);
    }
  }
}

// ==================== AUTH HELPER ====================
bool isAuth() {
  lastActivityTime = millis();
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    if (cookie.indexOf("VARMA_SID_V4_SECURE=GSV_OS_TRUSTED") != -1) {
      return true;
    }
  }
  return false;
}

// ==================== GOOGLE SHEET HTTP PROXIES ====================
struct ProxyTaskData {
  String action;
  String response;
  int httpCode;
  volatile bool done;
};

static ProxyTaskData globalProxyData;
volatile bool proxyInProgress = false;

void proxyTask(void *pvParameters) {
  WiFiClientSecure client;
  client.setInsecure(); // Bypass SSL verification
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=" + globalProxyData.action + "&mac=" + myMac + "&user=" + deviceUser + "&pass=" + devicePass;
  
  webLog("[Proxy Task] Connecting to Google Sheets...");
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (http.begin(client, url)) {
    http.setTimeout(25000); // 25 seconds timeout
    int code = http.GET();
    globalProxyData.httpCode = code;
    webLog("[Proxy Task] HTTP Code: " + String(code));
    if (code == 200 || code == 302) {
      globalProxyData.response = http.getString();
    } else {
      globalProxyData.response = "[]";
    }
    http.end();
  } else {
    globalProxyData.httpCode = -1;
    globalProxyData.response = "[]";
    webLog("[Proxy Task] Connection failed.");
  }
  
  globalProxyData.done = true;
  vTaskDelete(NULL);
}

void proxyGoogleSheets(String action) {
  if (!isAuth()) {
    server.send(403, "application/json", "{\"error\":\"Unauthorized\"}");
    return;
  }
  if (scriptId == "") {
    server.send(200, "application/json", "[]");
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    webLog("[Proxy] WiFi disconnected. Cannot proxy Sheets request.");
    server.send(503, "application/json", "{\"error\":\"WiFi Disconnected\"}");
    return;
  }

  if (proxyInProgress) {
    webLog("[Proxy] Sheets proxy already in progress. Rejecting concurrent request.");
    server.send(503, "application/json", "{\"error\":\"Proxy Busy\"}");
    return;
  }
  proxyInProgress = true;

  // Setup global task data
  globalProxyData.action = action;
  globalProxyData.response = "";
  globalProxyData.httpCode = 0;
  globalProxyData.done = false;

  webLog("[Proxy] Spawning dedicated Sheets proxy task...");
  // Create task with 16KB stack size pinned to Core 1
  xTaskCreatePinnedToCore(proxyTask, "proxy_task", 16384, NULL, 1, NULL, 1);

  unsigned long start = millis();
  // Wait up to 30 seconds
  while (!globalProxyData.done && (millis() - start < 30000)) {
    delay(50);
  }

  if (globalProxyData.done) {
    server.send(200, "application/json", globalProxyData.response);
  } else {
    webLog("[Proxy] Timeout waiting for Google Sheets proxy task");
    server.send(504, "application/json", "{\"error\":\"Gateway Timeout\"}");
  }
  proxyInProgress = false;
}

void handleGetCalls() {
  // Always return local logs directly from ESP32 memory to avoid SSL handshake overload
  StaticJsonDocument<1536> doc;
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < logCount; i++) {
    int idx = (logInsertIndex - 1 - i + MAX_LOGS) % MAX_LOGS;
    JsonObject obj = arr.createNestedObject();
    obj["timeStr"] = callLogs[idx].timeStr;
    obj["phoneNumber"] = callLogs[idx].phoneNumber;
    obj["dtmfData"] = callLogs[idx].dtmfData;
    obj["callStatus"] = callLogs[idx].callStatus;
  }
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  server.send(200, "application/json", jsonResponse);
}

void handleGetMessages() {
  proxyGoogleSheets("get_messages");
}

void handleGetMobiles() {
  proxyGoogleSheets("get_mobiles");
}

void sendLargePage(const char* data) {
  size_t length = strlen(data);
  size_t chunk_size = 2048;
  size_t sent = 0;
  while (sent < length) {
    size_t to_send = length - sent;
    if (to_send > chunk_size) {
      to_send = chunk_size;
    }
    char buffer[2049];
    memcpy(buffer, data + sent, to_send);
    buffer[to_send] = '\0';
    server.sendContent(buffer);
    sent += to_send;
    yield();
  }
}

File uploadFile;
void handleFileUpload() {
  if (server.uri() != "/upload_audio") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    webLog("SD Upload Start: " + filename);
    uploadFile = SD.open(filename, FILE_WRITE);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      webLog("SD Upload End: " + upload.filename + " Size: " + String(upload.totalSize));
    }
  }
}

// ==================== LOCAL WEB SERVER ROUTES ====================
void setupWebServer() {
  if (serverStarted) return;

  const char *headerkeys[] = {"Cookie"};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
  server.collectHeaders(headerkeys, headerkeyssize);

  server.on("/", []() {
    if (currentSystemState == SYSTEM_WIFI_CONFIG) {
      String host = server.hostHeader();
      if (host.length() > 0 && host.indexOf("192.168.4.1") == -1 && host.indexOf("vvarmaivr.local") == -1) {
        server.sendHeader("Location", "http://192.168.4.1/", true);
        server.send(302, "text/plain", "");
        return;
      }
      
      bool authed = isAuth();
      if (authed) {
        server.send(200, "text/html", wifi_config_html);
      } else {
        // Serve login overlay
        server.setContentLength(CONTENT_LENGTH_UNKNOWN);
        server.send(200, "text/html", "");
        sendLargePage(index_html);
        server.sendContent(""); // End of chunked response
      }
    } else {
      server.sendHeader("Location", "/dashboard", true);
      server.send(302, "text/plain", "");
    }
  });

  // Redirects for captive portal redirection
  server.on("/generate_204", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/hotspot-detect.html", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/library/test/success.html", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/success.html", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/ncsi.txt", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/connecttest.txt", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  server.on("/wpad.dat", []() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });

  server.on("/dashboard", []() {
    if (currentSystemState == SYSTEM_WIFI_CONFIG) {
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "");
    } else {
      if (!isAuth()) {
        // Not authenticated — serve login page
        server.setContentLength(CONTENT_LENGTH_UNKNOWN);
        server.send(200, "text/html", "");
        sendLargePage(index_html);
        server.sendContent("");
        return;
      }
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "text/html", "");
      sendLargePage(index_html);
      server.sendContent(""); // End of chunked response
    }
  });

  server.on("/login", []() {
    String u = server.hasArg("u") ? server.arg("u") : server.arg("user");
    String p = server.hasArg("p") ? server.arg("p") : server.arg("pass");
    String r = server.arg("r");

    if (u == deviceUser && p == devicePass) {
      String cookie = "VARMA_SID_V4_SECURE=GSV_OS_TRUSTED; path=/";
      if (r == "1") {
        cookie += "; Max-Age=31536000"; // 1 Year session
      }
      server.sendHeader("Set-Cookie", cookie);
      server.send(200, "application/json", "{\"success\":true}");
    } else {
      server.send(200, "application/json", "{\"success\":false}");
    }
  });

  server.on("/status", []() {
    StaticJsonDocument<1024> doc;
    doc["mcu"] = true;
    doc["net"] = (gsmRegStatus == "1" || gsmRegStatus == "5");
    doc["db"] = (WiFi.status() == WL_CONNECTED && scriptId.length() > 0);
    doc["wifi"] = (WiFi.status() == WL_CONNECTED);
    doc["ssid"] = (WiFi.status() == WL_CONNECTED) ? WiFi.SSID() : "Not Connected";
    doc["rssi"] = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
    doc["ip"] = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
    doc["mac"] = myMac;
    doc["script"] = (scriptId.length() > 0);
    doc["scriptId"] = scriptId;
    doc["devUser"] = deviceUser;
    doc["devPass"] = devicePass;
    doc["mode"] = (currentSystemState == SYSTEM_WIFI_CONFIG) ? "AP" : "STA";
    doc["uptime"] = millis() / 1000;
    doc["syncCount"] = sheetsSyncCount;
    doc["syncErrors"] = sheetsSyncErrors;
    doc["lastSyncTime"] = sheetsLastSyncTime;
    doc["callSystemActive"] = callSystemActive;
    doc["smsSystemActive"] = smsSystemActive;
    
    // Call Status Variables
    switch(currentCallState) {
      case STATE_IDLE:             doc["state"] = "IDLE"; break;
      case STATE_RINGING:          doc["state"] = "RINGING"; break;
      case STATE_ACTIVE_CALL:      doc["state"] = "ACTIVE CALL"; break;
      case STATE_COLLECTING_DTMF:  doc["state"] = "COLLECTING DTMF"; break;
      case STATE_ENDED:            doc["state"] = "ENDED"; break;
      case STATE_CALLBACK_DIALING: doc["state"] = "CALLBACK DIALING"; break;
    }
    doc["callerNumber"] = callerNumber;
    doc["dtmfBuffer"] = dtmfBuffer;
    doc["lastDtmfDigit"] = lastDtmfDigit;
    doc["queueSize"] = callbackQueue.size();
    if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF) {
      doc["callDuration"] = (millis() - callStartMillis) / 1000;
    } else {
      doc["callDuration"] = 0;
    }

    // GSM Status sub-object
    JsonObject gsmObj = doc.createNestedObject("gsm");
    gsmObj["sim"] = gsmSimStatus;
    gsmObj["registration"] = gsmRegStatus;
    gsmObj["operator"] = gsmOperator;
    gsmObj["signal"] = gsmSignal;
    gsmObj["battery"] = gsmBattery;
    gsmObj["imei"] = "358294058291048"; // Placeholder until AT command is added
    gsmObj["network"] = (gsmRegStatus == "1" || gsmRegStatus == "5") ? "4G LTE" : "Unknown";
    gsmObj["simNumber"] = "+919876543210"; // Placeholder

    // AP Clients
    doc["apClients"] = WiFi.softAPgetStationNum();


    // System Hardware Info
    JsonObject sysObj = doc.createNestedObject("system");
    sysObj["flashSize"] = String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB";
    sysObj["freeHeap"] = ESP.getFreeHeap();
    sysObj["chipRevision"] = "v" + String(ESP.getChipRevision());
    sysObj["sdkVersion"] = ESP.getSdkVersion();
    sysObj["firmware"] = FIRMWARE_VERSION;
    sysObj["macSta"] = WiFi.macAddress();
    sysObj["macAp"] = WiFi.softAPmacAddress();
    sysObj["cpuFreq"] = ESP.getCpuFreqMHz();
    
    // SD Card
    JsonObject sdObj = doc.createNestedObject("sd");
    if (SD.cardType() != CARD_NONE) {
      sdObj["mounted"] = true;
      sdObj["totalBytes"] = SD.totalBytes();
      sdObj["usedBytes"] = SD.usedBytes();
    } else {
      sdObj["mounted"] = false;
    }
    
    sysObj["temperature"] = temperatureRead();
    sysObj["socModel"] = ESP.getChipModel();
    sysObj["coreFreq"] = String(ESP.getCpuFreqMHz()) + " MHz";
    sysObj["minHeap"] = ESP.getMinFreeHeap();
    sysObj["rtosTasks"] = uxTaskGetNumberOfTasks();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/serial_data", []() {
    server.send(200, "text/plain", webSerialLog);
    webSerialLog = ""; // Clear log buffer on query
  });

  server.on("/answer", []() {
    if (currentCallState == STATE_RINGING) {
      webLog("ATA command issued via local Web server.");
      Serial2.println("ATA");
      delay(500);
      Serial2.println("AT+CLIP=1");
      delay(100);
      Serial2.println("AT+DDET=1"); // Ensure DTMF detector is enabled
      callStartMillis = millis();
      lastDtmfMillis = millis();
      changeState(STATE_ACTIVE_CALL);
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Lines not ringing\"}");
    }
  });

  server.on("/hangup", []() {
    webLog("ATH hang up command issued via local Web server.");
    Serial2.println("ATH");
    if (currentCallState == STATE_RINGING) {
      addCallLog(callerNumber, "", "Rejected");
    } else if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF) {
      addCallLog(callerNumber, dtmfBuffer, "Terminated");
    }
    changeState(STATE_ENDED);
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });

  server.on("/send_at", []() {
    if (server.hasArg("cmd")) {
      String cmd = server.arg("cmd");
      webLog("Web AT: " + cmd);
      
      while(Serial2.available() > 0) { Serial2.read(); }
      Serial2.println(cmd);
      
      unsigned long start = millis();
      String response = "";
      while (millis() - start < 1500) {
        while (Serial2.available() > 0) {
          response += (char)Serial2.read();
        }
        if (response.indexOf("OK") != -1 || response.indexOf("ERROR") != -1 || response.indexOf(">") != -1) {
          delay(50);
          while (Serial2.available() > 0) { response += (char)Serial2.read(); }
          break;
        }
        delay(10);
      }
      response.trim();
      
      StaticJsonDocument<512> doc;
      doc["status"] = "success";
      doc["response"] = response;
      String jsonResponse;
      serializeJson(doc, jsonResponse);
      server.send(200, "application/json", jsonResponse);
    } else {
      server.send(400, "application/json", "{\"status\":\"error\"}");
    }
  });

  server.on("/get_calls", handleGetCalls);
  server.on("/get_messages", handleGetMessages);
  server.on("/get_mobiles", handleGetMobiles);
  server.on("/get_varma_data", []() {
    proxyGoogleSheets("getVarmaData");
  });
  server.on("/init_varma_sheets", []() {
    proxyGoogleSheets("initVarmaSheets");
  });

  // ==================== NEW API ENDPOINTS ====================
  server.on("/api/login", HTTP_POST, []() {
    String pUser = server.arg("user");
    String pPass = server.arg("pass");
    if (pUser == deviceUser && pPass == devicePass) {
      server.send(200, "application/json", "{\"success\":true,\"token\":\"AUTH_VALID\"}");
    } else {
      server.send(401, "application/json", "{\"success\":false}");
    }
  });

  server.on("/api/telegram-config", HTTP_POST, []() {
    tgToken = server.arg("token");
    tgChatId = server.arg("chatId");
    preferences.begin("wifi-config", false);
    preferences.putString("tgToken", tgToken);
    preferences.putString("tgChatId", tgChatId);
    preferences.end();
    server.send(200, "application/json", "{\"success\":true}");
  });

  server.on("/api/sync-config", HTTP_POST, []() {
    syncIncoming = server.arg("syncIncoming") == "true";
    syncOutgoing = server.arg("syncOutgoing") == "true";
    syncSms = server.arg("syncSms") == "true";
    syncSystem = server.arg("syncSystem") == "true";
    syncNetwork = server.arg("syncNetwork") == "true";
    preferences.begin("wifi-config", false);
    preferences.putBool("syncInc", syncIncoming);
    preferences.putBool("syncOut", syncOutgoing);
    preferences.putBool("syncSms", syncSms);
    preferences.putBool("syncSys", syncSystem);
    preferences.putBool("syncNet", syncNetwork);
    preferences.end();
    server.send(200, "application/json", "{\"success\":true}");
  });

  server.on("/api/gsm-settings", HTTP_POST, []() {
    if (server.hasArg("callSystemActive")) {
      callSystemActive = server.arg("callSystemActive") == "true";
    }
    if (server.hasArg("smsSystemActive")) {
      smsSystemActive = server.arg("smsSystemActive") == "true";
    }
    preferences.begin("wifi-config", false);
    preferences.putBool("callSysAct", callSystemActive);
    preferences.putBool("smsSysAct", smsSystemActive);
    preferences.end();
    
    webLog("GSM Settings updated: Call=" + String(callSystemActive ? "ON" : "OFF") + ", SMS=" + String(smsSystemActive ? "ON" : "OFF"));
    server.send(200, "application/json", "{\"success\":true}");
  });

  server.on("/api/get-config", HTTP_GET, []() {
    String json = "{";
    json += "\"tgToken\":\"" + tgToken + "\",";
    json += "\"tgChatId\":\"" + tgChatId + "\",";
    json += "\"syncIncoming\":" + String(syncIncoming ? "true" : "false") + ",";
    json += "\"syncOutgoing\":" + String(syncOutgoing ? "true" : "false") + ",";
    json += "\"syncSms\":" + String(syncSms ? "true" : "false") + ",";
    json += "\"syncSystem\":" + String(syncSystem ? "true" : "false") + ",";
    json += "\"syncNetwork\":" + String(syncNetwork ? "true" : "false") + ",";
    json += "\"callSystemActive\":" + String(callSystemActive ? "true" : "false") + ",";
    json += "\"smsSystemActive\":" + String(smsSystemActive ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/logs", []() {
    if (server.hasArg("clear") && server.arg("clear") == "true") {
      logCount = 0;
      logInsertIndex = 0;
      server.send(200, "application/json", "{\"status\":\"success\"}");
      return;
    }
    
    StaticJsonDocument<1536> doc;
    JsonArray arr = doc.to<JsonArray>();
    for (int i = 0; i < logCount; i++) {
      int idx = (logInsertIndex - 1 - i + MAX_LOGS) % MAX_LOGS;
      JsonObject obj = arr.createNestedObject();
      obj["timeStr"] = callLogs[idx].timeStr;
      obj["phoneNumber"] = callLogs[idx].phoneNumber;
      obj["dtmfData"] = callLogs[idx].dtmfData;
      obj["callStatus"] = callLogs[idx].callStatus;
    }
    String jsonResponse;
    serializeJson(doc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/save_net", HTTP_POST, []() {
    if (server.hasArg("ssid")) {
      String ssid = server.arg("ssid");
      String pass = server.arg("pass");
      
      webLog("NVS Config: Saving router credentials SSID=" + ssid);
      preferences.begin("wifi-config", false);
      preferences.putString("ssid", ssid);
      preferences.putString("pass", pass);
      preferences.end();
      
      server.send(200, "application/json", "{\"success\":true}");
      delay(2000);
      ESP.restart();
    } else {
      server.send(400, "application/json", "{\"success\":false}");
    }
  });

  server.on("/save_sys", []() {
    String script = server.arg("script");
    String adminU = server.arg("adminUser");
    String adminP = server.arg("adminPass");
    
    webLog("NVS Config: Saving Google Script ID & admin credentials");
    preferences.begin("wifi-config", false);
    if (server.hasArg("script")) preferences.putString("scriptId", script);
    if (server.hasArg("adminUser")) preferences.putString("adminUser", adminU);
    if (server.hasArg("adminPass")) preferences.putString("adminPass", adminP);
    preferences.end();
    
    server.send(200, "application/json", "{\"success\":true}");
    delay(2000);
    ESP.restart();
  });

  server.on("/scan_wifi", []() {
    int n = WiFi.scanNetworks();
    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.to<JsonArray>();
    for (int i = 0; i < n; ++i) {
      bool duplicate = false;
      for (int j = 0; j < i; ++j) {
        if (WiFi.SSID(i) == WiFi.SSID(j)) {
          duplicate = true;
          break;
        }
      }
      if (!duplicate && WiFi.SSID(i).length() > 0) {
        JsonObject obj = arr.createNestedObject();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
      }
    }
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/factory_reset", []() {
    server.send(200, "application/json", "{\"success\":true}");
    delay(1000);
    factoryReset();
  });

  // ==================== SD CARD ENDPOINTS ====================
  server.on("/sd_status", []() {
    StaticJsonDocument<256> doc;
    doc["found"] = sdCardFound;
    doc["total"] = sdCardTotalBytes;
    doc["used"] = sdCardUsedBytes;
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/sd_files", []() {
    if (!sdCardFound) {
      server.send(200, "application/json", "[]");
      return;
    }
    File root = SD.open("/");
    if (!root) {
      server.send(200, "application/json", "[]");
      return;
    }
    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.to<JsonArray>();
    File file = root.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        String fname = String(file.name());
        if (fname.endsWith(".amr") || fname.endsWith(".wav")) {
          JsonObject obj = arr.createNestedObject();
          obj["name"] = fname;
          obj["size"] = file.size();
        }
      }
      file = root.openNextFile();
    }
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/upload_audio", HTTP_POST, []() {
    server.send(200, "application/json", "{\"success\":true}");
  }, handleFileUpload);

  server.on("/delete_audio", HTTP_POST, []() {
    if (server.hasArg("file")) {
      String f = server.arg("file");
      if (!f.startsWith("/")) f = "/" + f;
      if (SD.exists(f)) {
        SD.remove(f);
        server.send(200, "application/json", "{\"success\":true}");
      } else {
        server.send(404, "application/json", "{\"success\":false,\"error\":\"File not found\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false}");
    }
  });

  server.on("/save_ivr_config", HTTP_POST, []() {
    if (server.hasArg("config")) {
      String conf = server.arg("config");
      File f = SD.open("/ivr_menu.json", FILE_WRITE);
      if (f) {
        f.print(conf);
        f.close();
        server.send(200, "application/json", "{\"success\":true}");
      } else {
        server.send(500, "application/json", "{\"success\":false,\"error\":\"Failed to write\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false}");
    }
  });

  server.on("/get_ivr_config", []() {
    if (SD.exists("/ivr_menu.json")) {
      File f = SD.open("/ivr_menu.json", FILE_READ);
      if (f) {
        server.setContentLength(f.size());
        server.send(200, "application/json", "");
        while(f.available()) {
          server.sendContent(f.readStringUntil('\n') + '\n');
        }
        f.close();
        return;
      }
    }
    server.send(200, "application/json", "{}");
  });

  // ==================== OTA UPDATE ROUTES ====================
  // [OTA] GET /ota_status — Poll-friendly; no auth needed for progress polling.
  server.on("/ota_status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["status"]     = otaStatus;
    doc["inProgress"] = otaInProgress;
    doc["urlSet"]     = (otaFirmwareUrl.length() > 0); // [OTA] true if URL has been saved
    doc["url"]        = otaFirmwareUrl;                // [OTA] Return URL so dashboard can show it
    String resp;
    serializeJson(doc, resp);
    server.send(200, "application/json", resp);
  });

  // [OTA] GET /ota_get_url — Returns the saved firmware URL (requires auth).
  server.on("/ota_get_url", HTTP_GET, []() {
    if (!isAuth()) { server.send(403, "application/json", "{\"error\":\"Unauthorized\"}"); return; }
    StaticJsonDocument<512> doc;
    doc["url"] = otaFirmwareUrl;
    String resp;
    serializeJson(doc, resp);
    server.send(200, "application/json", resp);
  });

  // [OTA] POST /ota_save_url — Saves firmware URL to NVS (persists across reboots).
  //        Body param: url=https://github.com/...
  server.on("/ota_save_url", HTTP_POST, []() {
    if (!isAuth()) { server.send(403, "application/json", "{\"error\":\"Unauthorized\"}"); return; }
    if (!server.hasArg("url")) {
      server.send(400, "application/json", "{\"error\":\"Missing url parameter\"}");
      return;
    }
    String newUrl = server.arg("url");
    newUrl.trim();
    otaFirmwareUrl = newUrl;                          // [OTA] Update runtime variable
    preferences.begin("wifi-config", false);
    preferences.putString("otaUrl", otaFirmwareUrl);  // [OTA] Persist to NVS
    preferences.end();
    webLog("[OTA] Firmware URL saved: " + otaFirmwareUrl);
    server.send(200, "application/json", "{\"success\":true}");
  });

  // [OTA] POST /ota_trigger — Downloads firmware and flashes. Requires auth.
  server.on("/ota_trigger", HTTP_POST, []() {
    if (!isAuth()) {
      server.send(403, "application/json", "{\"error\":\"Unauthorized\"}");
      return;
    }
    if (otaFirmwareUrl.length() == 0) {
      server.send(400, "application/json",
        "{\"error\":\"No firmware URL saved. Set it in OTA Settings first.\"}");
      return;
    }
    if (otaInProgress) {
      server.send(409, "application/json", "{\"error\":\"OTA already in progress\"}");
      return;
    }
    triggerOtaUpdate();
    server.send(200, "application/json",
      "{\"success\":true,\"message\":\"OTA update started. Poll /ota_status for progress.\"}");
  });
  // ==================== END OTA UPDATE ROUTES ====================

  server.onNotFound([]() {
    if (currentSystemState == SYSTEM_WIFI_CONFIG) {
      server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/", true);
      server.send(302, "text/plain", "");
    } else {
      server.send(404, "text/plain", "Route Not Found");
    }
  });

  server.begin();
  serverStarted = true;
}

// GSM Initialization Helper
void initializeGSM() {
  webLog("Initializing GSM Module AT commands...");
  Serial2.println("AT");
  delay(100);
  Serial2.println("AT+CLIP=1"); // Enable caller ID presentation
  delay(100);
  Serial2.println("AT+DDET=1"); // Enable DTMF detector
  delay(100);
  Serial2.println("AT+CMGF=1"); // Set SMS to text mode
  delay(100);
  Serial2.println("AT+CNMI=2,2,0,0,0"); // Direct SMS routing to terminal
  delay(100);
}

// Non-blocking LED Indicators Update Loop
void updateLedIndicators() {
  unsigned long now = millis();
  if (now - lastLedUpdate < 100) return; // run every 100ms
  lastLedUpdate = now;
  ledBlinkCounter++;

  // 1. SMS flash active (rapid alternate blinking)
  if (smsFlashCounter > 0) {
    smsFlashCounter--;
    if (smsFlashCounter % 2 == 0) {
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_PIN, LOW);
    } else {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_PIN, HIGH);
    }
    return;
  }

  // 2. Active call states (Green LED solid, Red LED off)
  if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF || currentCallState == STATE_CALLBACK_DIALING) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_PIN, LOW);
    return;
  }

  // 3. Ringing or Validating states (Red LED blinks rapidly, Green LED off)
  if (currentCallState == STATE_RINGING || currentCallState == STATE_VALIDATING_CALLER || currentCallState == STATE_VALIDATING_SECOND_CALL) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PIN, (ledBlinkCounter % 2 == 0) ? HIGH : LOW);
    return;
  }

  // 4. Idle state status indicators
  bool gsmRegistered = (gsmRegStatus == "1" || gsmRegStatus == "5");
  bool gsmSimOk = (gsmSimStatus == "READY" || gsmSimStatus.indexOf("READY") != -1);
  int signalVal = gsmSignal.toInt();
  bool gsmSignalOk = (signalVal > 0 && signalVal != 99);

  if (gsmSimOk && gsmRegistered && gsmSignalOk) {
    // SIM signal ok, received tower good -> glow green LED (solid HIGH)
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_PIN, LOW);
  } else {
    // Cellular Error / No signal / Unregistered -> fast blink red LED (100ms cycle)
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PIN, (ledBlinkCounter % 2 == 0) ? HIGH : LOW);
  }
}

// ==================== SYSTEM MAIN ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_GREEN, LOW);
  
  playBootSound();
  
  // Fetch local MAC Address ID
  myMac = WiFi.macAddress();
  myMac.replace(":", "");
  
  webLog("-----------------------------------------");
  webLog("GSV Gatekeeper OS: v1.1-IVR System Booting");
  webLog("MAC address identity: " + myMac);

  // Initialize SD Card
  if (!SD.begin(5)) {
    webLog("SD Card Mount Failed. SD features will be disabled.");
    sdCardFound = false;
  } else {
    webLog("SD Card Mount Successful!");
    sdCardFound = true;
    sdCardTotalBytes = SD.totalBytes();
    sdCardUsedBytes = SD.usedBytes();
  }

  // Initialize GSM Serial2 Communication (GPIO16=RX, GPIO17=TX)
  Serial2.begin(GSM_BAUD, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  initializeGSM();

  // Initialize MP3-TF-16P player (GPIO27=RX, GPIO26=TX)
  mp3Init();

  // Read saved configurations
  preferences.begin("wifi-config", false);
  savedSSID = preferences.getString("ssid", "");
  savedPass = preferences.getString("pass", "");
  scriptId  = preferences.getString("scriptId", "1JY8HDpGbMYZ0Dw5lWRpXgKj4aeZBZeH7HSRyADqwr6uAc9lyT3hXFBwr");
  deviceUser = preferences.getString("adminUser", "admin");
  devicePass = preferences.getString("adminPass", "GSVIVR001");
  
  tgToken = preferences.getString("tgToken", "");
  tgChatId = preferences.getString("tgChatId", "");
  syncIncoming = preferences.getBool("syncInc", true);
  syncOutgoing = preferences.getBool("syncOut", true);
  syncSms = preferences.getBool("syncSms", true);
  syncSystem = preferences.getBool("syncSys", false);
  syncNetwork = preferences.getBool("syncNet", false);
  
  callSystemActive = preferences.getBool("callSysAct", true);
  smsSystemActive = preferences.getBool("smsSysAct", true);
  otaFirmwareUrl  = preferences.getString("otaUrl", ""); // [OTA] Load saved firmware URL from NVS
  
  preferences.end();

  if (savedSSID.length() > 0) {
    webLog("Attempting connection to local router: " + savedSSID);
    
    // Stop any AP first, then switch to pure STA mode
    WiFi.softAPdisconnect(true);
    delay(200);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(DEVICE_NAME);
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    
    unsigned long startConnect = millis();
    bool connected = false;
    while (millis() - startConnect < 25000) { // 25 seconds Wi-Fi connection timeout
      delay(400);
      digitalWrite(LED_GREEN, !digitalRead(LED_GREEN)); // blink green led
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        break;
      }
    }
    digitalWrite(LED_GREEN, LOW);
    
    if (connected) {
      // AP was already stopped above — do NOT call softAPdisconnect(true) again,
      // it would also sever the STA connection on some ESP-IDF builds.
      localIpStr = WiFi.localIP().toString();
      webLog("WiFi STA connected. IP Address: " + localIpStr);
      currentSystemState = SYSTEM_RUNNING;
      playSuccessSound();
      
      setupWebServer();
      mDNS_begin();
    } else {
      webLog("WiFi connection timeout. Entering captive portal setup...");
      playErrorSound();
      startAPMode();
    }
  } else {
    webLog("No router SSID configured in NVS. Starting Access Point setup...");
    startAPMode();
  }

  gsmQueryTimer = millis();
}

void loop() {
  updateLedIndicators();
  // 0. Physical Factory Reset Check
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    if (!resetBtnPressed) {
      resetBtnPressed = true;
      resetBtnStartTime = millis();
    } else if (millis() - resetBtnStartTime > 3000) {
      webLog("Physical reset button held for 3 seconds.");
      factoryReset();
    }
  } else {
    resetBtnPressed = false;
  }

  // 1. Maintain DNS captives in AP config mode
  if (currentSystemState == SYSTEM_WIFI_CONFIG) {
    dnsServer.processNextRequest();
  }

  // 2. Local client handlers
  server.handleClient();

  // 3. Keep mDNS alive
  #if defined(ESP32)
  // mDNS handles loop inside ESP-IDF core
  #endif

  // 4. SIM800L Non-blocking GSM Query Scheduler
  // Run checks sequentially every 15 seconds to avoid flooding GSM module
  if (millis() - gsmQueryTimer > 15000) {
    gsmQueryTimer = millis();
    
    switch(gsmQueryStep) {
      case 0:
        sendATCommand("AT+CPIN?", "+CPIN:", 1000);
        gsmQueryStep++;
        break;
      case 1:
        sendATCommand("AT+CREG?", "+CREG:", 1000);
        gsmQueryStep++;
        break;
      case 2:
        sendATCommand("AT+COPS?", "+COPS:", 1000);
        gsmQueryStep++;
        break;
      case 3:
        sendATCommand("AT+CSQ", "+CSQ:", 1000);
        gsmQueryStep++;
        break;
      case 4:
        sendATCommand("AT+CBC", "+CBC:", 1000);
        gsmQueryStep = 0; // Reset sequence step
        break;
      default:
        gsmQueryStep = 0;
        break;
    }
  }

  // 5. Read incoming serial characters from SIM800L (Real-time URC checks)
  static String gsmSerialBuffer = "";
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    if (c == '\n') {
      gsmSerialBuffer.trim();
      if (gsmSerialBuffer.length() > 0) {
        parseGsmResponse(gsmSerialBuffer);
      }
      gsmSerialBuffer = "";
    } else if (c != '\r') {
      gsmSerialBuffer += c;
    }
    delay(1);
  }

  // 5.5 Validate Caller ID Result Checker
  if (currentCallState == STATE_VALIDATING_CALLER) {
    if (validationResult == 1) {
      webLog("Caller Validated. Answering call.");
      Serial2.println("ATA");
      callStartMillis = millis();
      lastDtmfMillis = millis();
      changeState(STATE_ACTIVE_CALL);
      validationResult = -1;
    } else if (validationResult == 0) {
      webLog("Caller Invalid/Unregistered. Rejecting call.");
      Serial2.println("ATH");
      addCallLog(callerNumber, "", "Rejected/Unregistered");
      changeState(STATE_ENDED);
      validationResult = -1;
    }
  }

  if (currentCallState == STATE_VALIDATING_SECOND_CALL) {
    if (validationResult == 1) {
      webLog("Second Caller Validated. Dropping Call 1 & Answering Call 2.");
      Serial2.println("AT+CHLD=1"); // Release active calls and accept waiting
      callerNumber = validatingNumber;
      callStartMillis = millis();
      lastDtmfMillis = millis();
      changeState(STATE_ACTIVE_CALL);
      validationResult = -1;
    } else if (validationResult == 0) {
      webLog("Second Caller Invalid. Rejecting waiting call.");
      Serial2.println("AT+CHLD=0"); // Reject waiting call
      changeState(STATE_ACTIVE_CALL); // Return to active call state
      validationResult = -1;
    }
  }

  // 6. Ringing timeout checker
  if (currentCallState == STATE_RINGING && (millis() - ringingStartMillis > 40000)) {
    webLog("Incoming call ring timeout.");
    addCallLog(callerNumber, "", "Missed");
    changeState(STATE_ENDED);
  }

  // 7. Active call timeout (security hangup after 5 minutes)
  if ((currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF) && (millis() - callStartMillis > 300000)) {
    webLog("Active call limit exceeded. Auto hanging up.");
    Serial2.println("ATH");
    addCallLog(callerNumber, dtmfBuffer, "Timeout Hangup");
    changeState(STATE_ENDED);
  }

  // 8. Callback Dialing Queue Scheduler
  if (currentCallState == STATE_IDLE && !callbackQueue.isEmpty()) {
    if (!callSystemActive) {
      String dialNumber = callbackQueue.dequeue();
      webLog("Call System is disabled. Skipping callback dial to: " + dialNumber);
      return;
    }
    String dialNumber = callbackQueue.peek();
    webLog("Dialing queued callback recipient: " + dialNumber);
    changeState(STATE_CALLBACK_DIALING);
    
    // Clear buffer, send call wait command
    while(Serial2.available() > 0) { Serial2.read(); }
    
    // Dial number
    Serial2.println("ATD" + dialNumber + ";");
    
    // Dequeue call log record, record dials
    callbackQueue.dequeue();
    addCallLog(dialNumber, "CALLBACK", "Dialing Callback");
    
    // Mark loop dialer
    callStartMillis = millis();
  }

  // Handle callback dial timeout (Auto hangup if no answer in 40s)
  if (currentCallState == STATE_CALLBACK_DIALING && (millis() - callStartMillis > 40000)) {
    webLog("Callback dialing ring timeout.");
    Serial2.println("ATH");
    changeState(STATE_ENDED);
  }

  delay(10);
}
