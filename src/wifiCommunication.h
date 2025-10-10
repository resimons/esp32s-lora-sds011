#include <WiFi.h>

extern bool wifiIsDisabled;
extern String accessPointName;

void wifi_setup(void);
void wifi_enable(void);
void wifi_disable(bool saveInStorage);
bool isWifiConnected();
