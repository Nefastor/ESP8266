/*
 *  Simple bit-banging library for reading temperature samples off a
 *  MAX6675 and attached K-type thermocouple.
 *
 *  Call the "max6675_init" function to setup the ESP8266 I/O pins prior to
 *  reading the chip.
 *
 *  Call the "max6675_read" function to grab the contents of the MAX6675 internal
 *  register, which is a 16-bit value. See the example code in "user_main.c"
 *  or read the MAX6675 datasheet for a complete description of the return value.
 *
 *  Visit Nefastor Online for more information, including the article connected to
 *  this code, which describes hardware connections and more :
 *
 */

/*
The MIT License (MIT)
Copyright (c) 2016 Nefastor Online

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "MAX6675.h"

#include "espressif/esp_common.h"	// includes pin_mux_register.h
#include "gpio.h"

// Set GPIO pins (D2 as input for SO, D3 and D4 and outputs for CS# and SCK respectively)
// D2 is GPIO 4, D3 is GPIO 0, D4 is GPIO 2.
inline void max6675_init (void)
{
	// First, mux the pins as GPIO
	// Reminder: the macro and constants are in pin_mux_register.h
	PIN_FUNC_SELECT (PERIPHS_IO_MUX_GPIO4_U,FUNC_GPIO4);	// D2 - SO
	PIN_FUNC_SELECT (PERIPHS_IO_MUX_GPIO0_U,FUNC_GPIO0);	// D3 - CS#
	PIN_FUNC_SELECT (PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);	// D4 - SCK

	// Now set the direction of each pin, using gpio.h macros
	GPIO_AS_INPUT (GPIO_Pin_4);
	GPIO_AS_OUTPUT (GPIO_Pin_0 | GPIO_Pin_2);

	// Finally, set the outputs' default state (SCK = 0, CS# = 1)
	GPIO_OUTPUT_SET (MAX6675_CS, 1);
	GPIO_OUTPUT_SET (MAX6675_SCK, 0);
}

inline uint16 max6675_read (void)
{
	// Start reading a sample by lowering CS#
	GPIO_OUTPUT_SET (MAX6675_CS, 0);

	// Send 16 SCK pulses and read SO each time
	int pulse = 16;
	uint16 retval = 0;

	while (pulse--)
	{
		retval = retval << 1;					// shift the result to clear the LSB for the next bit
		retval |= GPIO_INPUT_GET (MAX6675_SO);	// read the SO pin

		GPIO_OUTPUT_SET (MAX6675_SCK, 1);		// start a clock pulse

		GPIO_OUTPUT_SET (MAX6675_SCK, 0);		// end a clock pulse
	}

	// After the read is complete, set CS# back up (starts a new sampling cycle)
	GPIO_OUTPUT_SET (MAX6675_CS, 1);

	return retval;
}
