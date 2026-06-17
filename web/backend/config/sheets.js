'use strict';

/**
 * Google Sheets tab names and column → index mappings.
 * Column indices are 0-based (A=0, B=1, ...).
 * Keep this in sync with your actual Spreadsheet layout.
 */

module.exports = {
  tabs: {
    DASHBOARD:    'Dashboard',
    DEVICE:       'Device_Status',
    AP_CONFIG:    'AP_Config',
    STA_CONFIG:   'STA_Config',
    EEPROM:       'EEPROM',
    CALLS:        'Call_Logs',
    SMS:          'SMS_Logs',
    ANALYTICS:    'Analytics',
    SYSTEM:       'System_Info',
    IVR:          'IVR_Logs',
  },

  columns: {
    // Dashboard tab
    dashboard: {
      MODE:           0,
      DEVICE_STATUS:  1,
      WIFI_STATUS:    2,
      IP_ADDRESS:     3,
      CLIENTS:        4,
      RSSI:           5,
      TOTAL_CALLS:    6,
      LAST_CALLER:    7,
      LAST_CALL_TIME: 8,
      UPTIME:         9,
      UPDATED_AT:     10,
    },

    // Device_Status tab
    device: {
      FIRMWARE:     0,
      HEAP_FREE:    1,
      HEAP_TOTAL:   2,
      CPU_LOAD:     3,
      FLASH_USED:   4,
      FLASH_CAP:    5,
      TEMPERATURE:  6,
      TIMESTAMP:    7,
    },

    // AP_Config tab
    ap: {
      SSID:         0,
      PASSWORD:     1,
      GATEWAY:      2,
      MAX_CLIENTS:  3,
      CHANNEL:      4,
      SECURITY:     5,
      STATUS:       6,
      UPDATED_AT:   7,
    },

    // STA_Config tab
    sta: {
      SSID:         0,
      PASSWORD:     1,
      IP:           2,
      GATEWAY:      3,
      DNS:          4,
      SUBNET:       5,
      RSSI:         6,
      STATUS:       7,
      UPDATED_AT:   8,
    },

    // EEPROM tab
    eeprom: {
      KEY:          0,
      VALUE:        1,
      BYTES_USED:   2,
      WRITE_CYCLES: 3,
      LAST_UPDATED: 4,
    },

    // Call_Logs tab
    calls: {
      ID:           0,
      CALLER:       1,
      DIRECTION:    2,
      STATUS:       3,
      DURATION:     4,
      MENU:         5,
      TIMESTAMP:    6,
    },

    // SMS_Logs tab
    sms: {
      ID:           0,
      SENDER:       1,
      MESSAGE:      2,
      TYPE:         3,
      STATUS:       4,
      TIMESTAMP:    5,
    },

    // Analytics tab
    analytics: {
      DATE:         0,
      TOTAL:        1,
      COMPLETED:    2,
      MISSED:       3,
      FAILED:       4,
      AVG_DURATION: 5,
    },

    // IVR_Logs tab
    ivr: {
      ID:           0,
      CALLER:       1,
      MENU:         2,
      DURATION:     3,
      STATUS:       4,
      TIMESTAMP:    5,
    },
  },

  // Header rows for each tab (used when creating sheets programmatically)
  headers: {
    dashboard:  ['Mode','DeviceStatus','WiFiStatus','IPAddress','ConnectedClients','RSSI','TotalCalls','LastCaller','LastCallTime','Uptime','UpdatedAt'],
    device:     ['FirmwareVersion','HeapFree','HeapTotal','CPULoad','FlashUsed','FlashCapacity','Temperature','Timestamp'],
    ap:         ['SSID','Password','Gateway','MaxClients','Channel','Security','Status','UpdatedAt'],
    sta:        ['SSID','Password','IP','Gateway','DNS','Subnet','RSSI','Status','UpdatedAt'],
    eeprom:     ['Key','Value','BytesUsed','WriteCycles','LastUpdated'],
    calls:      ['ID','CallerNumber','Direction','Status','Duration','MenuSelection','Timestamp'],
    sms:        ['ID','Sender','Message','Type','Status','Timestamp'],
    analytics:  ['Date','TotalCalls','Completed','Missed','Failed','AvgDuration'],
    system:     ['Key','Value','Timestamp'],
    ivr:        ['ID','CallerNumber','MenuSelection','CallDuration','CallStatus','Timestamp'],
  },
};
