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

Chip8 c8;

/*
Main Chip8 task. Responsible for cycling the 
chip8.
*/
static void chip8_task(void *pvParameters){
    printf("    at CHIP8 task\n");
    while(1){
        while(!c8.halt){
            chip8_clockcycle(&c8);
        }

        _window_kill();
    }
    
}

/*
Task that is responsible for drawing onto the OLED. 
It is set of whenever the chip8's redraw flag is set
to true. At which point, the chip8's graphics buffer 
is loaded onto the OLED's graphics memory.
*/
static void draw_task(void *pvParameters){
    while(1){
        if(c8.redraw == true){
            for (uint16_t y = 0; y < 32; y++) {
                for (uint16_t x = 0; x < 64; x++) {
                    bool state = (chip8_pixel_test(&c8, x, y)) ? true : false;
                    if(state == true)
                        ssd1306_fill_rectangle(0, x*2, y*2, 10, 10, SSD1306_COLOR_WHITE);
                    else
                        ssd1306_fill_rectangle(0, x*2, y*2, 10, 10, SSD1306_COLOR_BLACK);

                }
            }
            ssd1306_refresh(0, true);
            c8.redraw = false;
        }
    }
}

/*
Keyscan task. Responsible for polling the keyboard
and registering keypresses. Whenever a keypress is
detected, the corresponding key flag is set in the
chip8's keypad register. It is cleared when the key
is released.

TODO: Add keyboard logic. For the time being, dummy
key values are loaded onto the chip8's keypad register.
*/
static void keyscan_task(void *pvParameters){
    printf("waiting 5 seconds");
    vTaskDelay(SECONDS(5));
    printf("5 seconds UP!!!");
    while(1){
        printf("press\n");
        chip8_keyset(&c8, 0x05);
        vTaskDelay(SECONDS(1));
        printf("release\n");
        chip8_keyreset(&c8, 0x05);
        vTaskDelay(SECONDS(1));
    }
}


void app_main(void)
{
    //load game rom onto main memory. for the time
    //being, 2 games are provided: invaders and pong
    chip8_loadmem(&c8, invaders, sizeof(invaders));
    // chip8_loadmem(&c8, pong, sizeof(pong));

    //register IO callback functions
    chip8_bind_io(&_getKeystate, &_drawScreen, &_get_tick, &_beep);

    //initialize game window
    _window_init();

    //initialize Chip8 core
    chip8_init(&c8);

    printf("Starting CHIP8\n");

    //create chip8 system tasks
    xTaskCreate(&chip8_task, "chip8_task", 16 * 1024, NULL, 1, NULL); 
    xTaskCreate(&draw_task, "draw_task", 16 * 1024, NULL, 1, NULL); 
    xTaskCreate(&keyscan_task, "keyscan_task", 16 * 1024, NULL, 1, NULL); 

}

