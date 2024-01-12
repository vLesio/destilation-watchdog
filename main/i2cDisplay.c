#include "display/display_functions.h"

void initDisplay(){
	i2c_master_init();
	ssd1306_init();
}

void displayMockedShit() {
	xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear",  2048, NULL, 6, NULL);
	vTaskDelay(100/portTICK_PERIOD_MS);
	xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text",  2048,
		(void *)" Hello Patryk!\n Mulitine is OK!\n Another line", 6, NULL);
}

//TODO: pass text as parameter
void displayTextShit(  ) {
	xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear",  2048, NULL, 6, NULL);
	vTaskDelay(100/portTICK_PERIOD_MS);
	xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text",  2048,
		(void *)"Hello Patryk!\nMulitine is OK!\nAnother line", 6, NULL);
}
