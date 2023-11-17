#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

#include "temps.h"

// START OF AREA 51, DO NOT TOUCH UNDER ANY CIRCUMSTANCES

OneWireBus* owb = NULL;
owb_rmt_driver_info rmt_driver_info;
OneWireBus_ROMCode device_rom_codes[MAX_DEVICES] = {0};

DS18B20_Info * devices[MAX_DEVICES] = {0};
int num_devices = 0;
int errors_count[MAX_DEVICES] = {0};
int sample_count = 0;
TickType_t last_wake_time;

bool IsTopTermConnected = 0;
bool IsBottomTermConnected = 0;

void initialize_temp_sensors() {
    printf("Initializing temperature sensors...\n");

    owb = owb_rmt_initialize(&rmt_driver_info, GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(owb, true);

    printf("Looking for termometers...\n");
    OneWireBus_SearchState search_state = {0};
    bool found = false;
    owb_search_first(owb, &search_state, &found);
    while (found)
    {
        char rom_code_s[17];
        owb_string_from_rom_code(search_state.rom_code, rom_code_s, sizeof(rom_code_s));
        printf("Termometer %d : %s\n", num_devices, rom_code_s);
        device_rom_codes[num_devices] = search_state.rom_code;
        ++num_devices;
        owb_search_next(owb, &search_state, &found);
    }
    printf("Found %d device%s\n", num_devices, num_devices == 1 ? "" : "s");

    if (num_devices == 1)
    {
        // For a single device only:
        OneWireBus_ROMCode rom_code;
        owb_status status = owb_read_rom(owb, &rom_code);
        if (status == OWB_STATUS_OK)
        {
            char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
            owb_string_from_rom_code(rom_code, rom_code_s, sizeof(rom_code_s));
            printf("Single termometer %s present\n", rom_code_s);
        }
        else
        {
            printf("An error occurred reading ROM code: %d", status);
        }
    }
    else
    {
        // Search for a known ROM code (LSB first):
        // For example: 0x1502162ca5b2ee28
        OneWireBus_ROMCode known_device = {
            .fields.family = { 0x28 },
            .fields.serial_number = { 0xee, 0xb2, 0xa5, 0x2c, 0x16, 0x02 },
            .fields.crc = { 0x15 },
        };
        char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
        owb_string_from_rom_code(known_device, rom_code_s, sizeof(rom_code_s));
        bool is_present = false;

        owb_status search_status = owb_verify_rom(owb, known_device, &is_present);
        if (search_status == OWB_STATUS_OK)
        {
            printf("Termometer %s is %s\n", rom_code_s, is_present ? "present" : "not present");
        }
        else
        {
            printf("An error occurred searching for known device: %d", search_status);
        }
    }

    // Create DS18B20 devices on the 1-Wire bus
    for (int i = 0; i < num_devices; ++i)
    {
        // DS18B20_Info * ds18b20_info = ds18b20_malloc();  // heap allocation
        devices[i] = ds18b20_malloc();

        if (num_devices == 1)
        {
            printf("Single device optimisations enabled\n");
            ds18b20_init_solo(devices[i], owb);          // only one device on bus
        }
        else
        {
            ds18b20_init(devices[i], owb, device_rom_codes[i]); // associate with bus and device
        }
        ds18b20_use_crc(devices[i], true);           // enable CRC check on all reads
        ds18b20_set_resolution(devices[i], DS18B20_RESOLUTION);
    }

    if (num_devices > 0)
    {
        TickType_t last_wake_time = xTaskGetTickCount();

        ds18b20_convert_all(owb);

        // In this application all devices use the same resolution,
        // so use the first device to determine the delay
        ds18b20_wait_for_conversion(devices[0]);

        // Read the results immediately after conversion otherwise it may fail
        // (using printf before reading may take too long)
        float readings[MAX_DEVICES] = { 0 };
        DS18B20_ERROR errors[MAX_DEVICES] = { 0 };

        for (int i = 0; i < num_devices; ++i)
        {
            errors[i] = ds18b20_read_temp(devices[i], &readings[i]);
        }

        // Print results in a separate loop, after all have been read
        printf("\nTemperature readings (degrees C): sample %d\n", ++sample_count);
        for (int i = 0; i < num_devices; ++i)
        {
            if (errors[i] != DS18B20_OK)
            {
                ++errors_count[i];
            }

            printf("  %d: %.1f    %d errors\n", i, readings[i], errors_count[i]);
        }

        vTaskDelayUntil(&last_wake_time, SAMPLE_PERIOD / portTICK_PERIOD_MS);
    }
    else
    {
        printf("\nNo DS18B20 devices detected!\n");
    }
}

void read_celcius(TemperatureReading* tempOne, TemperatureReading* tempTwo) {
    ds18b20_convert_all(owb);

    // In this application all devices use the same resolution,
    // so use the first device to determine the delay
    ds18b20_wait_for_conversion(devices[0]);

    // Read the results immediately after conversion otherwise it may fail
    // (using printf before reading may take too long)
    float readings[MAX_DEVICES] = { 0 };
    DS18B20_ERROR errors[MAX_DEVICES] = { 0 };

    for (int i = 0; i < num_devices; ++i)
    {
        errors[i] = ds18b20_read_temp(devices[i], &readings[i]);
        if (errors[i] != DS18B20_OK)
        {
            ++errors_count[i];
        }
    }

    tempOne->probe = 1;
    tempOne->temperature = readings[0];
    tempOne->sample = sample_count;
    tempOne->errorsCount = errors[0];
    
    tempTwo->probe = 2;
    tempTwo->temperature = readings[1];
    tempTwo->sample = sample_count;
    tempTwo->errorsCount = errors[1];
}

// END OF AREA 51

