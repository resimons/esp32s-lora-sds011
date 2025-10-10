extern char ssid[];
extern char sMacAddr[];

void mqtt_setup(void);
void mqtt_loop(void);
void publish_mqtt_message( const char *topic, const char *payload);
