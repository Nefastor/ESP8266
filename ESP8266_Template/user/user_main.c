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

// GPIO Constants, see pin_mux_register.h (SDK) included through esp_common.h
#define LED_GPIO 		2		// ESP8266 GPIO pin number, NOT module D pin number
#define LED_GPIO_MUX	PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC 	FUNC_GPIO2

// MCUnity setup, phase 1 : declare the global variables MCUnity will let you interact with
int	exposed_variable = 0x12345678;
int adc;
int test = -120;
int led = 1;

// MCUnity setup, phase 2 : add in functions that can be triggered from GUI buttons
void increment_a_variable ()
{
	test++;
}

void toggle_led ()
{
	led = 1 - led;
	GPIO_OUTPUT_SET (LED_GPIO, led);
}

// MCUnity setup, phase 3 : define GUI tiles appearance flags

// macro parameters : tile location (X and Y), tile size (W and H), two colors, a reserved byte
// position and dimension are expressed in 1/16th of a display's dimensions.
// example : a row of 8 tiles of 2 x 2 cells each
#define	TILE_1	GUI_FLAGS(0,0,2,2,0,0,0)
#define	TILE_2	GUI_FLAGS(2,0,2,2,0,0,0)
#define	TILE_3	GUI_FLAGS(4,0,2,2,0,0,0)
#define	TILE_4	GUI_FLAGS(6,0,2,2,0,0,0)
#define	TILE_5	GUI_FLAGS(8,0,2,2,0,0,0)
#define	TILE_6	GUI_FLAGS(10,0,2,2,0,0,0)
#define	TILE_7	GUI_FLAGS(12,0,2,2,0,0,0)
#define	TILE_8	GUI_FLAGS(14,0,2,2,0,0,0)

// MCUnity setup, phase 4 : write a task to setup the application's GUI

void task_gui_setup(void *pvParameters)
{
	while (1)		// this loop allows MCUnity to re-setup the GUI if the application requests it
	{
		vTaskSuspend (NULL);		// start suspended, let MCUnity decide when to setup the GUI

		while (unity_not_ready())
			vTaskDelay (1);	// delay is necessary, let other tasks work while waiting
		// note : longer delay doesn't help with initial setup glitch

		////////// project-specific GUI setup code starts here ////////////

		// Application (firmware) specific GUI setup operations:
		unity_setup_int (&exposed_variable,	"A variable", 0, 1000,	TILE_1); // GUI_FLAGS(0,0,4,3,0,0,0));
		unity_setup_int (&adc,				"ADC sample", 0, 1023,			TILE_2); // GUI_FLAGS(4,0,4,3,0,0,0));
		unity_setup_int (&test, "Test Value", -300, 300,			TILE_3); // GUI_FLAGS(0,3,4,3,0,0,0));

		// Setup firmware functions so that they can be called from the Unity application
		unity_setup_function (increment_a_variable, "Increment",				TILE_4); // GUI_FLAGS(4,3,4,3,0,0,0));

		// Setup "int" variable and function call button for the LED
		unity_setup_int (&led,	"LED State", 0, 1,	TILE_5);
		unity_setup_function (toggle_led, "Toggle LED", TILE_6);

		// Let's add some more !
//		unity_setup_function (toggle_led, "Toggle LED 2", TILE_7);
//		unity_setup_function (toggle_led, "Toggle LED 3", TILE_8);

		////////// project-specific GUI setup code ends here ////////////

		// After setting up the GUI, transition to "update" mode
		unity_setup_rtos_complete ();
	}

}

// Example of a task that sends updates to the GUI

// sample the ADC periodically
void task_adc(void *pvParameters)
{
	while (1)
	{
		adc = system_adc_read();

		// variable has been modified: update the remote GUI :
		unity_update_int (0, 0);	// update all "int" variables (implicit)
		// unity_update_int (0, 3);	// update all "int" variables (explicit)
		// unity_update_int (1, 1);	// update only the "adc" variable (reduced network load)
		// unity_update_int (1, 2); // update "adc" and the next "int" variable

		vTaskDelay(50);		// dictates GUI refresh rate : be reasonable
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

	// MCUnity initialization : tell it which task will setup the GUI
	xTaskHandle setup_tsk;
    xTaskCreate(task_gui_setup, "tsk1", 256, NULL, 2, &setup_tsk);
    unity_init_rtos (setup_tsk);

    // Start the firmware's actual tasks, those that matter to you
    xTaskCreate(task_adc, "tsk2", 256, NULL, 2, NULL);
}

