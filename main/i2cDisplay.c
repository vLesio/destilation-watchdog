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
	vTaskDelay(100/portTICK_PERIOD_MS);
}


void displayTextTest(const char *displayText) {
    xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear", 2048, NULL, 6, NULL);
    vTaskDelay(100/portTICK_PERIOD_MS);
    xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text", 2048,
        (void *)displayText, 6, NULL);
}

void displayTextAt() {
    // Define row and column where the text will start
    int startRow = 2;
    int startCol = 2;

    // Define the text to be displayed
    const char* text = "DUPA";

    // Create a structure to pass both the text and the position
    struct displayTextParams {
        int row;
        int col;
        const char* text;
    } params;

    params.row = startRow;
    params.col = startCol;
    params.text = text;

    // Create a task to display the text at the specified position
    xTaskCreate(&task_ssd1306_display_text_at, "ssd1306_display_text_at", 2048, &params, 6, NULL);
}
