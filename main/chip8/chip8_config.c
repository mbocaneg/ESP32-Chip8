#include "chip8_config.h"
#include "esp_log.h"
#include "ssd1306.h"

#define I2C_SDA 18
#define I2C_SCL 19

void _window_init(){
    if (ssd1306_init(0, I2C_SCL, I2C_SDA)) {
        ESP_LOGI("OLED", "oled initialized");
    }
    else {
        ESP_LOGE("OLED", "oled init failed");
    }
    ssd1306_clear(0);
    ssd1306_refresh(0, true);
}
void _window_kill(){
    ssd1306_clear(0);
    ssd1306_refresh(0, true);
}
void _drawScreen(Chip8 *chip8){
    // ssd1306_clear(0);
    ssd1306_draw_rectangle(0, 0*2, 0*2, 10, 10, SSD1306_COLOR_BLACK);
    ssd1306_draw_rectangle(0, 10*2, 10*2, 10, 10, SSD1306_COLOR_BLACK);
	for (uint16_t y = 0; y < 32; y++) {
		for (uint16_t x = 0; x < 64; x++) {
			bool state = (PIXELTEST(x, y)) ? true : false;
			if(state == true)
                ssd1306_fill_rectangle(0, x*2, y*2, 10, 10, SSD1306_COLOR_WHITE);
			else
                ssd1306_fill_rectangle(0, x*2, y*2, 10, 10, SSD1306_COLOR_BLACK);

		}
	}
    ssd1306_refresh(0, true);
}
void _drawScreenInvert(Chip8 *chip8){

}
void _getKeystate(Chip8 *chip8){

}
uint64_t _get_tick(){
    return esp_timer_get_time();
}
void _beep(){
    
}