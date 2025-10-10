#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <SPIFFS.h>

#include "config.h"
#include "wifiCommunication.h"
#include "mqtt.h"

unsigned long reconnectInterval = 5000;
// in order to do reconnect immediately ...
unsigned long lastReconnectAttempt = millis() - reconnectInterval - 1;

WiFiClientSecure net;
MQTTClient client;

bool checkMQTTconnection();

char ssid[23];
char sMacAddr[18];

void mqtt_setup() {

  // Get deviceId
  uint8_t macAddr[6];
  snprintf(ssid, 23, "MCUDEVICE-%llX", ESP.getEfuseMac());
  WiFi.macAddress(macAddr);   // The MAC address is stored in the macAddr array.
  snprintf(sMacAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  Serial.println(ssid);
  Serial.println(sMacAddr);


  net.setCACert(rootCABuff);
	net.setCertificate(certificateBuff);
	net.setPrivateKey(privateKeyBuff);

	client.begin(mqtt_server, mqtt_server_port, net);

  	while (!client.connect(sMacAddr)) {
		Serial.print(".");
		delay(100);
	}

  Serial.printf("Successfully connected to MQTT broker\r\n");
}

void mqtt_loop(){
  if (!client.connected()) {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastReconnectAttempt) > reconnectInterval) {
      lastReconnectAttempt = currentMillis;
      // Attempt to reconnect
      checkMQTTconnection();
    }
  }  

  if (client.connected()) {
    client.loop();
  }
}

bool checkMQTTconnection() {
  if (wifiIsDisabled) return false;

  if (WiFi.isConnected()) {
    if (client.connected()) {
      return true;
    } else {
      // try to connect to mqtt server
      if (client.connect(sMacAddr)) {
        Serial.printf("Successfully connected to MQTT broker\r\n");
      } else {
        Serial.printf("MQTT connection failed (but WiFi is available). Will try later ...\r\n");
      }
      return client.connected();
    }
  } else {
    Serial.printf("  No connection to MQTT server, because WiFi ist not connected.\r\n");
    return false;
  }  
}

void publish_mqtt_message( const char *topic, const char *payload){
  if (wifiIsDisabled) return;

  if (checkMQTTconnection()) {
      
    if (!client.publish(topic, payload)) {
      Serial.printf("Publish failed\r\n");
    }
  } else {
    Serial.printf("Cannot publish mqtt message, because checkMQTTconnection failed (WiFi or mqtt is not connected)\r\n");
  }
}
