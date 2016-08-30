/******************************************************************************
 * Copyright 2016 Nefastor Online (nefastor.com)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * This version demonstrates the ILI9341 LCD controller library
 *
 * Full article and hardware schematics at :
 *
 * www.nefastor.com/esp8266-ili9341-lcd-library
 *
*******************************************************************************/

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/udp.h"

#include "ILI9341.h"

void task_lcd_1(void *pvParameters)
{


	while (1)
	{
		int adc = system_adc_read();

		drawNumber(adc,0,0,6);
	}


	int color = 0;

	drawString("All Hail Nefastor !",0,16,2);

	while(1)
	{
		color++;
		if (color > 374)
			color = 0;

		setTextColor (color * 700);
		drawString(" - nefastor.com -",0,112,4);
		vTaskDelay (100);

		setTextColor (0);	// black
		drawString(" - nefastor.com -",0,112,4);
		vTaskDelay (100);
	}
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

	// Deactivate WiFi to prevent "pause" on boot and save the planet. I mean power.
	wifi_set_opmode(NULL_MODE);

	// Initialize TFT (also takes care of HSPI)
	begin();

	setRotation(0);	// 0-2 : portrait. 1-3 : landscape

	//fillScreen(0xFFFF);	// make the screen white
	fillScreen(0x0000);		// make the screen black

	// Let's try something simple : printing a string to the LCD
	// drawString("Test",0,0,2);	// Font 2 is a small font
	// drawString("Test",0,16,4);	// Font 4 is a medium font
	// drawString("Test",0,60,6);	// Font 6 is a large font
	// There's also a font 7 (7-segment display) which only works for numbers :
	// drawNumber(1234,0,200,7);

    // FreeRTOS task creation : function, name, stack depth, parameter to function, priority, handle
    // for more details read : http://www.freertos.org/a00125.html
    xTaskCreate(task_lcd_1, "tsk1", 256, NULL, 2, NULL);
}

