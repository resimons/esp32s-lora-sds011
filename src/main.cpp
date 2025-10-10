#include <Arduino.h>
#include <SdsDustSensor.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include "wifiCommunication.h"
#include "mqtt.h"
#include "config.h"

#define LORA_FREQ 433E6

HardwareSerial hs(2); // UART2
SdsDustSensor sds(hs); //  additional parameters: retryDelayMs and maxRetriesNotAvailable

const int MINUTE = 60000;
const int WAKEUP_WORKING_TIME = MINUTE; // 30 seconds.
const int MEASUREMENT_INTERVAL = 2 * MINUTE;

void publish_alive();
void publish_pm_data(float pm25, float pm10);
void sendMessage(String outgoing);

void setup() {
  delay(500);
  Serial.begin(115200);

  wifi_setup();
  wifi_enable();
  delay(2000);

  mqtt_setup();
  mqtt_loop();
  delay(2000);

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
  mqtt_loop();

  // Wake up SDS011
  sds.wakeup();
  delay(WAKEUP_WORKING_TIME);
  // Get data from SDS011
  PmResult pm = sds.queryPm();
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25); // float, μg/m3
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);
    publish_pm_data(pm.pm10, pm.pm25);
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
    delay(MEASUREMENT_INTERVAL);
  }
}

void publish_pm_data(float pm25, float pm10) {

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
  publish_mqtt_message(mqttSensorTopic, payload.c_str());
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
  publish_mqtt_message(mqqtTopicAlive, payload.c_str());
  sendMessage(payload);
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}