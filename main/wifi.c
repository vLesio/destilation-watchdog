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
#include "buzzer.h"

// Extern
char IsWifiConnected = 0;

// Private
char connectRetryTries = 0;

static void wifi_event_handler(void* args, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    switch(event_id) {
        case WIFI_EVENT_STA_START:
            printf("Trying to connect to WiFi for the first time...\n");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            printf("No WiFi connection, trying to reconnect...\n");
            start_buzzer();
            IsWifiConnected = 0;
            esp_wifi_connect();
            break;
        case IP_EVENT_STA_GOT_IP:
            //esp_netif_ip_info_t* info = (esp_netif_ip_info_t*)args;
            printf("Connected to WiFi network.\n");
            IsWifiConnected = 1;
            stop_buzzer();
            break;
        default:
            printf("Unknown WiFi event received.\n");
            break;
    }
}

void initialize_wifi() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation_config);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration ={ //struct wifi_config_t var wifi_configuration
        .sta= {
            .ssid = SSID,
            .password= PASS
        }
    };

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}