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

//#include <stdio.h>				// For sprintf

#include "credentials.h"		// WiFi network credentials (WIFI_SSID and WIFI_PASS)
#include "unity.h"				// MCUnity library
#include "mpu9250.h"
#include "gpio.h"				// To operate the LED

// EXPERIMENTAL
#include "hspi.h"

// GPIO Constants, see pin_mux_register.h (SDK) included through esp_common.h
#define LED_GPIO 		2		// ESP8266 GPIO pin number, NOT module D pin number
#define LED_GPIO_MUX	PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC 	FUNC_GPIO2

// MCUnity setup, phase 1 : declare the global variables MCUnity will let you interact with
//int	exposed_variable = 0x12345678;
int adc;
//int test = -120;
int led = 1;
// SPI stuff
int predivider = 4;		// clocking defaults for SPI (appear to work)
int sck_length = 40;
int sck_up = 8;
int sck_down = 32;
int spi_address = 117;	// default to MPU9250 "who am I" register address (7 bits)
int spi_data_in = 0;	// whatever comes out of the MPU9250


// MCUnity setup, phase 2 : add in functions that can be triggered from GUI buttons
// Special SPI GUI

void toggle_led ()
{
	led = 1 - led;
	GPIO_OUTPUT_SET (LED_GPIO, led);
}

// Test of sensor burst read
void spi_transaction ()
{

	// Start by reading 16 bits from registers 59-60
    mpu9250_read160 (59);
	// mpu9250_read_burst (59, 160); // Read 160 bits starting from register 59

    //spi_data_in = HSPI_FIFO_16[2]; // appears to be the temperature
    // Note : with these 16 bit pointers the array indices are "swapped".
    spi_data_in = HSPI_FIFO_16[3]; // appears to be Acc_Z

    //spi_data_in = HSPI_FIFO[0] >> 16;  // should be Acc_X
    //spi_data_in = HSPI_FIFO[0] & 0xffff;  // should be Acc_Y
    //spi_data_in = HSPI_FIFO[1] >> 16;  // should be Acc_Z
	//spi_data_in = HSPI_FIFO[1] & 0xffff; // appears to be the temperature

	// update the remote GUI (nothing : currently done by a FreeRTOS task)
}

void spi_transaction_who_am_I ()
{
	hspi_clock(predivider);		// set the clock
	// perform the transaction
	//spi_data_in = (int) hspi_transaction(0,0,0,0,8,spi_address | 0x80,8,0); // read / dummy
	//spi_data_in = (int) hspi_transaction(0,0,8,spi_address | 0x80,0,0,8,0); // also works
	spi_data_in = (int) hspi_transaction(8,spi_address | 0x80,0,0,0,0,8,0); // also works
	// update the remote GUI (nothing : currently done by a FreeRTOS task)
}

