/**
 * V-VARMA ESP32 A7670C IVR MANAGEMENT SYSTEM
 *
 * Hardware Connections:
 * ─── A7670C GSM Module ────────────────────────────────────────────
 * GPIO16  (Serial2 RX)  →  A7670C TX
 * GPIO17  (Serial2 TX)  →  A7670C RX
 * A7670C VCC            →  4.2 V regulated supply
 * Common GND            →  ESP32 GND
 *
 * ─── LED Indicators & Buzzer ─────────────────────────────────────
 * GPIO2   →  Red  LED   (error / ringing)
 * GPIO13  →  Green LED  (active / connected)
 * GPIO4   →  Blue LED   (SD card status)
 * GPIO25  →  Buzzer     (tone alerts)
 *
 * ─── MAX98357A I2S Audio Amplifier ───────────────────────────────
 * GPIO27  →  BCLK  (Bit Clock)
 * GPIO26  →  LRC   (Word Select / WS)
 * GPIO14  →  DIN   (Data In to amplifier)
 * 3.3V    →  VDD
 * GND     →  GND, SD (pull low for always-on gain)
 *
 * ─── SD Card Reader (SPI) ────────────────────────────────────────
 * GPIO18  →  CLK
 * GPIO19  →  MISO
 * GPIO23  →  MOSI
 * GPIO5   →  CS  (primary, fallbacks: 4, 15, 2)
 *
 * SD-card folder structure:
 *   /01/001.mp3  — Boot jingle (optional)
 *   /01/002.mp3  — Incoming ring
 *   /01/003.mp3  — Call answered
 *   /01/004.mp3  — Call ended
 *   /01/005.mp3  — IVR welcome message
 */

#include "esp_log.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <NetBIOS.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <SPIFFS.h>
#include <Wire.h>
#include "CallbackQueue.h"
#include <esp_mac.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "index_html.h"
#include "wifi_config_html.h"

// Forward definition of WAV Header parsing structure to satisfy Arduino auto-prototypes
struct WavHeaderInfo {
  uint32_t sampleRate = 16000;
  uint16_t channels = 1;
  uint16_t bitsPerSample = 16;
  uint32_t dataSize = 0;
  uint32_t dataStartOffset = 44;
};

// ==================== DEFINITIONS ====================
#define FIRMWARE_VERSION "v1.3-IVR"
#define DEVICE_NAME "vvarmaivr"
#define AP_SSID "V VARMA IVR"
#define AP_PASS "GSVIVR001"

// ─── Continuous Voice Loop (Test Mode) ──────────────────────────────────────────
#define CONTINUOUS_VOICE_LOOP false
#define VOICE_LOOP_PATH "/01/003.wav" // Path to play and loop (WAV format only)

#define GSM_RX_PIN 16
#define GSM_TX_PIN 17
#define GSM_BAUD 115200
#define GSM_PWRKEY_PIN -1 // Disabled (For boards with built-in auto power-on)

// ─── Indicators ────────────────────────────────────────────────────────────────
#define LED_PIN        2   // Red  LED — ringing / error
#define LED_GREEN     13   // Green LED — active / connected / WiFi blink
#define LED_SD_STATUS  4   // SD Card Status LED — GPIO 4
#define BUZZER_PIN    27   // Piezo buzzer (moved from 25 to 27)
#define RESET_BUTTON_PIN 0 // Boot / factory-reset button

// GPIO 25 is used for the internal DAC voice output.
// Buzzer is on GPIO 27. I2S pins (26, 14) are freed.

// ==================== GLOBALS ====================
unsigned long resetBtnStartTime = 0;
bool resetBtnPressed = false;

// Playback flags and mutexes
volatile bool audioPlaying = false;
bool mp3Initialized = false;
bool mp3QueryWorks = false; // Kept for status api compatibility
SemaphoreHandle_t logMutex = NULL;
SemaphoreHandle_t audioMutex = NULL;
FS* myFS = &SD;
volatile bool serialUploadActive = false; // Suppress logging during serial file transfer

// ==================== DAC AUDIO PLAYER INTEGRATION ====================

// Ring Buffer for 8-bit DAC samples (single-producer single-consumer lock-free)
#define RING_BUF_SIZE 4096
volatile uint8_t ringBuffer[RING_BUF_SIZE];
volatile int ringHead = 0;
volatile int ringTail = 0;

inline int ringAvailableForRead() {
  int head = ringHead;
  int tail = ringTail;
  if (head >= tail) {
    return head - tail;
  } else {
    return RING_BUF_SIZE - tail + head;
  }
}

inline int ringAvailableForWrite() {
  int head = ringHead;
  int tail = ringTail;
  int diff = tail - head - 1;
  if (diff < 0) diff += RING_BUF_SIZE;
  return diff;
}

inline void ringWrite(uint8_t val) {
  int head = ringHead;
  ringBuffer[head] = val;
  ringHead = (head + 1) % RING_BUF_SIZE;
}

inline uint8_t ringRead() {
  int tail = ringTail;
  uint8_t val = ringBuffer[tail];
  ringTail = (tail + 1) % RING_BUF_SIZE;
  return val;
}

// Timer and play state control
hw_timer_t *audioTimer = NULL;
volatile bool audioPaused = false;
char nextAudioPath[64] = "";
volatile bool nextAudioPending = false;
volatile bool audioStopPending = false;
uint8_t audioVolume = 30; // Default volume (0 to 30)

// Timer ISR: called at the configured sample rate
void IRAM_ATTR onTimer() {
  if (audioPlaying && !audioPaused) {
    if (ringAvailableForRead() > 0) {
      uint8_t sample = ringRead();
      dacWrite(25, sample);
    } else {
      // Buffer underrun: output silence (mid-scale)
      dacWrite(25, 128);
    }
  } else {
    // Silence output (mid-scale)
    dacWrite(25, 128);
  }
}



// Robust WAV parsing function
bool parseWavFile(File &file, WavHeaderInfo &info) {
  char id[4];
  if (file.read((uint8_t*)id, 4) != 4) return false;
  if (strncmp(id, "RIFF", 4) != 0) return false;
  
  file.seek(8); // Skip file size
  if (file.read((uint8_t*)id, 4) != 4) return false;
  if (strncmp(id, "WAVE", 4) != 0) return false;
  
  bool foundData = false;
  bool foundFmt = false;
  
  while (file.available()) {
    if (file.read((uint8_t*)id, 4) != 4) break;
    uint32_t chunkIdx = file.position();
    uint32_t chunkSize = 0;
    if (file.read((uint8_t*)&chunkSize, 4) != 4) break;
    
    if (strncmp(id, "fmt ", 4) == 0) {
      uint16_t formatType = 0;
      file.read((uint8_t*)&formatType, 2);
      file.read((uint8_t*)&info.channels, 2);
      file.read((uint8_t*)&info.sampleRate, 4);
      file.seek(file.position() + 6); // Skip byte rate and block align
      file.read((uint8_t*)&info.bitsPerSample, 2);
      
      foundFmt = true;
      // Skip the rest of the fmt chunk if there is extra format data
      file.seek(chunkIdx + 4 + chunkSize); 
    } 
    else if (strncmp(id, "data", 4) == 0) {
      info.dataSize = chunkSize;
      info.dataStartOffset = file.position();
      foundData = true;
      break; // Start of audio data reached
    } 
    else {
      // Skip unknown chunks
      file.seek(chunkIdx + 4 + chunkSize);
    }
  }
  
  return foundFmt && foundData;
}

