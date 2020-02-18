
# ESP32-Chip8

A threaded Chip8 core implemented on an ESP32 microcontroller that runs under FreeRtos. Play retro video games in the palm of you hand!

# Usage

This entire project targets the ESP32 microcontroller using the Espressif build environment. As far as
libraries go, I am using my homegrown CHIP8 core and Keypad libraries. For the display, I am using
imxieyie's SSD1306 library, but will likely use my own display library in the future.

The entire bill of materials is minimal and includes only the following:

* ESP32 MCU
* SSD1306 OLED display(I2C variant in this case)
* Hex button keypad(8 leads)
* 3.3V power source(in my case, a Lipo battery)

To recreate the hardware, one first needs to connect the keypad to 8 of the ESP32's GPIO lines.
In my case, I used GPIO 16, 17, 18, and 19 for the keypad's row lines, and GPIO 32, 33, 34, and 35 
for its column lines. Next, the OLED needs to be connected to available i2c enabled pins. In my case,
I used GPIO pins 21 and 22 for SDA and SCL, respectively.

By default, the ROM Space Invaders is loaded into memory and launched, but one may substitute this
ROM with Pong or Tetris by commenting out the appropriate line(lines 188-190). This is not a 
very "graceful" approach and will require some looking into. Furthermore, more games need to be
tested for this project to truly be finished.
