#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "buzzer.h"

char IsBuzzing = 0;
char IsAlreadyBuzzing = 0;

void initialize_buzzer() {
    // ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    printf("Initializing buzzer...\n");
    gpio_reset_pin(BUZEER_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BUZEER_PIN, GPIO_MODE_OUTPUT);
    IsAlreadyBuzzing = 0;
}

void buzz(void *pvParameters) {
    if(IsAlreadyBuzzing == 1) {
        vTaskDelete(NULL);
    }
    IsAlreadyBuzzing = 1;
    for(;;){
        gpio_set_level(BUZEER_PIN, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(BUZEER_PIN, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        if(IsBuzzing == 0) {
            IsAlreadyBuzzing = 0;
            vTaskDelete(NULL);
        }
    }
}

void start_buzzer(void *pvParameters) {
    IsBuzzing = 1;
    xTaskCreate(buzz, "buzz", 4096, NULL, 5, NULL);
}

void stop_buzzer(){
    IsBuzzing = 0;
}