// Audio Task: Decodes WAV data from file and puts it in the ring buffer
void audioTask(void *pvParameters) {
  Serial.println("[AudioTask] Background WAV decoder task started on Core 0.");
  File wavFile;
  WavHeaderInfo wavInfo;
  bool fileOpen = false;
  uint8_t readBuf[512];
  
  while (true) {
    if (audioStopPending) {
      if (fileOpen) {
        wavFile.close();
        fileOpen = false;
      }
      ringHead = 0;
      ringTail = 0;
      audioPlaying = false;
      audioStopPending = false;
      webLog("[AudioTask] Playback stopped.");
    }
    
    if (nextAudioPending) {
      if (fileOpen) {
        wavFile.close();
        fileOpen = false;
      }
      ringHead = 0;
      ringTail = 0;
      
      fs::FS *targetFS = myFS;
      bool found = targetFS->exists(nextAudioPath);
      if (!found) {
        if (targetFS == &SD && SPIFFS.exists(nextAudioPath)) {
          targetFS = &SPIFFS;
          found = true;
          webLog("[AudioTask] File not on SD, using SPIFFS fallback: " + String(nextAudioPath));
        } else if (targetFS == &SPIFFS && SD.exists(nextAudioPath)) {
          targetFS = &SD;
          found = true;
          webLog("[AudioTask] File not on SPIFFS, using SD fallback: " + String(nextAudioPath));
        }
      }
      
      if (found) {
        wavFile = targetFS->open(nextAudioPath, FILE_READ);
        if (wavFile) {
          if (parseWavFile(wavFile, wavInfo)) {
            fileOpen = true;
            audioPlaying = true;
            
            // Adjust timer frequency dynamically based on sample rate
            if (audioTimer) {
              uint32_t alarmVal = 160000 / wavInfo.sampleRate;
              if (alarmVal < 1) alarmVal = 1;
              timerAlarm(audioTimer, alarmVal, true, 0);
            }
            
            webLog("[AudioTask] Playing WAV: " + String(nextAudioPath) + 
                   " | SR: " + String(wavInfo.sampleRate) + "Hz" +
                   " | Ch: " + String(wavInfo.channels) + 
                   " | Bits: " + String(wavInfo.bitsPerSample));
          } else {
            wavFile.close();
            webLog("[AudioTask] ERROR: Invalid WAV header in " + String(nextAudioPath));
            audioPlaying = false;
          }
        } else {
          webLog("[AudioTask] ERROR: Failed to open " + String(nextAudioPath));
          audioPlaying = false;
        }
      } else {
        webLog("[AudioTask] ERROR: File not found: " + String(nextAudioPath));
        audioPlaying = false;
      }
      nextAudioPending = false;
    }
    
    if (fileOpen && audioPlaying) {
      int space = ringAvailableForWrite();
      if (space >= 256) {
        int bytesPerSample = (wavInfo.bitsPerSample / 8) * wavInfo.channels;
        int samplesToRead = space;
        if (wavInfo.channels > 1 || wavInfo.bitsPerSample > 8) {
          samplesToRead = space / bytesPerSample;
          if (samplesToRead > 128) samplesToRead = 128;
        } else {
          if (samplesToRead > 256) samplesToRead = 256;
        }
        
        int bytesToRead = samplesToRead * bytesPerSample;
        if (bytesToRead > sizeof(readBuf)) bytesToRead = sizeof(readBuf);
        
        int bytesRead = wavFile.read(readBuf, bytesToRead);
        if (bytesRead <= 0) {
          wavFile.close();
          fileOpen = false;
          audioPlaying = false;
          webLog("[AudioTask] EOF reached for " + String(nextAudioPath));
        } else {
          int actualSamples = bytesRead / bytesPerSample;
          for (int i = 0; i < actualSamples; i++) {
            int32_t val = 0;
            int offset = i * bytesPerSample;
            
            if (wavInfo.bitsPerSample == 8) {
              uint8_t rawVal = readBuf[offset];
              val = ((int32_t)rawVal - 128) * 256;
            } else if (wavInfo.bitsPerSample == 16) {
              int16_t rawVal = (int16_t)(readBuf[offset] | (readBuf[offset + 1] << 8));
              val = rawVal;
            }
            
            // Apply volume scaling
            int32_t scaled = (val * audioVolume) / 30;
            // Convert to 8-bit unsigned
            uint8_t dacVal = (scaled / 256) + 128;
            ringWrite(dacVal);
          }
        }
      } else {
        vTaskDelay(pdMS_TO_TICKS(5));
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }
}

// API compatible volume mapping (0-30 input)
void mp3SetVolume(uint8_t vol) {
  if (vol > 30) vol = 30;
  audioVolume = vol;
  webLog("[Audio] Volume set to " + String(audioVolume) + "/30");
}

// safePlayAudio: triggers playback of a WAV file
bool safePlayAudio(fs::FS &fs, const char* path) {
  if (serialUploadActive) return false;
  webLog("[Audio] safePlayAudio: " + String(path));
  
  bool fileExists = fs.exists(path);
  if (!fileExists) {
    if (&fs == &SD && SPIFFS.exists(path)) {
      fileExists = true;
      webLog("[Audio] Found file on fallback SPIFFS: " + String(path));
    } else if (&fs == &SPIFFS && SD.exists(path)) {
      fileExists = true;
      webLog("[Audio] Found file on fallback SD: " + String(path));
    }
  }
  
  if (!fileExists) {
    webLog("[Audio] SKIP — file not found on SD or SPIFFS: " + String(path));
    return false;
  }
  
  if (audioMutex != NULL && xSemaphoreTake(audioMutex, pdMS_TO_TICKS(2000)) == pdTRUE) {
    // Pass the path to the background task
    strncpy(nextAudioPath, path, sizeof(nextAudioPath) - 1);
    nextAudioPath[sizeof(nextAudioPath) - 1] = '\0';
    nextAudioPending = true;
    audioPlaying = true; // Set to true immediately so checkAudioFinished works correctly
    xSemaphoreGive(audioMutex);
    return true;
  } else {
    webLog("[Audio] safePlayAudio FAILED to take mutex!");
    return false;
  }
}

// mp3Play: plays tracks from /XX/YYY.wav (or fallback)
void mp3Play(uint8_t folder, uint8_t track) {
  if (!mp3Initialized) {
    webLog("[Audio] mp3Play called before mp3Init — ignoring.");
    return;
  }
  char path[32];
  fs::FS &fs = *myFS;

  // Try .wav → .mp3 → .amr (We only decode .wav, but we check path existence)
  snprintf(path, sizeof(path), "/%02d/%03d.wav", folder, track);
  if (!fs.exists(path)) {
    snprintf(path, sizeof(path), "/%02d/%03d.mp3", folder, track);
    if (fs.exists(path)) {
      webLog("[Audio] Warning: MP3 playback requested, but only WAV is supported. Attempting to play as WAV.");
    } else {
      snprintf(path, sizeof(path), "/%02d/%03d.amr", folder, track);
      if (fs.exists(path)) {
        webLog("[Audio] Warning: AMR playback requested, but only WAV is supported. Attempting to play as WAV.");
      }
    }
  }

  safePlayAudio(fs, path);
}

// mp3Stop: stops playback
void mp3Stop() {
  audioStopPending = true;
  webLog("[Audio] Stop requested.");
}

// mp3PlayAsync: non-blocking play
void mp3PlayAsync(uint8_t folder, uint8_t track) {
  mp3Play(folder, track);
}

// mp3Init: configures pins, starts timer and task
void mp3Init() {
  webLog("[HEAP] mp3Init start. Free: " + String(ESP.getFreeHeap()) + " B");
  
  // Set up DAC pin GPIO 25
  pinMode(25, ANALOG);
  dacWrite(25, 128); // Set to mid-scale (silence)

  // Configure timer at 160kHz
  audioTimer = timerBegin(160000);
  if (audioTimer) {
    timerAttachInterrupt(audioTimer, &onTimer);
    timerAlarm(audioTimer, 10, true, 0); // Default 16kHz
    timerStart(audioTimer);
    webLog("[Audio] Hardware timer started at 160kHz");
  } else {
    webLog("[Audio] ERROR: Failed to start hardware timer!");
  }

  // Spawn audio task on Core 0 (small stack size required: 8KB)
  BaseType_t res = xTaskCreatePinnedToCore(
      audioTask, "audio_task", 8192, NULL, 5, NULL, 0);
  if (res != pdPASS) {
    webLog("[Audio] CRITICAL: Failed to create audioTask!");
  }

  mp3Initialized = true;
  webLog("[Audio] DAC audio engine initialization complete.");
}



CallbackQueue callbackQueue;
Preferences preferences;
DNSServer dnsServer;
WebServer server(80);

// NVS Settings
String savedSSID = "";
String savedPass = "";
String scriptId = "";
String sheetId = "1K8SnQA8H2LVEnxyRTKYfTdLYRe2YHC4qQFqWUyOejQw";
String sheetName = "IVR";
String savedApSSID = "V VARMA IVR";
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
unsigned long wifiConnectStartMillis = 0;
bool wifiConnecting = false;

enum CallState {
  STATE_IDLE,
  STATE_RINGING,
  STATE_VERIFY_USER,
  STATE_ANSWERING,
  STATE_CALL_CONNECTED,
  STATE_PLAY_WELCOME,
  STATE_PLAY_MENU,
  STATE_WAIT_DTMF,
  STATE_PLAY_SELECTION,
  STATE_RETURN_MENU,
  STATE_HANGUP,
  STATE_CALLBACK_DIALING,
  STATE_COLLECTING_PRODUCT_NO,
  STATE_AGENT_CONNECTING,
  STATE_VALIDATING_SECOND_CALL,
  STATE_ENDED,
  STATE_ACTIVE_CALL,
  STATE_COLLECTING_DTMF
};
CallState currentCallState = STATE_IDLE;

// IVR Variables
String callerNumber = "";
String dtmfBuffer = "";
String lastDtmfDigit = "";
String warrantyInputBuffer = "";
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
volatile int ivrSelectedLanguage = 0; // 0: not chosen, 1: English, 2: Tamil, 3: Hindi
bool languageWarningPending = false;
bool languageDisconnectPending = false;
String validatingNumber = "";
unsigned long ringingStartMillis = 0;

// Tracking variables for new IVR call flow
int incomingRingCount = 0;
unsigned long lastAudioPlayMillis = 0;
int dtmfTimeoutCount = 0;
int currentAudioTrack = 0;
int currentAudioFolder = 0;
unsigned long lastActivityTime = 0;

// [OTA] Firmware URL — stored in NVS so it survives reboots.
String otaFirmwareUrl = "https://raw.githubusercontent.com/gsvrnd2025-alt/VVarmaIVR/main/firmware.bin";      // Default raw binary URL
String otaDescriptorUrl = "https://raw.githubusercontent.com/gsvrnd2025-alt/VVarmaIVR/main/version.json";  // Default JSON descriptor URL
String otaStatus      = "idle";  // [OTA] idle | starting | downloading | flashing | success | failed
bool   otaInProgress  = false;   // [OTA] Guard flag — prevents concurrent OTA attempts
bool   otaAutoUpdate  = false;   // [OTA] Automatically check and update at boot

// SIM800L Status variables
bool gsmModuleConnected = false;
String gsmSignal = "0";
String gsmOperator = "Searching...";
String gsmBattery = "0.0V";
String gsmSimStatus = "Checking...";
String gsmRegStatus = "0";
String gsmImei = "";
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
int sdCardFileCount = 0;
int sdCardFolderCount = 0;
bool sdCardUnsupportedFS = false;
int sdCardCS = -1;           // CS pin used for successful SD mount
volatile unsigned long lastSdActivityMillis = 0; // Tracks SD activity for status LED
uint32_t sdCardSectorSize = 0;
uint64_t sdCardNumSectors = 0;
String   sdCardCardType = "NONE";  // NONE, MMC, SD, SDHC, UNKNOWN

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
void otaTask(void* pvParameters);
void triggerOtaUpdate();
void otaAutoCheckTask(void* pvParameters);
void validateTask(void *pvParameters);
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
void updateSDCache();
bool checkAudioFinished(unsigned long maxDurationMs = 60000);
String getDFPlayerCompatiblePath(String originalFilename);
void processNewIvrDtmf(char key);
String getStateName(CallState state);



// ==================== SERIAL LOGGER ====================
void webLog(String msg) {
  if (logMutex != NULL) {
    if (xSemaphoreTake(logMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
      if (!serialUploadActive) {
        Serial.println("[LOG] " + msg);
      }
      webSerialLog += msg + "\n";
      if (webSerialLog.length() > 2048) {
        webSerialLog = webSerialLog.substring(1024); // Truncate old logs if buffer exceeds 2KB
      }
      xSemaphoreGive(logMutex);
    } else {
      if (!serialUploadActive) {
        Serial.println("[LOG_TIMEOUT] " + msg);
      }
    }
  } else {
    if (!serialUploadActive) {
      Serial.println("[LOG_NO_MUTEX] " + msg);
    }
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

// playBootSound() — REPLACED with buzzer-only boot chime.
// I2S/MP3 boot audio was removed to prevent heap fragmentation during the
// critical window between hardware init and Wi-Fi TLS stack allocation.
// Restoring I2S audio at boot caused m_ID3Hdr allocation failures & boot loops.
void playBootSound() {
  webLog("[Boot] Buzzer boot chime (I2S audio skipped to preserve heap).");
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  beep(1047, 100); delay(120);
  beep(1318, 100); delay(120);
  beep(1568, 150); delay(170);
  // NO mp3Play() here — decoder heap safety
  webLog("[HEAP] After boot chime. Free: " + String(ESP.getFreeHeap()) +
         " B | MaxBlock: " + String(ESP.getMaxAllocHeap()) + " B");
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
  bool staActive = (WiFi.status() == WL_CONNECTED && WiFi.localIP() != IPAddress(0, 0, 0, 0));
  bool apActive = (WiFi.softAPIP() != IPAddress(0, 0, 0, 0) && (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA));
  if (!staActive && !apActive) return;

  MDNS.end();
  delay(50);
  if (MDNS.begin(DEVICE_NAME)) {
    MDNS.addService("_http", "_tcp", 80);
    MDNS.addServiceTxt("_http", "_tcp", "id", myMac);
    MDNS.addServiceTxt("_http", "_tcp", "ver", FIRMWARE_VERSION);
    webLog("mDNS responder active at http://" + String(DEVICE_NAME) + ".local");
  } else {
    webLog("CRITICAL: mDNS startup failed");
  }

  // NBNS (NetBIOS Name Service) responds to NetBIOS name queries on port 137.
  // Re-enabled to allow Windows devices to resolve http://vvarmaivr/ without Bonjour.
  NBNS.end();
  NBNS.begin(DEVICE_NAME);
  webLog("NetBIOS responder active at http://" + String(DEVICE_NAME));
}

bool isPrintableASCII(const String& str) {
  if (str.length() == 0 || str.length() > 64) return false;
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (c < 32 || c > 126) return false;
  }
  return true;
}

bool startSoftAPWithLog(const String& ssid, const String& pass) {
  webLog("[WiFi] Starting softAP with SSID: '" + ssid + "' | Password: '" + pass + "' on Channel 1");
  bool success = WiFi.softAP(ssid.c_str(), pass.c_str(), 1, 0, 4);
  if (success) {
    webLog("[WiFi] softAP started successfully. IP: " + WiFi.softAPIP().toString() + " | SSID: '" + ssid + "'");
  } else {
    webLog("[WiFi] CRITICAL: softAP start FAILED!");
  }
  return success;
}

void startAPMode() {
  currentSystemState = SYSTEM_WIFI_CONFIG;
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);
  delay(200);
  WiFi.mode(WIFI_AP);
  delay(100);
  startSoftAPWithLog(savedApSSID, savedApPass);
  delay(1000);
  IPAddress apIP = WiFi.softAPIP();
  localIpStr = apIP.toString();

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);
  
  setupWebServer();
  mDNS_begin();
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
    
    // Heap allocate to prevent stack overflow
    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *http = new HTTPClient();
    
    if (client != nullptr && http != nullptr) {
      client->setInsecure(); // Bypass SSL verification
      
      // URL Encoding + as %2B in case there is a +
      String encPhone = data->phoneNumber;
      encPhone.replace("+", "%2B");
      String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=validateUser&phone=" + encPhone + "&sheetId=" + sheetId;
      
      if (http->begin(*client, url)) {
        http->setTimeout(25000); // 25 seconds timeout for caller validation
        http->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http->GET();
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http->getString();
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
        http->end();
      } else {
        webLog("[HTTP Task] Validation connection failed.");
        tempResult = 0;
      }
    } else {
      webLog("[HTTP Task] Memory allocation failed in validation task.");
      tempResult = 0;
    }
    
    if (client != nullptr) delete client;
    if (http != nullptr) delete http;
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
    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *http = new HTTPClient();
    
    if (client != nullptr && http != nullptr) {
      client->setInsecure();
      String url = "https://script.google.com/macros/s/" + scriptId + "/exec";
      
      if (http->begin(*client, url)) {
        http->setTimeout(25000); // 25 seconds timeout for SMS sync
        http->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        http->addHeader("Content-Type", "application/json");
        
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
        
        int httpCode = http->POST(jsonStr);
        if (httpCode == 200 || httpCode == 302) {
          webLog("[HTTP Task] SMS Sync status code: " + String(httpCode));
        } else {
          webLog("[HTTP Task] SMS Sync POST failure: " + (httpCode > 0 ? String(httpCode) : http->errorToString(httpCode)));
        }
        http->end();
      }
    } else {
      webLog("[HTTP Task] Memory allocation failed for SMS sync client/http");
    }
    
    if (client != nullptr) delete client;
    if (http != nullptr) delete http;
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
    xTaskCreatePinnedToCore(syncSmsTask, "sms_sync_task", 6144, (void *)data, 1, NULL, 0);
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
    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *http = new HTTPClient();
    
    if (client != nullptr && http != nullptr) {
      client->setInsecure(); // Bypass SSL verification
      String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=logCall";
      
      if (http->begin(*client, url)) {
        http->setTimeout(25000); // 25 seconds timeout for Sheets event sync
        http->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        http->addHeader("Content-Type", "application/json");
        
        StaticJsonDocument<400> doc;  // Increased from 300 to 400 for all fields
        doc["action"] = "logCall";    // Tell GScript router to log this as a call record
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
        
        int httpCode = http->POST(jsonStr);
        if (httpCode == 200 || httpCode == 302) {
          webLog("[HTTP Task] Sync status code: " + String(httpCode));
          sheetsSyncCount++;
          sheetsLastSyncTime = data->timeStr;
        } else {
          webLog("[HTTP Task] Sync POST failure: " + (httpCode > 0 ? String(httpCode) : http->errorToString(httpCode)));
          sheetsSyncErrors++;
        }
        http->end();
      } else {
        webLog("[HTTP Task] Unable to resolve endpoint URL");
      }
    } else {
      webLog("[HTTP Task] Memory allocation failed for sync client/http");
    }
    
    if (client != nullptr) delete client;
    if (http != nullptr) delete http;
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
    
    // Allocate 6KB stack on core 0 to handle HTTPS handshake processes (heap allocated clients)
    BaseType_t res = xTaskCreatePinnedToCore(syncTask, "g_sync_task", 6144, (void *)data, 1, NULL, 0);
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
  webLog("[STATE CHANGE] " + getStateName(oldState) + " -> " + getStateName(newState));

  switch(newState) {
    case STATE_IDLE:
      digitalWrite(LED_PIN, HIGH); // Onboard LED HIGH as power indicator
      digitalWrite(LED_GREEN, LOW);
      mp3Stop();                     // Stop any playing track
      delay(150);                    // Stabilize DFPlayer after stop
      // --- Full call-state reset ---
      validatingNumber       = "";
      validationResult       = -1;
      ivrSelectedLanguage    = 0;
      languageWarningPending = false;
      languageDisconnectPending = false;
      callerNumber           = "";   // Clear stale caller
      dtmfBuffer             = "";   // Clear DTMF buffer
      lastDtmfDigit          = "";   // Clear last digit
      warrantyInputBuffer    = "";   // Clear warranty input buffer
      callerIsRegistered     = false;// Reset registered flag
      welcomeMenuPending     = false;// Cancel any pending welcome-menu delay
      ivrCurrentNode         = "start"; // Reset IVR node pointer
      ivrWarrantyCheckResult = 0;    // Reset warranty check state
      ivrComplaintResult     = 0;    // Reset complaint state
      incomingRingCount      = 0;    // Reset ring count
      dtmfTimeoutCount       = 0;    // Reset DTMF timeout count
      currentAudioTrack      = 0;
      currentAudioFolder     = 0;
      // Flush GSM serial buffer
      while (Serial2.available() > 0) { Serial2.read(); }
      Serial2.println("AT+CLIP=1"); // Re-arm caller ID notification for next call
      break;

    case STATE_VERIFY_USER:
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(LED_GREEN, LOW);
      break;

    case STATE_RINGING:
      digitalWrite(LED_PIN, HIGH);   // Red LED on — incoming call
      digitalWrite(LED_GREEN, LOW);
      beep(1000, 300);               // Short buzzer alert
      mp3PlayAsync(1, 2);            // /01/002.mp3 — ring tone
      break;

    case STATE_ANSWERING:
    case STATE_PLAY_WELCOME:
    case STATE_PLAY_MENU:
    case STATE_WAIT_DTMF:
    case STATE_PLAY_SELECTION:
    case STATE_RETURN_MENU:
    case STATE_HANGUP:
      digitalWrite(LED_PIN, HIGH); // Onboard LED solid HIGH as power indicator
      digitalWrite(LED_GREEN, HIGH);
      break;

    case STATE_CALL_CONNECTED:
      digitalWrite(LED_PIN, HIGH); // Onboard LED solid HIGH as power indicator
      digitalWrite(LED_GREEN, HIGH);
      webLog("Call connected. Playing Language Selection Menu...");
      currentAudioFolder = 4;
      currentAudioTrack = 2; // Language selection menu WAV
      mp3Play(currentAudioFolder, currentAudioTrack);
      lastAudioPlayMillis = millis();
      changeState(STATE_WAIT_DTMF);
      break;

    case STATE_ENDED:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_PIN, HIGH); // Onboard LED stays HIGH as power indicator
      mp3PlayAsync(1, 4);            // /01/004.mp3 — "call ended" tone
      beep(800, 200);
      delay(200);
      digitalWrite(LED_PIN, HIGH); // Onboard LED stays HIGH as power indicator
      
      // Follow-up SMS check
      if (testModeEnabled && testAutoSms && (callerNumber.indexOf(testPhoneNumber) != -1 || testPhoneNumber.indexOf(callerNumber) != -1)) {
        webLog("[DEBUG] Sending confirmation SMS to debug target: " + testSmsTemplate);
        sendSMS(callerNumber, testSmsTemplate);
        addSmsLog(callerNumber, testSmsTemplate, "Outgoing", "Sent");
      } else if (smsSystemActive) {
        bool isRegistered = callerIsRegistered || (validationResult == 1);
        
        // 1. Answered but hung up without selecting option
        if (isRegistered && 
            (oldState == STATE_PLAY_WELCOME || oldState == STATE_PLAY_MENU || oldState == STATE_WAIT_DTMF || oldState == STATE_PLAY_SELECTION || oldState == STATE_COLLECTING_DTMF) && 
            dtmfBuffer.length() == 0) {
          String msg = "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in";
          sendSMS(callerNumber, msg);
          addSmsLog(callerNumber, msg, "Outgoing", "Sent");
        } 
        // 2. Incoming call was missed or line busy (for registered user)
        else if (isRegistered && (oldState == STATE_RINGING || oldState == STATE_VERIFY_USER || oldState == STATE_VALIDATING_SECOND_CALL)) {
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
      digitalWrite(LED_PIN, HIGH); // Onboard LED solid HIGH as power indicator
      digitalWrite(LED_GREEN, HIGH);
      beep(1200, 200);
      break;

    case STATE_COLLECTING_PRODUCT_NO:
    case STATE_AGENT_CONNECTING:
    case STATE_VALIDATING_SECOND_CALL:
      digitalWrite(LED_PIN, HIGH); // Onboard LED solid HIGH as power indicator
      digitalWrite(LED_GREEN, HIGH);
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
  bool gotError = false;
  
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
        gotError = true;
        break;
      }
    }
    if (success || gotError) break;
    delay(10);
  }
  
  static int consecutiveFailures = 0;
  if (success || gotError) {
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
      
      // Prevent duplicate processing of the same caller while ringing/verifying, but count the rings
      if (currentCallState != STATE_IDLE && (tempNumber == callerNumber || tempNumber == validatingNumber)) {
        incomingRingCount++;
        webLog("Repeat ring/CLIP from current caller. Ring count: " + String(incomingRingCount));
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
        incomingRingCount = 1;
        
        webLog("Incoming call. Answering immediately to present Language Selection Menu...");
        validationResult = -1;
        ivrSelectedLanguage = 0;
        changeState(STATE_ANSWERING);
      } else {
        webLog("Line busy. Rejecting concurrent call from: " + tempNumber + " (current state: " + getStateName(currentCallState) + ")");
        Serial2.println("ATH");
      }
    }
  }
  
  // RING detected
  else if (line.equals("RING")) {
    gsmModuleConnected = true; // Auto-recover status since URC was received
    webLog("RING detected.");
    if (currentCallState == STATE_RINGING || currentCallState == STATE_VERIFY_USER || currentCallState == STATE_ANSWERING) {
      incomingRingCount++;
      webLog("Ring count: " + String(incomingRingCount));
    }
  }
  
  // DTMF Tone captured
  else if (line.startsWith("+DTMF:") || line.startsWith("+RXDTMF:")) {
    int colon = line.indexOf(':');
    if (colon != -1) {
      String key = line.substring(colon + 1);
      key.trim();
      if (key.length() > 0) {
        char keyChar = key.charAt(0);
        lastDtmfDigit = key;
        dtmfBuffer += key;
        lastDtmfMillis = millis();
        lastActivityTime = millis();
        webLog("DTMF Tone: " + key + " | Buffer: " + dtmfBuffer);
        
        if (currentCallState == STATE_WAIT_DTMF || 
            currentCallState == STATE_PLAY_MENU || 
            currentCallState == STATE_PLAY_WELCOME || 
            currentCallState == STATE_PLAY_SELECTION ||
            currentCallState == STATE_RETURN_MENU ||
            currentCallState == STATE_CALL_CONNECTED) {
          processNewIvrDtmf(keyChar);
        } else {
          changeState(STATE_COLLECTING_DTMF);
          processIvrDtmf(key);
        }
      }
    }
  }
  
  // Remote Hang Up
  else if (line.equals("NO CARRIER") || line.equals("BUSY") || line.equals("NO ANSWER") || line.equals("NO DIALTONE") || line.indexOf("VOICE CALL: END") != -1) {
    webLog("Remote call terminated.");
    if (customIvrEnabled && currentCallState == STATE_AGENT_CONNECTING) {
      webLog("[IVR Case 18] Caller requested agent, but agent call failed: " + line);
      playIvrAudio("/01/012.mp3"); // Agent unavailable
      delay(5000);
      Serial2.println("ATH");
      addCallLog(callerNumber, dtmfBuffer, "Agent Call Failed (" + line + ")");
      changeState(STATE_ENDED);
    } else if (currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_WAIT_DTMF || currentCallState == STATE_COLLECTING_DTMF || currentCallState == STATE_COLLECTING_PRODUCT_NO) {
      webLog("[IVR] Active call hung up.");
      addCallLog(callerNumber, dtmfBuffer, callerIsRegistered ? "Callback Completed" : "Answered");
      changeState(STATE_ENDED);
    } else if (currentCallState == STATE_CALLBACK_DIALING) {
      webLog("Callback dialing rejected or busy.");
      addCallLog(callerNumber, "", "Callback Busy/Rejected");
      changeState(STATE_ENDED);
    } else if (currentCallState == STATE_RINGING || currentCallState == STATE_VERIFY_USER) {
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
                sendATCommand("AT+DDET=1", "OK", 1000); // Enable DTMF detector (SIM800L)
                sendATCommand("AT+DTMFDET=1", "OK", 1000); // Enable DTMF detector (A7670C)
                callStartMillis = millis();
                lastDtmfMillis = millis();
                callerIsRegistered = true;
                changeState(STATE_CALL_CONNECTED);
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
  
  // Registration status parser (+CREG, +CEREG, +CGREG)
  else if (line.startsWith("+CREG:") || line.startsWith("+CEREG:") || line.startsWith("+CGREG:")) {
    int colon = line.indexOf(':');
    String stat = "";
    if (colon != -1) {
      String suffix = line.substring(colon + 1);
      suffix.trim();
      int comma = suffix.indexOf(',');
      if (comma != -1) {
        // Extract registration status field
        stat = suffix.substring(comma + 1);
        stat.trim();
        int nextComma = stat.indexOf(',');
        if (nextComma != -1) {
          stat = stat.substring(0, nextComma);
          stat.trim();
        }
      } else {
        stat = suffix;
      }
    }
    
    // Store 2G vs LTE registration separately to avoid overwriting each other
    static String gsmReg2G = "0";
    static String gsmRegLTE = "0";
    
    if (line.startsWith("+CREG:")) {
      gsmReg2G = stat;
    } else if (line.startsWith("+CEREG:")) {
      gsmRegLTE = stat;
    } else { // +CGREG
      gsmReg2G = stat; 
    }
    
    // Unified status logic: registered if EITHER 2G OR LTE reports home (1) or roaming (5)
    // Important: once registered, do NOT let a non-LTE response (0) overwrite a valid 2G registration
    if (gsmReg2G == "1" || gsmReg2G == "5") {
      gsmRegStatus = gsmReg2G;   // 2G registered — definitive
    } else if (gsmRegLTE == "1" || gsmRegLTE == "5") {
      gsmRegStatus = gsmRegLTE;  // LTE registered — definitive
    } else {
      gsmRegStatus = gsmReg2G;   // Both unregistered — use 2G stat as canonical value
    }
    
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
  
  // IMEI parser (response to AT+CGSN)
  else if (line.length() == 15 && line.charAt(0) >= '0' && line.charAt(0) <= '9') {
    bool allDigits = true;
    for (int i = 0; i < line.length(); i++) {
      if (!isDigit(line.charAt(i))) { allDigits = false; break; }
    }
    if (allDigits) {
      gsmImei = line;
      webLog("GSM IMEI captured: " + gsmImei);
    }
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

  webLog("[Proxy] Running Sheets proxy synchronously...");
  
  uint32_t freeH = ESP.getFreeHeap();
  uint32_t freeInternal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  uint32_t maxH = ESP.getMaxAllocHeap();
  uint32_t maxInternal = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
  
  IPAddress googleIP;
  bool dnsOK = WiFi.hostByName("script.google.com", googleIP);
  webLog("[Proxy] DNS lookup for script.google.com: " + String(dnsOK ? "SUCCESS (" + googleIP.toString() + ")" : "FAILED"));
  webLog("[Proxy] Network IP: " + WiFi.localIP().toString() + 
         " | Gateway: " + WiFi.gatewayIP().toString() + 
         " | Subnet: " + WiFi.subnetMask().toString() + 
         " | DNS: " + WiFi.dnsIP().toString());

  WiFiClientSecure *client = new WiFiClientSecure();
  HTTPClient *http = new HTTPClient();
  int code = -2;
  bool streamed = false;

  if (client != nullptr && http != nullptr) {
    client->setInsecure(); // Bypass SSL verification
    String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=" + action + "&mac=" + myMac + "&user=" + deviceUser + "&pass=" + devicePass + "&sheetId=" + sheetId;
    
    webLog("[Proxy] Connecting to Google Sheets...");
    http->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    if (http->begin(*client, url)) {
      http->setTimeout(45000); // 45 seconds timeout
      code = http->GET();
      char sslErr[128] = "";
      client->lastError(sslErr, sizeof(sslErr));
      webLog("[Proxy] HTTP Code: " + String(code) + " | SSL last error: " + String(sslErr));
      if (code == 200 || code == 302) {
        String payload = http->getString();
        server.send(200, "application/json", payload);
        streamed = true;
      }
      http->end();
    } else {
      code = -1;
      char sslErr[128] = "";
      client->lastError(sslErr, sizeof(sslErr));
      webLog("[Proxy] Connection failed. SSL last error: " + String(sslErr));
    }
  } else {
    webLog("[Proxy] Memory allocation failed for client/http");
  }

  if (client != nullptr) delete client;
  if (http != nullptr) delete http;

  proxyInProgress = false;
  
  if (!streamed) {
    server.send(200, "application/json", "[]");
  }
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
  size_t length = strlen_P(data);   // PROGMEM-safe: reads length from flash
  size_t chunk_size = 2048;
  size_t sent = 0;
  while (sent < length) {
    size_t to_send = length - sent;
    if (to_send > chunk_size) {
      to_send = chunk_size;
    }
    char buffer[2049];
    memcpy_P(buffer, data + sent, to_send);  // PROGMEM-safe: copies from flash to RAM
    buffer[to_send] = '\0';
    server.sendContent(buffer);
    sent += to_send;
    yield();
  }
}

int getFileCountHelper(File dir) {
  int count = 0;
  File file = dir.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      count += getFileCountHelper(file);
    } else {
      count++;
    }
    file.close();
    file = dir.openNextFile();
  }
  return count;
}

int getFileCount() {
  if (!sdCardFound) return 0;
  File root = myFS->open("/");
  if (!root) return 0;
  int count = getFileCountHelper(root);
  root.close();
  return count;
}

int getFolderCountHelper(File dir) {
  int count = 0;
  File file = dir.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      count++;
      count += getFolderCountHelper(file);
    }
    file.close();
    file = dir.openNextFile();
  }
  return count;
}

