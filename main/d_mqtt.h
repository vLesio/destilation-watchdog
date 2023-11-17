#ifndef D_MQTT_
#define D_MQTT_

#include "temps.h"

#define TEMPERATURE_TOPIC "TEMP"

static const char *TAG_MQGTT = "MQTT_EXAMPLE";

void initialize_mqtt();
void mqtt_publish();
void mqtt_publish_temp(TemperatureReading* temperature);
    
#endif