void i2c_transaction ()
{
	// read Who Am I to spi_data_in
	// This will be a single-byte read (see MEMS datasheet page 35)
	i2c_start();

	// send the IMU address
	i2c_writeByte(0xD0);	// write address
	// check for ACK. Assume the device responds
	i2c_check_ack ();

	// send the register address
	i2c_writeByte(117);	// "who am i"
	// check for ACK. Assume the device responds
	i2c_check_ack ();

	// restart
	i2c_start();

	// send the IMU address
	i2c_writeByte(0xD1);	// read address
	// check for ACK. Assume the device responds
	i2c_check_ack ();

	// read one byte
	spi_data_in = i2c_readByte();

	// NACK to end the read burst
	i2c_send_ack(0);

	i2c_stop();

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
		unity_setup_int (&predivider,	"Prediv", 0, 1000,	TILE_1); // GUI_FLAGS(0,0,4,3,0,0,0));
		//unity_setup_int (&sck_up,				"SCK up", 0, 63,			TILE_2); // GUI_FLAGS(4,0,4,3,0,0,0));
		//unity_setup_int (&sck_down,				"SCK down", 0, 63,			TILE_3); // GUI_FLAGS(4,0,4,3,0,0,0));
		unity_setup_int (&spi_address, "Register", -300, 300,			TILE_4); // GUI_FLAGS(0,3,4,3,0,0,0));
		unity_setup_int (&spi_data_in,	"Value", 0, 1,	TILE_5);

		// Setup firmware functions so that they can be called from the Unity application
		unity_setup_function (spi_transaction, "Transaction",				TILE_6); // GUI_FLAGS(4,3,4,3,0,0,0));
		//unity_setup_function (i2c_transaction, "Transaction",				TILE_6); // GUI_FLAGS(4,3,4,3,0,0,0));

		// Setup "int" variable and function call button for the LED

		unity_setup_function (toggle_led, "Toggle LED", TILE_7);

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
	// seems to have no impact on my current issue with SPI
	// And that's a problem, I think. Unless the IMU really doesn't care about the doubling of SCK

	// Connect to WiFi network
	wifi_set_opmode(STATION_MODE);	// need to set opmode before you set config
	struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
	sprintf(config->ssid, WIFI_SSID);
	sprintf(config->password, WIFI_PASS);
	wifi_station_set_config_current(config);
	free(config);

	// Multiplex the LED pin as GPIO
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);

	// MEMS IMU init
	mpu9250_init();
	//mpu9250_i2c_init();

	// Disable I²C interface
//	hspi_transaction(0,0,8,106,8, 0x10   ,0,0);


/*
	// MEMS test
	uint8 rv = mpu9250_read(117 + 0x80);
	if ((rv == 0x71) || (rv == 0x68))
		GPIO_OUTPUT_SET (LED_GPIO, 0);
	else
		GPIO_OUTPUT_SET (LED_GPIO, 1);

	// The previous test shows that I read neither 71 nor 68
	exposed_variable = rv;
	// shows that I read 148, which is 0x94. This is consistent across several resets.
*/
	// coded as 0 bit command, 8 bit address, 0 bit out, 8 bit in, 8 dummy bits
	//exposed_variable = (int) hspi_transaction(0,0,8,117 | 0x80,0,0,8,8);

	// coded as 1 bit command, 7 bit address, 0 bit out, 8 bit in, 8 dummy bits
	// fails (reads zero)
	// exposed_variable = (int) hspi_transaction(1,1,7,117,0,0,8,8);

	// reads 148 almost all the time
	// exposed_variable = (int) hspi_transaction(0,0,0,0,8,117 | 0x80,8,8);

	// attempt 32-bit read
	//exposed_variable = (int) hspi_transaction(0,0,0,0,8,115 | 0x80,32,32);

	// attempt to reset and wake up the device. No effect.
	/*
	os_delay_us (10);
	hspi_transaction(0,0,0,0,16,0x6B80,0,0);
	os_delay_us (10);
	hspi_transaction(0,0,0,0,16,0x6B01,0,0);
	os_delay_us (10);
	*/

	// attempt to read another register
	//exposed_variable = (int) hspi_transaction(0,0,0,0,8,117 | 0x80,8,8);

	// replace dummy cycles with a null write
	// 117 | 0x80 is 245 / 0xF5.
	// exposed_variable = (int) hspi_transaction(0,0,0,0,16,0xF500,8,0);
	// this reads 211, it seems.

	// Attempt a 24-bit read
	//exposed_variable = (int) hspi_transaction(0,0,0,0,32,0xF5000000,24,0);

	// MCUnity initialization : tell it which task will setup the GUI
	xTaskHandle setup_tsk;
    xTaskCreate(task_gui_setup, "tsk1", 256, NULL, 2, &setup_tsk);
    unity_init_rtos (setup_tsk);

    // Start the firmware's actual tasks, those that matter to you
    xTaskCreate(task_adc, "tsk2", 256, NULL, 2, NULL);
}