int getFolderCount() {
  if (!sdCardFound) return 0;
  File root = myFS->open("/");
  if (!root) return 0;
  int count = getFolderCountHelper(root);
  root.close();
  return count;
}

void updateSDCache() {
  lastSdActivityMillis = millis();
  if (sdCardFound) {
    sdCardTotalBytes = getTotalBytes();
    sdCardUsedBytes = getUsedBytes();
    sdCardFileCount = getFileCount();
    sdCardFolderCount = getFolderCount();
    sdCardUnsupportedFS = false;
  } else {
    sdCardTotalBytes = 0;
    sdCardUsedBytes = 0;
    sdCardFileCount = 0;
    sdCardFolderCount = 0;
    if (SD.cardType() != CARD_NONE) {
      sdCardUnsupportedFS = true;
    } else {
      sdCardUnsupportedFS = false;
    }
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
      String fname = String(file.path());
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
    lastSdActivityMillis = millis();
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      String filename = upload.filename;
      if (!filename.startsWith("/")) filename = "/" + filename;
      webLog("SD Upload End: " + filename + " Size: " + String(upload.totalSize));
      updateSDCache();
    }
  }
}

void handleCaptivePortalRedirect() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/html", "<script>window.location.href='http://192.168.4.1/';</script>");
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
      if (host.length() > 0 && host.indexOf("192.168.4.1") == -1 && host.indexOf("vvarmaivr") == -1) {
        handleCaptivePortalRedirect();
        return;
      }
      // Serve wifi_config_html directly to allow offline captive portal configuration
      String html = String(wifi_config_html);
      html.replace("V VARMA IVR", savedApSSID);
      server.send(200, "text/html", html);
    } else {
      // SYSTEM_RUNNING: redirect to /dashboard regardless of how we got here (STA IP or AP IP)
      server.sendHeader("Location", "/dashboard", true);
      server.send(302, "text/plain", "");
    }
  });

  // Redirects for captive portal redirection
  server.on("/generate_204", handleCaptivePortalRedirect);
  server.on("/hotspot-detect.html", handleCaptivePortalRedirect);
  server.on("/library/test/success.html", handleCaptivePortalRedirect);
  server.on("/success.html", handleCaptivePortalRedirect);
  server.on("/ncsi.txt", handleCaptivePortalRedirect);
  server.on("/connecttest.txt", handleCaptivePortalRedirect);
  server.on("/wpad.dat", handleCaptivePortalRedirect);

  server.on("/dashboard", []() {
    // Always serve the dashboard — AP clients and STA clients both need access
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");

    bool fileServed = false;
    if (sdCardFound && myFS) {
      File f = myFS->open("/index.html", FILE_READ);
      if (f) {
        char buf[1025];
        while (f.available()) {
          int n = f.read((uint8_t*)buf, 1024);
          if (n > 0) {
            buf[n] = '\0';
            server.sendContent(buf);
          }
        }
        f.close();
        fileServed = true;
      }
    }

    if (!fileServed) {
      sendLargePage(index_html);
    }
    server.sendContent(""); // End of chunked response
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
    
    switch(currentCallState) {
      case STATE_IDLE:             doc["state"] = "IDLE"; break;
      case STATE_RINGING:          doc["state"] = "RINGING"; break;
      case STATE_VERIFY_USER:      doc["state"] = "VERIFY USER"; break;
      case STATE_ANSWERING:        doc["state"] = "ANSWERING"; break;
      case STATE_CALL_CONNECTED:   doc["state"] = "CALL CONNECTED"; break;
      case STATE_PLAY_WELCOME:     doc["state"] = "PLAY WELCOME"; break;
      case STATE_PLAY_MENU:        doc["state"] = "PLAY MENU"; break;
      case STATE_WAIT_DTMF:        doc["state"] = "WAIT DTMF"; break;
      case STATE_PLAY_SELECTION:   doc["state"] = "PLAY SELECTION"; break;
      case STATE_RETURN_MENU:      doc["state"] = "RETURN MENU"; break;
      case STATE_HANGUP:           doc["state"] = "HANGUP"; break;
      case STATE_CALLBACK_DIALING: doc["state"] = "CALLBACK DIALING"; break;
      case STATE_COLLECTING_PRODUCT_NO: doc["state"] = "WARRANTY INPUT"; break;
      case STATE_AGENT_CONNECTING: doc["state"] = "AGENT CONNECTING"; break;
      case STATE_VALIDATING_SECOND_CALL: doc["state"] = "VALIDATING SECOND CALL"; break;
      case STATE_ENDED:            doc["state"] = "ENDED"; break;
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
    gsmObj["imei"] = (gsmImei.length() > 0) ? gsmImei : "Fetching...";
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
      sdObj["type"] = (myFS == &SD) ? "SD" : "SPIFFS";
      sdObj["totalBytes"] = sdCardTotalBytes;
      sdObj["usedBytes"] = sdCardUsedBytes;
      sdObj["fileCount"] = sdCardFileCount;
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
    if (currentCallState == STATE_RINGING || currentCallState == STATE_VERIFY_USER) {
      webLog("Answer command issued via local Web server. Transitioning to STATE_ANSWERING...");
      changeState(STATE_ANSWERING);
      server.send(200, "application/json", "{\"status\":\"success\"}");
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
      if (currentCallState == STATE_RINGING || currentCallState == STATE_VERIFY_USER) {
        addCallLog(callerNumber, "", "Rejected");
      } else if (currentCallState == STATE_ANSWERING || currentCallState == STATE_CALL_CONNECTED || currentCallState == STATE_PLAY_WELCOME || currentCallState == STATE_PLAY_MENU || currentCallState == STATE_WAIT_DTMF || currentCallState == STATE_PLAY_SELECTION || currentCallState == STATE_RETURN_MENU || currentCallState == STATE_HANGUP || currentCallState == STATE_COLLECTING_DTMF) {
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

  server.on("/api/test_mp3", []() {
    String action = server.hasArg("action") ? server.arg("action") : "play";
    if (action == "stop") {
      mp3Stop();
      webLog("[API TEST] Stopping MP3 playback");
      server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Stopped playback\"}");
    } else if (action == "diagnose") {
      // Raw serial protocol is write-only — return cached/known values
      int vol = -1, eq = -1, filesSD = -1, state = -1;
      if (mp3Initialized) {
        vol = 25; // Cached value (we set 25 at init)
        eq = 0;   // Normal EQ (default)
        filesSD = -1; // Not queryable in raw mode
        state = -1;   // Not queryable in raw mode
      }
      StaticJsonDocument<256> doc;
      doc["status"]      = "success";
      doc["volume"]      = vol;
      doc["eq"]          = eq;
      doc["filesSD"]     = filesSD;
      doc["state"]       = state;
      doc["initialized"] = mp3Initialized;
      doc["queryWorks"]  = mp3QueryWorks;
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      int folder = server.hasArg("folder") ? server.arg("folder").toInt() : 1;
      int track  = server.hasArg("track")  ? server.arg("track").toInt()  : 1;
      webLog("[API TEST] Playing MP3 Folder: " + String(folder) + ", Track: " + String(track));
      mp3PlayAsync(folder, track); // fire-and-forget, won't block web server
      server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Playing folder " + String(folder) + ", track " + String(track) + "\"}");
    }
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
      
      bool shouldReboot = true;
      if (server.hasArg("reboot")) {
        shouldReboot = (server.arg("reboot") == "true" || server.arg("reboot") == "1");
      }
      
      server.send(200, "application/json", "{\"success\":true}");
      if (shouldReboot) {
        delay(2000);
        ESP.restart();
      }
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
    StaticJsonDocument<512> doc;
    doc["found"]       = sdCardFound;
    bool isSD          = (myFS == &SD);
    doc["type"]        = isSD ? "SD" : "SPIFFS";
    doc["total"]       = sdCardTotalBytes;
    doc["used"]        = sdCardUsedBytes;
    doc["free"]        = sdCardTotalBytes - sdCardUsedBytes;
    doc["fileCount"]   = sdCardFileCount;
    doc["folderCount"] = sdCardFolderCount;
    doc["unsupported"] = sdCardUnsupportedFS;
    
    // Card health status
    if (sdCardFound) {
      doc["status"] = "Healthy";
    } else if (sdCardUnsupportedFS) {
      doc["status"] = "Unsupported Filesystem (Requires Formatting)";
    } else {
      doc["status"] = "Not Inserted / Wiring Error";
    }
    
    doc["cardType"]    = sdCardCardType;
    doc["csPin"]       = sdCardCS;
    doc["sectorSize"]  = isSD ? 512 : 0;
    doc["numSectors"]  = isSD ? (uint64_t)(SD.cardSize() / 512) : 0;
    doc["cardSizeMB"]  = isSD ? (uint64_t)(SD.cardSize() / (1024 * 1024)) : 0;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // Remount SD card endpoint — call from dashboard when SD is showing SPIFFS fallback or format is requested
  server.on("/api/remount_sd", HTTP_POST, []() {
    webLog("[SD] Web-triggered SD remount requested...");
    
    // End existing mounts
    SD.end();
    delay(100);
    
    sdCardFound = false;
    sdCardCS = -1;
    sdCardCardType = "NONE";
    sdCardTotalBytes = 0;
    sdCardUsedBytes = 0;
    sdCardSectorSize = 0;
    sdCardNumSectors = 0;
    
    // Re-attempt SD mount
    int csPins2[] = {5, 4, 15, 2};
    uint32_t spiSpeeds2[] = {1000000U, 4000000U, 8000000U};
    bool remounted = false;
    
    for (int si = 0; si < 3 && !remounted; si++) {
      for (int i = 0; i < 4 && !remounted; i++) {
        int cs = csPins2[i];
        SD.end(); delay(50);
        if (SD.begin(cs, SPI, spiSpeeds2[si])) {
          uint64_t tot = SD.totalBytes();
          if (tot > 0) {
            myFS = &SD;
            sdCardFound = true;
            sdCardCS = cs;
            sdCardTotalBytes = tot;
            sdCardUsedBytes = SD.usedBytes();
            sdCardSectorSize = 512;
            sdCardNumSectors = SD.cardSize() / 512;
            uint8_t ctype = SD.cardType();
            if      (ctype == CARD_MMC)  sdCardCardType = "MMC";
            else if (ctype == CARD_SD)   sdCardCardType = "SD";
            else if (ctype == CARD_SDHC) sdCardCardType = "SDHC";
            else                          sdCardCardType = "UNKNOWN";
            remounted = true;
            webLog("[SD] Remount SUCCESS: CS=" + String(cs) + " Type=" + sdCardCardType + " Size=" + String(tot/1048576) + "MB");
          } else { SD.end(); }
        }
      }
    }
    
    if (!remounted) {
      webLog("[SD] Remount FAILED — staying on SPIFFS fallback");
      sdCardCardType = "SPIFFS";
    }
    
    updateSDCache();
    
    StaticJsonDocument<256> doc;
    doc["success"]  = remounted;
    doc["type"]     = remounted ? sdCardCardType : "SPIFFS";
    doc["total"]    = sdCardTotalBytes;
    doc["used"]     = sdCardUsedBytes;
    doc["cardType"] = sdCardCardType;
    doc["csPin"]    = sdCardCS;
    doc["cardSizeMB"] = remounted ? (uint64_t)(SD.cardSize() / (1024 * 1024)) : 0;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // Dedicated Format SD endpoint
  server.on("/api/format_sd", HTTP_POST, []() {
    webLog("[SD] Web-triggered SD format requested...");
    
    mp3Stop();
    delay(100);
    
    bool formatSuccess = false;
    int cs = (sdCardCS != -1) ? sdCardCS : 5;
    
    // 1. Write zeroes to sector 0 if card is mounted to trigger format_if_empty
    if (sdCardFound && myFS == &SD) {
      uint8_t zeroBuf[512] = {0};
      webLog("[SD] Zeroing sector 0 to force formatting...");
      if (SD.writeRAW(zeroBuf, 0)) {
        webLog("[SD] Sector 0 zeroed.");
      } else {
        webLog("[SD] writeRAW failed.");
      }
    }
    
    // 2. Unmount card
    SD.end();
    delay(200);
    sdCardFound = false;
    
    // 3. Mount with format_if_empty = true
    webLog("[SD] Formatting and mounting with format_if_empty=true...");
    if (SD.begin(cs, SPI, 4000000, "/sd", 5, true)) {
      uint64_t tot = SD.totalBytes();
      if (tot > 0) {
        myFS = &SD;
        sdCardFound = true;
        sdCardCS = cs;
        sdCardTotalBytes = tot;
        sdCardUsedBytes = SD.usedBytes();
        sdCardSectorSize = 512;
        sdCardNumSectors = SD.cardSize() / 512;
        uint8_t ctype = SD.cardType();
        if      (ctype == CARD_MMC)  sdCardCardType = "MMC";
        else if (ctype == CARD_SD)   sdCardCardType = "SD";
        else if (ctype == CARD_SDHC) sdCardCardType = "SDHC";
        else                          sdCardCardType = "UNKNOWN";
        sdCardUnsupportedFS = false;
        formatSuccess = true;
        webLog("[SD] Format successful. Card mounted.");
      }
    }
    
    if (formatSuccess) {
      // Re-create necessary directories
      SD.mkdir("/01");
      SD.mkdir("/04");
      updateSDCache();
      
      StaticJsonDocument<128> doc;
      doc["success"] = true;
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      webLog("[SD] Format failed.");
      StaticJsonDocument<128> doc;
      doc["success"] = false;
      doc["error"] = "Format failed";
      String response;
      serializeJson(doc, response);
      server.send(500, "application/json", response);
    }
  });

  // Folder creation endpoint
  server.on("/create_folder", HTTP_POST, []() {
    if (!sdCardFound) {
      server.send(404, "application/json", "{\"success\":false,\"error\":\"SD Card not mounted\"}");
      return;
    }
    if (server.hasArg("path")) {
      String path = server.arg("path");
      if (!path.startsWith("/")) path = "/" + path;
      if (myFS->mkdir(path)) {
        updateSDCache();
        server.send(200, "application/json", "{\"success\":true}");
      } else {
        server.send(500, "application/json", "{\"success\":false,\"error\":\"Failed to create directory\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Path required\"}");
    }
  });

  // Folder deletion endpoint
  server.on("/delete_folder", HTTP_POST, []() {
    if (!sdCardFound) {
      server.send(404, "application/json", "{\"success\":false,\"error\":\"SD Card not mounted\"}");
      return;
    }
    if (server.hasArg("path")) {
      String path = server.arg("path");
      if (!path.startsWith("/")) path = "/" + path;
      if (myFS->rmdir(path)) {
        updateSDCache();
        server.send(200, "application/json", "{\"success\":true}");
      } else {
        server.send(500, "application/json", "{\"success\":false,\"error\":\"Failed to delete directory (must be empty)\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Path required\"}");
    }
  });

  // File download endpoint
  server.on("/download_file", HTTP_GET, []() {
    if (!sdCardFound) {
      server.send(404, "text/plain", "SD Card not mounted");
      return;
    }
    if (server.hasArg("file")) {
      String path = server.arg("file");
      lastSdActivityMillis = millis();
      if (!path.startsWith("/")) path = "/" + path;
      if (myFS->exists(path)) {
        File file = myFS->open(path, FILE_READ);
        String contentType = "application/octet-stream";
        if (path.endsWith(".mp3")) contentType = "audio/mpeg";
        else if (path.endsWith(".wav")) contentType = "audio/wav";
        else if (path.endsWith(".json")) contentType = "application/json";
        else if (path.endsWith(".txt")) contentType = "text/plain";
        
        server.streamFile(file, contentType);
        file.close();
      } else {
        server.send(404, "text/plain", "File Not Found");
      }
    } else {
      server.send(400, "text/plain", "File argument required");
    }
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
    lastSdActivityMillis = millis();
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
    updateSDCache();
    
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
      lastSdActivityMillis = millis();
      if (!f.startsWith("/")) f = "/" + f;

      // Safety check to prevent OOM crash when previewing large binary files
      String fLower = f;
      fLower.toLowerCase();
      if (fLower.endsWith(".mp3") || fLower.endsWith(".wav") || fLower.endsWith(".amr") || fLower.endsWith(".bin") || fLower.endsWith(".elf")) {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Binary files cannot be previewed as text. Use Download/Play buttons.\"}");
        return;
      }
      
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
        updateSDCache();
        server.send(200, "application/json", "{\"success\":true}");
      } else {
        server.send(404, "application/json", "{\"success\":false,\"error\":\"File not found\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false}");
    }
  });

  server.on("/rename_file", HTTP_POST, []() {
    if (server.hasArg("file") && server.hasArg("new_name")) {
      String oldPath = server.arg("file");
      String newPath = server.arg("new_name");
      if (!oldPath.startsWith("/")) oldPath = "/" + oldPath;
      if (!newPath.startsWith("/")) newPath = "/" + newPath;
      
      if (myFS->exists(oldPath)) {
        createParentDirs(newPath); // Ensure folders for new path exist
        if (myFS->rename(oldPath, newPath)) {
          updateSDCache();
          server.send(200, "application/json", "{\"success\":true}");
        } else {
          server.send(500, "application/json", "{\"success\":false,\"error\":\"Rename failed\"}");
        }
      } else {
        server.send(404, "application/json", "{\"success\":false,\"error\":\"Source file not found\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing arguments\"}");
    }
  });

  server.on("/api/play_audio", []() {
    if (server.hasArg("file")) {
      String path = server.arg("file");
      if (!path.startsWith("/")) path = "/" + path;

      // Resolve friendly name to SD path if needed
      fs::FS &fs = *myFS;
      String fileToPlay = path;
      if (!fs.exists(fileToPlay.c_str())) {
        fileToPlay = getDFPlayerCompatiblePath(path);
      }

      // Use heap-guarded safePlayAudio() -- prevents OOM decoder crashes
      bool ok = safePlayAudio(fs, fileToPlay.c_str());
      if (ok) {
        server.send(200, "application/json",
          "{\"success\":true,\"message\":\"Playing " + fileToPlay + "\"}");
      } else {
        server.send(200, "application/json",
          "{\"success\":false,\"error\":\"Cannot play: heap too low or file missing\"}");
      }
    } else if (server.hasArg("action") && server.arg("action") == "stop") {
      mp3Stop();
      server.send(200, "application/json", "{\"success\":true,\"message\":\"Stopped\"}");
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing arguments\"}");
    }
  });

  server.on("/save_ivr_config", HTTP_POST, []() {
    if (server.hasArg("config")) {
      String conf = server.arg("config");
      File f = myFS->open("/ivr_menu.json", FILE_WRITE);
      if (f) {
        f.print(conf);
        f.close();
        updateSDCache();
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

  // ==================== OTA UPDATE ROUTES ====================
  // [OTA] GET /ota_status — Poll-friendly; no auth needed for progress polling.
  server.on("/ota_status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["status"]     = otaStatus;
    doc["inProgress"] = otaInProgress;
    doc["urlSet"]     = (otaFirmwareUrl.length() > 0); // [OTA] true if URL has been saved
    doc["url"]        = otaFirmwareUrl;                // [OTA] Return URL so dashboard can show it
    doc["descriptorUrl"] = otaDescriptorUrl;
    doc["autoUpdate"] = otaAutoUpdate;                 // Return auto-update state
    String resp;
    serializeJson(doc, resp);
    server.send(200, "application/json", resp);
  });

  // [OTA] POST /api/ota_save_descriptor — Saves descriptor URL to NVS.
  server.on("/api/ota_save_descriptor", HTTP_POST, []() {
    if (!isAuth()) { server.send(403, "application/json", "{\"error\":\"Unauthorized\"}"); return; }
    if (!server.hasArg("url")) {
      server.send(400, "application/json", "{\"error\":\"Missing url parameter\"}");
      return;
    }
    String newUrl = server.arg("url");
    newUrl.trim();
    otaDescriptorUrl = newUrl;                        // Update runtime variable
    preferences.begin("wifi-config", false);
    preferences.putString("otaDescUrl", otaDescriptorUrl); // Persist to NVS
    preferences.end();
    webLog("[OTA] Descriptor URL saved: " + otaDescriptorUrl);
    server.send(200, "application/json", "{\"success\":true}");
  });

  // [OTA] POST /api/ota_auto_update — Save auto update preference.
  server.on("/api/ota_auto_update", HTTP_POST, []() {
    if (!isAuth()) { server.send(403, "application/json", "{\"error\":\"Unauthorized\"}"); return; }
    if (server.hasArg("enabled")) {
      otaAutoUpdate = (server.arg("enabled") == "true");
      preferences.begin("wifi-config", false);
      preferences.putBool("otaAuto", otaAutoUpdate);
      preferences.end();
      webLog("[OTA] Auto-update config saved: " + String(otaAutoUpdate ? "ENABLED" : "DISABLED"));
      server.send(200, "application/json", "{\"success\":true}");
    } else {
      server.send(400, "application/json", "{\"error\":\"Missing enabled parameter\"}");
    }
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

// Helper function to safely compare version strings (e.g., "v1.2-IVR" vs "v1.3-IVR")
bool isNewerVersion(String remoteVer, String localVer) {
  int rStart = -1, lStart = -1;
  for (int i = 0; i < remoteVer.length(); i++) {
    if (isDigit(remoteVer.charAt(i))) { rStart = i; break; }
  }
  for (int i = 0; i < localVer.length(); i++) {
    if (isDigit(localVer.charAt(i))) { lStart = i; break; }
  }
  if (rStart == -1 || lStart == -1) return false;
  
  float rVal = remoteVer.substring(rStart).toFloat();
  float lVal = localVer.substring(lStart).toFloat();
  return rVal > lVal;
}

// [OTA AUTO] Automatic background update task at startup
void otaAutoCheckTask(void* pvParameters) {
  webLog("[OTA AUTO] Checking for updates at boot...");
  
  if (otaDescriptorUrl.length() == 0) {
    webLog("[OTA AUTO] Descriptor URL is empty. Aborting auto-update.");
    vTaskDelete(NULL);
    return;
  }
  
  // Wait a few seconds for network interfaces to stabilize
  vTaskDelay(pdMS_TO_TICKS(5000));
  
  { // Inner block to destruct C++ objects before FreeRTOS task deletion
    WiFiClientSecure secureClient;
    secureClient.setInsecure();
    
    HTTPClient http;
    bool begun = http.begin(secureClient, otaDescriptorUrl);
    if (!begun) {
      webLog("[OTA AUTO] Failed to connect to version descriptor URL.");
      vTaskDelete(NULL);
      return;
    }

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        const char* newVersion = doc["version"];
        const char* binUrl = doc["url"];
        
        if (newVersion && binUrl && isNewerVersion(String(newVersion), String(FIRMWARE_VERSION))) {
          webLog("[OTA AUTO] New version found: " + String(newVersion) + " (current: " + FIRMWARE_VERSION + ")");
          webLog("[OTA AUTO] Starting auto-update from: " + String(binUrl));
          
          // Save URL to otaFirmwareUrl and trigger update
          otaFirmwareUrl = String(binUrl);
          preferences.begin("wifi-config", false);
          preferences.putString("otaUrl", otaFirmwareUrl);
          preferences.end();
          
          triggerOtaUpdate();
        } else {
          webLog("[OTA AUTO] Firmware is up-to-date.");
        }
      } else {
        webLog("[OTA AUTO] Failed to parse version JSON descriptor.");
      }
    } else {
      webLog("[OTA AUTO] Failed to fetch version descriptor, HTTP Code: " + String(httpCode));
    }
    
    http.end();
  }
  vTaskDelete(NULL);
}

// GSM Initialization Helper
void initializeGSM(bool isBoot = false) {
  webLog("Initializing GSM Module AT commands...");
  
  long baudRates[] = {115200, 9600, 19200, 38400, 57600};
  int totalBauds = sizeof(baudRates) / sizeof(baudRates[0]);
  
  // If we are doing a runtime auto-recovery (not boot), only test 115200 to avoid blocking the loop for 23 seconds
  if (!isBoot) {
    totalBauds = 1;
  }
  
  bool synced = false;
  long successfulBaud = 0;
  
  for (int b = 0; b < totalBauds; b++) {
    long testBaud = baudRates[b];
    webLog("Testing GSM communication at baud rate: " + String(testBaud));
    
    // Re-initialize Serial2 at the test baud rate
    Serial2.end();
    delay(100);
    Serial2.setRxBufferSize(1024);
    Serial2.begin(testBaud, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
    delay(200);
    
    // Clear serial buffers
    while(Serial2.available() > 0) { Serial2.read(); }
    
    // Send AT command (fewer attempts/timeouts for runtime auto-recovery)
    int maxAttempts = isBoot ? 5 : 2;
    unsigned long atTimeout = isBoot ? 800 : 400;
    
    for (int i = 0; i < maxAttempts; i++) {
      webLog("Scanning baud... (attempt " + String(i + 1) + ")");
      if (sendATCommand("AT", "OK", atTimeout)) {
        synced = true;
        successfulBaud = testBaud;
        break;
      }
      delay(150);
    }
    
    if (synced) {
      webLog("Successfully synced with GSM module at baud rate: " + String(successfulBaud));
      break;
    }
  }
  
  gsmModuleConnected = synced;
  if (gsmModuleConnected) {
    sendATCommand("ATS0=0",    "OK", 1000); // Disable hardware auto-answer
    sendATCommand("AT+CMEE=1", "OK", 1000); // Enable numeric error reporting
    sendATCommand("AT+CLIP=1", "OK", 1000); // Enable caller ID
    sendATCommand("AT+CMGF=1", "OK", 1000); // SMS text mode
    sendATCommand("AT+CNMI=2,2,0,0,0", "OK", 1000); // Route SMS to UART
    sendATCommand("AT+CSDVC=1", "OK", 1000); // Audio channel = Handset
    sendATCommand("AT+CHFA=0", "OK", 1000);  // Audio channel = Handset (SIM800L)
    sendATCommand("AT+DDET=1", "OK", 1000); // Enable DTMF decoder at boot (SIM800L)
    sendATCommand("AT+DTMFDET=1", "OK", 1000); // Enable DTMF decoder at boot (A7670C)
    // Note: AT+CMUT, AT+CLVL, AT+CMIC require an active call — sent dynamically on answer
    
    // Query IMEI at boot to populate status page immediately
    sendATCommand("AT+CGSN", "\r", 1000);   // Request IMEI
    // Query registration and signal immediately so LEDs show correct state ASAP
    sendATCommand("AT+CPIN?",  "+CPIN:",  1500);
    sendATCommand("AT+CREG?",  "+CREG:",  1500);
    sendATCommand("AT+CEREG?", "+CEREG:", 1500);
    sendATCommand("AT+COPS?",  "+COPS:",  2000);
    sendATCommand("AT+CSQ",    "+CSQ:",   1500);
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

  // ─── SD Card Status LED indicator logic ───
  bool isSdActive = (audioPlaying || (now - lastSdActivityMillis < 500));
  if (!sdCardFound || sdCardUnsupportedFS) {
    // SD Card error or not inserted -> blink slowly (500ms on, 500ms off)
    digitalWrite(LED_SD_STATUS, (ledBlinkCounter % 10 < 5) ? HIGH : LOW);
  } else if (isSdActive) {
    // SD Card active (playing audio or file traffic) -> blink rapidly (100ms on, 100ms off)
    digitalWrite(LED_SD_STATUS, (ledBlinkCounter % 2 == 0) ? HIGH : LOW);
  } else {
    // SD Card mounted and idle -> steady ON (solid HIGH)
    digitalWrite(LED_SD_STATUS, HIGH);
  }

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

  // 2. Active call states (Green LED blinks, Onboard LED stays solid HIGH as power light)
  if (currentCallState == STATE_ANSWERING || 
      currentCallState == STATE_CALL_CONNECTED ||
      currentCallState == STATE_PLAY_WELCOME || 
      currentCallState == STATE_PLAY_MENU || 
      currentCallState == STATE_WAIT_DTMF || 
      currentCallState == STATE_PLAY_SELECTION || 
      currentCallState == STATE_RETURN_MENU ||
      currentCallState == STATE_HANGUP || 
      currentCallState == STATE_CALLBACK_DIALING ||
      currentCallState == STATE_COLLECTING_PRODUCT_NO ||
      currentCallState == STATE_AGENT_CONNECTING) {
    digitalWrite(LED_GREEN, (ledBlinkCounter % 10 < 5) ? HIGH : LOW); // Green blinks
    digitalWrite(LED_PIN, HIGH); // Onboard LED stays ON
    return;
  }

  // 3. Ringing or Validating states (Onboard LED blinks rapidly, Green LED off)
  if (currentCallState == STATE_RINGING || currentCallState == STATE_VERIFY_USER || currentCallState == STATE_VALIDATING_SECOND_CALL) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PIN, (ledBlinkCounter % 2 == 0) ? HIGH : LOW);
    return;
  }

  // 4. Idle state status indicators
  if (!gsmModuleConnected) {
    // No GSM module with ESP -> blink Onboard LED rapidly
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_PIN, (ledBlinkCounter % 2 == 0) ? HIGH : LOW);
  } else {
    bool gsmRegistered = (gsmRegStatus == "1" || gsmRegStatus == "5");

    if (gsmRegistered) {
      // SIM registered -> Green LED steady (solid HIGH), Onboard LED solid HIGH as power light
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_PIN, HIGH);
    } else {
      // Cellular Error -> Green LED blinks, Onboard LED solid HIGH as power light
      digitalWrite(LED_GREEN, (ledBlinkCounter % 10 < 5) ? HIGH : LOW);
      digitalWrite(LED_PIN, HIGH);
    }
  }
}

void listSDFiles(File dir, int numTabs = 0) {
  File file = dir.openNextFile();
  while (file) {
    for (int i = 0; i < numTabs; i++) { Serial.print("  "); }
    Serial.print(file.name());
    if (file.isDirectory()) {
      Serial.println("/");
      listSDFiles(file, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(file.size(), DEC);
    }
    file.close();
    file = dir.openNextFile();
  }
}

// ==================== SYSTEM MAIN ====================
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector to prevent resets during GSM power spikes
  Serial.begin(115200);
  esp_log_level_set("*", ESP_LOG_WARN);      // Set global ESP-IDF log level to WARNING to prevent excessive stack usage
  esp_log_level_set("mdns", ESP_LOG_NONE);   // Turn off mDNS logs completely
  logMutex = xSemaphoreCreateMutex();
  audioMutex = xSemaphoreCreateMutex();
  delay(1000);

  // Configure LED pins EARLY so the user sees life immediately on power-on
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_SD_STATUS, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  // Visual boot indicator — LEDs ON so user knows board is alive
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_SD_STATUS, HIGH);
  Serial.println("[BOOT] LEDs ON — initializing SD card first...");

  // Fetch local MAC Address ID directly from ESP32 eFuse hardware MAC register
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char macStr[13];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X",
           baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  myMac = String(macStr);
  
  webLog("-----------------------------------------");
  webLog("GSV Gatekeeper OS: " + String(FIRMWARE_VERSION) + " System Booting");
  webLog("MAC address identity: " + myMac);

  // Initialize SPI bus with standard VSPI pins
  SPI.begin(18, 19, 23, 5);
  delay(100);

  // Initialize SD Card (try multiple common CS pins and SPI speeds for maximum compatibility)
  // NOTE: GPIO 4 = LED_SD_STATUS and GPIO 2 = LED_PIN — never use as SPI CS!
  int csPins[] = {5, 15};
  int numCsPins = 2;
  uint32_t spiSpeeds[] = {1000000U, 4000000U, 8000000U}; // Try 1MHz first for reliability
  bool sdMounted = false;
  for (int si = 0; si < 3 && !sdMounted; si++) {
    for (int i = 0; i < numCsPins && !sdMounted; i++) {
      int cs = csPins[i];
      webLog("Attempting SD mount: CS=" + String(cs) + " Speed=" + String(spiSpeeds[si]/1000) + "kHz");
      SD.end(); // Reset before each attempt
      delay(50);
      if (SD.begin(cs, SPI, spiSpeeds[si])) {
        uint64_t tot = SD.totalBytes();
        if (tot > 0) { // Validate — a real card must have storage
          webLog("SD Card Mount OK: CS=" + String(cs) + " Total=" + String(tot / 1048576) + "MB");
          {
            File root = SD.open("/");
            Serial.println("--- SD Card Files ---");
            listSDFiles(root);
            Serial.println("---------------------");
            root.close();
          }
          myFS = &SD;
          sdCardFound = true;
          sdCardCS = cs;
          sdCardTotalBytes = tot;
          sdCardUsedBytes = SD.usedBytes();
          sdCardSectorSize = SD.cardSize() > 0 ? 512 : 0; // Standard sector size
          sdCardNumSectors = SD.cardSize();  // cardSize() returns numSectors on ESP32 SD lib
          // Determine card type string
          uint8_t ctype = SD.cardType();
          if      (ctype == CARD_MMC)  sdCardCardType = "MMC";
          else if (ctype == CARD_SD)   sdCardCardType = "SD";
          else if (ctype == CARD_SDHC) sdCardCardType = "SDHC";
          else                          sdCardCardType = "UNKNOWN";
          sdMounted = true;
        } else {
          webLog("SD mounted but totalBytes=0 — likely SPIFFS bleeding through, rejecting.");
          SD.end();
        }
      }
    }
  }

  if (!sdMounted) {
    webLog("SD Card Mount Failed on all pins/speeds. Attempting SPIFFS fallback...");
    if (!SPIFFS.begin(true)) {
      webLog("SPIFFS Fallback Mount Failed. File storage disabled.");
      sdCardFound = false;
    } else {
      webLog("SPIFFS Fallback Mount Successful! (Note: This is internal flash, NOT an SD card)");
      myFS = &SPIFFS;
      sdCardFound = true; // Still mark true to allow fallback file storage
      sdCardCS = -1;
      sdCardCardType = "SPIFFS";
      sdCardSectorSize = 0;
      sdCardNumSectors = 0;
      sdCardTotalBytes = SPIFFS.totalBytes();
      sdCardUsedBytes = SPIFFS.usedBytes();
    }
  }

  // Populate dynamic storage specs cache
  updateSDCache();

  // Reset indicator LEDs to prepare for GSM boot sequence
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_SD_STATUS, LOW);

  // Power-on/wait for SIM A7670C module
  if (GSM_PWRKEY_PIN != -1) {
    pinMode(GSM_PWRKEY_PIN, OUTPUT);
    digitalWrite(GSM_PWRKEY_PIN, HIGH);
    delay(100);
    digitalWrite(GSM_PWRKEY_PIN, LOW);
    delay(2000);
    pinMode(GSM_PWRKEY_PIN, INPUT); // Set to high-impedance to release PWRKEY
  } else {
    // Blink Green LED during 12s GSM boot wait so board doesn't look dead
    for (int i = 0; i < 24; i++) {
      digitalWrite(LED_GREEN, (i % 2 == 0) ? HIGH : LOW);
      delay(500);
    }
  }

  // Reset LEDs after GSM boot wait
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_SD_STATUS, LOW);

  // Ensure ivr_menu.json exists and is configured as requested
  ensureIvrMenuJson();

  // Initialize GSM Serial2 Communication (GPIO16=RX, GPIO17=TX)
  Serial2.setRxBufferSize(1024);
  Serial2.begin(GSM_BAUD, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  initializeGSM(true);

  // Initialize MAX98357A I2S Audio (BCLK=27, LRC=26, DIN=14)
  // Called AFTER GSM init and NVS load so that:
  //   a) Volume preference is available
  //   b) GSM serial buffers are already allocated (no post-init fragmentation)
  // Does NOT play any audio — boot audio is buzzer-only to protect heap.
  mp3Init();
  playBootSound(); // Buzzer-only chime (see playBootSound() implementation)

  // Read saved configurations
  preferences.begin("wifi-config", false);
  savedSSID = preferences.getString("ssid", "");
  if (!isPrintableASCII(savedSSID)) {
    savedSSID = "";
  }
  savedPass = preferences.getString("pass", "");
  if (!isPrintableASCII(savedPass)) {
    savedPass = "";
  }
  scriptId  = preferences.getString("scriptId", "AKfycbwgvQ_-3aT78j2fy7G9FdBWVlNjzrSp446ZNXVCiuc3_vhsXmqJyquVii8X4VSIPq6u");
  sheetId   = preferences.getString("sheetId", "1K8SnQA8H2LVEnxyRTKYfTdLYRe2YHC4qQFqWUyOejQw");
  sheetName = preferences.getString("sheetName", "IVR");
  savedApSSID = preferences.getString("apSsid", "V VARMA IVR");
  if (!isPrintableASCII(savedApSSID) || savedApSSID.length() == 0 || savedApSSID == "vvarmaivr" || savedApSSID == "V-VARMA-IVR") {
    savedApSSID = "V VARMA IVR";
    preferences.putString("apSsid", savedApSSID);
  }
  savedApPass = preferences.getString("apPass", "GSVIVR001");
  if (!isPrintableASCII(savedApPass) || savedApPass.length() < 8) {
    savedApPass = "GSVIVR001";
    preferences.putString("apPass", savedApPass);
  }
  deviceUser = preferences.getString("adminUser", "admin");
  if (!isPrintableASCII(deviceUser)) {
    deviceUser = "admin";
  }
  devicePass = preferences.getString("adminPass", "GSVIVR001");
  if (!isPrintableASCII(devicePass)) {
    devicePass = "GSVIVR001";
  }
  
  tgToken = preferences.getString("tgToken", "");
  if (!isPrintableASCII(tgToken)) tgToken = "";
  tgChatId = preferences.getString("tgChatId", "");
  if (!isPrintableASCII(tgChatId)) tgChatId = "";
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
  if (!isPrintableASCII(agentPhoneNumber)) agentPhoneNumber = "+919092610415";
  
  testModeEnabled = preferences.getBool("testMode", false);
  testAutoAttend = preferences.getBool("testAttend", false);
  testAutoCallback = preferences.getBool("testCallback", false);
  testAutoSms = preferences.getBool("testSms", false);
  testPhoneNumber = preferences.getString("testPhone", "");
  if (!isPrintableASCII(testPhoneNumber)) testPhoneNumber = "";
  testSmsTemplate = preferences.getString("testSmsTmp", "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in");
  if (!isPrintableASCII(testSmsTemplate)) testSmsTemplate = "Dear customer, you called but did not select any options. Thank you for calling V-Varma. Website: vvarma.gsvee.in";
  
  otaFirmwareUrl  = preferences.getString("otaUrl", "https://raw.githubusercontent.com/gsvrnd2025-alt/VVarmaIVR/main/firmware.bin");
  if (!isPrintableASCII(otaFirmwareUrl)) otaFirmwareUrl = "https://raw.githubusercontent.com/gsvrnd2025-alt/VVarmaIVR/main/firmware.bin";
  otaDescriptorUrl = preferences.getString("otaDescUrl", "https://raw.githubusercontent.com/gsvrnd2025-alt/VVarmaIVR/main/version.json");
  if (!isPrintableASCII(otaDescriptorUrl)) otaDescriptorUrl = "https://raw.githubusercontent.com/gsvrnd2025-alt/VVarmaIVR/main/version.json";
  otaAutoUpdate   = preferences.getBool("otaAuto", true);

  preferences.end();

  if (savedSSID.length() > 0) {
    // ── DUAL MODE (AP+STA) ── Credentials exist: connect to router and run AP hotspot ──
    webLog("[WiFi] Credentials found. Booting in AP+STA dual mode.");
    webLog("[HEAP] Before WiFi.mode. Free heap: " + String(ESP.getFreeHeap()) + " bytes");
    
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_OFF);
    delay(200);
    WiFi.mode(WIFI_AP_STA);   // AP+STA Dual Mode — both AP hotspot and STA router connection
    delay(100);
    WiFi.setHostname(DEVICE_NAME);
    WiFi.setAutoReconnect(true);
    WiFi.setSleep(false);  // Disable WiFi power saving to prevent latency/disconnects
    
    // Start AP hotspot
    startSoftAPWithLog(savedApSSID, savedApPass);
    delay(100);
    IPAddress apIP = WiFi.softAPIP();
    
    // Start DNS server for captive portal requests on AP interface
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", apIP);
    
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    webLog("[WiFi] Connecting to router SSID: '" + savedSSID + "'...");
    
    setupWebServer();   // Web server available immediately
    
    // Set asynchronous connection tracker
    wifiConnectStartMillis = millis();
    wifiConnecting = true;
    currentSystemState = SYSTEM_RUNNING;  // Dashboard accessible immediately via AP hotspot
    
    // Initialize mDNS / NetBIOS under dual mode
    mDNS_begin();
  } else {
    // ── AP-ONLY MODE ── No credentials: open hotspot for initial Wi-Fi setup ──
    webLog("[WiFi] No credentials found. Booting in AP-only config mode.");
    startAPMode();
  }

  webLog("[IVR Case 20] System restarts or recovers after power failure and returns to idle state.");
  gsmQueryTimer = millis();

}

void handleSerialUpload() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("UPLOAD:")) {
      int firstColon = cmd.indexOf(':');
      int secondColon = cmd.indexOf(':', firstColon + 1);
      if (firstColon != -1 && secondColon != -1) {
        String path = cmd.substring(firstColon + 1, secondColon);
        String sizeStr = cmd.substring(secondColon + 1);
        long fileSize = sizeStr.toInt();
        
        serialUploadActive = true;
        
        webLog("[SerialUpload] Uploading " + path + " (" + String(fileSize) + " bytes)");
        mp3Stop();
        delay(200);
        
        fs::FS &fs = *myFS;
        int slashIdx = path.lastIndexOf('/');
        if (slashIdx > 0) {
          String dirPath = path.substring(0, slashIdx);
          if (!fs.exists(dirPath)) {
            fs.mkdir(dirPath);
          }
        }
        
        File f = fs.open(path, FILE_WRITE);
        if (!f) {
          Serial.println("ERROR: Open Failed");
          serialUploadActive = false;
          return;
        }
        
        Serial.println("READY");
        Serial.flush();
        
        long bytesRead = 0;
        unsigned long lastByteMillis = millis();
        uint8_t buffer[256];
        
        while (bytesRead < fileSize && (millis() - lastByteMillis < 10000)) {
          int targetChunkSize = (fileSize - bytesRead < 256) ? (fileSize - bytesRead) : 256;
          int chunkBytesRead = 0;
          while (chunkBytesRead < targetChunkSize && (millis() - lastByteMillis < 10000)) {
            if (Serial.available() > 0) {
              int toRead = Serial.available();
              if (toRead > (targetChunkSize - chunkBytesRead)) toRead = targetChunkSize - chunkBytesRead;
              int r = Serial.readBytes(buffer + chunkBytesRead, toRead);
              if (r > 0) {
                chunkBytesRead += r;
                lastByteMillis = millis();
              }
            }
            yield();
          }
          if (chunkBytesRead > 0) {
            f.write(buffer, chunkBytesRead);
            bytesRead += chunkBytesRead;
            Serial.write(0x06);
            Serial.flush();
          }
        }
        
        f.close();
        if (bytesRead == fileSize) {
          Serial.println("SUCCESS");
          webLog("[SerialUpload] Successfully wrote " + String(bytesRead) + " bytes to " + path);
        } else {
          Serial.println("ERROR: Timeout/Size Mismatch");
          fs.remove(path);
        }
        
        serialUploadActive = false;
      }
    }
  }
}

void loop() {
  handleSerialUpload();
  updateLedIndicators();

  // Handle initial connection timeout (only if still trying to connect for the first time)
  if (wifiConnecting && (millis() - wifiConnectStartMillis > 60000)) {
    wifiConnecting = false;
    webLog("[WiFi] STA initial connection timed out after 60s. Switching to stable AP-Only mode.");
    
    // Switch to pure AP mode to prevent background scanning channel-hopping
    WiFi.disconnect(true, false);
    WiFi.mode(WIFI_AP);
    delay(100);
    
    // Re-initialize SoftAP with the correct SSID and password
    startSoftAPWithLog(savedApSSID, savedApPass);
    IPAddress apIP = WiFi.softAPIP();
    localIpStr = apIP.toString();
    
    // Start DNS server on AP interface
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", apIP);
    
    playErrorSound();
  }
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

  // 1. Maintain DNS captives
  dnsServer.processNextRequest();

  // 2. Local client handlers
  server.handleClient();

  // Auto-recovery: If GSM module connection is lost, attempt to re-initialize periodically
  static unsigned long lastGsmReconnectAttempt = 0;
  if (!gsmModuleConnected && (millis() - lastGsmReconnectAttempt > 30000)) {
    lastGsmReconnectAttempt = millis();
    webLog("[GSM Reconnect] GSM is offline. Attempting to re-initialize GSM module (fast reconnect)...");
    initializeGSM(false);
  }

  // 3. Keep mDNS alive
  #if defined(ESP32)
  // mDNS handles loop inside ESP-IDF core
  #endif

  // 3.1 WiFi Connection and Access Point Monitoring
  static wl_status_t lastWiFiStatus = WL_IDLE_STATUS;
  wl_status_t currentWiFiStatus = WiFi.status();
  if (currentWiFiStatus != lastWiFiStatus) {
    lastWiFiStatus = currentWiFiStatus;
    webLog("[WiFi] STA status changed: " + String(currentWiFiStatus));
  }

  static IPAddress lastLocalIP = IPAddress(0, 0, 0, 0);
  IPAddress currentLocalIP = WiFi.localIP();
  if (currentLocalIP != lastLocalIP) {
    lastLocalIP = currentLocalIP;
    if (currentLocalIP != IPAddress(0, 0, 0, 0)) {
      wifiConnecting = false; // Stop tracking initial connection phase
      localIpStr = currentLocalIP.toString();
      webLog("[WiFi] STA IP Assigned: " + localIpStr);
      
      // If we weren't fully running, transition and play startup success chime
      if (currentSystemState != SYSTEM_RUNNING) {
        currentSystemState = SYSTEM_RUNNING;
        playSuccessSound();
        configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
        webLog("[NTP] Time sync initiated (GMT+5:30).");
      }
      
      // Always register/restart mDNS and NetBIOS services on connection
      mDNS_begin();

      // Trigger automatic OTA update check on boot/WiFi connection
      static bool otaCheckedAtBoot = false;
      if (!otaCheckedAtBoot && otaAutoUpdate) {
        otaCheckedAtBoot = true;
        webLog("[OTA AUTO] Spawning version auto-check task on Core 1...");
        xTaskCreatePinnedToCore(otaAutoCheckTask, "ota_auto_task", 8192, NULL, 1, NULL, 1);
      }
    } else {
      webLog("[WiFi] STA IP cleared.");
    }
  }

  static int lastApClientsCount = 0;
  static unsigned long lastApCheck = 0;
  if (millis() - lastApCheck > 1000) {
    lastApCheck = millis();
    int currentApClientsCount = WiFi.softAPgetStationNum();
    if (currentApClientsCount > lastApClientsCount) {
      webLog("New client connected to Access Point. Total clients: " + String(currentApClientsCount));
      uint32_t freeHeap = ESP.getFreeHeap();
      uint32_t maxBlock = ESP.getMaxAllocHeap();
      webLog("[HEAP] Free heap: " + String(freeHeap) + " bytes | Largest free block: " + String(maxBlock) + " bytes");
      // Only play tone if heap is sufficient (MP3 decoder needs ~25KB contiguous minimum)
      if (currentCallState == STATE_IDLE && mp3Initialized && maxBlock > 30000) {
        webLog("Playing client connection voice tone (/01/003.mp3)...");
        mp3PlayAsync(1, 3);
      } else if (maxBlock <= 30000) {
        webLog("[Audio] Skipping AP connect tone - insufficient heap (" + String(maxBlock) + " bytes max block)");
      }
    } else if (currentApClientsCount < lastApClientsCount) {
      webLog("Client disconnected from Access Point. Total clients: " + String(currentApClientsCount));
    }
    lastApClientsCount = currentApClientsCount;
  }

  // 4. Non-blocking GSM Query Scheduler — 5 steps, each fires every 5s
  // CREG and CEREG are in SEPARATE steps to avoid response collision
  if (gsmModuleConnected && currentCallState == STATE_IDLE && (millis() - gsmQueryTimer > 5000)) {
    gsmQueryTimer = millis();
    
    switch(gsmQueryStep) {
      case 0:  // SIM PIN status
        sendATCommand("AT+CPIN?", "+CPIN:", 1000);
        gsmQueryStep++;
        break;
      case 1:  // 2G/3G registration (CREG alone, no CEREG here)
        sendATCommand("AT+CREG?", "+CREG:", 1500);
        gsmQueryStep++;
        break;
      case 2:  // 4G LTE registration (CEREG alone, separate step)
        sendATCommand("AT+CEREG?", "+CEREG:", 1500);
        gsmQueryStep++;
        break;
      case 3:  // Operator name
        sendATCommand("AT+COPS?", "+COPS:", 2000);
        gsmQueryStep++;
        break;
      case 4:  // Signal quality
        sendATCommand("AT+CSQ", "+CSQ:", 1000);
        gsmQueryStep++;
        break;
      case 5:  // Heartbeat ping — resets gsmModuleConnected flag
        sendATCommand("AT", "OK", 1000);
        gsmQueryStep = 0;
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

  // 5.5 Validate Caller ID Result Checker (No longer needed since we answer immediately, but kept as placeholder)
  if (currentCallState == STATE_VERIFY_USER) {
    // Legacy: unused
  }

  // 5.55 Answering Call Handler
  if (currentCallState == STATE_ANSWERING) {
    webLog("Answering call physically via ATA...");
    delay(500); // 500ms delay for stability
    if (sendATCommand("ATA", "OK", 5000)) {
      webLog("Call answered successfully.");
      delay(100);
      sendATCommand("AT+DDET=1", "OK", 1000); // Enable DTMF detector (SIM800L)
      sendATCommand("AT+DTMFDET=1", "OK", 1000); // Enable DTMF detector (A7670C)
      delay(100);
      sendATCommand("AT+CHFA=0", "OK", 1000);  // Set audio channel to handset (SIM800L)
      delay(100);
      sendATCommand("AT+CMUT=0", "OK", 1000); // Unmute microphone
      delay(100);
      sendATCommand("AT+CMIC=0,15", "OK", 1000); // Max gain channel 0 (handset mic)
      delay(100);
      sendATCommand("AT+CMIC=1,15", "OK", 1000); // Max gain channel 1 (headset mic)
      delay(100);
      sendATCommand("AT+CMICGAIN=7", "OK", 1000); // Max gain (A7670C)
      
      callStartMillis = millis();
      lastActivityTime = millis();
      dtmfTimeoutCount = 0;
      
      changeState(STATE_CALL_CONNECTED);
    } else {
      webLog("Failed to physically answer call (ATA failed).");
      Serial2.println("ATH");
      addCallLog(callerNumber, "", "Answer Failed");
      changeState(STATE_ENDED);
    }
  }

  // 5.56 Call Connected Handler (Wait for validation task to finish)
  if (currentCallState == STATE_CALL_CONNECTED) {
    if (CONTINUOUS_VOICE_LOOP) {
      static unsigned long lastLoopAttempt = 0;
      if (checkAudioFinished()) {
        if (millis() - lastLoopAttempt > 2000) {
          lastLoopAttempt = millis();
          webLog("[Continuous Loop] Playing / Looping audio: " + String(VOICE_LOOP_PATH));
          safePlayAudio(*myFS, VOICE_LOOP_PATH);
          lastAudioPlayMillis = millis();
        }
      }
    } else if (ivrSelectedLanguage == 0) {
      // Language selection phase
      webLog("Language not selected yet. Playing Language Selection Menu...");
      currentAudioFolder = 4;
      currentAudioTrack = 2; // Language menu `/04/002.wav`
      changeState(STATE_PLAY_MENU);
      mp3Play(currentAudioFolder, currentAudioTrack);
      lastAudioPlayMillis = millis();
    } else if (validationResult == -1) {
      // Still checking with Google Sheets, wait...
      if (checkAudioFinished()) {
        webLog("Looping validation audio in language: " + String(ivrSelectedLanguage));
        if (ivrSelectedLanguage == 2) {
          mp3Play(4, 8); // Tamil hold /04/008.wav
        } else if (ivrSelectedLanguage == 3) {
          mp3Play(4, 11); // Hindi hold /04/011.wav
        } else {
          mp3Play(4, 1); // English hold /04/001.wav
        }
        lastAudioPlayMillis = millis();
      }
      if (millis() - callStartMillis > 20000) {
        webLog("Validation timeout during active call. Defaulting to unregistered.");
        validationResult = 0;
      }
    } else {
      // Validation completed!
      if (validationResult == 1) {
        callerIsRegistered = true;
        webLog("Caller verified as registered. Playing customer greeting...");
        if (ivrSelectedLanguage == 2) {
          currentAudioFolder = 4;
          currentAudioTrack = 9; // Tamil welcome + menu /04/009.wav
        } else if (ivrSelectedLanguage == 3) {
          currentAudioFolder = 4;
          currentAudioTrack = 12; // Hindi welcome + menu /04/012.wav
        } else {
          currentAudioFolder = 4;
          currentAudioTrack = 3; // English welcome + menu /04/003.wav
        }
        changeState(STATE_PLAY_WELCOME);
        mp3Play(currentAudioFolder, currentAudioTrack);
        lastAudioPlayMillis = millis();
        lastActivityTime = millis();
        validationResult = -1; // Reset validation result
      } else {
        callerIsRegistered = false;
        webLog("Caller is unregistered. Playing warning message before disconnect...");
        if (ivrSelectedLanguage == 2) {
          currentAudioFolder = 4;
          currentAudioTrack = 10; // Tamil warning /04/010.wav
        } else if (ivrSelectedLanguage == 3) {
          currentAudioFolder = 4;
          currentAudioTrack = 13; // Hindi warning /04/013.wav
        } else {
          currentAudioFolder = 4;
          currentAudioTrack = 6; // English warning /04/006.wav
        }
        changeState(STATE_PLAY_SELECTION);
        mp3Play(currentAudioFolder, currentAudioTrack);
        lastAudioPlayMillis = millis();
        validationResult = -1; // Reset validation result
        
        // Send notification SMS in background
        String msg = "";
        if (ivrSelectedLanguage == 2) {
          msg = "அன்புள்ள வாடிக்கையாளரே, நீங்கள் பதிவுசெய்யப்பட்ட பயனர் இல்லை. பதிவு செய்ய, 9092610415 என்ற எண்ணில் வி-வர்மா நிர்வாகியைத் தொடர்பு கொள்ளவும்.";
        } else if (ivrSelectedLanguage == 3) {
          msg = "प्रिय ग्राहक, आप एक पंजीकृत उपयोगकर्ता नहीं हैं। पंजीकरण के लिए, कृपया 9092610415 पर वी-वर्मा प्रशासक से संपर्क करें।";
        } else {
          msg = "Dear customer, you are not a registered user. To register, please contact V-Varma administrator at 9092610415.";
        }
        sendSMS(callerNumber, msg);
        addSmsLog(callerNumber, msg, "Outgoing", "Sent");
      }
    }
  }

  // 5.6 Welcome Message Completion Checker (Customer Greeting)
  if (currentCallState == STATE_PLAY_WELCOME) {
    if (checkAudioFinished()) {
      webLog("Customer greeting finished. Transitioning directly to STATE_WAIT_DTMF...");
      changeState(STATE_WAIT_DTMF);
      lastActivityTime = millis();
    }
  }

  // 5.7 Main Menu Completion Checker
  if (currentCallState == STATE_PLAY_MENU) {
    if (checkAudioFinished()) {
      webLog("Main Menu finished. Transitioning to STATE_WAIT_DTMF...");
      changeState(STATE_WAIT_DTMF);
      lastActivityTime = millis();
    }
  }

  // 5.8 Selection Audio Completion Checker
  if (currentCallState == STATE_PLAY_SELECTION) {
    if (checkAudioFinished()) {
      webLog("Selection audio completed.");
      if (currentAudioFolder == 4 && currentAudioTrack == 4) {
        // Warning 1 completed: replay language menu
        webLog("Language warning 1 completed. Replaying Language Selection Menu...");
        languageWarningPending = false;
        currentAudioFolder = 4;
        currentAudioTrack = 2; // Language menu `/04/002.wav`
        changeState(STATE_PLAY_MENU);
        mp3Play(currentAudioFolder, currentAudioTrack);
        lastAudioPlayMillis = millis();
      } else if (currentAudioFolder == 4 && (currentAudioTrack == 5 || currentAudioTrack == 6 || currentAudioTrack == 10 || currentAudioTrack == 13)) {
        webLog("Rejection, timeout warning, or unregistered warning audio completed. Transitioning to STATE_HANGUP...");
        changeState(STATE_HANGUP);
      } else {
        changeState(STATE_WAIT_DTMF);
        lastActivityTime = millis();
      }
    }
  }

  // 5.9 DTMF Timeout Checker (4s for language, 15s otherwise)
  if (currentCallState == STATE_WAIT_DTMF) {
    unsigned long timeoutVal = (ivrSelectedLanguage == 0) ? 4000 : 15000;
    if (millis() - lastActivityTime > timeoutVal) {
      dtmfTimeoutCount++;
      webLog("DTMF Wait Timeout (" + String(dtmfTimeoutCount) + ") for language: " + String(ivrSelectedLanguage));
      
      if (ivrSelectedLanguage == 0) {
        if (dtmfTimeoutCount == 1) {
          webLog("Language selection timeout #1. Playing warning /04/004.wav...");
          languageWarningPending = true;
          currentAudioFolder = 4;
          currentAudioTrack = 4; // Warning 1
          changeState(STATE_PLAY_SELECTION);
          mp3Play(currentAudioFolder, currentAudioTrack);
          lastAudioPlayMillis = millis();
        } else {
          webLog("Language selection timeout #2. Playing warning /04/005.wav and disconnecting...");
          languageDisconnectPending = true;
          currentAudioFolder = 4;
          currentAudioTrack = 5; // Warning 2
          changeState(STATE_PLAY_SELECTION);
          mp3Play(currentAudioFolder, currentAudioTrack);
          lastAudioPlayMillis = millis();
        }
      } else {
        // Standard DTMF timeout
        if (dtmfTimeoutCount < 3) {
          changeState(STATE_RETURN_MENU);
        } else {
          webLog("Max DTMF timeouts reached. Transitioning to STATE_HANGUP...");
          changeState(STATE_HANGUP);
        }
      }
    }
  }

  // 5.91 Return Menu Handler
  if (currentCallState == STATE_RETURN_MENU) {
    webLog("Replaying Main Menu due to return/timeout...");
    if (callerIsRegistered) {
      if (ivrSelectedLanguage == 2) {
        currentAudioFolder = 4;
        currentAudioTrack = 9; // Tamil welcome + menu
      } else if (ivrSelectedLanguage == 3) {
        currentAudioFolder = 4;
        currentAudioTrack = 12; // Hindi welcome + menu
      } else {
        currentAudioFolder = 4;
        currentAudioTrack = 3; // English welcome + menu
      }
    } else {
      currentAudioFolder = 4;
      currentAudioTrack = 2; // Language menu WAV
    }
    changeState(STATE_PLAY_MENU);
    mp3Play(currentAudioFolder, currentAudioTrack);
    lastAudioPlayMillis = millis();
  }

  // 5.95 Exit message handler (Hangup state)
  if (currentCallState == STATE_HANGUP) {
    bool skipExitMessage = (currentAudioFolder == 4 && (currentAudioTrack == 5 || currentAudioTrack == 6 || currentAudioTrack == 10 || currentAudioTrack == 13));
    if (skipExitMessage) {
      webLog("Skipping exit message for unregistered / timeout disconnect. Hanging up immediately.");
      Serial2.println("ATH");
      addCallLog(callerNumber, dtmfBuffer, callerIsRegistered ? "Completed" : "Rejected/Unregistered");
      changeState(STATE_ENDED);
    } else {
      if (currentAudioTrack != 12 || currentAudioFolder != 1) {
        currentAudioFolder = 1;
        currentAudioTrack = 12; // Exit message
        mp3Play(currentAudioFolder, currentAudioTrack);
        lastAudioPlayMillis = millis();
      }
      if (checkAudioFinished() || (millis() - lastAudioPlayMillis > 5000)) {
        webLog("Exit message finished or timed out. Hanging up.");
        Serial2.println("ATH");
        addCallLog(callerNumber, dtmfBuffer, callerIsRegistered ? "Completed" : "Rejected/Unregistered");
        changeState(STATE_ENDED);
      }
    }
  }

  // 5.96 Verification timeout checker
  if (currentCallState == STATE_VERIFY_USER && (millis() - ringingStartMillis > 15000)) {
    webLog("Verification timeout. Defaulting to unregistered.");
    validationResult = 0;
  }

  // 6. Ringing timeout checker
  if ((currentCallState == STATE_RINGING || currentCallState == STATE_VERIFY_USER) && (millis() - ringingStartMillis > 40000)) {
    webLog("Incoming call ring/verify timeout.");
    addCallLog(callerNumber, "", "Missed");
    changeState(STATE_ENDED);
  }

  // 6.8 Warranty input silent timeout handler (5s after typing stops, or 10s idle)
  if (customIvrEnabled && currentCallState == STATE_COLLECTING_PRODUCT_NO && ivrWarrantyCheckResult != 99) {
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
  if ((currentCallState == STATE_ACTIVE_CALL || currentCallState == STATE_WAIT_DTMF || currentCallState == STATE_COLLECTING_DTMF || currentCallState == STATE_COLLECTING_PRODUCT_NO || currentCallState == STATE_AGENT_CONNECTING) && (millis() - callStartMillis > 300000)) {
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
    callerNumber = dialNumber; // Map callerNumber to dialed recipient for correct logging and SMS mapping
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
    addCallLog(callerNumber, "", "Callback No Answer");
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
  
  int folder = 0; // Default folder for menu (0 means root playback)
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
  
  if (myFS->exists("/ivr_menu.json")) {
    webLog("[IVR] /ivr_menu.json already exists. Skipping default creation.");
    return;
  }
  
  File f = myFS->open("/ivr_menu.json", FILE_WRITE);
  if (f) {
    String jsonStr = "{\n"
                     "  \"nodes\": {\n"
                     "    \"start\": {\n"
                     "      \"audio\": \"/01/005.mp3\",\n"
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
      currentIvrNode.audio = "/01/005.mp3";
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
        if (warrantyInputBuffer.length() > 0) {
          verifyIvrWarranty(warrantyInputBuffer);
        } else {
          webLog("Empty warranty code entered.");
          playIvrAudio("/01/008.mp3");
        }
      } else if (key == "*") {
        warrantyInputBuffer = "";
        webLog("Cleared warranty buffer.");
        beep(1000, 100);
      } else {
        warrantyInputBuffer += key;
        webLog("Buffered code: " + warrantyInputBuffer);
      }
      return;
    }
    
    // In menu state (STATE_ACTIVE_CALL or STATE_COLLECTING_DTMF)
    if (ivrCurrentNode == "start") {
      if (key == "1") {
        webLog("[IVR Case 5] Caller presses 1 (Warranty option).");
        playIvrAudio("/01/005.mp3"); // Enter product number followed by #
        warrantyInputBuffer = "";
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
    BaseType_t res = xTaskCreatePinnedToCore(verifyIvrWarrantyTask, "warranty_task", 6144, (void *)data, 1, NULL, 0);
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
  if (data == NULL) { vTaskDelete(NULL); return; }

  int tempResult = 3; // default not found
  if (WiFi.status() == WL_CONNECTED && scriptId.length() > 0) {
    webLog("[HTTP Task] Verifying warranty via Google Sheets: " + data->productCode);
    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *http = new HTTPClient();
    
    if (client != nullptr && http != nullptr) {
      client->setInsecure();
      String encCode = data->productCode;
      encCode.replace("+", "%2B");
      encCode.replace(" ", "%20");
      String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=verifyWarranty&productCode=" + encCode + "&sheetId=" + sheetId;
      
      if (http->begin(*client, url)) {
        http->setTimeout(25000);
        http->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http->GET();
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http->getString();
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
        http->end();
      } else {
        webLog("[HTTP Task] Warranty connection failed.");
        tempResult = 3;
      }
    } else {
      webLog("[HTTP Task] Memory allocation failed for warranty verification client/http");
    }
    
    if (client != nullptr) delete client;
    if (http != nullptr) delete http;
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
    BaseType_t res = xTaskCreatePinnedToCore(registerComplaintTask, "complaint_task", 6144, (void *)data, 1, NULL, 0);
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
    WiFiClientSecure *client = new WiFiClientSecure();
    HTTPClient *http = new HTTPClient();
    
    if (client != nullptr && http != nullptr) {
      client->setInsecure();
      String encWarranty = data->warrantyId;
      encWarranty.replace("+", "%2B");
      encWarranty.replace(" ", "%20");
      String encPhone = data->phone;
      encPhone.replace("+", "%2B");
      encPhone.replace(" ", "%20");
      
      String url = "https://script.google.com/macros/s/" + scriptId + "/exec?action=registerComplaint&warrantyId=" + encWarranty + "&phone=" + encPhone + "&issue=IVR_Auto_Register&sheetId=" + sheetId;
      
      if (http->begin(*client, url)) {
        http->setTimeout(25000);
        http->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        int httpCode = http->GET();
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http->getString();
          webLog("[HTTP Task] Complaint Response: " + payload);
          
          // Find ticket number
          int startIdx = payload.indexOf("\"ticketNumber\":\"");
          if (startIdx != -1) {
            int endIdx = payload.indexOf("\"", startIdx + 16);
            if (endIdx != -1) {
              ticket = payload.substring(startIdx + 16, endIdx);
            }
          } else {
            startIdx = payload.indexOf("\"ticket\":\"");
            if (startIdx != -1) {
              int endIdx = payload.indexOf("\"", startIdx + 10);
              if (endIdx != -1) {
                ticket = payload.substring(startIdx + 10, endIdx);
              }
            }
          }
          
          if (payload.indexOf("\"status\":\"success\"") != -1) {
            tempResult = 1;
          } else {
            tempResult = 2;
          }
        } else {
          webLog("[HTTP Task] Complaint registration HTTP failed. Code: " + String(httpCode));
          tempResult = 2;
        }
        http->end();
      } else {
        webLog("[HTTP Task] Complaint registration connection failed.");
        tempResult = 2;
      }
    } else {
      webLog("[HTTP Task] Memory allocation failed for complaint registration client/http");
    }
    
    if (client != nullptr) delete client;
    if (http != nullptr) delete http;
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

String getStateName(CallState state) {
  switch(state) {
    case STATE_IDLE:             return "IDLE";
    case STATE_RINGING:          return "RINGING";
    case STATE_VERIFY_USER:      return "VERIFY USER";
    case STATE_ANSWERING:        return "ANSWERING";
    case STATE_CALL_CONNECTED:   return "CALL CONNECTED";
    case STATE_PLAY_WELCOME:     return "PLAY WELCOME";
    case STATE_PLAY_MENU:        return "PLAY MENU";
    case STATE_WAIT_DTMF:        return "WAIT DTMF";
    case STATE_PLAY_SELECTION:   return "PLAY SELECTION";
    case STATE_RETURN_MENU:      return "RETURN MENU";
    case STATE_HANGUP:           return "HANGUP";
    case STATE_CALLBACK_DIALING: return "CALLBACK DIALING";
    case STATE_COLLECTING_PRODUCT_NO: return "WARRANTY INPUT";
    case STATE_AGENT_CONNECTING: return "AGENT CONNECTING";
    case STATE_VALIDATING_SECOND_CALL: return "VALIDATING SECOND CALL";
    case STATE_ENDED:            return "ENDED";
    case STATE_ACTIVE_CALL:      return "ACTIVE CALL";
    case STATE_COLLECTING_DTMF:  return "COLLECTING DTMF";
    default:                     return "UNKNOWN";
  }
}

unsigned long getAudioDuration(uint8_t folder, uint8_t track) {
  return 0; // Unused, library manages EOF dynamically
}

bool checkAudioFinished(unsigned long maxDurationMs) {
  // If the background task set audioPlaying to false, playback has finished.
  if (!audioPlaying) {
    return true;
  }
  // Safety timeout check to prevent infinite loops in case EOF is not captured
  if (millis() - lastAudioPlayMillis > maxDurationMs) {
    webLog("[Audio] Safety timeout reached. Forcing playback finished.");
    audioPlaying = false;
    mp3Stop();
    return true;
  }
  return false;
}

String getDFPlayerCompatiblePath(String filename) {
  filename.toLowerCase();
  filename.trim();
  int lastSlash = filename.lastIndexOf('/');
  String name = (lastSlash != -1) ? filename.substring(lastSlash + 1) : filename;
  
  if (name.indexOf("welcome") != -1) {
    return "/01/001.mp3";
  } else if (name.indexOf("main menu") != -1 || name.indexOf("mainmenu") != -1 || name.indexOf("menu") != -1) {
    return "/01/002.mp3";
  } else if (name.indexOf("option 1") != -1 || name.indexOf("option1") != -1) {
    return "/01/003.mp3";
  } else if (name.indexOf("option 2") != -1 || name.indexOf("option2") != -1) {
    return "/01/004.mp3";
  } else if (name.indexOf("option 3") != -1 || name.indexOf("option3") != -1) {
    return "/01/005.mp3";
  } else if (name.indexOf("option 4") != -1 || name.indexOf("option4") != -1) {
    return "/01/006.mp3";
  } else if (name.indexOf("option 5") != -1 || name.indexOf("option5") != -1) {
    return "/01/007.mp3";
  } else if (name.indexOf("option 6") != -1 || name.indexOf("option6") != -1) {
    return "/01/008.mp3";
  } else if (name.indexOf("option 7") != -1 || name.indexOf("option7") != -1) {
    return "/01/009.mp3";
  } else if (name.indexOf("option 8") != -1 || name.indexOf("option8") != -1) {
    return "/01/010.mp3";
  } else if (name.indexOf("option 9") != -1 || name.indexOf("option9") != -1) {
    return "/01/011.mp3";
  } else if (name.indexOf("exit") != -1) {
    return "/01/012.mp3";
  } else if (name.indexOf("rejection") != -1 || name.indexOf("unregistered") != -1) {
    return "/01/013.mp3";
  }
  
  // Folder 02
  else if (name.indexOf("e_1001") != -1) {
    return "/02/001.mp3";
  } else if (name.indexOf("e_1002") != -1) {
    return "/02/002.mp3";
  }
  
  // Folder 03
  else if (name.indexOf("t-001") != -1) {
    return "/03/001.mp3";
  } else if (name.indexOf("t-002") != -1) {
    return "/03/002.mp3";
  } else if (name.indexOf("t-004") != -1 || name.indexOf("t-003") != -1) {
    return "/03/003.mp3";
  } else if (name.indexOf("t-005") != -1) {
    return "/03/004.mp3";
  } else if (name.indexOf("t-006") != -1) {
    return "/03/005.mp3";
  }
  
  return filename;
}

void processNewIvrDtmf(char key) {
  webLog("[New IVR] Processing DTMF Key: " + String(key));
  lastActivityTime = millis();
  
  if (ivrSelectedLanguage == 0) {
    if (key == '1' || key == '2' || key == '3') {
      ivrSelectedLanguage = key - '0';
      webLog("[New IVR] Language selected: " + String(ivrSelectedLanguage));
      
      // Reset language warnings and timeout counts on successful selection
      languageWarningPending = false;
      languageDisconnectPending = false;
      dtmfTimeoutCount = 0;
      
      // Start validation task
      validationResult = -1; // Pending
      ValidateTaskData *vData = new ValidateTaskData();
      vData->phoneNumber = callerNumber;
      validatingNumber = callerNumber;
      xTaskCreatePinnedToCore(validateTask, "validate_task", 6144, (void *)vData, 1, NULL, 0);
      
      // Change state back to STATE_CALL_CONNECTED to wait for validation check
      changeState(STATE_CALL_CONNECTED);
      
      // Play validation audio in selected language
      if (ivrSelectedLanguage == 2) {
        // Tamil hold
        currentAudioFolder = 4;
        currentAudioTrack = 8;
      } else if (ivrSelectedLanguage == 3) {
        // Hindi hold
        currentAudioFolder = 4;
        currentAudioTrack = 11;
      } else {
        // English hold
        currentAudioFolder = 4;
        currentAudioTrack = 1;
      }
      mp3Play(currentAudioFolder, currentAudioTrack);
      lastAudioPlayMillis = millis();
    } else {
      webLog("[New IVR] Invalid language selection. Replaying Language Menu...");
      changeState(STATE_RETURN_MENU);
    }
    return;
  }
  
  if (callerIsRegistered) {
    if (key == '1') {
      // Warranty Check
      webLog("[Registered IVR] Option 1 selected (Warranty).");
      if (ivrSelectedLanguage == 2) {
        playIvrAudio("/01/005.wav"); // Tamil "enter serial number" or fallback
      } else if (ivrSelectedLanguage == 3) {
        playIvrAudio("/04/014.wav"); // Hindi "enter serial number"
      } else {
        playIvrAudio("/01/005.wav"); // English/fallback
      }
      warrantyInputBuffer = "";
      changeState(STATE_COLLECTING_PRODUCT_NO);
    } else if (key == '2') {
      // Complaint
      webLog("[Registered IVR] Option 2 selected (Complaint).");
      startComplaintRegistration();
    } else if (key == '3') {
      // Installation / Service
      webLog("[Registered IVR] Option 3 selected (Installation).");
      startAgentConnection();
    } else if (key == '4') {
      // Speak to Executive
      webLog("[Registered IVR] Option 4 selected (Executive).");
      startAgentConnection();
    } else if (key == '#') {
      webLog("[Registered IVR] '#' pressed. Replaying menu...");
      changeState(STATE_RETURN_MENU);
    } else if (key == '*') {
      webLog("[Registered IVR] '*' pressed. Hanging up...");
      changeState(STATE_HANGUP);
    }
    return;
  }
  
  // Legacy / custom handler fallback
  if (key >= '1' && key <= '9') {
    int track = 2 + (key - '0');
    currentAudioFolder = 1;
    currentAudioTrack = track;
    changeState(STATE_PLAY_SELECTION);
    mp3Play(currentAudioFolder, currentAudioTrack);
    lastAudioPlayMillis = millis();
  } else if (key == '0') {
    changeState(STATE_RETURN_MENU);
  } else if (key == '#') {
    changeState(STATE_RETURN_MENU);
  } else if (key == '*') {
    changeState(STATE_HANGUP);
  }
}
