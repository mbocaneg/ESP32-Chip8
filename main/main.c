#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "ssd1306.h"

#include "chip8.h"
#include "chip8_config.h"
#include "roms.h"

#define SECONDS(X) ((X) * (1000 / portTICK_RATE_MS))
#define MINUTES(Y) SECONDS(60 * (Y) )

Chip8 chip8;

static void hello_task(void *pvParameters)
{
    while(1){
        printf("Hello World\n");
        vTaskDelay(SECONDS(2));
    }
}

static void chip8_task(void *pvParameters){
    printf("    at CHIP8 task\n");
    while(1){
        while(!chip8.halt){
            // printf("        clockcycling...\n");
            chip8_clockcycle(&chip8);
        }

        _window_kill();
    }
    

}


void app_main(void)
{



    // chip8_loadmem(&chip8, invaders, sizeof(invaders));
    chip8_loadmem(&chip8, pong, sizeof(pong));
    chip8_bind_io(&_getKeystate, &_drawScreen, &_get_tick, &_beep);
    _window_init();
    chip8_init(&chip8);

    // xTaskCreate(&hello_task, "hello_task", 1024, NULL, 5, NULL);
    printf("Starting CHIP8\n");
    xTaskCreate(&chip8_task, "chip8_task", 16 * 1024, NULL, 1, NULL); 
}

