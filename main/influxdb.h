#ifndef INFLUX_
#define INFLUX_

#include "temps.h"

#define TAG "INFLUX"

void publish_temperatures(TemperatureReading* one, TemperatureReading* two);

#endif