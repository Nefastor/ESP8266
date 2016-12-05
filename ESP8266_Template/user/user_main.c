/*
	The MIT License (MIT)

	Copyright (c) 2016 Nefastor Online

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

// change done in GitHub (directly on the web) and committed from GitHub
// and now a second change.

#include "esp_common.h"			// Includes a number of commonly-used header files

#include "freertos/FreeRTOS.h"	// FreeRTOS core headers
#include "freertos/task.h"

#include "gpio.h"				// GPIO library
#include "hspi.h"

// GPIO Constants, see pin_mux_register.h (SDK) included through esp_common.h
#define LED_GPIO 		2		// ESP8266 GPIO pin number, NOT module D pin number
#define LED_GPIO_MUX	PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC 	FUNC_GPIO2


/******************************************************************************
 * FunctionName : blink_task
 * Description  : FreeRTOS-compliant task function
 * Parameters   : see FreeRTOS documentation
 * Returns      : nothing
*******************************************************************************/

void ICACHE_FLASH_ATTR
blink_task(void *pvParameters)
{
    uint8_t state = 0;							// Internal state of our LED pin
    
    while (1)
    {
    	state = 1 - state;						// Toggles between 0 and 1
    	GPIO_OUTPUT_SET (LED_GPIO, state);		// gpio.h macro to set or clear a pin
    	vTaskDelay (20);						// FreeRTOS delay function
    }
}

void ICACHE_FLASH_ATTR
counter_task(void *pvParameters)
{
	// MAX7219 :
	uint8 k = 0;
	uint8 digits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	taskENTER_CRITICAL();
	hspi_wait_ready ();
	hspi_send_uint16 (0x0C01);	// exit shutdown mode
	hspi_wait_ready ();
	hspi_send_uint16 (0x0A01);	// set brightness medium high
	hspi_wait_ready ();
	hspi_send_uint16 (0x0B07);	// scan all digits
	hspi_wait_ready ();
	hspi_send_uint16 (0x09FF);	// BCD decoding
	hspi_wait_ready ();
	taskEXIT_CRITICAL();
	while (1)
	{
		taskENTER_CRITICAL();
		hspi_send_uint16 (0x0100 | digits[0]);	// set first digit segments
		hspi_wait_ready ();
		hspi_send_uint16 (0x0200 | digits[1]);	// set first digit segments
		hspi_wait_ready ();
		hspi_send_uint16 (0x0300 | digits[2]);	// set first digit segments
		hspi_wait_ready ();
		hspi_send_uint16 (0x0400 | digits[3]);
		hspi_wait_ready ();
		hspi_send_uint16 (0x0500 | digits[4]);
		hspi_wait_ready ();
		hspi_send_uint16 (0x0600 | digits[5]);
		hspi_wait_ready ();
		hspi_send_uint16 (0x0700 | digits[6]);
		hspi_wait_ready ();
		hspi_send_uint16 (0x0800 | digits[7]);
		hspi_wait_ready ();
		taskEXIT_CRITICAL();
		vTaskDelay (1);	// comment-out for maximum counting speed
		// Counter increment
		digits[0]++;
		if (digits[0] == 10)
		{
			digits[0] = 0;
			digits[1]++;
		}
		if (digits[1] == 10)
		{
			digits[1] = 0;
			digits[2]++;
		}
		if (digits[2] == 10)
		{
			digits[2] = 0;
			digits[3]++;
		}
		if (digits[3] == 10)
		{
			digits[3] = 0;
			digits[4]++;
		}
		if (digits[4] == 10)
		{
			digits[4] = 0;
			digits[5]++;
		}
		if (digits[5] == 10)
		{
			digits[5] = 0;
			digits[6]++;
		}
		if (digits[6] == 10)
		{
			digits[6] = 0;
			digits[7]++;
		}
		if (digits[7] == 10)
		{
			for (k = 0; k<7; k++) digits[k] = 0;
		}
	}
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry point of user code, the user's "main" function
 * Parameters   : none
 * Returns      : nothing
*******************************************************************************/

void ICACHE_FLASH_ATTR
user_init(void)
{
	// Multiplex the LED pin as GPIO
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);

	// Tell FreeRTOS to start the LED blink task
    xTaskCreate(blink_task, "blink_task", 256, NULL, 2, NULL);

	hspi_init();

	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;

	hspi_mode(1, 0);
	hspi_clock (4);	// 10 MHz SCK

	xTaskCreate(counter_task, "counter_task", 256, NULL, 2, NULL);
}

