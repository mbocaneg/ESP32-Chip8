#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "ssd1306.h"
#include "freertos/semphr.h"

#include "chip8.h"
#include "chip8_config.h"
#include "roms.h"
#include "keypad/Keypad.h"
#include "keypad/config.h"

#define SECONDS(X) ((X) * (1000 / portTICK_RATE_MS))
#define MINUTES(Y) SECONDS(60 * (Y) )

//ROWS
#define GPIO_NUM_16 16
#define GPIO_NUM_17 17
#define GPIO_NUM_18 18
#define GPIO_NUM_19 19

//COLUMNS
#define GPIO_NUM_32 23
#define GPIO_NUM_33 27
#define GPIO_NUM_34 32
#define GPIO_NUM_35 33

#define GPIO_OUT_MASK  ( (1ULL<<GPIO_NUM_16) | (1ULL<<GPIO_NUM_17) | (1ULL<<GPIO_NUM_18) | (1ULL<<GPIO_NUM_19) ) 

#define GPIO_IN_MASK  ( (1ULL<<GPIO_NUM_32) | (1ULL<<GPIO_NUM_33) | (1ULL<<GPIO_NUM_34) | (1ULL<<GPIO_NUM_35) )

Chip8 c8;

SemaphoreHandle_t redraw_signal;

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
    int refresh = 0;
    while(1){
        if(xSemaphoreTake(redraw_signal, 9999999)){
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
        }
        
    }
}

/*
Keyscan task. Responsible for polling the keyboard
and registering keypresses. Whenever a keypress is
detected, the corresponding key flag is set in the
chip8's keypad register. It is cleared when the key
is released.
*/
static void keyscan_task(void *pvParameters){
    Keypad keypad;
    keypad._write_row = _write_row;
    keypad._read_col = _read_col;
    keypad._get_ms_tick = _get_ms_tick;
    keypad_init(&keypad, keymap);
    while(1){
        if (keypad_getchar(&keypad)) {
            char key = keypad.keypress;
            printf("KEYPRESS: %c\n", key);

            (key == '0')? chip8_keyset(&c8, 0x00): chip8_keyreset(&c8, 0x00);
            (key == '1')? chip8_keyset(&c8, 0x01): chip8_keyreset(&c8, 0x01);
            (key == '2')? chip8_keyset(&c8, 0x02): chip8_keyreset(&c8, 0x02);
            (key == '3')? chip8_keyset(&c8, 0x03): chip8_keyreset(&c8, 0x03);
            (key == '4')? chip8_keyset(&c8, 0x04): chip8_keyreset(&c8, 0x04);
            (key == '5')? chip8_keyset(&c8, 0x05): chip8_keyreset(&c8, 0x05);
            (key == '6')? chip8_keyset(&c8, 0x06): chip8_keyreset(&c8, 0x06);
            (key == '7')? chip8_keyset(&c8, 0x07): chip8_keyreset(&c8, 0x07);
            (key == '8')? chip8_keyset(&c8, 0x08): chip8_keyreset(&c8, 0x08);
            (key == '9')? chip8_keyset(&c8, 0x09): chip8_keyreset(&c8, 0x09);
            (key == 'A')? chip8_keyset(&c8, 0x0A): chip8_keyreset(&c8, 0x0A);
            (key == 'B')? chip8_keyset(&c8, 0x0B): chip8_keyreset(&c8, 0x0B);
            (key == 'C')? chip8_keyset(&c8, 0x0C): chip8_keyreset(&c8, 0x0C);
            (key == 'D')? chip8_keyset(&c8, 0x0D): chip8_keyreset(&c8, 0x0D);
            (key == '*')? chip8_keyset(&c8, 0x0E): chip8_keyreset(&c8, 0x0E);
            (key == '#')? chip8_keyset(&c8, 0x0F): chip8_keyreset(&c8, 0x0F);
        }
        else{
            printf("NO KEYPRESS\n");
            chip8_keyreset(&c8, 0x00);
            chip8_keyreset(&c8, 0x01);
            chip8_keyreset(&c8, 0x02);
            chip8_keyreset(&c8, 0x03);
            chip8_keyreset(&c8, 0x04);
            chip8_keyreset(&c8, 0x05);
            chip8_keyreset(&c8, 0x06);
            chip8_keyreset(&c8, 0x07);
            chip8_keyreset(&c8, 0x08);
            chip8_keyreset(&c8, 0x09);
            chip8_keyreset(&c8, 0x0A);
            chip8_keyreset(&c8, 0x0B);
            chip8_keyreset(&c8, 0x0C);
            chip8_keyreset(&c8, 0x0D);
            chip8_keyreset(&c8, 0x0E);
            chip8_keyreset(&c8, 0x0F);
        }


        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

// CHIP8 specific counter that is used for system delay.
static void delay_counter_task(void *pvParameters){
    while(1){
        if(c8.delay > 0){
            c8.delay -= 1;
            vTaskDelay(16 / portTICK_RATE_MS);
        }
    }
    

}

// CHIP8 specific counter that is used for system sound. After
// it counts down to zero, a buzzing tone needs to be triggered.
// I need to procure a speaker and add it to the setup to make it
// work.
static void sound_counter_task(void *pvParameters){
    while(1){
        if(c8.sound > 0){
            c8.sound -= 1;
            vTaskDelay(16 / portTICK_RATE_MS);
        }
    }
    
   
}


void app_main(void)
{

    //configure the keypad gpio lines
    gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = GPIO_OUT_MASK;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = GPIO_IN_MASK;
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);


    //load game rom onto main memory. for the time
    //being, 2 games are provided: invaders and pong

    chip8_loadmem(&c8, invaders, sizeof(invaders));
    // chip8_loadmem(&c8, pong, sizeof(pong));
    // chip8_loadmem(&c8, tetris, sizeof(tetris));

    chip8_get_tick = _get_tick;

    //initialize game window
    _window_init();

    vSemaphoreCreateBinary(redraw_signal);

    //initialize Chip8 core
    chip8_init(&c8);

    printf("Starting CHIP8\n");

    //create chip8 system tasks
    xTaskCreate(&chip8_task, "chip8_task", 16 * 1024, NULL, 1, NULL); 
    xTaskCreate(&draw_task, "draw_task", 16 * 1024, NULL, 1, NULL); 
    xTaskCreate(&keyscan_task, "keyscan_task", 16 * 1024, NULL, 1, NULL); 
    xTaskCreate(&delay_counter_task, "delay_counter_task", 1024, NULL, 1, NULL);
    xTaskCreate(&sound_counter_task, "sound_counter_task", 1024, NULL, 1, NULL);

}

