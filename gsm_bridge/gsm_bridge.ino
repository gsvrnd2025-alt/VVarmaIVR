#define GSM_RX_PIN 16
#define GSM_TX_PIN 17

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  delay(500);
  Serial.println("GSM_BRIDGE_ACTIVE");
}

void loop() {
  while (Serial.available() > 0) {
    Serial2.write(Serial.read());
  }
  while (Serial2.available() > 0) {
    Serial.write(Serial2.read());
  }
}
