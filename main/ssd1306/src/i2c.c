/**
  ******************************************************************************
  * @file    i2c.c
  * @author  Baoshi <mail(at)ba0sh1(dot)com>
  * @version 0.1
  * @date    Dec 24, 2014
  * @brief   Bit-bang I2C interface for ESP8266
  *
  ******************************************************************************
  * @copyright
  *
  * Copyright (c) 2015, Baoshi Zhu. All rights reserved.
  * Use of this source code is governed by a BSD-style license that can be
  * found in the LICENSE.txt file.
  *
  * THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED
  * WARRANTY.  IN NO EVENT WILL THE AUTHOR(S) BE HELD LIABLE FOR ANY DAMAGES
  * ARISING FROM THE USE OF THIS SOFTWARE.
  *
  */

//#include "esp_common.h"
#include "i2c.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"


/**
 * @name Pin selection and configuration block
 * @{
 */

//! @brief GPIO MUX for SDA pin
//#define SDA_MUX  PERIPHS_IO_MUX_GPIO5_U
//! @brief GPIO FUNC for SDA pin
//#define SDA_FUNC FUNC_GPIO5
//! @brief GPIO pin location for SDA pin
//#define SDA_PIN  5
//! @brief GPIO bit location for SDA pin
//#define SDA_BIT  BIT5

//! @brief GPIO MUX for SCL pin
//#define SCL_MUX  PERIPHS_IO_MUX_GPIO4_U
//! @brief GPIO FUNC for SCL pin
//#define SCL_FUNC FUNC_GPIO4
//! @brief GPIO pin location for SCL pin
//#define SCL_PIN  4
//! @brief GPIO bit location for SCL pin
//#define SCL_BIT  BIT4

//! Delay amount in-between bits, with os_delay_us(1) I get ~300kHz I2C clock
#define _DELAY ets_delay_us(1)

/** @} */

static uint8_t g_scl_pin;
static uint8_t g_sda_pin;

// this is found in gpio.h from IoT SDK but not in FreeRTOS SDK
//#ifndef GPIO_PIN_ADDR
//#define GPIO_PIN_ADDR(i) (GPIO_PIN0_ADDRESS + i*4)
//#endif

#define _SDA1 gpio_set_level(g_sda_pin,1)
#define _SDA0 gpio_set_level(g_sda_pin,0)

#define _SCL1 gpio_set_level(g_scl_pin,1)
#define _SCL0 gpio_set_level(g_scl_pin,0)

#define _SDAX gpio_get_level(g_sda_pin)
#define _SCLX gpio_get_level(g_scl_pin)

i2c_cmd_handle_t cmd;
#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

void i2c_init(uint8_t scl_pin, uint8_t sda_pin)
{
    // g_scl_pin = scl_pin;
    // g_sda_pin = sda_pin;

    // gpio_set_pull_mode(g_scl_pin,GPIO_PULLUP_ONLY);
    // gpio_set_pull_mode(g_sda_pin,GPIO_PULLUP_ONLY);

    // gpio_set_direction(g_scl_pin,GPIO_MODE_INPUT_OUTPUT);
    // gpio_set_direction(g_sda_pin,GPIO_MODE_INPUT_OUTPUT);

    // // I2C bus idle state.
    // gpio_set_level(g_scl_pin,1);
    // gpio_set_level(g_sda_pin,1);



    i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = sda_pin,
		.scl_io_num = scl_pin,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 400000
	};
	i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}


bool i2c_start(void)
{
    // _SDA1;
    // _SCL1;
    // _DELAY;
    // if (_SDAX == 0) return false; // Bus busy
    // _SDA0;
    // _DELAY;
    // _SCL0;
    // return true;


    cmd = i2c_cmd_link_create();
    if(i2c_master_start(cmd) == ESP_OK)
        return true;
    else
        return false;
}



void i2c_stop(void)
{
    // _SDA0;
    // _SCL1;
    // _DELAY;
    // while (_SCLX == 0); // clock stretching
    // _SDA1;
    // _DELAY;

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}


// return: true - ACK; false - NACK
bool i2c_write(uint8_t data)
{
    // uint8_t ibit;
    // bool ret;

    // for (ibit = 0; ibit < 8; ++ibit)
    // {
    //     if (data & 0x80)
    //         _SDA1;
    //     else
    //         _SDA0;
    //     _DELAY;
    //     _SCL1;
    //     _DELAY;
    //     data = data << 1;
    //     _SCL0;
    // }
    // _SDA1;
    // _DELAY;
    // _SCL1;
    // _DELAY;
    // ret = (_SDAX == 0);
    // _SCL0;
    // _DELAY;

    // return ret;


    if(i2c_master_write_byte(cmd, data, true) == ESP_OK){
        return true;
    }
    else
        return false;

}


uint8_t i2c_read(void)
{
    uint8_t data = 0;
    // uint8_t ibit = 8;

    // _SDA1;
    // while (ibit--)
    // {
    //     data = data << 1;
    //     _SCL0;
    //     _DELAY;
    //     _SCL1;
    //     _DELAY;
    //     if (_SDAX)
    //         data = data | 0x01;
    // }
    // _SCL0;

    // return data;


    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
    return data;
}


// void i2c_set_ack(bool ack)
// {
//     _SCL0;
//     if (ack)
//         _SDA0;  // ACK
//     else
//         _SDA1;  // NACK
//     _DELAY;
//     // Send clock
//     _SCL1;
//     _DELAY;
//     _SCL0;
//     _DELAY;
//     // ACK end
//     _SDA1;
// }
