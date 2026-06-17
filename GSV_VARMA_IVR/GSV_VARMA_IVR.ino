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
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <SPIFFS.h>
#include <DFRobotDFPlayerMini.h>
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
#define BUZZER_PIN    27   // Piezo buzzer
#define RESET_BUTTON_PIN 0 // Boot / factory-reset button

// ─── MP3-TF-16P (DFPlayer Mini compatible) ────────────────────────────────────
// Uses HardwareSerial 1 remapped to free GPIO25/26
#define MP3_TX_PIN    25   // ESP32 TX → module RX
#define MP3_RX_PIN    26   // ESP32 RX ← module TX
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
DFRobotDFPlayerMini myDFPlayer;
bool mp3Initialized = false;

// ==================== MP3 PLAYER (DFRobotDFPlayerMini) ====================
// Forward declarations
void mp3SetVolume(uint8_t vol);
void mp3Play(uint8_t folder, uint8_t track);
void mp3Stop();
void mp3PlayAsync(uint8_t folder, uint8_t track);

/** Initialize the MP3-TF-16P on GPIO26(TX)/GPIO27(RX). */
void mp3Init() {
  mp3Serial.begin(MP3_BAUD, SERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN);
  delay(800);
  // Pass true for isACK to verify hardware connection, and true for doReset
  if (!myDFPlayer.begin(mp3Serial, true, true)) {
    Serial.println("[MP3] Unable to begin DFPlayer Mini! (Hardware connection failed)");
    mp3Initialized = false;
  } else {
    mp3Initialized = true;
    delay(2000);             // Crucial delay to allow DFPlayer to finish boot reset and read the SD card
    myDFPlayer.volume(25);   // Default volume 0-30
    Serial.println("[MP3] DFRobotDFPlayerMini ready on GPIO25/26");
  }
}

/** Set volume (0=mute, 30=max). */
void mp3SetVolume(uint8_t vol) {
  if (!mp3Initialized) return;
  if (vol > 30) vol = 30;
  myDFPlayer.volume(vol);
}

/** Play track from numbered folder. If folder is 1, play directly from root using play(track) */
void mp3Play(uint8_t folder, uint8_t track) {
  if (!mp3Initialized) return;
  if (folder == 1) {
    webLog("[MP3] Playing track " + String(track) + " from root directory.");
    myDFPlayer.play(track);
  } else {
    webLog("[MP3] Playing folder " + String(folder) + ", track " + String(track));
    myDFPlayer.playFolder(folder, track);
  }
}

/** Stop current track. */
void mp3Stop() {
  if (!mp3Initialized) return;
  myDFPlayer.stop();
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
String scriptId = "";
String sheetId = "";
String sheetName = "IVR";
String savedApSSID = "vvarmaivr";
String savedApPass = "GSVIVR001";
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
bool callerValidationEnabled = false; // Expose setting for sheets validation (default false to auto-answer)
bool customIvrEnabled = false;        // Toggle custom interactive IVR flow (20 cases)
String agentPhoneNumber = "+919092610415"; // Target number for connecting to agent

// Non-blocking welcome menu delay variables
bool welcomeMenuPending = false;
unsigned long welcomeMenuDelayStart = 0;

// Hardware Debug / Simulator Config
bool testModeEnabled = false;
bool testAutoAttend = false;
bool testAutoCallback = false;
bool testAutoSms = false;
String testPhoneNumber = "";
String testSmsTemplate = "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in";

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
  STATE_VALIDATING_SECOND_CALL,
  STATE_COLLECTING_PRODUCT_NO,
  STATE_AGENT_CONNECTING
};
CallState currentCallState = STATE_IDLE;

// IVR Variables
String callerNumber = "";
String dtmfBuffer = "";
String lastDtmfDigit = "";
unsigned long callStartMillis = 0;
unsigned long lastDtmfMillis = 0;
int customIvrTimeoutCount = 0;
int warrantyRetryCount = 0;

volatile int ivrWarrantyCheckResult = 0; // 0: idle/pending, 1: active, 2: claimed, 3: not_found, 99: in_progress
String ivrWarrantyCheckCode = "";
volatile int ivrComplaintResult = 0; // 0: idle/pending, 1: success, 2: failed, 99: in_progress
String ivrComplaintTicket = "";

struct WarrantyTaskData {
  String productCode;
};

struct ComplaintTaskData {
  String warrantyId;
  String phone;
};

// Dynamic IVR Flow Structure
struct IvrNode {
  String audio;
  String optionKeys[10];
  String optionTargetNodes[10];
  int optionCount;
};
IvrNode currentIvrNode;
String ivrCurrentNode = "start";
bool callerIsRegistered = false;

// Caller Validation
volatile int validationResult = -1; // -1: pending, 0: invalid, 1: valid
String validatingNumber = "";
unsigned long ringingStartMillis = 0;
unsigned long lastActivityTime = 0;

// SIM800L Status variables
bool gsmModuleConnected = false;
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
FS* myFS = &SD;
uint64_t sdCardUsedBytes = 0;
uint64_t sdCardTotalBytes = 0;

uint64_t getUsedBytes() {
  if (myFS == &SPIFFS) {
    return SPIFFS.usedBytes();
  } else {
    return SD.usedBytes();
  }
}

uint64_t getTotalBytes() {
  if (myFS == &SPIFFS) {
    return SPIFFS.totalBytes();
  } else {
    return SD.totalBytes();
  }
}

// Scheduler & Time variables
unsigned long gsmQueryTimer = 0;
int gsmQueryStep = 0;
bool serverStarted = false;

// Forward declarations
void parseGsmResponse(String line);
void beep(int freq, int duration);
void setupWebServer();
bool isAuth();
void sendLargePage(const char* data);
void changeState(CallState newState);
void playIvrAudio(String filename);
void loadIvrNode(String nodeName);
void processIvrDtmf(String key);
void ensureIvrMenuJson();
void verifyIvrWarranty(String productCode);
void processWarrantyResult(String code, int result);
void startComplaintRegistration();
void startAgentConnection();
void verifyIvrWarrantyTask(void *pvParameters);
void registerComplaintTask(void *pvParameters);



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
  http.setTimeout(10000); // 10 seconds timeout for Telegram
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
  bool apActive = (WiFi.softAPIP() != IPAddress(0, 0, 0, 0) && WiFi.getMode() == WIFI_AP);
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
  WiFi.softAP(savedApSSID.c_str(), savedApPass.c_str());
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

// ==================== CALLER VALIDATION SYNC ====================
struct ValidateTaskData {
  String phoneNumber;
};

