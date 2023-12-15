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

#include "d_mqtt.h"
#include "mqtt_client.h"

esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_CONNECTED");
        // esp_mqtt_client_subscribe(client, "my_topic", 0);
        esp_mqtt_client_publish(client, "my_topic", "Hi to all from ESP32 .........", 0, 1, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_DATA");
        printf("\nTOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MQGTT, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG_MQGTT, "Other event id: jakies");
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG_MQGTT, "Event dispatched from event loop base=");
    mqtt_event_handler_cb(event_data);
}

void initialize_mqtt(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.0.19",
  };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_publish(char topic[], char message[]) {
    esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
}

void mqtt_publish_temp(TemperatureReading* temp) {
    char buffer[100];
    sprintf(&buffer, "{probe: %d, temperature: %.1f}", temp->probe, temp->temperature);
    mqtt_publish(TEMPERATURE_TOPIC, buffer);
    //TODO: mqtt_publish(TEMPERATURE_TOPIC + deviceID, buffer);
}
