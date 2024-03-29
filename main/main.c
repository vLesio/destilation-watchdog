#include <stdio.h> //for basic printf commands
#include <string.h> //for handling strings
#include "freertos/FreeRTOS.h" //for delay,mutexs,semphrs rtos operations
#include <freertos/task.h>
#include "esp_system.h" //esp_init funtions esp_err_t 
#include "esp_wifi.h" //esp_wifi_init functions and wifi operations
#include "esp_log.h" //for showing logs
#include "esp_event.h" //for wifi event
#include "nvs_flash.h" //non volatile storage
#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_http_client.h"


#include "i2cDisplay.c"


#include "wifi.h"
#include "temps.h"
#include "d_mqtt.h"
#include "buzzer.h"
// #include "influxdb.h"
#include "gatts_demo.h"
// #include "ble.h"

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void rest_get()
{
    esp_http_client_config_t config_get = {
        .url = "http://worldclockapi.com/api/json/utc/now",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    vTaskDelay(500.0 / portTICK_PERIOD_MS);

    nvs_flash_init();

    initDisplay();
    displayLoadingInfo();

    initialize_buzzer();
    initialize_wifi();
    initialize_temp_sensors();
    initialize_mqtt();
    run_bt();
    // start_ble_server();
    
    TemperatureReading* tempKeg = malloc(sizeof(TemperatureReading));
    TemperatureReading* tempColumn = malloc(sizeof(TemperatureReading));
    tempKeg->lastValid = 20.0;
    tempKeg->invalidCount = 0;
    tempColumn->lastValid = 20.0;
    tempColumn->invalidCount = 0;

    //xTaskCreate(buzz, "buzz", 4096, NULL, 5, NULL);
    while(1) {
        vTaskDelay(5000.0 / portTICK_PERIOD_MS);
        // printf("MAC Address: %s\n", deviceId);
        // printf("Topic: %s\n", Topic);
        // printf("Char1: %s\n", char1_str);
        // printf("SSID: %s\n", char_ssid);
        // Read temp
        read_celcius(tempKeg, tempColumn);
        printf("Keg: %.1f Sample: %d\n", tempKeg->temperature, tempKeg->sample);
        printf("Column: %.1f Sample: %d\n", tempColumn->temperature, tempColumn->sample);

        // Display information on screen
        char displayText[70];
        snprintf(displayText, sizeof(displayText), " Temperatures\n Column: %.2f%s\n Keg:    %.2f%s\n\n Wifi:%s MQTT:%s",
            tempColumn->temperature,
            !tempColumn->invalidCount ? "" : " E",
            tempKeg->temperature,
            !tempKeg->invalidCount ? "" : " E",
            IsWifiConnected ? "OK" : "NC",
            IsMqttConnected ? "OK" : "NC");
        displayTextTest(displayText);

        // GET wifi
        if(IsWifiConnected == 0) {
            printf("WiFi not connected, skipping. WIFI_STATUS: %d\n", IsWifiConnected);
            start_buzzer();
            continue;
        }else{
            stop_buzzer();
        }
        
        check_temperatures(tempKeg, tempColumn);

        // setTemps(3.5, 8.999);

        // publish_temperatures(tempKeg, tempColumn);
        mqtt_publish_temp(tempKeg, tempColumn);
    }
}