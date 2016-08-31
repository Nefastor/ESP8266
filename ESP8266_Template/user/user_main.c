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

//#include "lwip/sockets.h"
//#include "lwip/dns.h"
//#include "lwip/netdb.h"
#include "lwip/udp.h"

#include "ILI9341.h"

#include <stdio.h>				// For sprintf

#include "credentials.h"		// WiFi network credentials (WIFI_SSID and WIFI_PASS)
#include "unity.h"

// extern globals for debugging unity.c
struct ip_addr unity_IP;

// global variables I want to expose on the Unity front-end
int	exposed_variable = 0x12345678;

void task_lcd_1(void *pvParameters)
{
	drawString ("Waiting...",0,0,4);

	// wait for connection to be established
	// wait until reception and processing of a broadcast packet from Unity
	while (unity_IP.addr == 0) vTaskDelay (1);	// delay is necessary, let other tasks work while waiting

	char ip_addr[50];
	int a = unity_IP.addr >> 24;
	int b = (unity_IP.addr >> 16) & 0xFF;
	int c = (unity_IP.addr >> 8) & 0xFF;
	int d = unity_IP.addr & 0xFF;
	sprintf (ip_addr,"%i.%i.%i.%i",d,c,b,a);	// reverse byte order
	drawString (ip_addr,0,0,4);				// shows the host IP

	// at this point, it's possible to setup the GUI
	unity_register_int (&exposed_variable, "exposed_variable", 0, 1000, 0xA5);

	while (1);
}


void task_lcd_1_old(void *pvParameters)
{


	while (1)
	{
		int adc = system_adc_read();

		char sample[5];
		sprintf (sample,"%04i",adc);

		//drawNumber(adc,0,0,6);
		drawString (sample,0,0,6);

		vTaskDelay(50);
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

	// Connect to WiFi network
	wifi_set_opmode(STATION_MODE);	// need to set opmode before you set config
	struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
	sprintf(config->ssid, WIFI_SSID);
	sprintf(config->password, WIFI_PASS);
	wifi_station_set_config_current(config);
	free(config);

	// Initialize TFT (also takes care of HSPI)
	begin();
	setRotation(0);	// 0-2 : portrait. 1-3 : landscape
	//fillScreen(0xFFFF);	// make the screen white
	fillScreen(0x0000);		// make the screen black

	// Initialize the Unity interface
	unity_init ();


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

