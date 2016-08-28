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

// Include your own WiFi credentials
#include "credentials.h" // contains WIFI_SSID and WIFI_PASS const strings for your WiFi network

// For sprintf
#include <stdio.h>

// This task demonstrates the boring, blocking way of reading a DHT22
void task_demo_1(void *pvParameters)
{
	// Upon powering the sensor, no valid reading will be available for 1 second
	vTaskDelay (200);	// Running at 80 MHz, 1 second = 100 ticks : do the math

	while (1)
	{
		vTaskDelay (400);	// DHT22 specs recommend a 0.5 Hz sampling rate, but it can work faster (up to 2 Hz)

		// given this task's long delays, feed the watchdog first
		system_soft_wdt_feed();

		// start reading
		dht22_read ();

		// display the results
		dht22_sample_display ();
	}
}

// now split measurement and display into two tasks and use the interrupt-driven read function
void task_demo_2(void *pvParameters)
{
	vTaskDelay (200);	// might be at least a couple seconds

	while (1)
	{
		vTaskDelay (200);	// might be at least a couple seconds
		// 200 = 1s in practice because of 160 MHz operation

		// given this task's long delays, feed the watchdog first
		system_soft_wdt_feed();

		// start reading
		dht22_read_ed ();
	}
}

// to do : add code to send data over WiFi.

void task_demo_3(void *pvParameters)
{
	while (1)
	{
		if (sample_valid == 1)
		{
			dht22_sample_display ();

			// send a packet to the PC through UDP (use broadcast address for simplicity)
			// first create a string from temperature and humidity samples
			char payload[50];
			// for some reason "%f" format doesn't produce anything... ESP issue ?
			sprintf (payload, "%i.%i - %i.%i",sample_rh / 10, sample_rh % 10, sample_t / 10, sample_t % 10);
			drawString(payload,0,290,4);	// Font 4 is a medium font

			sample_valid = 0;	// sample "consumed"
		}

		vTaskDelay (1);	// 10 ms delay = 100 Hz execution
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

	// Disable watchdog
	system_soft_wdt_stop();

	// Deactivate WiFi to prevent "pause" on boot and save the planet. I mean power.
	// wifi_set_opmode(NULL_MODE);
	// new : I'm adding WiFi transmission of the samples !
	wifi_set_opmode(STATIONAP_MODE);

	// set WiFi credentials and connect to network
	struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
	sprintf(config->ssid, WIFI_SSID);
	sprintf(config->password, WIFI_PASS);
	wifi_station_set_config_current(config);	// should test the return value if network presence not guaranteed
	free(config);

	// Initialize DHT22 sensor module
	dht22_init();

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
    // xTaskCreate(task_demo_1, "tsk1", 256, NULL, 2, NULL);

    xTaskCreate(task_demo_2, "tsk1", 256, NULL, 2, NULL);
    xTaskCreate(task_demo_3, "tsk2", 256, NULL, 2, NULL);
}

