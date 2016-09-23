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

	// initialize bit-banging driver
	i2c_init ();
}










/////////////////////////// SPI ACCESS /////////////////////////////////////////////////////////////




// modified version of hspi clock setup
void mpu9250_hspi_clock_old(uint16 prediv, uint8 cntdiv)
{
	uint8 uptime = cntdiv >> 2;		// divide by 4
	uint8 downtime = cntdiv - uptime;

	if((prediv==0)|(cntdiv==0))
		WRITE_PERI_REG(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);
	else
		WRITE_PERI_REG(SPI_CLOCK(HSPI),
		   (((prediv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
		   (((cntdiv - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
		   (((downtime - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
		   (((uptime - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...

	// it appears the "high" level must always be set to a length of zero, otherwise there's no SCK output...
}

void mpu9250_hspi_clock(uint16 prediv, uint8 cntdiv)
{
	if((prediv==0)|(cntdiv==0))
		WRITE_PERI_REG(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);
	else
		WRITE_PERI_REG(SPI_CLOCK(HSPI),
		   (((prediv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
		   (((cntdiv - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
		   ((((cntdiv >> 1) - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
		   ((((cntdiv >> 1) - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...

	// it appears the "high" level must always be set to a length of zero, otherwise there's no SCK output...
}

void mpu9250_hspi_clock_upgrade(uint16 prediv, uint8 sck_up, uint8 sck_dn)
{
	uint8 cntdiv = sck_up + sck_dn;

//	if((prediv==0)|(cntdiv==0))
//		WRITE_PERI_REG(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);
//	else
		WRITE_PERI_REG(SPI_CLOCK(HSPI),
		   (((prediv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
		   (((cntdiv - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
		   (((sck_up - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
		   (((sck_dn - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...

	// it appears the "high" level must always be set to a length of zero, otherwise there's no SCK output...
}



// This replaces the HSPI library's init function
void mpu9250_init()
{
	// Mux FSYNC (D1 / GPIO5) to GPIO and make it zero.
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	GPIO_OUTPUT_SET (5, 0);

	hspi_enable_80Mhz;	// Use 80 MHz system clock as SCK




	// try different drive modes / pull-ups
	// MISO
	/*
	hspi_io[0].GPIO_Pin = GPIO_Pin_12;
	hspi_io[0].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[0].GPIO_Pullup = 1;		// 0 to disable, 1 to enable
	hspi_io[0].GPIO_Mode = GPIO_Mode_Input;

	// MOSI
	hspi_io[1].GPIO_Pin = GPIO_Pin_13;
	hspi_io[1].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[1].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[1].GPIO_Mode = GPIO_Mode_Output;

	// SCK
	hspi_io[2].GPIO_Pin = GPIO_Pin_14;
	hspi_io[2].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[2].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[2].GPIO_Mode = GPIO_Mode_Output;

	// SSEL
	hspi_io[3].GPIO_Pin = GPIO_Pin_15;
	hspi_io[3].GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	hspi_io[3].GPIO_Pullup = 0;		// 0 to disable, 1 to enable
	hspi_io[3].GPIO_Mode = GPIO_Mode_Output;

	gpio_config(&hspi_io[0]);
	gpio_config(&hspi_io[1]);
	gpio_config(&hspi_io[2]);
	gpio_config(&hspi_io[3]);
	*/

	// hspi_init_gpio(); copied contents in this function
	// Set pin muxing for HSPI - known good
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); //GPIO12 is HSPI MISO pin (Master Data In)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); //GPIO13 is HSPI MOSI pin (Master Data Out)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); //GPIO14 is HSPI CLK pin (Clock)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); //GPIO15 is HSPI CS pin (Chip Select / Slave Select)

	hspi_mode(1, 0);


	//hspi_clock(HSPI_PRESCALER, HSPI_DIVIDER);
	//hspi_clock(1, 4);	// only settings that work appear to use prescaler 0 or 1
	//hspi_clock(0, 0);	// force 80 MHz SCK
	// hspi_clock(1, 39);	// 1 MHz SCK
	hspi_clock(3, 39);	// 250 KHz SCK, only setting that seems to be working
	//hspi_clock(15, 39);	// 62.5 KHz SCK fails (read zero)
	//hspi_clock(3, 60);	// slightly slower clock also appears to work
	//hspi_clock(3, 30);	// slightly faster clock also appears to work

	// EXPERIMENTAL
	// hspi_enable_ck_out_edge;	// fails
	//hspi_disable_ck_out_edge;	// appears to have no effect

	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;

	// Disable I²C interface
	// hspi_transaction(0,0,8,106,8, 0x10   ,0,0);
}

uint8 mpu9250_read (uint8 addr)
{

	return hspi_transaction(0,0,8,addr,0,0,8,8);
}
