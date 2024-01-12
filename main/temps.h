#ifndef TEMPS_
#define TEMPS_

extern bool IsTopTermConnected;
extern bool IsBottomTermConnected;

typedef struct {
    int probe;
    float timestamp;
    float temperature;
    int invalidCount;
    float lastValid;
    int sample;
    int errorsCount;
}TemperatureReading;


#define GPIO_DS18B20_0       (CONFIG_ONE_WIRE_GPIO)
#define MAX_DEVICES          (2)
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define SAMPLE_PERIOD        (1000)   // milliseconds

void initialize_temp_sensors();
void read_celcius();
void check_temperatures();

extern TemperatureReading* tempOne;
extern TemperatureReading* tempTwo; 

#endif