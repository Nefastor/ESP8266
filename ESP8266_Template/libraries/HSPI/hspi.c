#include "hspi.h"
#include "spi.c"

/*
	JRO : This library controls the HSPI (Hardware SPI) controller of the ESP8266.
	It is the SPI interface that uses the following pins :

	MISO GPIO12
	MOSI GPIO13
	CLK GPIO14
	CS GPIO15

	Important issue : this library CANNOT READ FROM SPI
	(unless of course there's a mechanism I'm missing such as a FIFO that receives
	data whenever data is sent) (speaking of which, receiving might be just as easy
	as reading the right "receive" register after transmitting anything)

	IIRC this library is based on work by someone called MetalPhreak or something like that.
	I will track down the source.
*/



// uint32_t *spi_fifo;		// pointer to the first SPI data register : SPI_W0(HSPI)
// uint32_t *spi_fifo = (uint32_t*) SPI_W0(HSPI);	// direct init from constant is OK

void hspi_init(void)
{
	//spi_fifo = (uint32_t*)SPI_W0(HSPI);
/*
	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105); //clear bit9 => why ?
	// answer : setting this bit meas SPI uses 80 MHz sys clock.
    // by clearing it, the prescaler is used.

	// Set pin muxing for HSPI
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); // HSPIQ MISO GPIO12
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); // HSPID MOSI GPIO13
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); // CLK GPIO14
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); // CS GPIO15
*/
	hspi_enable_80Mhz;

	// both work with ILI8341 but I don't know the real SCLK frequency.
	//spi_init_gpio(HSPI,SPI_CLK_USE_DIV);
	spi_init_gpio(HSPI,SPI_CLK_80MHZ_NODIV);
	//hspi_init_gpio();

	// Nefastor : a bit of research :
	/* HSPI_PRESCALER == 1
	 * I set clock cycle to 1, clock high for 0 cycle (somehow), clock low for 1 cycle
	 * In spi.c, MP sets clock low for 0 cycle (always) clock high for half of "cntdiv", which
	 * is set to 2 by default.
	 * So his settings are : SCLK period 2 cycle, high 1 cycle, low 0 cycle
	 * and my settings are : SCLK period 1 cycle, high 0 cycle, low 1 cycle
	 */

	// Set SCLK frequency and duty cycle
	/*
	WRITE_PERI_REG(SPI_CLOCK(HSPI),
	   (((HSPI_PRESCALER - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
	   ((1 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
	   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
	   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for one cycle
	   */

	// the following is not done in spi.c : superfluous ?
	// WRITE_PERI_REG(SPI_CTRL1(HSPI), 0);	// why ?

	// TO DO : explain what this does : (spi.c doesn't do it... yet it is necessary)
	// ANSWERS : enables / disables specific phases of an SPI command / transaction
	// not necessary if using spi.c "transaction" function, which manages those bits
	/*
	uint32_t regvalue = SPI_USR_MOSI;
    regvalue &= ~(BIT2 | SPI_USR_ADDR | SPI_USR_DUMMY | SPI_USR_MISO | SPI_USR_COMMAND | SPI_DOUTDIN);
	WRITE_PERI_REG(SPI_USER(HSPI), regvalue);
	*/

	// spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);
	// spi_rx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);
	spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_LOW_TO_HIGH);	// ILI9341 works like this
	spi_rx_byte_order(HSPI, SPI_BYTE_ORDER_LOW_TO_HIGH);	// this is the default order, by the way
}

// Send the same 16-bit value multiple times.
// Useful for filling pixels with the same color on 16bpp displays.
void hspi_send_uint16_r(uint16_t data, int32_t repeats)
{
	uint32_t i;
	uint32_t bts;	// bytes to send
	uint32_t word = data << 16 | data;	// SPI data registers are 32-bit : send two words at once !

	while (repeats > 0)
	{
		// outer loop for up to 512-bit transfers
		hspi_wait_ready();	// wait for previous transfer to complete

		// determine how many bytes will be sent during this iteration
		if (repeats >= 32)	// 512 bits = 32 x 16 bit words. Sending 32 or more means a full transfer right now
			bts = SPIFIFOSIZE << 2;		// cheap multiply by four
		else
			bts = repeats << 1;			// cheap multiply by two

		// now fill the FIFO and update "repeats"
		i = 0;	// FIFO index
		while ((repeats > 0) && (i < SPIFIFOSIZE))
		{
			// inner loop to fill the HSPI FIFO (up to SPIFIFOSIZE words of 32 bits)
			repeats -= 2;	// because I'm sending two words at once
			// spi_fifo[i++] = word;
			HSPI_FIFO[i++] = word;
		}

		// perform the transfer : (hspi_prepare_tx(bts)) and send :
		WRITE_PERI_REG(SPI_USER1(HSPI), (((bts << 3) - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);
		SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // send
	}
}

// Send up to SPIFIFOSIZE x 4 = 64 bytes. Warning : sending more will overflow the HSPI
void hspi_send_data(const uint8_t * data, int8_t datasize)
{
	// recast data pointer from 8 bits to 32 bits (the width of the HSPI data registers)
	uint32_t *_data = (uint32_t*)data;

	//hspi_prepare_tx(datasize); :
	WRITE_PERI_REG(SPI_USER1(HSPI), (((datasize << 3) - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);

	uint8_t i = 0;

	while (datasize > 0)	// because this will go negative if datasize % 4 != 0
	{
		HSPI_FIFO[i++] = *_data++;
		datasize -= 4;	// because I'm sending 4 bytes at a time

		// If FIFO overflow is an issue (shouldn't be !)
		// if (i == SPIFIFOSIZE) break;	// FIFO overflow protection
	}

	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

inline void hspi_wait_ready(void)
{
	while (READ_PERI_REG(SPI_CMD(HSPI))&SPI_USR);
}

inline void hspi_prepare_tx(uint32_t bytecount)
{
	WRITE_PERI_REG(SPI_USER1(HSPI), (((bytecount << 3) - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);
}


inline void hspi_start_tx()
{
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // send
}


inline void hspi_send_uint8(uint8_t data)
{
	// hspi_prepare_tx(1);
	WRITE_PERI_REG(SPI_USER1(HSPI), (((uint32_t) 7) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);
	*HSPI_FIFO = data;
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

inline void hspi_send_uint16(uint16_t data)
{
	//hspi_prepare_tx(2);
	WRITE_PERI_REG(SPI_USER1(HSPI), (((uint32_t) 15) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);

	*HSPI_FIFO = data;
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

inline void hspi_send_uint32(uint32_t data)
{
	// hspi_prepare_tx(4);
	WRITE_PERI_REG(SPI_USER1(HSPI), (((uint32_t) 31) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);

	*HSPI_FIFO = data;
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

/* NEFASTOR - IN PROGRESS :
 *
 * I'm going to implement functions similar to spi.c for setting up the HSPI
 *
 */

inline void hspi_init_gpio (void)
{
	// Set pin muxing for HSPI
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); //GPIO12 is HSPI MISO pin (Master Data In)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); //GPIO13 is HSPI MOSI pin (Master Data Out)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); //GPIO14 is HSPI CLK pin (Clock)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); //GPIO15 is HSPI CS pin (Chip Select / Slave Select)
}


