/*
    I2C driver for the ESP8266 
    Copyright (C) 2016 Nefastor Online (nefastor.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "i2c.h"

/**
 * Set SDA and SCL state. Nefastor made it inline
 *
 * Nefastor asks: exactly what is used to change the pin's state? Driver or pull-up?
 */
LOCAL inline void ICACHE_FLASH_ATTR
i2c_sda(uint8 state)
{
    // Use the pull-up to drive the bus
	gpio_set_pullup (I2C_SDA_MUX, state);
}

LOCAL inline void ICACHE_FLASH_ATTR
i2c_sck(uint8 state)
{
	// Use the pull-up to drive the bus
	gpio_set_pullup (I2C_SCK_MUX, state);
}

/**
 * I2C init function
 * This sets up the GPIO io
 */
void ICACHE_FLASH_ATTR
i2c_init(void)
{
    //Disable interrupts
//    ETS_GPIO_INTR_DISABLE();

    //Set pin functions as GPIO
    gpio_mux (I2C_SDA_PIN);
    gpio_mux (I2C_SCK_PIN);

    // Set pins as inputs
    gpio_setup_input(I2C_SDA_PIN);
	gpio_setup_input(I2C_SCK_PIN);

	// Set drivers as open drain
	gpio_setup_drive_strength (I2C_SDA_PIN, 1);
	gpio_setup_drive_strength (I2C_SCK_PIN, 1);

    // Activate pull-ups on both pins (sets the bus to its default state)
	gpio_pullup_on (I2C_SDA_MUX);
	gpio_pullup_on (I2C_SCK_MUX);

    //Turn interrupt back on
//    ETS_GPIO_INTR_ENABLE();

    return;
}

/**
 * I2C Start signal 
 */
void inline ICACHE_FLASH_ATTR
i2c_start(void)
{
    i2c_sda(1);
    i2c_sck(1);
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sda(0);
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sck(0);
    os_delay_us(I2C_SLEEP_TIME);
}

/**
 * I2C Stop signal 
 */
void inline ICACHE_FLASH_ATTR
i2c_stop(void)
{
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sck(1);
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sda(1);
    os_delay_us(I2C_SLEEP_TIME);
}

/**
 * Receive byte from the I2C bus
 * returns the byte
 */
uint8 ICACHE_FLASH_ATTR
i2c_readByte(void)
{
    uint8 data = 0;
    uint8 i;

    i2c_sda(1);	// release the bus (enable the pull-up, which the slave will need to overcome)


    for (i = 0; i < 8; i++)
    {
        i2c_sck(0);
        os_delay_us(I2C_SLEEP_TIME);

        i2c_sck(1);
        os_delay_us(I2C_SLEEP_TIME);

        data = (data << 1) | i2c_read();
    }

    os_delay_us(I2C_SLEEP_TIME);	// precautionary, might be unnecessary
    i2c_sck(0);			// in prevision for the ACK/NACK sent next
    os_delay_us(I2C_SLEEP_TIME);

    return data;
}

/**
 * Send I2C ACK to the bus
 * uint8 state 1 or 0
 *  0 for ACK
 *  1 for NACK
 */
void inline ICACHE_FLASH_ATTR
i2c_send_ack(uint8 state)
{
    i2c_sda(state);
    //Pulse the SCK
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sck(1);
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sck(0);
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sda(1);						// release the bus
    os_delay_us(I2C_SLEEP_TIME);
}


/**
 * Write byte to I2C bus
 * uint8 data: to byte to be written
 * Note : does NOT release SDA, this is done in i2c_check_ack()
 */
void ICACHE_FLASH_ATTR
i2c_writeByte(uint8 data)
{
    uint8 i = 8;

    while (i)		// can i-- go there instead ?
    {
    	i--;	// will go from 7 down to 0, and the loop will execute for 0

    	i2c_sck(0);
        os_delay_us(I2C_SLEEP_TIME);

        i2c_sda((data >> i) & 0x1);

        os_delay_us(I2C_SLEEP_TIME);
        i2c_sck(1);

        os_delay_us(I2C_SLEEP_TIME);
    }
}
/**
 * Receive I2C ACK from the bus
 * returns 1 or 0
 *  0 for ACK
 *  1 for NACK
 */
uint8 ICACHE_FLASH_ATTR
i2c_check_ack(void)
{
    uint8 ack;

    i2c_sck(0);  // following a byte write, SCK == 1 and has been for a while
    os_delay_us(I2C_SLEEP_TIME);
    i2c_sda(1);			// enable the pull-up, which the slave will need to overcome
    i2c_sck(1);
    os_delay_us(I2C_SLEEP_TIME);

    ack = i2c_read(); 	//Get SDA pin status (that's a macro, not a function)

    // the following two statements appears necessary, not sure why : investigate
    i2c_sck(0);
    os_delay_us(I2C_SLEEP_TIME);

    return (ack);	// bus state at that point : SCK low, SDA pulled up
}
