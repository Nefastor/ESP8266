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



// Function to set a specific LED, by index (all others will be turned off)

uint32 kolor = 0x1;

void LED (int i)
{
	// start the frame
	hspi_wait_ready ();
	hspi_send_uint32 (0x00000000);
	// if necessary, turn off the LED's at the start of the strip
	int k = i;
	while (k--)
	{
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0000000);
	}
	// Turn on the desired LED
	hspi_wait_ready ();
	hspi_send_uint32 (0xF0001000);
	//kolor++;
	// Clean all other LED's
	/*k = 24 - i;
	while (k--)
	{
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0000000);
	}*/
	//hspi_send_uint32 (0xFFFFFFFF);
	//hspi_wait_ready ();
}

void ICACHE_FLASH_ATTR
rgb_task(void *pvParameters)
{
	int k = 0;
	while (1)
	{
		LED(k);
		k++;
		if (k == 145) k = 0;
		//os_delay_us (5000);
	}
	/*
    uint8_t state = 0;		// LED index on the strip

    while (1)
    {
    	LED(state);
    	state++;
    	if (state = 145) state = 0;
    	vTaskDelay (20);						// FreeRTOS delay function
    }
    */
}

void ICACHE_FLASH_ATTR
counter_task(void *pvParameters)
{
	// MAX7219 : can't be written to using HSPI.
		uint8 k = 0;
		uint8 digits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		hspi_wait_ready ();
		hspi_send_uint16 (0x0C01);	// exit shutdown mode
		hspi_wait_ready ();
		hspi_send_uint16 (0x0A0A);	// set brightness medium high
		hspi_wait_ready ();
		hspi_send_uint16 (0x0B07);	// scan all digits
		hspi_wait_ready ();
		hspi_send_uint16 (0x09FF);	// BCD decoding
		hspi_wait_ready ();
		while (1)
		{
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
			//os_delay_us (50000);
			//os_delay_us (50000);
			//os_delay_us (50000);
			//os_delay_us (50000);
			//k++;
			//if (k == 10) k = 0;
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
				digits[7] = 0;
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
	// Kill the watchdog
	system_soft_wdt_stop();	// blob API

	// Multiplex the LED pin as GPIO
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);

	// Tell FreeRTOS to start the LED blink task
    //xTaskCreate(blink_task, "blink_task", 256, NULL, 2, NULL);
    //while(1);

	hspi_init();

	// Specific implementation of the SCK setup that seems to solve the timing issue
	// This may well be specific to the APA102 LED's
	/******* WORKING CODE ************************************
	hspi_enable_prediv;
	WRITE_PERI_REG(SPI_CLOCK(HSPI),
	   (((8 - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
	   ((2 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
	   ((1 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
	   ((2 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...
	***********************************/

	// MORE RELIABLE TIMINGS
	/*
	hspi_enable_prediv;
	WRITE_PERI_REG(SPI_CLOCK(HSPI),
	   (((400 - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
	   ((2 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
	   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
	   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...
*/
/*
	hspi_enable_prediv;
	WRITE_PERI_REG(SPI_CLOCK(HSPI),
	   (((400 - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
	   ((3 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
	   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
	   ((3 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...
*/

	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;

	hspi_mode(1, 0);
	hspi_clock (40);	// .1 MHz SCK

	//xTaskCreate(rgb_task, "rgb_task", 256, NULL, 2, NULL);
	//while (1);

	// start frame, send one LED value, end frame
	//hspi_wait_ready ();
	//hspi_send_uint32 (0xFFFFFFFF);
	/*
	hspi_wait_ready ();
	hspi_send_uint32 (0x00000001);
	hspi_wait_ready ();
	hspi_send_uint32 (0xF0000011);	// rather random
	hspi_wait_ready ();
	hspi_send_uint32 (0xF0001001);	// rather random
	hspi_wait_ready ();
	hspi_send_uint32 (0xF0100001);	// rather random
	hspi_wait_ready ();
	//hspi_send_uint32 (0xFFFFFFFF);
	hspi_wait_ready ();
	*/

	xTaskCreate(counter_task, "counter_task", 256, NULL, 2, NULL);


	while (1);


	int k = 0;
	while (1)
	{
		LED(k);
		k++;
		if (k == 145) k = 0;
		//os_delay_us (5000);
	}


	while (1)
	{
		hspi_wait_ready ();
		hspi_send_uint32 (0x00000000);
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0000010);	// rather random
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0001000);	// rather random
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0100000);	// rather random
		hspi_wait_ready ();
		//hspi_send_uint32 (0xFFFFFFFF);
		hspi_wait_ready ();

		os_delay_us (50000);
		os_delay_us (50000);
		os_delay_us (50000);
		os_delay_us (50000);

		hspi_wait_ready ();
		hspi_send_uint32 (0x00000000);
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0000000);	// rather random
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0000000);	// rather random
		hspi_wait_ready ();
		hspi_send_uint32 (0xF0000000);	// rather random
		hspi_wait_ready ();
		//hspi_send_uint32 (0xFFFFFFFF);
		hspi_wait_ready ();

		os_delay_us (50000);
		os_delay_us (50000);
		os_delay_us (50000);
		os_delay_us (50000);
	}

}

