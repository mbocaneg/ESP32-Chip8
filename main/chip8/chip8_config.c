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

void _getKeystate(Chip8 *chip8){

}
uint64_t _get_tick(){
    return esp_timer_get_time();
}
void _beep(){
    
}