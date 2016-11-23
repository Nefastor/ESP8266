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
	hspi_clock(40);		    // set SCK to 1 MHz
	//hspi_clock(0);		    // set SCK to 80 MHz - may not be reliable
	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;

	// IMPORTANT - REMEMBER TO SET SCK FREQUENCY BEFORE USING HSPI

    // MEMS init
	// Disable I²C interface
	// hspi_transaction(0,0,8,106,8, 0x10   ,0,0);
	// Reset MEMS
	//hspi_transaction(0,0,8,107,8, 0x80   ,0,0);
	// Power on the MEMS, Auto-select clock source
	hspi_transaction(0,0,8,107,8, 0x01   ,0,0);
/*
	hspi_transaction(0,0,8,108,8, 0x00   ,0,0);
	//os_delay_us (10);
	 */
/*
	hspi_transaction(0,0,8,0x1B,8, 0x18   ,0,0);
	hspi_transaction(0,0,8,0x1C,8, 0x08   ,0,0);
	hspi_transaction(0,0,8,0x1D,8, 0x09   ,0,0);
	hspi_transaction(0,0,8,0x37,8, 0x30   ,0,0);*/

//os_delay_us (10);
}

// Read a single 8 bit register (no auto-incrementation)
uint8 mpu9250_read (uint8 addr)
{
	return hspi_transaction(0,0,8,addr | 0x80,0,0,8,0);
}

// Read a single 16 bit register (no auto-incrementation)
uint16 mpu9250_read16 (uint8 addr)
{
	return hspi_transaction(0,0,8,addr | 0x80,0,0,16,0);
}

// Read all sensor registers (16 bits x 10)
uint16 mpu9250_read160 (uint8 addr)
{
	return hspi_transaction(0,0,8,addr | 0x80,0,0,160,0);
}

//////////// DOES NOT WORK //////////////////
// EXPERIMENTAL - Burst Read, 1 to 64 bytes
// Result will be in the HSPI buffer
// din_bits should never be zero
void mpu9250_read_burst (uint32 addr, uint32 din_bits)
{
	// read command needs to have MSB set:
	addr |= 0x80;

	hspi_wait_ready (); //wait for SPI to be ready

	//disable all phases of the transaction in case they were previously set
	//CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI|SPI_USR_MISO|SPI_USR_COMMAND|SPI_USR_ADDR|SPI_USR_DUMMY);

	// Setup the number of bits for each phase of the SPI transaction
	WRITE_PERI_REG(SPI_USER1(HSPI), ((7)&SPI_USR_ADDR_BITLEN)<<SPI_USR_ADDR_BITLEN_S |   			// Address
									  ((din_bits-1)&SPI_USR_MISO_BITLEN)<<SPI_USR_MISO_BITLEN_S); 			// Data In


	// Enable SPI transaction phases that send no data
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO);

	// Setup the address phase
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_ADDR); // enable the address phase
	WRITE_PERI_REG(SPI_ADDR(HSPI), addr<<(24)); // align address data to the high bits

	// Start the SPI transaction
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);

	// Return incoming (MISO) data

	hspi_wait_ready ();		//wait for SPI transaction to complete
}




