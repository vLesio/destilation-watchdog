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

#include "wifi.h"
#include "temps.h"
// #include "d_mqtt.h"
#include "buzzer.h"
#include "influxdb.h"
#include "gatts_demo.h"

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
    vTaskDelay(1000.0 / portTICK_PERIOD_MS);

    nvs_flash_init();

    initialize_buzzer();
    initialize_wifi();
    initialize_temp_sensors();
    // initialize_mqtt();
    run_bt();
    
    TemperatureReading* tempOne = malloc(sizeof(TemperatureReading));
    TemperatureReading* tempTwo = malloc(sizeof(TemperatureReading));

    //xTaskCreate(buzz, "buzz", 4096, NULL, 5, NULL);
    while(1) {
        vTaskDelay(5000.0 / portTICK_PERIOD_MS);

        // GET wifi
        if(IsWifiConnected == 0) {
            printf("WiFi not connected, skipping. WIFI_STATUS: %d\n", IsWifiConnected);
            start_buzzer();
            continue;
        }else if(IsWifiConnected == 1) {
            stop_buzzer();
        }

        // Read temp
        read_celcius(tempOne, tempTwo);
        printf("Reading: %.1f Sample: %d\n", tempOne->temperature, tempOne->sample);
        printf("Reading: %.1f Sample: %d\n", tempTwo->temperature, tempTwo->sample);

        publish_temperatures(tempOne, tempTwo);
        // mqtt_publish_temp(tempOne);
    }
}