/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/12/1, v1.0 create this file.
*******************************************************************************/

/*
	JRO : This project is an attempt to get the HSPI working in the RTOS
	example project. If I can do that, then I can port my LCD library to this
	example and use it with FreeRTOS.

	Also, I don't want to mess with the makefile.

	C:\Espressif\ESP8266_RTOS_SDK\include should contain everything I need and
	is in the path for this project.


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
#include "credentials.h"

// it appears my LCD library isn't thread-safe :

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
	static int count = 0;

	while(1)
	{
		count++;
		drawNumber(count++,0,80,4);
	}
}

void task_lcd_3(void *pvParameters)
{
	static int count = 0;

	while(1)
	{
		count+=2;
		drawNumber(count++,0,112,4);
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
	// go to 160 MHz
	system_update_cpu_freq(160);

	// deactivate WiFi to prevent "pause" on boot.
	wifi_set_opmode(NULL_MODE);

	// Initialize TFT (also takes care of HSPI)
	begin();

	//os_delay_us (10000);	// maybe this could help
	setRotation(0);	// 0-2 : portrait. 1-3 : landscape

	//fillScreen(0xFFFF);
	fillScreen(0x0000);

	// JRO tests

	// Let's try something simple : printing a string to the LCD
	//drawString("JRO Test",0,0,2);
	//drawString("JRO Test",0,16,4);


    // The following nested call is based on using the return value of drawString directly
    // drawString(system_get_sdk_version(),drawString("SDK version : ",0,0,2),0,2);
	drawString("All Hail Nefastor !",0,16,2);
//	drawString(" - nefastor.com -",0,112,4);

    // FreeRTOS task creation : function, name, stack depth, parameter to function, priority, handle
    // for more details : http://www.freertos.org/a00125.html
    xTaskCreate(task_lcd_1, "tsk1", 256, NULL, 2, NULL); // try with different priority levels
    //xTaskCreate(task_lcd_2, "tsk2", 256, NULL, 2, NULL);
    //xTaskCreate(task_lcd_3, "tsk3", 256, NULL, 2, NULL);
    //xTaskCreate(task_lcd_4, "tsk4", 256, NULL, 2, NULL);


}

