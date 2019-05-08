
#include "config.h"

uint32_t col_arr[] = {23, 27, 32, 33}; //input
uint32_t row_arr[] = {16, 17, 18, 19}; //output

/*
 * Function that returns the number of milliseconds elapsed after boot. Most MCUs offer
 * this functionality, e.g. Arduino's millis() or STM32's HAL_GetTick(), but it could
 * just as easily be implemented using a timer.
 * */
uint64_t _get_ms_tick(void) {
	return esp_timer_get_time();
}

/*
 * Function that simply sets/resets a keypad row. Again, this will vary from MCU to MCU, but
 * this would usually be done by toggling a GPIO pin.
 * */
void _write_row(uint8_t idx, uint8_t pinstate){
	gpio_set_level(row_arr[idx], pinstate);
}

/*
 * Function that reads in a keypad row. This will usually be implemented by reading in a
 * GPIO pin. MAKE SURE SAID GPIO IS CONFIGURED WITH A PULLUP, EITHER INTERNALLY OR
 * WITH AN EXTERNAL RESISTOR.
 * */
bool _read_col(uint8_t idx){
	if(gpio_get_level(col_arr[idx]) == 0)
		return false;
	else 
		return true;
}