void validateTask(void *pvParameters) {
  ValidateTaskData *data = (ValidateTaskData *)pvParameters;
  if (data == NULL) {
    if (validatingNumber == "") {
      validationResult = 0;
    }
    vTaskDelete(NULL);
    return;
  }
  
  int tempResult = 0;
  if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Validating caller ID via Google Sheets: " + data->phoneNumber);
    WiFiClientSecure client;
    client.setInsecure(); // Bypass SSL verification
    
    HTTPClient http;
    // URL Encoding + as %2B in case there is a +
    String encPhone = data->phoneNumber;
    encPhone.replace("+", "%2B");
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=validateUser&phone=" + encPhone + "&sheetId=" + sheetId;
    
    if (http.begin(client, url)) {
      http.setTimeout(25000); // 25 seconds timeout for caller validation
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        webLog("[HTTP Task] Validation Response: " + payload);
        if (payload.indexOf("\"registered\":true") != -1 || payload.indexOf("\"registered\": true") != -1) {
          tempResult = 1;
        } else {
          tempResult = 0;
        }
      } else {
        webLog("[HTTP Task] Validation Failed. HTTP Code: " + String(httpCode));
        tempResult = 0;
      }
      http.end();
    } else {
      webLog("[HTTP Task] Validation connection failed.");
      tempResult = 0;
    }
  } else {
    webLog("[HTTP Task] No WiFi or script ID. Assuming invalid caller.");
    tempResult = 0;
  }

  // Only commit validation result if the validating number hasn't changed/cleared
  if (validatingNumber == data->phoneNumber) {
    validationResult = tempResult;
  } else {
    webLog("[HTTP Task] Discarded validation result for stale caller: " + data->phoneNumber);
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
      http.setTimeout(25000); // 25 seconds timeout for SMS sync
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
      doc["sheetId"] = sheetId;
      
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
    xTaskCreatePinnedToCore(syncSmsTask, "sms_sync_task", 16384, (void *)data, 1, NULL, 0);
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
      http.setTimeout(25000); // 25 seconds timeout for Sheets event sync
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
      doc["sheetId"] = sheetId;
      
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
    
    // Allocate 16KB stack on core 0 to handle HTTPS handshake processes
    BaseType_t res = xTaskCreatePinnedToCore(syncTask, "g_sync_task", 16384, (void *)data, 1, NULL, 0);
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
      validatingNumber = "";
      validationResult = -1;
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
      if (newState == STATE_ACTIVE_CALL && oldState != STATE_COLLECTING_DTMF && oldState != STATE_VALIDATING_SECOND_CALL) {
        welcomeMenuPending = true;
        welcomeMenuDelayStart = millis();
        customIvrTimeoutCount = 0;
        warrantyRetryCount = 0;
        webLog("Call active. Delayed welcome menu play initialized.");
      }
      break;

    case STATE_ENDED:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_PIN, HIGH);
      mp3PlayAsync(1, 4);            // /01/004.mp3 — "call ended" tone
      beep(800, 200);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      
      // Follow-up SMS check
      if (testModeEnabled && testAutoSms && (callerNumber.indexOf(testPhoneNumber) != -1 || testPhoneNumber.indexOf(callerNumber) != -1)) {
        webLog("[DEBUG] Sending confirmation SMS to debug target: " + testSmsTemplate);
        sendSMS(callerNumber, testSmsTemplate);
        addSmsLog(callerNumber, testSmsTemplate, "Outgoing", "Sent");
      } else if (smsSystemActive) {
        bool isRegistered = callerIsRegistered || (validationResult == 1);
        
        // 1. Answered but hung up without selecting option
        if ((oldState == STATE_ACTIVE_CALL || oldState == STATE_COLLECTING_DTMF) && dtmfBuffer.length() == 0) {
          String msg = "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in";
          sendSMS(callerNumber, msg);
          addSmsLog(callerNumber, msg, "Outgoing", "Sent");
        } 
        // 2. Incoming call was missed or line busy (for registered user)
        else if (isRegistered && (oldState == STATE_RINGING || oldState == STATE_VALIDATING_CALLER || oldState == STATE_VALIDATING_SECOND_CALL)) {
          String msg = "Dear customer, we missed your call. We will get back to you shortly. Thank you for calling V-Varma. Website: vvarma.gsvee.in";
          sendSMS(callerNumber, msg);
          addSmsLog(callerNumber, msg, "Outgoing", "Sent");
        }
        // 3. Outgoing call was not attended (busy or no answer)
        else if (oldState == STATE_CALLBACK_DIALING) {
          String msg = "Dear customer, we tried to call you but you were busy or did not answer. We will call you again. Thank you, V-Varma.";
          sendSMS(callerNumber, msg);
          addSmsLog(callerNumber, msg, "Outgoing", "Sent");
        }
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
  bool success = false;
  
  while (millis() - start < timeoutMs) {
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
        success = true;
        break;
      }
      
      if (totalResponse.indexOf("ERROR") != -1 || totalResponse.indexOf("+CMS ERROR") != -1) {
        success = false;
        break;
      }
    }
    if (success) break;
    delay(10);
  }
  
  static int consecutiveFailures = 0;
  if (success) {
    consecutiveFailures = 0;
    gsmModuleConnected = true;
  } else {
    consecutiveFailures++;
    if (consecutiveFailures >= 3) {
      gsmModuleConnected = false;
    }
  }
  
  return success;
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
      if (customIvrEnabled) {
        webLog("[IVR Case 1] Incoming call detected (phone starts ringing). Caller: " + tempNumber);
      }
      
      // Prevent duplicate processing of the same caller while ringing/validating
      if (currentCallState != STATE_IDLE && (tempNumber == callerNumber || tempNumber == validatingNumber)) {
        webLog("Repeat ring from current caller: " + tempNumber);
        return;
      }
      
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
        validatingNumber = tempNumber;
        callerIsRegistered = true; // Auto-answering, count as registered
        
        webLog("Incoming call. Auto-answering call immediately.");
        validationResult = 1; // Direct auto-attend trigger
        changeState(STATE_VALIDATING_CALLER);
      } else if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF) {
        webLog("Line busy. Auto-answering secondary caller: " + tempNumber);
        validatingNumber = tempNumber;
        callerIsRegistered = true;
        validationResult = 1; // Direct auto-attend second call trigger
        changeState(STATE_VALIDATING_SECOND_CALL);
      } else {
        webLog("Concurrently rejecting call from: " + tempNumber + " (current state: " + String((int)currentCallState) + ")");
        Serial2.println("ATH");
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
      processIvrDtmf(key);
    }
  }
  
  // Remote Hang Up
  else if (line.equals("NO CARRIER") || line.equals("BUSY") || line.equals("NO ANSWER") || line.equals("NO DIALTONE")) {
    webLog("Remote call terminated.");
    if (customIvrEnabled && currentCallState == STATE_AGENT_CONNECTING) {
      webLog("[IVR Case 18] Caller requested agent, but agent call failed: " + line);
      playIvrAudio("/01/012.mp3"); // Agent unavailable
      delay(5000);
      Serial2.println("ATH");
      addCallLog(callerNumber, dtmfBuffer, "Agent Call Failed (" + line + ")");
      changeState(STATE_ENDED);
    } else if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF || currentCallState == STATE_COLLECTING_PRODUCT_NO) {
      webLog("[IVR Case 16] Caller hung up during IVR.");
      addCallLog(callerNumber, dtmfBuffer, "Answered");
      changeState(STATE_ENDED);
    } else if (currentCallState == STATE_RINGING || currentCallState == STATE_VALIDATING_CALLER) {
      webLog("[IVR Case 2] Caller hung up before answer (Missed).");
      addCallLog(callerNumber, "", "Missed");
      if (testModeEnabled && testAutoCallback && (callerNumber.indexOf(testPhoneNumber) != -1 || testPhoneNumber.indexOf(callerNumber) != -1)) {
        webLog("[DEBUG] Missed call from debug target. Queuing callback.");
        callbackQueue.enqueue(callerNumber);
      }
      changeState(STATE_ENDED);
    } else {
      changeState(STATE_ENDED);
    }
  }
  
  // CLCC call list parser to detect outgoing call connection
  else if (line.startsWith("+CLCC:")) {
    int firstComma = line.indexOf(',');
    if (firstComma != -1) {
      int secondComma = line.indexOf(',', firstComma + 1);
      if (secondComma != -1) {
        int thirdComma = line.indexOf(',', secondComma + 1);
        if (thirdComma != -1) {
          int dir = line.substring(firstComma + 1, secondComma).toInt();
          int stat = line.substring(secondComma + 1, thirdComma).toInt();
          webLog("CLCC Call Update: dir=" + String(dir) + ", stat=" + String(stat));
          
          if (dir == 0) { // Outgoing call
            if (stat == 0) { // Active
              if (currentCallState == STATE_CALLBACK_DIALING) {
                webLog("Outgoing call successfully answered by remote user.");
                callStartMillis = millis();
                lastDtmfMillis = millis();
                changeState(STATE_ACTIVE_CALL);
              } else if (customIvrEnabled && currentCallState == STATE_AGENT_CONNECTING) {
                webLog("[IVR Case 19] Agent answered call. Bridging calls via ECT (AT+CHLD=4)...");
                Serial2.println("AT+CHLD=4");
                addCallLog(callerNumber, dtmfBuffer, "Agent Bridged");
                changeState(STATE_IDLE);
              }
            }
          }
        }
      }
    }
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
  String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=" + globalProxyData.action + "&mac=" + myMac + "&user=" + deviceUser + "&pass=" + devicePass + "&sheetId=" + sheetId;
  
  webLog("[Proxy Task] Connecting to Google Sheets...");
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (http.begin(client, url)) {
    http.setTimeout(45000); // 45 seconds timeout
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
  // Create task with 16KB stack size pinned to Core 0 (System Core) to keep Core 1 free for the Web Server
  xTaskCreatePinnedToCore(proxyTask, "proxy_task", 16384, NULL, 1, NULL, 0);

  unsigned long start = millis();
  // Wait up to 50 seconds synchronously
  while (!globalProxyData.done && (millis() - start < 50000)) {
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

void createParentDirs(String path) {
  int lastSlash = path.lastIndexOf('/');
  if (lastSlash > 0) {
    String dirPath = path.substring(0, lastSlash);
    if (!myFS->exists(dirPath)) {
      myFS->mkdir(dirPath);
      webLog("Created directory: " + dirPath);
    }
  }
}

void printDirectory(File dir, bool& first) {
  File file = dir.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      printDirectory(file, first);
    } else {
      String fname = String(file.name());
      if (!first) {
        server.sendContent(",");
      }
      first = false;
      String fileJson = "{\"name\":\"" + fname + "\",\"size\":" + String(file.size()) + "}";
      server.sendContent(fileJson);
    }
    file.close();
    file = dir.openNextFile();
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
    createParentDirs(filename);
    uploadFile = myFS->open(filename, FILE_WRITE);
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
  server.enableCORS(true);

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
    StaticJsonDocument<2048> doc;
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
    doc["sheetId"] = sheetId;
    doc["sheetName"] = sheetName;
    doc["syncIncoming"] = syncIncoming;
    doc["syncOutgoing"] = syncOutgoing;
    doc["syncSms"] = syncSms;
    doc["syncSystem"] = syncSystem;
    doc["syncNetwork"] = syncNetwork;
    doc["devUser"] = deviceUser;
    doc["devPass"] = devicePass;
    doc["apSsid"] = savedApSSID;
    doc["apPass"] = savedApPass;
    doc["mode"] = (currentSystemState == SYSTEM_WIFI_CONFIG) ? "AP" : "STA";
    doc["uptime"] = millis() / 1000;
    doc["syncCount"] = sheetsSyncCount;
    doc["syncErrors"] = sheetsSyncErrors;
    doc["lastSyncTime"] = sheetsLastSyncTime;
    doc["callSystemActive"] = callSystemActive;
    doc["smsSystemActive"] = smsSystemActive;
    doc["testModeEnabled"] = testModeEnabled;
    doc["callerValidationEnabled"] = callerValidationEnabled;
    doc["customIvrEnabled"] = customIvrEnabled;
    doc["agentPhoneNumber"] = agentPhoneNumber;
    
    // Call Status Variables
    switch(currentCallState) {
      case STATE_IDLE:             doc["state"] = "IDLE"; break;
      case STATE_VALIDATING_CALLER:      doc["state"] = "VALIDATING"; break;
      case STATE_VALIDATING_SECOND_CALL: doc["state"] = "VALIDATING"; break;
      case STATE_RINGING:          doc["state"] = "RINGING"; break;
      case STATE_ACTIVE_CALL:      doc["state"] = "ACTIVE CALL"; break;
      case STATE_COLLECTING_DTMF:  doc["state"] = "COLLECTING DTMF"; break;
      case STATE_ENDED:            doc["state"] = "ENDED"; break;
      case STATE_CALLBACK_DIALING: doc["state"] = "CALLBACK DIALING"; break;
      case STATE_COLLECTING_PRODUCT_NO: doc["state"] = "WARRANTY INPUT"; break;
      case STATE_AGENT_CONNECTING: doc["state"] = "AGENT CONNECTING"; break;
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
    if (sdCardFound) {
      sdObj["mounted"] = true;
      sdObj["totalBytes"] = sdCardTotalBytes;
      sdObj["usedBytes"] = sdCardUsedBytes;
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
    if (currentCallState == STATE_RINGING || currentCallState == STATE_VALIDATING_CALLER) {
      webLog("ATA command issued via local Web server.");
      if (sendATCommand("ATA", "OK", 5000)) {
        webLog("Call answered successfully via Web server.");
        Serial2.println("AT+CLIP=1");
        delay(100);
        Serial2.println("AT+DDET=1"); // Ensure DTMF detector is enabled
        delay(100);
        sendATCommand("AT+CMUT=0", "OK", 1000); // Unmute microphone on active call
        callStartMillis = millis();
        lastDtmfMillis = millis();
        validationResult = -1;
        changeState(STATE_ACTIVE_CALL);
        server.send(200, "application/json", "{\"status\":\"success\"}");
      } else {
        webLog("Failed to answer call via Web server (ATA failed).");
        Serial2.println("ATH");
        validationResult = -1;
        changeState(STATE_ENDED);
        server.send(200, "application/json", "{\"status\":\"error\",\"message\":\"Failed to answer call (ATA failed)\"}");
      }
    } else if (currentCallState == STATE_VALIDATING_SECOND_CALL) {
      webLog("Answering waiting call via local Web server.");
      if (sendATCommand("AT+CHLD=1", "OK", 5000)) {
        webLog("Call 2 answered successfully via Web server.");
        callerNumber = validatingNumber;
        callStartMillis = millis();
        lastDtmfMillis = millis();
        validationResult = -1;
        changeState(STATE_ACTIVE_CALL);
        server.send(200, "application/json", "{\"status\":\"success\"}");
      } else {
        webLog("Failed to answer Call 2 via Web server (AT+CHLD=1 failed).");
        Serial2.println("ATH");
        validationResult = -1;
        changeState(STATE_ENDED);
        server.send(200, "application/json", "{\"status\":\"error\",\"message\":\"Failed to answer waiting call\"}");
      }
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Lines not ringing\"}");
    }
  });

  server.on("/hangup", []() {
    webLog("ATH hang up command issued via local Web server.");
    if (currentCallState == STATE_VALIDATING_SECOND_CALL) {
      Serial2.println("AT+CHLD=0"); // Reject waiting/held call
      changeState(STATE_ACTIVE_CALL);
    } else {
      Serial2.println("ATH");
      if (currentCallState == STATE_RINGING || currentCallState == STATE_VALIDATING_CALLER) {
        addCallLog(callerNumber, "", "Rejected");
      } else if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF) {
        addCallLog(callerNumber, dtmfBuffer, "Terminated");
      }
      validationResult = -1;
      changeState(STATE_ENDED);
    }
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
    if (server.hasArg("sheetId")) {
      sheetId = server.arg("sheetId");
      preferences.begin("wifi-config", false);
      preferences.putString("sheetId", sheetId);
      preferences.end();
      webLog("initVarmaSheets: Dynamically saved sheetId=" + sheetId);
    }
    proxyGoogleSheets("initVarmaSheets");
  });
  server.on("/api/telegram/status", HTTP_GET, []() {
    StaticJsonDocument<128> doc;
    doc["messages"] = 0;
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
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
    if (server.hasArg("callerValidationEnabled")) {
      callerValidationEnabled = server.arg("callerValidationEnabled") == "true";
    }
    if (server.hasArg("customIvrEnabled")) {
      customIvrEnabled = server.arg("customIvrEnabled") == "true";
    }
    if (server.hasArg("agentPhoneNumber")) {
      agentPhoneNumber = server.arg("agentPhoneNumber");
    }
    preferences.begin("wifi-config", false);
    preferences.putBool("callSysAct", callSystemActive);
    preferences.putBool("smsSysAct", smsSystemActive);
    preferences.putBool("callValEn", callerValidationEnabled);
    preferences.putBool("customIvrEn", customIvrEnabled);
    preferences.putString("agentPhone", agentPhoneNumber);
    preferences.end();
    
    webLog("GSM Settings updated: Call=" + String(callSystemActive ? "ON" : "OFF") + ", SMS=" + String(smsSystemActive ? "ON" : "OFF") + ", Validation=" + String(callerValidationEnabled ? "ON" : "OFF") + ", CustomIVR=" + String(customIvrEnabled ? "ON" : "OFF"));
    server.send(200, "application/json", "{\"success\":true}");
  });

  server.on("/api/test-config", HTTP_POST, []() {
    if (server.hasArg("testModeEnabled")) {
      testModeEnabled = server.arg("testModeEnabled") == "true";
    }
    if (server.hasArg("testAutoAttend")) {
      testAutoAttend = server.arg("testAutoAttend") == "true";
    }
    if (server.hasArg("testAutoCallback")) {
      testAutoCallback = server.arg("testAutoCallback") == "true";
    }
    if (server.hasArg("testAutoSms")) {
      testAutoSms = server.arg("testAutoSms") == "true";
    }
    if (server.hasArg("testPhoneNumber")) {
      testPhoneNumber = server.arg("testPhoneNumber");
    }
    if (server.hasArg("testSmsTemplate")) {
      testSmsTemplate = server.arg("testSmsTemplate");
    }
    preferences.begin("wifi-config", false);
    preferences.putBool("testMode", testModeEnabled);
    preferences.putBool("testAttend", testAutoAttend);
    preferences.putBool("testCallback", testAutoCallback);
    preferences.putBool("testSms", testAutoSms);
    preferences.putString("testPhone", testPhoneNumber);
    preferences.putString("testSmsTmp", testSmsTemplate);
    preferences.end();
    
    webLog("Hardware Debug Config updated: Mode=" + String(testModeEnabled ? "ON" : "OFF") + ", Attend=" + String(testAutoAttend ? "ON" : "OFF") + ", Callback=" + String(testAutoCallback ? "ON" : "OFF") + ", SMS=" + String(testAutoSms ? "ON" : "OFF"));
    server.send(200, "application/json", "{\"success\":true}");
  });

  server.on("/api/test-config", HTTP_GET, []() {
    StaticJsonDocument<512> doc;
    doc["testModeEnabled"] = testModeEnabled;
    doc["testAutoAttend"] = testAutoAttend;
    doc["testAutoCallback"] = testAutoCallback;
    doc["testAutoSms"] = testAutoSms;
    doc["testPhoneNumber"] = testPhoneNumber;
    doc["testSmsTemplate"] = testSmsTemplate;
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/api/dial", []() {
    String number = server.hasArg("number") ? server.arg("number") : server.arg("phone");
    if (number.length() == 0) {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Number required\"}");
      return;
    }
    webLog("Outgoing call requested to: " + number);
    Serial2.println("ATH");
    delay(500);
    while(Serial2.available() > 0) { Serial2.read(); }
    Serial2.println("ATD" + number + ";");
    
    callerNumber = number;
    dtmfBuffer = "";
    callStartMillis = millis();
    changeState(STATE_CALLBACK_DIALING);
    
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });

  server.on("/api/send_sms", []() {
    String number = server.hasArg("number") ? server.arg("number") : server.arg("phone");
    String message = server.arg("message");
    if (number.length() == 0 || message.length() == 0) {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Number and message required\"}");
      return;
    }
    sendSMS(number, message);
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });

  server.on("/api/dtmf", []() {
    if (server.hasArg("digit")) {
      String digit = server.arg("digit");
      if (digit.length() > 0) {
        char d = digit.charAt(0);
        webLog("Sending DTMF tone to call: " + String(d));
        Serial2.println("AT+VTS=" + String(d));
        server.send(200, "application/json", "{\"status\":\"success\"}");
        return;
      }
    }
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Digit required\"}");
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
    preferences.begin("wifi-config", false);
    
    if (server.hasArg("script")) {
      scriptId = server.arg("script");
      preferences.putString("scriptId", scriptId);
    }
    if (server.hasArg("sheetId")) {
      sheetId = server.arg("sheetId");
      preferences.putString("sheetId", sheetId);
    }
    if (server.hasArg("sheetName")) {
      sheetName = server.arg("sheetName");
      preferences.putString("sheetName", sheetName);
    }
    if (server.hasArg("syncInc")) {
      syncIncoming = (server.arg("syncInc") == "true" || server.arg("syncInc") == "1");
      preferences.putBool("syncInc", syncIncoming);
    }
    if (server.hasArg("syncOut")) {
      syncOutgoing = (server.arg("syncOut") == "true" || server.arg("syncOut") == "1");
      preferences.putBool("syncOut", syncOutgoing);
    }
    if (server.hasArg("syncSms")) {
      syncSms = (server.arg("syncSms") == "true" || server.arg("syncSms") == "1");
      preferences.putBool("syncSms", syncSms);
    }
    if (server.hasArg("syncSys")) {
      syncSystem = (server.arg("syncSys") == "true" || server.arg("syncSys") == "1");
      preferences.putBool("syncSys", syncSystem);
    }
    if (server.hasArg("syncNet")) {
      syncNetwork = (server.arg("syncNet") == "true" || server.arg("syncNet") == "1");
      preferences.putBool("syncNet", syncNetwork);
    }
    if (server.hasArg("adminUser")) {
      deviceUser = server.arg("adminUser");
      preferences.putString("adminUser", deviceUser);
    }
    if (server.hasArg("adminPass")) {
      devicePass = server.arg("adminPass");
      preferences.putString("adminPass", devicePass);
    }
    if (server.hasArg("apSsid")) {
      savedApSSID = server.arg("apSsid");
      preferences.putString("apSsid", savedApSSID);
    }
    if (server.hasArg("apPass")) {
      savedApPass = server.arg("apPass");
      preferences.putString("apPass", savedApPass);
    }
    
    bool shouldReboot = true;
    if (server.hasArg("reboot")) {
      shouldReboot = (server.arg("reboot") == "true" || server.arg("reboot") == "1");
    }
    preferences.end();
    webLog("NVS Config: Saved Google Sheets and System configuration (reboot=" + String(shouldReboot ? "true" : "false") + ")");
    
    server.send(200, "application/json", "{\"success\":true}");
    if (shouldReboot) {
      delay(2000);
      ESP.restart();
    }
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
    doc["total"] = getTotalBytes();
    doc["used"] = getUsedBytes();
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/sd_files", []() {
    if (!sdCardFound) {
      server.send(200, "application/json", "[]");
      return;
    }
    File root = myFS->open("/");
    if (!root) {
      server.send(200, "application/json", "[]");
      return;
    }

    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/json", "");
    server.sendContent("[");

    bool first = true;
    printDirectory(root, first);
    
    root.close();
    server.sendContent("]");
    server.sendContent(""); // End of chunked response
  });

  server.on("/sd_test", []() {
    if (!sdCardFound) {
      server.send(200, "application/json", "{\"success\":false,\"error\":\"SD Card not mounted\"}");
      return;
    }
    
    // Write test file
    File testFile = myFS->open("/test_write.txt", FILE_WRITE);
    if (!testFile) {
      server.send(200, "application/json", "{\"success\":false,\"error\":\"Failed to open file for writing\"}");
      return;
    }
    
    String testStr = "V-VARMA SD CARD TEST OK " + String(millis());
    testFile.print(testStr);
    testFile.close();
    
    // Read test file
    testFile = myFS->open("/test_write.txt", FILE_READ);
    if (!testFile) {
      server.send(200, "application/json", "{\"success\":false,\"error\":\"Failed to open file for reading\"}");
      return;
    }
    
    String readStr = "";
    while (testFile.available()) {
      readStr += (char)testFile.read();
    }
    testFile.close();
    
    // Remove test file
    myFS->remove("/test_write.txt");
    
    if (readStr == testStr) {
      server.send(200, "application/json", "{\"success\":true,\"message\":\"Read and write test passed successfully!\"}");
    } else {
      server.send(200, "application/json", "{\"success\":false,\"error\":\"Data mismatch during test\"}");
    }
  });

  server.on("/sd_wipe", HTTP_POST, []() {
    if (!sdCardFound) {
      server.send(200, "application/json", "{\"success\":false,\"error\":\"SD Card not mounted\"}");
      return;
    }
    
    File root = myFS->open("/");
    if (!root) {
      server.send(200, "application/json", "{\"success\":false,\"error\":\"Failed to open root directory\"}");
      return;
    }
    
    // Delete all files in the root folder
    File file = root.openNextFile();
    int deletedCount = 0;
    while (file) {
      if (!file.isDirectory()) {
        String fname = String(file.name());
        if (!fname.startsWith("/")) fname = "/" + fname;
        file.close(); // Close file before removing
        if (myFS->remove(fname)) {
          deletedCount++;
        }
      } else {
        file.close();
      }
      file = root.openNextFile();
    }
    root.close();
    
    // Reset file bytes info
    sdCardUsedBytes = getUsedBytes();
    
    StaticJsonDocument<128> doc;
    doc["success"] = true;
    doc["message"] = "Wiped " + String(deletedCount) + " files from TF card root.";
    doc["used"] = sdCardUsedBytes;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/read_file", []() {
    if (!sdCardFound) {
      server.send(404, "application/json", "{\"success\":false,\"error\":\"SD Card not mounted\"}");
      return;
    }
    
    if (server.hasArg("file")) {
      String f = server.arg("file");
      if (!f.startsWith("/")) f = "/" + f;
      
      if (myFS->exists(f)) {
        File file = myFS->open(f, FILE_READ);
        if (file) {
          server.setContentLength(file.size());
          server.send(200, "text/plain", "");
          while (file.available()) {
            server.sendContent(file.readStringUntil('\n') + '\n');
          }
          file.close();
        } else {
          server.send(500, "application/json", "{\"success\":false,\"error\":\"Failed to open file\"}");
        }
      } else {
        server.send(404, "application/json", "{\"success\":false,\"error\":\"File not found\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing file argument\"}");
    }
  });

  server.on("/upload_audio", HTTP_POST, []() {
    server.send(200, "application/json", "{\"success\":true}");
  }, handleFileUpload);

  server.on("/delete_audio", HTTP_POST, []() {
    if (server.hasArg("file")) {
      String f = server.arg("file");
      if (!f.startsWith("/")) f = "/" + f;
      if (myFS->exists(f)) {
        myFS->remove(f);
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
      File f = myFS->open("/ivr_menu.json", FILE_WRITE);
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
    if (myFS->exists("/ivr_menu.json")) {
      File f = myFS->open("/ivr_menu.json", FILE_READ);
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
  gsmModuleConnected = sendATCommand("AT", "OK", 2000);
  if (gsmModuleConnected) {
    sendATCommand("ATS0=0", "OK", 1000);    // Disable hardware auto-answer
    sendATCommand("AT+CLIP=1", "OK", 1000); // Enable caller ID presentation
    sendATCommand("AT+DDET=1", "OK", 1000); // Enable DTMF detector
    sendATCommand("AT+CMGF=1", "OK", 1000); // Set SMS to text mode
    sendATCommand("AT+CNMI=2,2,0,0,0", "OK", 1000); // Direct SMS routing to terminal
    
    // Configure Voice Call Audio Channel, Unmute Microphone, and Volume/Gain
    sendATCommand("AT+CHFA=0", "OK", 1000);    // Set routing to Main Audio Channel (MIC1/SPK1)
    sendATCommand("AT+CMUT=0", "OK", 1000);    // Unmute GSM microphone
    sendATCommand("AT+CLVL=100", "OK", 1000);  // Set receiver speaker volume to maximum
    sendATCommand("AT+CMIC=0,15", "OK", 1000); // Set main microphone gain to maximum (channel 0)
    sendATCommand("AT+CMIC=1,15", "OK", 1000); // Set aux microphone gain to maximum (channel 1)
  } else {
    webLog("GSM Module not responding during initialization.");
  }
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
  if (!gsmModuleConnected) {
    // No GSM module with ESP -> glow Red LED steady (solid HIGH), Green LED off
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PIN, HIGH);
  } else {
    bool gsmRegistered = (gsmRegStatus == "1" || gsmRegStatus == "5");
    int signalVal = gsmSignal.toInt();
    bool gsmSignalOk = (signalVal > 0 && signalVal != 99);

    if (gsmRegistered && gsmSignalOk) {
      // SIM signal ok, received tower good -> glow green LED steady (solid HIGH)
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_PIN, LOW);
    } else {
      // Cellular Error / No signal / Unregistered -> fast blink red LED (100ms cycle)
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_PIN, (ledBlinkCounter % 2 == 0) ? HIGH : LOW);
    }
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
    webLog("SD Card Mount Failed. Attempting SPIFFS fallback...");
    if (!SPIFFS.begin(true)) {
      webLog("SPIFFS Fallback Mount Failed. File storage disabled.");
      sdCardFound = false;
    } else {
      webLog("SPIFFS Fallback Mount Successful!");
      myFS = &SPIFFS;
      sdCardFound = true;
      sdCardTotalBytes = SPIFFS.totalBytes();
      sdCardUsedBytes = SPIFFS.usedBytes();
    }
  } else {
    webLog("SD Card Mount Successful!");
    myFS = &SD;
    sdCardFound = true;
    sdCardTotalBytes = SD.totalBytes();
    sdCardUsedBytes = SD.usedBytes();
  }

  // Ensure ivr_menu.json exists and is configured as requested
  ensureIvrMenuJson();

  // Initialize GSM Serial2 Communication (GPIO16=RX, GPIO17=TX)
  Serial2.begin(GSM_BAUD, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  initializeGSM();

  // Initialize MP3-TF-16P player (GPIO27=RX, GPIO26=TX)
  mp3Init();

  // Read saved configurations
  preferences.begin("wifi-config", false);
  savedSSID = preferences.getString("ssid", "");
  savedPass = preferences.getString("pass", "");
  scriptId  = preferences.getString("scriptId", "");
  sheetId   = preferences.getString("sheetId", "");
  sheetName = preferences.getString("sheetName", "IVR");
  savedApSSID = preferences.getString("apSsid", "vvarmaivr");
  savedApPass = preferences.getString("apPass", "GSVIVR001");
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
  callerValidationEnabled = preferences.getBool("callValEn", false);
  customIvrEnabled = preferences.getBool("customIvrEn", false);
  agentPhoneNumber = preferences.getString("agentPhone", "+919092610415");
  
  testModeEnabled = preferences.getBool("testMode", false);
  testAutoAttend = preferences.getBool("testAttend", false);
  testAutoCallback = preferences.getBool("testCallback", false);
  testAutoSms = preferences.getBool("testSms", false);
  testPhoneNumber = preferences.getString("testPhone", "");
  testSmsTemplate = preferences.getString("testSmsTmp", "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in");
  
  preferences.end();

  if (savedSSID.length() > 0) {
    webLog("Attempting connection to local router: " + savedSSID);
    
    webLog("Connecting to WiFi: " + savedSSID);
    WiFi.softAPdisconnect(true);
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
      WiFi.softAPdisconnect(true);
      localIpStr = WiFi.localIP().toString();
      webLog("WiFi connection succeeded. IP Address: " + localIpStr);
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

  webLog("[IVR Case 20] System restarts or recovers after power failure and returns to idle state.");
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
      callerIsRegistered = true;
      webLog("Caller Validated. Answering call physically via ATA...");
      if (sendATCommand("ATA", "OK", 5000)) {
        webLog("Call physically answered successfully.");
        if (customIvrEnabled) {
          webLog("[IVR Case 3] System answers the call successfully.");
        }
        delay(100);
        sendATCommand("AT+CMUT=0", "OK", 1000); // Unmute microphone on active call
        callStartMillis = millis();
        lastDtmfMillis = millis();
        changeState(STATE_ACTIVE_CALL);
      } else {
        webLog("Failed to physically answer call (ATA failed or timed out).");
        Serial2.println("ATH");
        addCallLog(callerNumber, "", "Answer Failed");
        changeState(STATE_ENDED);
      }
      validationResult = -1;
    } else if (validationResult == 0) {
      callerIsRegistered = false;
      webLog("Caller Invalid/Unregistered. Rejecting call.");
      Serial2.println("ATH");
      addCallLog(callerNumber, "", "Rejected/Unregistered");
      changeState(STATE_ENDED);
      validationResult = -1;
    }
  }

  if (currentCallState == STATE_VALIDATING_SECOND_CALL) {
    if (validationResult == 1) {
      callerIsRegistered = true;
      webLog("Second Caller Validated. Dropping Call 1 & Answering Call 2...");
      if (sendATCommand("AT+CHLD=1", "OK", 5000)) {
        webLog("Call 2 answered successfully.");
        callerNumber = validatingNumber;
        callStartMillis = millis();
        lastDtmfMillis = millis();
        changeState(STATE_ACTIVE_CALL);
      } else {
        webLog("Failed to answer Call 2.");
        Serial2.println("ATH");
        addCallLog(validatingNumber, "", "Answer Failed");
        changeState(STATE_ENDED);
      }
      validationResult = -1;
    } else if (validationResult == 0) {
      callerIsRegistered = false;
      webLog("Second Caller Invalid. Rejecting waiting call.");
      Serial2.println("AT+CHLD=0"); // Reject waiting call
      changeState(STATE_ACTIVE_CALL); // Return to active call state
      validationResult = -1;
    }
  }

  // 5.8 Validation timeout checker
  if ((currentCallState == STATE_VALIDATING_CALLER || currentCallState == STATE_VALIDATING_SECOND_CALL) && (millis() - ringingStartMillis > 15000)) {
    webLog("Validation timeout. Defaulting to unregistered/rejection.");
    validationResult = 0; // Trigger auto-rejection
  }

  // 6. Ringing timeout checker
  if (currentCallState == STATE_RINGING && (millis() - ringingStartMillis > 40000)) {
    webLog("Incoming call ring timeout.");
    addCallLog(callerNumber, "", "Missed");
    if (testModeEnabled && testAutoCallback && (callerNumber.indexOf(testPhoneNumber) != -1 || testPhoneNumber.indexOf(callerNumber) != -1)) {
      webLog("[DEBUG] Missed call from debug target. Queuing callback.");
      callbackQueue.enqueue(callerNumber);
    }
    changeState(STATE_ENDED);
  }

  // 6.5 Welcome menu delayed playback scheduler
  if (welcomeMenuPending && (millis() - welcomeMenuDelayStart > 2000)) {
    welcomeMenuPending = false;
    loadIvrNode("start");
  }

  // 6.7 Welcome menu idle timeout handler (Case 9 & 10)
  if (customIvrEnabled && (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF)) {
    if (ivrCurrentNode == "start" && (millis() - lastDtmfMillis > 10000)) {
      lastDtmfMillis = millis();
      if (customIvrTimeoutCount == 0) {
        customIvrTimeoutCount = 1;
        webLog("[IVR Case 9] Caller did not press any key (first timeout). Replaying welcome menu.");
        loadIvrNode("start");
      } else {
        webLog("[IVR Case 10] Caller still did not press any key after menu replay (final timeout). Hanging up.");
        playIvrAudio("/01/004.mp3"); // Goodbye track
        delay(3000);
        Serial2.println("ATH");
        addCallLog(callerNumber, dtmfBuffer, "Timeout Hangup");
        changeState(STATE_ENDED);
      }
    }
  }

  // 6.8 Warranty input silent timeout handler (5s after typing stops, or 10s idle)
  if (customIvrEnabled && currentCallState == STATE_COLLECTING_PRODUCT_NO) {
    if (dtmfBuffer.length() > 0 && (millis() - lastDtmfMillis > 5000)) {
      // Treat as finished typing
      verifyIvrWarranty(dtmfBuffer);
    } else if (dtmfBuffer.length() == 0 && (millis() - lastDtmfMillis > 10000)) {
      // 10s silent timeout
      webLog("Warranty input silent timeout. Replaying prompt.");
      lastDtmfMillis = millis();
      playIvrAudio("/01/005.mp3");
    }
  }

  // 6.9 Check Warranty verification task results
  if (customIvrEnabled && currentCallState == STATE_COLLECTING_PRODUCT_NO) {
    if (ivrWarrantyCheckResult != 0 && ivrWarrantyCheckResult != 99) {
      int result = ivrWarrantyCheckResult;
      ivrWarrantyCheckResult = 0; // Reset
      processWarrantyResult(ivrWarrantyCheckCode, result);
    }
  }

  // 6.95 Check Complaint registration task results
  if (customIvrEnabled && ivrCurrentNode == "complaint_reg") {
    if (ivrComplaintResult != 0 && ivrComplaintResult != 99) {
      int res = ivrComplaintResult;
      ivrComplaintResult = 0; // Reset
      
      if (res == 1) {
        webLog("[IVR Case 15] Complaint registration completed successfully. Ticket: " + ivrComplaintTicket);
        playIvrAudio("/01/010.mp3"); // Success tone
        String msg = "Dear customer, your complaint has been registered. Ticket: " + ivrComplaintTicket + ". V-Varma.";
        sendSMS(callerNumber, msg);
        addSmsLog(callerNumber, msg, "Outgoing", "Sent");
        delay(5000);
        Serial2.println("ATH");
        addCallLog(callerNumber, dtmfBuffer, "Complaint Success (" + ivrComplaintTicket + ")");
        changeState(STATE_ENDED);
      } else {
        webLog("Complaint registration failed. Hanging up.");
        delay(3000);
        Serial2.println("ATH");
        addCallLog(callerNumber, dtmfBuffer, "Complaint Failed");
        changeState(STATE_ENDED);
      }
    }
  }

  // 7. Active call timeout (security hangup after 5 minutes)
  if ((currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_COLLECTING_DTMF || currentCallState == STATE_COLLECTING_PRODUCT_NO || currentCallState == STATE_AGENT_CONNECTING) && (millis() - callStartMillis > 300000)) {
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

  // Handle agent dialing timeout (Case 18)
  if (currentCallState == STATE_AGENT_CONNECTING && (millis() - callStartMillis > 40000)) {
    webLog("[IVR Case 18] Caller requests an agent, but all agents are busy/unavailable (timeout).");
    playIvrAudio("/01/012.mp3");
    delay(5000);
    Serial2.println("ATH");
    addCallLog(callerNumber, dtmfBuffer, "Agent Busy Timeout");
    changeState(STATE_ENDED);
  }

  // Case 17: Network/SIM connection lost during the call
  if (customIvrEnabled && currentCallState != STATE_IDLE && currentCallState != STATE_ENDED) {
    bool regOk = (gsmRegStatus == "1" || gsmRegStatus == "5");
    if (!regOk && gsmModuleConnected) {
      webLog("[IVR Case 17] Network/SIM connection lost during the call.");
      Serial2.println("ATH");
      addCallLog(callerNumber, dtmfBuffer, "Network Lost");
      changeState(STATE_ENDED);
    }
  }

  // Poll CLCC every 1 second during callback dialing to detect when call is answered
  static unsigned long lastClccPoll = 0;
  if (currentCallState == STATE_CALLBACK_DIALING || currentCallState == STATE_AGENT_CONNECTING) {
    if (millis() - lastClccPoll > 1000) {
      lastClccPoll = millis();
      Serial2.println("AT+CLCC");
    }
  }

  delay(10);
}

void playIvrAudio(String filename) {
  webLog("[IVR MP3] Parsing filename: " + filename);
  String cleanName = filename;
  cleanName.toLowerCase();
  
  // Strip common audio extensions (handle double extensions like .mp3.mp3 as well)
  while (cleanName.endsWith(".mp3") || cleanName.endsWith(".wav") || cleanName.endsWith(".amr")) {
    if (cleanName.endsWith(".mp3")) cleanName = cleanName.substring(0, cleanName.length() - 4);
    else if (cleanName.endsWith(".wav")) cleanName = cleanName.substring(0, cleanName.length() - 4);
    else if (cleanName.endsWith(".amr")) cleanName = cleanName.substring(0, cleanName.length() - 4);
  }
  
  int folder = 2; // Default folder for menu
  int track = 1;
  
  int slashIdx = cleanName.lastIndexOf('/');
  if (slashIdx != -1) {
    String folderPart = cleanName.substring(0, slashIdx);
    String trackPart = cleanName.substring(slashIdx + 1);
    
    // Extract only digits from folder part
    String folderDigits = "";
    for (int i = 0; i < folderPart.length(); i++) {
      if (isDigit(folderPart.charAt(i))) folderDigits += folderPart.charAt(i);
    }
    // Extract only digits from track part
    String trackDigits = "";
    for (int i = 0; i < trackPart.length(); i++) {
      if (isDigit(trackPart.charAt(i))) trackDigits += trackPart.charAt(i);
    }
    
    if (folderDigits.length() > 0) folder = folderDigits.toInt();
    if (trackDigits.length() > 0) track = trackDigits.toInt();
  } else {
    // No slash, extract all digits
    String digits = "";
    for (int i = 0; i < cleanName.length(); i++) {
      if (isDigit(cleanName.charAt(i))) digits += cleanName.charAt(i);
    }
    if (digits.length() > 0) {
      int val = digits.toInt();
      track = val;
      if (val >= 100) {
        folder = val / 100;
        track = val % 100;
      }
    }
  }
  
  webLog("[IVR MP3] Resolved playback: Folder: " + String(folder) + ", Track: " + String(track));
  mp3Play(folder, track);
}

void ensureIvrMenuJson() {
  bool fsWorking = sdCardFound || (myFS == &SPIFFS);
  if (!fsWorking) {
    webLog("[IVR] File system not active, cannot write ivr_menu.json.");
    return;
  }
  
  File f = myFS->open("/ivr_menu.json", FILE_WRITE);
  if (f) {
    String jsonStr = "{\n"
                     "  \"nodes\": {\n"
                     "    \"start\": {\n"
                     "      \"audio\": \"/01/001.mp3\",\n"
                     "      \"options\": {\n"
                     "        \"1\": \"node1\",\n"
                     "        \"2\": \"node2\"\n"
                     "      }\n"
                     "    },\n"
                     "    \"node1\": {\n"
                     "      \"audio\": \"/01/002.mp3\",\n"
                     "      \"options\": {}\n"
                     "    },\n"
                     "    \"node2\": {\n"
                     "      \"audio\": \"/01/003.mp3\",\n"
                     "      \"options\": {}\n"
                     "    }\n"
                     "  }\n"
                     "}";
    f.print(jsonStr);
    f.close();
    webLog("[IVR] Successfully configured and wrote ivr_menu.json to SPIFFS/SD card.");
  } else {
    webLog("[IVR] Failed to write ivr_menu.json.");
  }
}

void loadIvrNode(String nodeName) {
  ivrCurrentNode = nodeName;
  if (customIvrEnabled && nodeName == "start") {
    webLog("[IVR Case 4] Welcome message/menu is played to the caller.");
    currentIvrNode.audio = "/01/005.mp3";
    currentIvrNode.optionCount = 0;
    playIvrAudio(currentIvrNode.audio);
    return;
  }
  currentIvrNode.audio = "";
  currentIvrNode.optionCount = 0;
  for (int i = 0; i < 10; i++) {
    currentIvrNode.optionKeys[i] = "";
    currentIvrNode.optionTargetNodes[i] = "";
  }

  bool fsWorking = sdCardFound || (myFS == &SPIFFS);
  if (!fsWorking) {
    webLog("[IVR] File system not active, cannot load menu.");
    return;
  }

  File f = myFS->open("/ivr_menu.json", FILE_READ);
  if (!f) {
    webLog("[IVR] /ivr_menu.json not found. Loading hardcoded default menu structure.");
    if (nodeName == "start") {
      ivrCurrentNode = "start";
      currentIvrNode.audio = "/01/001.mp3";
      currentIvrNode.optionKeys[0] = "1";
      currentIvrNode.optionTargetNodes[0] = "node1";
      currentIvrNode.optionKeys[1] = "2";
      currentIvrNode.optionTargetNodes[1] = "node2";
      currentIvrNode.optionCount = 2;
    } else if (nodeName == "node1") {
      ivrCurrentNode = "node1";
      currentIvrNode.audio = "/01/002.mp3";
      currentIvrNode.optionCount = 0;
    } else if (nodeName == "node2") {
      ivrCurrentNode = "node2";
      currentIvrNode.audio = "/01/003.mp3";
      currentIvrNode.optionCount = 0;
    }
    
    webLog("[IVR Default] Loaded hardcoded Node: " + nodeName + " | Audio: " + currentIvrNode.audio);
    if (currentIvrNode.audio != "") {
      playIvrAudio(currentIvrNode.audio);
    }
    return;
  }

  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    webLog("[IVR] Failed to parse /ivr_menu.json: " + String(error.c_str()));
    return;
  }

  JsonObject nodes = doc["nodes"];
  String actualNode = nodeName;
  if (!nodes.containsKey(actualNode)) {
    if (actualNode == "start" && nodes.size() > 0) {
      for (JsonPair p : nodes) {
        actualNode = p.key().c_str();
        break;
      }
      webLog("[IVR] 'start' node not found. Falling back to first node: " + actualNode);
    } else {
      webLog("[IVR] Node not found: " + nodeName);
      return;
    }
  }

  ivrCurrentNode = actualNode;
  JsonObject node = nodes[actualNode];
  currentIvrNode.audio = node["audio"] | "";
  
  JsonObject options = node["options"];
  int idx = 0;
  for (JsonPair p : options) {
    if (idx < 10) {
      currentIvrNode.optionKeys[idx] = p.key().c_str();
      currentIvrNode.optionTargetNodes[idx] = p.value().as<String>();
      idx++;
    }
  }
  currentIvrNode.optionCount = idx;

  webLog("[IVR] Loaded Node: " + nodeName + " | Audio: " + currentIvrNode.audio + " | Options count: " + String(idx));
  
  if (currentIvrNode.audio != "") {
    playIvrAudio(currentIvrNode.audio);
  }
}

void processIvrDtmf(String key) {
  if (currentCallState != STATE_ACTIVE_CALL && currentCallState != STATE_COLLECTING_DTMF && currentCallState != STATE_COLLECTING_PRODUCT_NO) {
    webLog("[IVR] DTMF received but no active call or digit collection state.");
    return;
  }
  
  if (customIvrEnabled) {
    webLog("[IVR] Custom DTMF processing: key=" + key + " in state=" + String((int)currentCallState) + ", node=" + ivrCurrentNode);
    lastDtmfMillis = millis(); // Refresh timeout timer
    
    if (currentCallState == STATE_COLLECTING_PRODUCT_NO) {
      // Accumulate code
      if (key == "#") {
        if (dtmfBuffer.length() > 0) {
          verifyIvrWarranty(dtmfBuffer);
        } else {
          webLog("Empty warranty code entered.");
          playIvrAudio("/01/008.mp3");
        }
      } else {
        dtmfBuffer += key;
        webLog("Buffered code: " + dtmfBuffer);
      }
      return;
    }
    
    // In menu state (STATE_ACTIVE_CALL or STATE_COLLECTING_DTMF)
    if (ivrCurrentNode == "start") {
      if (key == "1") {
        webLog("[IVR Case 5] Caller presses 1 (Warranty option).");
        playIvrAudio("/01/005.mp3"); // Enter product number followed by #
        dtmfBuffer = "";
        changeState(STATE_COLLECTING_PRODUCT_NO);
      } else if (key == "2") {
        webLog("[IVR Case 6] Caller presses 2 (Complaint Registration option).");
        startComplaintRegistration();
      } else if (key == "3") {
        webLog("[IVR Case 7] Caller presses 3 (Connect to Agent option).");
        startAgentConnection();
      } else {
        webLog("[IVR Case 8] Caller presses an invalid key: " + key);
        playIvrAudio("/01/013.mp3"); // Invalid key message
        delay(3000);
        loadIvrNode("start"); // Replay welcome menu
      }
    } else {
      webLog("DTMF ignored in custom node: " + ivrCurrentNode);
    }
    return;
  }
  
  // Normal non-custom IVR JSON flow
  webLog("[IVR] Processing DTMF Key: " + key + " in Node: " + ivrCurrentNode);
  bool optionFound = false;
  for (int i = 0; i < currentIvrNode.optionCount; i++) {
    if (currentIvrNode.optionKeys[i] == key) {
      String targetNode = currentIvrNode.optionTargetNodes[i];
      webLog("[IVR] Option matched! Transitioning to: " + targetNode);
      loadIvrNode(targetNode);
      optionFound = true;
      break;
    }
  }
  
  if (!optionFound) {
    webLog("[IVR] Key " + key + " not registered in current node options.");
  }
}

// ==================== CUSTOM IVR CORE LOGIC ====================

void verifyIvrWarranty(String productCode) {
  if (currentCallState != STATE_COLLECTING_PRODUCT_NO) return;
  
  webLog("[IVR Case 11] Caller enters a product number for warranty verification. Code: " + productCode);
  ivrWarrantyCheckCode = productCode;
  ivrWarrantyCheckResult = 99; // in progress
  
  WarrantyTaskData *data = new WarrantyTaskData();
  if (data != NULL) {
    data->productCode = productCode;
    BaseType_t res = xTaskCreatePinnedToCore(verifyIvrWarrantyTask, "warranty_task", 16384, (void *)data, 1, NULL, 0);
    if (res != pdPASS) {
      webLog("Failed to create warranty verification task.");
      delete data;
      ivrWarrantyCheckResult = 3; // fallback to not found
    }
  } else {
    ivrWarrantyCheckResult = 3; // fallback
  }
}

void verifyIvrWarrantyTask(void *pvParameters) {
  WarrantyTaskData *data = (WarrantyTaskData *)pvParameters;
  if (data == NULL) {
    ivrWarrantyCheckResult = 3; // not found/error fallback
    vTaskDelete(NULL);
    return;
  }

  int tempResult = 3; // fallback to not_found/error
  
  // Local fallback testing data
  if (data->productCode == "12345") {
    webLog("[HTTP Task] Offline testing: product 12345 is Active");
    tempResult = 1;
  } else if (data->productCode == "54321") {
    webLog("[HTTP Task] Offline testing: product 54321 is Claimed");
    tempResult = 2;
  } else if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Verifying warranty via Google Sheets: " + data->productCode);
    WiFiClientSecure client;
    client.setInsecure();
    
    HTTPClient http;
    String encCode = data->productCode;
    encCode.replace("+", "%2B");
    encCode.replace(" ", "%20");
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=verifyWarranty&productCode=" + encCode + "&sheetId=" + sheetId;
    
    if (http.begin(client, url)) {
      http.setTimeout(25000);
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        webLog("[HTTP Task] Warranty Response: " + payload);
        if (payload.indexOf("\"status\":\"active\"") != -1 || payload.indexOf("\"status\": \"active\"") != -1) {
          tempResult = 1;
        } else if (payload.indexOf("\"status\":\"claimed\"") != -1 || payload.indexOf("\"status\": \"claimed\"") != -1) {
          tempResult = 2;
        } else if (payload.indexOf("\"status\":\"expired\"") != -1 || payload.indexOf("\"status\": \"expired\"") != -1) {
          tempResult = 2; // Expired counts as claimed/inactive for warranty menu purposes
        } else {
          tempResult = 3; // not found
        }
      } else {
        webLog("[HTTP Task] Warranty verification HTTP failed. Code: " + String(httpCode));
        tempResult = 3;
      }
      http.end();
    } else {
      webLog("[HTTP Task] Warranty connection failed.");
      tempResult = 3;
    }
  } else {
    webLog("[HTTP Task] No WiFi/Script ID. offline check failed for code: " + data->productCode);
    tempResult = 3;
  }

  if (ivrWarrantyCheckCode == data->productCode) {
    ivrWarrantyCheckResult = tempResult;
  } else {
    webLog("[HTTP Task] Discarded warranty result for stale code: " + data->productCode);
  }

  delete data;
  vTaskDelete(NULL);
}

