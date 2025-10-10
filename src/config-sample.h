
// --- wifi ---------------------------------------------------------------------------------------------------------------------------------
const char* const wifi_ssid              = "<SSID>";
const char* const wifi_password          = "<password>";

// --- mqtt ---------------------------------------------------------------------------------------------------------------------------------
const char* const mqtt_server            = "<hostname>";
const int mqtt_server_port               = port number;

const char* const mqttSensorTopic        = "<topic>";
const char* const mqttWifiTopic          = "<topic>";
const char* const mqqtTopicAlive         = "<topic>";

// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define myTimezone                 "Europe/Amsterdam"

// --- Certificates MQTT ---------------------------------------------------------------------------------------------------------------------

const char* const rootCABuff = \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";

// Fill with your certificate.pem.crt with LINE ENDING
const char* const certificateBuff = \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";

// Fill with your private.pem.key with LINE ENDING
const char* const privateKeyBuff = \
"-----BEGIN PRIVATE KEY-----\n" \
"-----END PRIVATE KEY-----\n";
