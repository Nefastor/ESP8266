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

#include <stdio.h>				// For sprintf

#include "credentials.h"		// WiFi network credentials (WIFI_SSID and WIFI_PASS)
#include "unity.h"				// MCUnity library

#include "gpio.h"

// Isolate as much MCUnity GUI code as possible
#include "GUI_code.h"

// GPIO Constants, see pin_mux_register.h (SDK) included through esp_common.h
#define LED_GPIO 		2		// ESP8266 GPIO pin number, NOT module D pin number
#define LED_GPIO_MUX	PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC 	FUNC_GPIO2

// NEW GUI SETUP PARADIGM : use a FreeRTOS task
// Make it into a "service" that can be triggered by the MCUnity packet parser
/////////////// WORK IN PROGRESS ////////////////////////////

#define SETUP_DELAY	1	// setup delay in FreeRTOS ticks (5 ms @ 160 MHz)

void task_gui_setup(void *pvParameters)
{
	while (1)
	{
		vTaskSuspend (NULL);		// start suspended

		while (unity_not_ready())
			vTaskDelay (1);	// delay is necessary, let other tasks work while waiting
		// note : longer delay doesn't help with initial setup glitch

		// Application (firmware) specific GUI setup operations:
		unity_setup_int (&exposed_variable,	"A variable", 0, 1000,	GUI1); // GUI_FLAGS(0,0,4,3,0,0,0));
		unity_setup_int (&adc,				"ADC sample", 0, 1023,			GUI2); // GUI_FLAGS(4,0,4,3,0,0,0));
		unity_setup_int (&test, "Test Value", -300, 300,			GUI3); // GUI_FLAGS(0,3,4,3,0,0,0));

		// Setup firmware functions so that they can be called from the Unity application
		unity_setup_function (increment_counter, "Increment",				GUI4); // GUI_FLAGS(4,3,4,3,0,0,0));

		// Setup "int" variable and function call button for the LED
		unity_setup_int (&led,	"LED State", 0, 1,	GUI5);
		unity_setup_function (toggle_led, "Toggle LED", GUI6);

		// Let's add some more !
//		unity_setup_function (toggle_led, "Toggle LED 2", GUI7);
//		unity_setup_function (toggle_led, "Toggle LED 3", GUI8);

		// After setting up the GUI, transition to "update" mode
		unity_setup_rtos_complete ();
	}

}


// sample the ADC periodically
void task_adc(void *pvParameters)
{
	while (1)
	{
		adc = system_adc_read();

		// variable has been modified: update the remote GUI

		// unity_update_int (1, 1);	// restrict update to adc variable

		// unity_update_int (0, 3);	// send all variables (explicit)
		unity_update_int (0, 0); // send all variables (implicit)

		// unity_update_int (1, 2); // send "adc" and the next variable


		vTaskDelay(50);
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

	// Multiplex the LED pin as GPIO
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);

	xTaskHandle setup_tsk;
    // FreeRTOS task creation : function, name, stack depth, parameter to function, priority, handle
    // for more details read : http://www.freertos.org/a00125.html
    xTaskCreate(task_gui_setup, "tsk1", 256, NULL, 2, &setup_tsk);
    unity_init_rtos (setup_tsk);

    xTaskCreate(task_adc, "tsk3", 256, NULL, 2, NULL);
}

