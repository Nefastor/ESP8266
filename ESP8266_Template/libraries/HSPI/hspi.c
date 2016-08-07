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

void hspi_init(void)
{
	hspi_enable_80Mhz;	// Use 80 MHz system clock as SCK

	hspi_init_gpio();

	//hspi_clock(HSPI_PRESCALER, HSPI_DIVIDER);
	//hspi_clock(1, 4);	// only settings that work appear to use prescaler 0 or 1
	hspi_clock(0, 0);	// force 80 MHz SCK

	hspi_enable_data_phase;
	hspi_disable_addr_phase;
	hspi_disable_dummy_phase;
	hspi_disable_read_phase;
	hspi_disable_command_phase;
	// some libraries disable SPI_USER bit 2 and bit 0 (also named "SPI_DOUTDIN")

	hspi_tx_byte_order_L_to_H;	// ILI9341 works like this
	hspi_rx_byte_order_L_to_H;	// this is the default order, by the way
}

/*
 * Clock predivider takes the 80 MHz system clock and divides it, feeding
 * the SCK clock generator. "cntdiv" defines the period of SCK in predivided
 * clock cycles.
 */
void hspi_clock(uint16 prediv, uint8 cntdiv)
{
	if((prediv==0)|(cntdiv==0))
		WRITE_PERI_REG(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);
	else
		WRITE_PERI_REG(SPI_CLOCK(HSPI),
		   (((prediv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
		   (((cntdiv - 1) & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
		   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
		   (((cntdiv - 1) & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...

	// it appears the "high" level must always be set to a length of zero, otherwise there's no SCK output...
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

#define hspi_busy READ_PERI_REG(SPI_CMD(HSPI))&SPI_USR


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


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: hspi_transaction
//   Description: HSPI transaction function
//    Parameters:
//				  cmd_bits - actual number of bits to transmit
//				  cmd_data - command data
//				  addr_bits - actual number of bits to transmit
//				  addr_data - address data
//				  dout_bits - actual number of bits to transmit
//				  dout_data - output data
//				  din_bits - actual number of bits to receive
//
//		 Returns: read data - uint32 containing read in data only if RX was set
//				  0 - something went wrong (or actual read data was 0)
//				  1 - data sent ok (or actual read data is 1)
//				  Note: all data is assumed to be stored in the lower bits of
//				  the data variables (for anything <32 bits).
//
////////////////////////////////////////////////////////////////////////////////

uint32 hspi_transaction(uint8 cmd_bits, uint16 cmd_data, uint32 addr_bits, uint32 addr_data, uint32 dout_bits, uint32 dout_data,
				uint32 din_bits, uint32 dummy_bits){


	hspi_wait_ready (); //wait for SPI to be ready

//########## Enable SPI Functions ##########//
	//disable MOSI, MISO, ADDR, COMMAND, DUMMY in case previously set.
	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI|SPI_USR_MISO|SPI_USR_COMMAND|SPI_USR_ADDR|SPI_USR_DUMMY);

	//enable functions based on number of bits. 0 bits = disabled.
	//This is rather inefficient but allows for a very generic function.
	//CMD ADDR and MOSI are set below to save on an extra if statement.
//	if(cmd_bits) {SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_COMMAND);}
//	if(addr_bits) {SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_ADDR);}
	if(din_bits) {SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO);}
	if(dummy_bits) {SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_DUMMY);}
//########## END SECTION ##########//

//########## Setup Bitlengths ##########//
	WRITE_PERI_REG(SPI_USER1(HSPI), ((addr_bits-1)&SPI_USR_ADDR_BITLEN)<<SPI_USR_ADDR_BITLEN_S | //Number of bits in Address
									  ((dout_bits-1)&SPI_USR_MOSI_BITLEN)<<SPI_USR_MOSI_BITLEN_S | //Number of bits to Send
									  ((din_bits-1)&SPI_USR_MISO_BITLEN)<<SPI_USR_MISO_BITLEN_S |  //Number of bits to receive
									  ((dummy_bits-1)&SPI_USR_DUMMY_CYCLELEN)<<SPI_USR_DUMMY_CYCLELEN_S); //Number of Dummy bits to insert
//########## END SECTION ##########//

//########## Setup Command Data ##########//
	if(cmd_bits) {
		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_COMMAND); //enable COMMAND function in SPI module
		uint16 command = cmd_data << (16-cmd_bits); //align command data to high bits
		command = ((command>>8)&0xff) | ((command<<8)&0xff00); //swap byte order
		WRITE_PERI_REG(SPI_USER2(HSPI), ((((cmd_bits-1)&SPI_USR_COMMAND_BITLEN)<<SPI_USR_COMMAND_BITLEN_S) | command&SPI_USR_COMMAND_VALUE));
	}
//########## END SECTION ##########//

//########## Setup Address Data ##########//
	if(addr_bits){
		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_ADDR); //enable ADDRess function in SPI module
		WRITE_PERI_REG(SPI_ADDR(HSPI), addr_data<<(32-addr_bits)); //align address data to high bits
	}


//########## END SECTION ##########//

//########## Setup DOUT data ##########//
	if(dout_bits) {
		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI); //enable MOSI function in SPI module
	//copy data to W0
	if(READ_PERI_REG(SPI_USER(HSPI))&SPI_WR_BYTE_ORDER) {
		WRITE_PERI_REG(SPI_W0(HSPI), dout_data<<(32-dout_bits));
	} else {

		uint8 dout_extra_bits = dout_bits%8;

		if(dout_extra_bits){
			//if your data isn't a byte multiple (8/16/24/32 bits)and you don't have SPI_WR_BYTE_ORDER set, you need this to move the non-8bit remainder to the MSBs
			//not sure if there's even a use case for this, but it's here if you need it...
			//for example, 0xDA4 12 bits without SPI_WR_BYTE_ORDER would usually be output as if it were 0x0DA4,
			//of which 0xA4, and then 0x0 would be shifted out (first 8 bits of low byte, then 4 MSB bits of high byte - ie reverse byte order).
			//The code below shifts it out as 0xA4 followed by 0xD as you might require.
			WRITE_PERI_REG(SPI_W0(HSPI), ((0xFFFFFFFF<<(dout_bits - dout_extra_bits)&dout_data)<<(8-dout_extra_bits) | (0xFFFFFFFF>>(32-(dout_bits - dout_extra_bits)))&dout_data));
		} else {
			WRITE_PERI_REG(SPI_W0(HSPI), dout_data);
		}
	}
	}
//########## END SECTION ##########//

//########## Begin SPI Transaction ##########//
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);
//########## END SECTION ##########//

//########## Return DIN data ##########//
	if(din_bits) {
		while(spi_busy(HSPI));	//wait for SPI transaction to complete

		if(READ_PERI_REG(SPI_USER(HSPI))&SPI_RD_BYTE_ORDER) {
			return READ_PERI_REG(SPI_W0(HSPI)) >> (32-din_bits); //Assuming data in is written to MSB. TBC
		} else {
			return READ_PERI_REG(SPI_W0(HSPI)); //Read in the same way as DOUT is sent. Note existing contents of SPI_W0 remain unless overwritten!
		}

		return 0; //something went wrong
	}
//########## END SECTION ##########//

	//Transaction completed
	return 1; //success
}

////////////////////////////////////////////////////////////////////////////////