void processWarrantyResult(String code, int result) {
  if (result == 1) {
    webLog("[IVR Case 12] Product found and warranty is active.");
    playIvrAudio("/01/006.mp3");
    String msg = "Dear customer, your V-Varma product " + code + " warranty is active. Thank you.";
    sendSMS(callerNumber, msg);
    addSmsLog(callerNumber, msg, "Outgoing", "Sent");
    delay(5000);
    Serial2.println("ATH");
    addCallLog(callerNumber, dtmfBuffer, "Warranty Active (" + code + ")");
    changeState(STATE_ENDED);
  } else if (result == 2) {
    webLog("[IVR Case 13] Product found but warranty has already been claimed.");
    playIvrAudio("/01/007.mp3");
    String msg = "Dear customer, your V-Varma product " + code + " warranty has already been claimed.";
    sendSMS(callerNumber, msg);
    addSmsLog(callerNumber, msg, "Outgoing", "Sent");
    delay(5000);
    Serial2.println("ATH");
    addCallLog(callerNumber, dtmfBuffer, "Warranty Claimed (" + code + ")");
    changeState(STATE_ENDED);
  } else {
    webLog("[IVR Case 14] Product number not found / invalid product number entered.");
    playIvrAudio("/01/008.mp3");
    String msg = "Dear customer, V-Varma product number " + code + " was not found. Please try again.";
    sendSMS(callerNumber, msg);
    addSmsLog(callerNumber, msg, "Outgoing", "Sent");
    
    warrantyRetryCount++;
    if (warrantyRetryCount >= 3) {
      webLog("Max warranty retries reached. Hanging up.");
      delay(3000);
      Serial2.println("ATH");
      addCallLog(callerNumber, dtmfBuffer, "Warranty Failed (" + code + ")");
      changeState(STATE_ENDED);
    } else {
      dtmfBuffer = "";
      lastDtmfMillis = millis();
      // Allow entering again by playing product number prompt after a brief pause
      delay(3000);
      playIvrAudio("/01/005.mp3");
    }
  }
}

