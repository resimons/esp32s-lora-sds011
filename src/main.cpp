#include <Arduino.h>
#include <SdsDustSensor.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include <WiFi.h>
#include <Wire.h>
#include <SensirionI2CSen5x.h>

#define LORA_FREQ 433E6

#define SEALEVELPRESSURE_HPA (1013.25)


HardwareSerial hs(1); // UART2
SdsDustSensor sds(hs); //  additional parameters: retryDelayMs and maxRetriesNotAvailable

const int MINUTE = 60000;
const int WAKEUP_WORKING_TIME = MINUTE; // 30 seconds.
const int MEASUREMENT_INTERVAL = 2 * MINUTE;

void publish_alive();
void publish_pm_data(float pm1p0, float pm2p5, float pm4p0, float pm10p0);
void publish_temp(float temp, float humidity, float gasResistance);
void sendMessage(String outgoing);

unsigned long lastRun = 0;

char ssid[23];
char sMacAddr[18];
unsigned char sensorName[32];

SensirionI2CSen5x sen5x;

void setup() {
    delay(5000);
    Serial.begin(115200);

    // Get deviceId
    uint8_t macAddr[6];
    snprintf(ssid, 23, "MCUDEVICE-%llX", ESP.getEfuseMac());
    WiFi.macAddress(macAddr);   // The MAC address is stored in the macAddr array.
    snprintf(sMacAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    Serial.println(ssid);
    Serial.println(sMacAddr);

    Wire.begin(SDA, SCL);

    delay(1000);

    Serial.println("LoRa init start");
    LoRa.setPins(SS, GPIO_NUM_11, GPIO_NUM_6);
    LoRa.setSPIFrequency (20000000);
    LoRa.setTxPower (20);
    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        delay (5000);
        ESP.restart();
    }

    Serial.println("LoRa started");

    sen5x.begin(Wire);

    uint16_t error;
    char errorMessage[256];
    error = sen5x.deviceReset();
    if (error) {
        Serial.print("Error trying to execute deviceReset(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    error = sen5x.getProductName(sensorName, sizeof(sensorName));

    if (!error) {
        Serial.print("Error trying to execute getProductName(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        sprintf((char*)sensorName, "SEN5x");
    }

    // set a temperature offset in degrees celsius
    // Note: supported by SEN54 and SEN55 sensors
    // By default, the temperature and humidity outputs from the sensor
    // are compensated for the modules self-heating. If the module is
    // designed into a device, the temperature compensation might need
    // to be adapted to incorporate the change in thermal coupling and
    // self-heating of other device components.
    //
    // A guide to achieve optimal performance, including references
    // to mechanical design-in examples can be found in the app note
    // “SEN5x – Temperature Compensation Instruction” at www.sensirion.com.
    // Please refer to those application notes for further information
    // on the advanced compensation settings used
    // in `setTemperatureOffsetParameters`, `setWarmStartParameter` and
    // `setRhtAccelerationMode`.
    //
    // Adjust tempOffset to account for additional temperature offsets
    // exceeding the SEN module's self heating.
    float tempOffset = 0.0;
    error = sen5x.setTemperatureOffsetSimple(tempOffset);
    if (error) {
        Serial.print("Error trying to execute setTemperatureOffsetSimple(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Temperature Offset set to ");
        Serial.print(tempOffset);
        Serial.println(" deg. Celsius (SEN54/SEN55 only");
    }

    // Start Measurement
    error = sen5x.startMeasurement();
    if (error) {
        Serial.print("Error trying to execute startMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    publish_alive();
}

void loop() {

    uint16_t error;
    char errorMessage[256];

    // Read Measurement
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = sen5x.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
        Serial.print("Error trying to execute readMeasuredValues(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        if (!isnan(massConcentrationPm1p0) && !isnan(ambientTemperature)) {
            publish_pm_data(massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0);
            publish_temp(ambientTemperature, ambientHumidity, vocIndex);
        }
    }

    delay(60000);
}

void publish_pm_data(float pm1p0, float pm2p5, float pm4p0, float pm10p0) {

    String payload = "";
    payload += "{\"PM1_0\":";
    payload += String(pm1p0);
    payload += ",\"PM2_5\":";
    payload += String(pm2p5);
    payload += ",\"PM4_0\":";
    payload += String(pm4p0);
    payload += ",\"PM10\":";
    payload += String(pm10p0);
    payload += ",\"sensor\":";
    payload += (char *) sensorName;
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

void publish_temp(float temp, float humidity, float gasResistance) {

    String payload = "";
    payload += "{\"temperature\":";
    payload += String(temp);
    payload += ",\"humidity\":";
    payload += String(humidity);
    payload += ",\"gas_resistance\":";
    payload += String(gasResistance);
    payload += ",\"sensor\":";
    payload += (char *) sensorName;
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
    LoRa.endPacket();
    Serial.println(outgoing);
}