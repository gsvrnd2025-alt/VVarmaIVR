# ESP32 IVR Dashboard — API Reference

All requests to the backend require the following API Key header:
```http
X-API-Key: <your_api_key_from_env>
```

Default Base URL: `http://localhost:3000`

---

## Endpoints

### Dashboard
* **GET `/api/dashboard`**
  Returns the consolidated dashboard status.
  * **Response:**
    ```json
    {
      "ok": true,
      "data": {
        "mode": "AP",
        "deviceStatus": "Online",
        "wifiStatus": "DISCONNECTED",
        "ip": "192.168.4.1",
        "clients": [],
        "rssi": -75,
        "totalCalls": 48,
        "uptime": "01h 05m 12s",
        "updatedAt": "10:15:30"
      }
    }
    ```

### AP Mode Control
* **GET `/api/ap/status`**
  Get current Access Point configuration and clients.
* **POST `/api/ap/config`**
  Save new AP settings. Body keys: `ssid`, `password`, `gateway`, `maxClients`, `channel`, `security`.
* **POST `/api/ap/action`**
  Triggers start/stop/restart on AP. Body: `{ "action": "start"|"stop"|"restart" }`.

### STA Mode Control
* **GET `/api/sta/status`**
  Get Station mode connection status.
* **POST `/api/sta/connect`**
  Attempt to connect to a new WiFi router. Body: `{ "ssid": "...", "password": "..." }`.
* **POST `/api/sta/disconnect`**
  Disconnect from current WiFi.
* **POST `/api/sta/save`**
  Save STA credentials to EEPROM without connecting. Body: `{ "ssid": "...", "password": "..." }`.
* **GET `/api/sta/scan`**
  Triggers a WiFi network scan. Returns list of available networks.

### Connectivity Check
* **GET `/api/connectivity`**
  Get real-time ping latency and network status.

### EEPROM Management
* **GET `/api/eeprom`**
  Get EEPROM partition status and byte allocations.
* **POST `/api/eeprom/clear`**
  Clear WiFi credentials in EEPROM.
* **POST `/api/eeprom/factory-reset`**
  Trigger complete device factory reset.

### Call History
* **GET `/api/calls`**
  Get call logs. Supports query parameters:
  * `page` (default 1)
  * `limit` (default 10)
  * `search` (phone number lookup)
  * `type` (`incoming` | `outgoing`)
  * `status` (`completed` | `missed` | `failed`)
  * `sortBy` (`timestamp` | `duration` | `callerNumber`)
  * `sortOrder` (`asc` | `desc`)
* **DELETE `/api/calls`**
  Clear call log history.
* **GET `/api/calls/export`**
  Download CSV export of call logs.

### SMS Logs
* **GET `/api/sms`**
  Get SMS logs. Supports query parameters: `page`, `limit`, `search`.
* **GET `/api/sms/export`**
  Download CSV export of SMS logs.

### Analytics
* **GET `/api/analytics`**
  Get aggregated dashboard charts statistics (daily, weekly, monthly summaries).

### System Info
* **GET `/api/system`**
  Get detailed hardware health, partition spaces and SDK versions.

### IVR Simulator
* **POST `/api/ivr/call`**
  Initiate simulated call. Body: `{ "callerNumber": "..." }`.
* **POST `/api/ivr/hangup`**
  Hang up active simulated call. Body: `{ "id": "..." }`.
* **GET `/api/ivr/logs`**
  Get simulator interaction flow logs.

### Device Integration (ESP32 Endpoint)
* **POST `/api/device/telemetry`**
  Endpoint for physical ESP32 to update its real-time metrics.
* **POST `/api/device/reboot`**
  Trigger ESP32 soft-reboot.

---

## Socket.IO Events (Port 3000)

Clients can subscribe to realtime events using Socket.IO:
* `call:new` - Fired when a new call begins.
* `call:end` - Fired when call ends (provides ID and final duration).
* `sms:new` - Fired when a new SMS is received.
* `device:status` - Fired for heartbeat and state updates.
* `signal:update` - Signal strength broadcasts.
