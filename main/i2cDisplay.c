#include "display/display_functions.h"
#include "wifi.h"

void initDisplay(){
	i2c_master_init();
	ssd1306_init();
}

void displayLoadingInfo() {
	xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear",  2048, NULL, 6, NULL);
	vTaskDelay(100/portTICK_PERIOD_MS);
	xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text",  2048,
		(void *)" Watchdog v0.4.3\n\n Loading...\n\n\n Creators:\n P. Lesiak\n M. Pieniazek", 6, NULL);
}

// displayWifiDisconnected() {
// 	xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear",  2048, NULL, 6, NULL);
// 	vTaskDelay(100/portTICK_PERIOD_MS);
// 	xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text",  2048,
// 		(void *)" WARNING\n\n WiFi has been\n Disconnected", 6, NULL);
// }

//TODO: pass text as parameter
void displayTextShit(  ) {
	xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear",  2048, NULL, 6, NULL);
	vTaskDelay(100/portTICK_PERIOD_MS);
	xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text",  2048,
		(void *)"Hello Patryk!\nMulitine is OK!\nAnother line", 6, NULL);
}

void displayTextTest(const char *displayText) {
    xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear", 2048, NULL, 6, NULL);
    vTaskDelay(100/portTICK_PERIOD_MS);
    xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text", 2048,
        (void *)displayText, 6, NULL);
}

void displayShitInfo(float tempDown, float tempUp) {
	xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear",  2048, NULL, 6, NULL);
	vTaskDelay(100/portTICK_PERIOD_MS);
	xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text",  2048,
		(void *)"Chuj", 6, NULL);
}