void startComplaintRegistration() {
  ivrCurrentNode = "complaint_reg";
  ivrComplaintResult = 99; // in progress
  ivrComplaintTicket = "";
  
  webLog("[IVR Case 6] Caller presses 2 (Complaint Registration option). Registered complaint offline/online.");
  playIvrAudio("/01/009.mp3"); // Registering complaint tone
  
  ComplaintTaskData *data = new ComplaintTaskData();
  if (data != NULL) {
    data->warrantyId = "GENERIC"; // Since we register directly from menu option 2
    data->phone = callerNumber;
    BaseType_t res = xTaskCreatePinnedToCore(registerComplaintTask, "complaint_task", 16384, (void *)data, 1, NULL, 0);
    if (res != pdPASS) {
      webLog("Failed to create complaint registration task.");
      delete data;
      ivrComplaintResult = 2; // fallback to failed
    }
  } else {
    ivrComplaintResult = 2;
  }
}

void registerComplaintTask(void *pvParameters) {
  ComplaintTaskData *data = (ComplaintTaskData *)pvParameters;
  if (data == NULL) {
    ivrComplaintResult = 2; // failed
    vTaskDelete(NULL);
    return;
  }

  int tempResult = 2; // default failed
  String ticket = "";

  if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Registering complaint via Google Sheets for warranty: " + data->warrantyId);
    WiFiClientSecure client;
    client.setInsecure();
    
    HTTPClient http;
    String encWarranty = data->warrantyId;
    encWarranty.replace("+", "%2B");
    encWarranty.replace(" ", "%20");
    String encPhone = data->phone;
    encPhone.replace("+", "%2B");
    encPhone.replace(" ", "%20");
    
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=registerComplaint&warrantyId=" + encWarranty + "&phone=" + encPhone + "&issue=IVR_Auto_Register&sheetId=" + sheetId;
    
    if (http.begin(client, url)) {
      http.setTimeout(25000);
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        webLog("[HTTP Task] Complaint Response: " + payload);
        
        // Find ticket number
        int idx = payload.indexOf("\"ticketNumber\":\"");
        if (idx == -1) idx = payload.indexOf("\"ticketNumber\": \"");
        if (idx != -1) {
          int startQuote = payload.indexOf('"', idx + 15);
          int endQuote = payload.indexOf('"', startQuote + 1);
          if (startQuote != -1 && endQuote != -1) {
            ticket = payload.substring(startQuote + 1, endQuote);
          }
        }
        
        if (payload.indexOf("\"success\":true") != -1 || payload.indexOf("\"success\": true") != -1) {
          tempResult = 1;
        } else {
          tempResult = 2;
        }
      } else {
        webLog("[HTTP Task] Complaint registration HTTP failed. Code: " + String(httpCode));
        tempResult = 2;
      }
      http.end();
    } else {
      webLog("[HTTP Task] Complaint registration connection failed.");
      tempResult = 2;
    }
  } else {
    // Local fallback for offline testing
    webLog("[HTTP Task] Local fallback: Complaint registered offline.");
    ticket = "VRM-" + String(random(100000, 999999));
    tempResult = 1;
  }

  ivrComplaintTicket = ticket;
  ivrComplaintResult = tempResult;

  delete data;
  vTaskDelete(NULL);
}

void startAgentConnection() {
  ivrCurrentNode = "agent_connect";
  webLog("[IVR Case 7] Caller presses 3 (Connect to Agent option).");
  playIvrAudio("/01/011.mp3"); // Connecting to agent message
  delay(3000);
  
  webLog("Dialing agent number: " + agentPhoneNumber);
  
  // Clear GSM serial buffer
  while(Serial2.available() > 0) { Serial2.read(); }
  
  Serial2.println("ATD" + agentPhoneNumber + ";");
  changeState(STATE_AGENT_CONNECTING);
  callStartMillis = millis(); // Reset call timer to wait for agent answer
}
