#include "mpu9250.h"
#include "hspi.h"
#include "gpio.h"	// to control the discrete signals.
// #include "esp_common.h"
#include "i2c.h"

// Let's try to use a more comprehensive IO setup routine:
GPIO_ConfigTypeDef hspi_io[4];

//////////////////////////// I2C ACCESS ///////////////////////////////////////////////////////////

void mpu9250_i2c_init ()
{
	// force the address LSB to zero (AD0)
	PIN_FUNC_SELECT(AD0_MUX, AD0_FUNC);
	GPIO_OUTPUT_SET(AD0_PIN,0);

	// force nCS (through GPIO15) to "1"
	PIN_FUNC_SELECT(nCS_MUX, nCS_FUNC);
	GPIO_OUTPUT_SET(nCS_PIN,1);

	// force FSYNC (through GPIO15) to "0"
	PIN_FUNC_SELECT(FSYNC_MUX, FSYNC_FUNC);
	GPIO_OUTPUT_SET(FSYNC_PIN,0);
/*
	// SDA
	hspi_io[1].GPIO_Pin = GPIO_Pin_13;
	hspi_io[1].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[1].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[1].GPIO_Mode = GPIO_Mode_Out_OD;

	// SCL
	hspi_io[2].GPIO_Pin = GPIO_Pin_14;
	hspi_io[2].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[2].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[2].GPIO_Mode = GPIO_Mode_Out_OD;
*/

	// initialize bit-banging driver
	i2c_init ();

	// moar config
	gpio_config(&hspi_io[1]);
	gpio_config(&hspi_io[2]);
}




/////////////////////////// SPI ACCESS /////////////////////////////////////////////////////////////

// This replaces the HSPI library's init function
void mpu9250_init()
{
	// Mux FSYNC (D1 / GPIO5) to GPIO and make it zero.
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	GPIO_OUTPUT_SET (5, 0);

	// HSPI init
	hspi_init_gpio();		// Set pin muxing for HSPI
	hspi_mode(1, 0);		// Set SPI mode (polarity / phase)
	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;

    // MEMS init
	// Disable I²C interface
	// hspi_transaction(0,0,8,106,8, 0x10   ,0,0);
}

uint8 mpu9250_read (uint8 addr)
{
	return hspi_transaction(0,0,8,addr,0,0,8,8);
}
