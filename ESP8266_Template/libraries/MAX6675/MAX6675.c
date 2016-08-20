

#include "MAX6675.h"

//#include "pin_mux_register.h"
#include "espressif/esp_common.h"	// includes pin_mux_register.h
#include "gpio.h"

// small delay routine, hopefully written in a way that can't be optimized out by the compiler
// 5 million equates to roughly 0.5 second.
void delay (long k)
{
	volatile int j = 0;
	while (j != k) j++;
}

// Set GPIO pins (D2 as input for SO, D3 and D4 and outputs for CS# and SCK respectively)
// D2 is GPIO 4, D3 is GPIO 0, D4 is GPIO 2.
void max6675_init (void)
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

uint16 max6675_read (void)
{
	// Start reading a sample by lowering CS#
	GPIO_OUTPUT_SET (MAX6675_CS, 0);
	delay (50000);

	// Send 16 SCK pulses and read SO each time
	int pulse = 16;
	uint16 retval = 0;

	//for (pulse = 0; pulse < 15; pulse++)
	while (pulse--)
	{
		retval = retval << 1;					// shift the result to clear the LSB for the next bit
		retval |= GPIO_INPUT_GET (MAX6675_SO);	// read the SO pin

		GPIO_OUTPUT_SET (MAX6675_SCK, 1);		// start a clock pulse
		delay (5000);

		GPIO_OUTPUT_SET (MAX6675_SCK, 0);		// end a clock pulse
		delay (5000);

	}

	// After the read is complete, set CS# back up
	GPIO_OUTPUT_SET (MAX6675_CS, 1);

	return retval;
}
