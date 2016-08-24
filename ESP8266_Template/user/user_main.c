/******************************************************************************
 * Copyright 2016 Nefastor Online (nefastor.com)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * This version demonstrates the DHT22 sensor library
 * This version demonstrates the ILI9341 LCD controller library
 *
 * Full article and hardware schematics at :
 *
 * www.nefastor.com
 *
*******************************************************************************/

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/udp.h"

#include "dht22.h"
#include "ILI9341.h"

int first_cycle = 1;

void task_lcd_1(void *pvParameters)
{
	if (first_cycle)
	{
		fillScreen(0x0000);		// make the screen black
		vTaskDelay (200);	// might be at least a couple seconds
		first_cycle = 0;
	}

	vTaskDelay (200);	// might be at least a couple seconds

	//portENTER_CRITICAL();
	dht22_read ();
	//portEXIT_CRITICAL();
/*²oo
	// experimental : display sensor start bit duration
	int wid;
	wid = drawNumber(bit_duration[0],0,0,4);
	wid += drawString (" - ",wid,0,4);
	drawNumber(bit_duration[1],wid,0,4);

	// experimental : display all sensor bit durations (low + high phases)
	int idx;
	int col = 0;	// 0 for left, 1 for center, 2 for right
	int line = 32;	// in pixels
	for (idx = 2; idx < 82; idx+=2)
	{
		wid = col * 80;	// values : 0, 80, 160

		// if (line >= 300) break;		// don't overflow the LCD

		wid += drawNumber(bit_duration[idx],wid,line,2);
		wid += drawString (" - ",wid,line,2);
		drawNumber(bit_duration[idx + 1],wid,line,2);o

		col++;
		if (col == 3)
		{
			col = 0;
			line += 16;
		}
	}
*/
	int line = 0;

	// displaying the samples
	line += 16;

//	if (0)
//	{
		drawNumber(sample_rh,0,line,2);
		drawNumber(sample_t,80,line,2);
		drawNumber(sample_valid,160,line,2);
/*	}
	else
	{*/
		line += 16;
		drawNumber(samples[0],0,line,2);
		drawNumber(samples[1],40,line,2);
		drawNumber(samples[2],80,line,2);
		drawNumber(samples[3],120,line,2);
		drawNumber(samples[4],160,line,2);
//	}


}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

void user_init(void)
{
	// Go to 160 MHz
	system_update_cpu_freq(160);

	// Disable watchdog
	//wdt_disable();
	system_soft_wdt_stop();

	// Deactivate WiFi to prevent "pause" on boot and save the planet. I mean power.
	wifi_set_opmode(NULL_MODE);

	// Initialize DHT22 sensor module
	dht22_init();

	// Initialize TFT (also takes care of HSPI)
	begin();

	setRotation(0);	// 0-2 : portrait. 1-3 : landscape

	//fillScreen(0xFFFF);	// make the screen white
	//fillScreen(0x0000);		// make the screen black

	// Let's try something simple : printing a string to the LCD
	// drawString("Test",0,0,2);	// Font 2 is a small font
	// drawString("Test",0,16,4);	// Font 4 is a medium font
	// drawString("Test",0,60,6);	// Font 6 is a large font
	// There's also a font 7 (7-segment display) which only works for numbers :
	// drawNumber(1234,0,200,7);

	// Task start delay : the sensor should not be used for at least one second
/*	int tmp;
	for (tmp = 0;tmp < 35;tmp++)
		os_delay_us(50000);		// 50 ms delay
*/
    // FreeRTOS task creation : function, name, stack depth, parameter to function, priority, handle
    // for more details read : http://www.freertos.org/a00125.html
    xTaskCreate(task_lcd_1, "tsk1", 256, NULL, 2, NULL);
}

