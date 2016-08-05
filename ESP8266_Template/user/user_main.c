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

/*
	Nefastor : This project is an attempt to get the HSPI working in the RTOS
	example project. If I can do that, then I can port my LCD library to this
	example and use it with FreeRTOS.
*/


#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/udp.h"

#include "ILI9341.h"

// WiFi credentials : create and use your own file in C:\Espressif\ESP8266_RTOS_SDK\include
// #include "credentials.h"

void task_lcd_1(void *pvParameters)
{

	int count = 1000;
	portTickType start, end;

	start = xTaskGetTickCount ();

	while(count)
	{
		drawNumber(count--,0,48,4);
		//vTaskDelay (1);
		//os_delay_us(1000);	// works even with FreeRTOS it seems
	}

	end = xTaskGetTickCount ();

	// display how many ticks this took.
	drawNumber(end - start,0,160,4);

	// Redo
	count = 1000;

	start = xTaskGetTickCount ();

	while(count)
	{
		drawNumber(count--,0,80,7);
		// vTaskDelay (2);
		//os_delay_us(1000);	// works even with FreeRTOS it seems
	}

	end = xTaskGetTickCount ();

	// display how many ticks this took.
	drawNumber(end - start,0,200,4);

	while (1);
}

void task_lcd_2(void *pvParameters)
{
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

void task_lcd_3(void *pvParameters)
{
	int color = 0;

	while(1)
	{
		color++;
		if (color > 650)
			color = 0;

		setTextColor (0);	// black
		drawString(" - nefastor.com -",0,160,4);
		vTaskDelay (100);
		setTextColor (color * 400);
		drawString(" - nefastor.com -",0,160,4);
		vTaskDelay (100);

	}
}

void task_lcd_4(void *pvParameters)
{
	static int count = 0;

	while(1)
	{
		count+=3;
		drawNumber(count++,0,144,4);
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

	// Simple tests

	// Let's try something simple : printing a string to the LCD
	//drawString("Test",0,0,2);
	//drawString("Test",0,16,4);


    // The following nested call is based on using the return value of drawString directly
    // drawString(system_get_sdk_version(),drawString("SDK version : ",0,0,2),0,2);

    // FreeRTOS task creation : function, name, stack depth, parameter to function, priority, handle
    // for more details : http://www.freertos.org/a00125.html
    //xTaskCreate(task_lcd_1, "tsk1", 256, NULL, 2, NULL); // try with different priority levels
    xTaskCreate(task_lcd_2, "tsk2", 256, NULL, 2, NULL);
    xTaskCreate(task_lcd_3, "tsk3", 256, NULL, 1, NULL);
    //xTaskCreate(task_lcd_4, "tsk4", 256, NULL, 2, NULL);


}

