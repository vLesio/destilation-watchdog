#ifndef D_MQTT_
#define D_MQTT_

#include "temps.h" 

#define TEMPERATURE_TOPIC "560fnv5112/watchdog"

extern char Topic[22];

static const char *TAG_MQGTT = "MQTT_EXAMPLE";

extern char IsMqttConnected;

void initialize_mqtt();
void mqtt_publish();
void mqtt_publish_temp(TemperatureReading* tempKeg, TemperatureReading* tempCol);
    
#endif