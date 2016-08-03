#include "hspi.h"

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



uint32_t *spi_fifo;		// pointer to the first SPI data register : SPI_W0(HSPI)

void hspi_init(void)
{
	spi_fifo = (uint32_t*)SPI_W0(HSPI);

	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105); //clear bit9 => why ?

	// Set pin muxing for HSPI
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); // HSPIQ MISO GPIO12
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); // HSPID MOSI GPIO13
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); // CLK GPIO14
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); // CS GPIO15

	// Set SCLK frequency and duty cycle
	WRITE_PERI_REG(SPI_CLOCK(HSPI),
	   (((HSPI_PRESCALER - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
	   ((1 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
	   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
	   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for one cycle

	WRITE_PERI_REG(SPI_CTRL1(HSPI), 0);	// why ?

	// TO DO : explain what this does :
	uint32_t regvalue = SPI_USR_MOSI;
    regvalue &= ~(BIT2 | SPI_USR_ADDR | SPI_USR_DUMMY | SPI_USR_MISO | SPI_USR_COMMAND | SPI_DOUTDIN);
	WRITE_PERI_REG(SPI_USER(HSPI), regvalue);
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
			spi_fifo[i++] = word;
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
		spi_fifo[i++] = *_data++;
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
	*spi_fifo = data;
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

inline void hspi_send_uint16(uint16_t data)
{
	//hspi_prepare_tx(2);
	WRITE_PERI_REG(SPI_USER1(HSPI), (((uint32_t) 15) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);

	*spi_fifo = data;
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

inline void hspi_send_uint32(uint32_t data)
{
	// hspi_prepare_tx(4);
	WRITE_PERI_REG(SPI_USER1(HSPI), (((uint32_t) 31) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);

	*spi_fifo = data;
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // hspi_start_tx();
}

