#include <Arduino.h>
#include <SdsDustSensor.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include <WiFi.h>

#define LORA_FREQ 433E6

HardwareSerial hs(2); // UART2
SdsDustSensor sds(hs); //  additional parameters: retryDelayMs and maxRetriesNotAvailable

const int MINUTE = 60000;
const int WAKEUP_WORKING_TIME = MINUTE; // 30 seconds.
const int MEASUREMENT_INTERVAL = 2 * MINUTE;

void publish_alive();
void publish_pm_data(float pm25, float pm10);
void sendMessage(String outgoing);

unsigned long lastRun = 0;

char ssid[23];
char sMacAddr[18];

void setup() {
  delay(500);
  Serial.begin(115200);

    // Get deviceId
  uint8_t macAddr[6];
  snprintf(ssid, 23, "MCUDEVICE-%llX", ESP.getEfuseMac());
  WiFi.macAddress(macAddr);   // The MAC address is stored in the macAddr array.
  snprintf(sMacAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  Serial.println(ssid);
  Serial.println(sMacAddr);

  LoRa.setPins(SS, GPIO_NUM_2, GPIO_NUM_4);
  LoRa.setSPIFrequency (20000000);
  LoRa.setTxPower (20);
  if (!LoRa.begin(LORA_FREQ)) {
    delay (5000);
    ESP.restart();
  }

  LoRa.setPreambleLength(8);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);


  publish_alive();

  Serial.println("SDS011 dust sensor");
  sds.begin();
  // Prints SDS011 firmware version:
  Serial.print("SDS011 ");
  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setQueryReportingMode().toString()); // ensures sensor is in 'query' reporting mode
}

void loop() {

  // Check if waited long enough.
  if ((millis() - lastRun > MEASUREMENT_INTERVAL) || lastRun == 0) {

    // Wake up SDS011
    sds.wakeup();
    Serial.println("Waking up sensor");
    delay(WAKEUP_WORKING_TIME);
    // Get data from SDS011
    Serial.println("Querying sensor");
    PmResult pm = sds.queryPm();
    if (pm.isOk()) {
      publish_pm_data(pm.pm25, pm.pm10);
    } else {
      Serial.print("Could not read values from sensor, reason: ");
      Serial.println(pm.statusToString());
    }
    // Put SDS011 back to sleep
    WorkingStateResult state = sds.sleep();
    if (state.isWorking()) {
      Serial.println("Problem with sleeping the SDS011 sensor.");
    } else {
      Serial.println("SDS011 sensor is sleeping");
    }

    lastRun = millis();
  }

  delay(50);
}

void publish_pm_data(float pm25, float pm10) {

  Serial.print("PM2.5 = ");
  Serial.print(pm25); // float, μg/m3
  Serial.print(", PM10 = ");
  Serial.println(pm10);

  // maximum message length 128 Byte
  String payload = "";
  payload += "{\"PM2_5\":";
  payload += String(pm25);
  payload += ",\"PM10\":";
  payload += String(pm10);
  payload += ",\"sensor\":";
  payload += "\"sds011\"";
  payload += ",\"device\":";
  payload += "\"";
  payload += ssid;
  payload += "\"";
  payload += ",\"mac\":";
  payload += "\"";
  payload += sMacAddr;
  payload += "\"";
  payload += "}";
  sendMessage(payload);
}


void publish_alive() {

  // maximum message length 128 Byte
  String payload = "";
  payload += "{\"device\":";
  payload += "\"";
  payload += ssid;
  payload += "\"";
  payload += ",\"type\":";
  payload += "\"iamalive\"";
  payload += ",\"mac\":";
  payload += "\"";
  payload += sMacAddr;
  payload += "\"";
  payload += "}";
  sendMessage(payload);
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}