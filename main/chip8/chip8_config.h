#include "freertos/FreeRTOS.h"
#include "chip8.h"


void _window_init();
void _window_kill();
void _getKeystate(Chip8 *chip8);
uint64_t _get_tick();
void _beep();