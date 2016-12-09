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

// Work in Progress : API redesign for higher versatility

//////////////////////// INTERFACE SETUP FUNCTIONS /////////////////////////

// Set pin muxing for HSPI. Also takes care of miscellaneous inits
inline void hspi_setup_pins ()
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); // GPIO12 is HSPI MISO pin (Master Data In)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); // GPIO13 is HSPI MOSI pin (Master Data Out)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); // GPIO14 is HSPI CLK pin (Clock)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); // GPIO15 is HSPI CS pin (Chip Select / Slave Select)

	hspi_disable_flash_mode;		// Disable Flash mode
	hspi_long_ssel;					// Lengthen SSEL (setup and hold)
	hspi_disable_all_phases;	// Disable all SPI transaction phases

	// some libraries also disable SPI_USER bit 0 (also named "SPI_DOUTDIN")
}

inline void hspi_init_gpio (void) {	hspi_setup_pins ();} // LEGACY API

// Set the SPI mode
void hspi_setup_mode(uint8 spi_cpha, uint8 spi_cpol)
{
	if(spi_cpha)
		CLEAR_PERI_REG_MASK(SPI_USER(1), SPI_CK_OUT_EDGE);
	else
		SET_PERI_REG_MASK(SPI_USER(1), SPI_CK_OUT_EDGE);

	if (spi_cpol)
		SET_PERI_REG_MASK(SPI_PIN(1), SPI_IDLE_EDGE);
	else
		CLEAR_PERI_REG_MASK(SPI_PIN(1), SPI_IDLE_EDGE);
}

void hspi_mode(uint8 spi_cpha, uint8 spi_cpol) {hspi_setup_mode(spi_cpha, spi_cpol);} // LEGACY API

// Set the byte transmission order (little or big endian)
inline void hspi_setup_big_endian ()
{
	hspi_tx_byte_order_H_to_L;
	hspi_rx_byte_order_H_to_L;
}

inline void hspi_setup_little_endian ()
{
	hspi_tx_byte_order_L_to_H;	// ILI9341 works like this
	hspi_rx_byte_order_L_to_H;	// this is the default order, by the way
}

void hspi_init(void) {	hspi_setup_pins();	hspi_setup_little_endian();} // LEGACY API

// SCK will be 80 MHz if prediv is 0, otherwise it'll be 40 MHz / prediv
void hspi_setup_clock (uint16 prediv)
{
	if (prediv==0)
	{
		hspi_enable_80Mhz;
		WRITE_PERI_REG(SPI_CLOCK(HSPI), SPI_CLK_EQU_SYSCLK);
	}
	else
	{
		hspi_enable_prediv;
		WRITE_PERI_REG(SPI_CLOCK(HSPI),
		   (((prediv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
		   ((1 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |  // SPI clock cycle
		   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |  // SCLK high for zero cycle ??? => tried 1, nothing works.
		   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S));  // SCLK low for the whole cycle...
	}

	// it appears the "high" level must always be set to a length of zero, otherwise there's no SCK output...
	// this function implements a 50% duty cycle on SCK. Frequency can be adjusted via the predivider only
}

void hspi_clock(uint16 prediv) {hspi_setup_clock (prediv);}  // LEGACY API

////////////////////// TRANSACTION SETUP //////////////////////////

// Clears transaction setup : use it when changing transaction format
// This should be called prior to calling the other transaction setup functions
inline void hspi_setup_clear ()
{
	WRITE_PERI_REG(SPI_USER1(HSPI), 0);	// clear the phase lengths register
	WRITE_PERI_REG(SPI_USER2(HSPI), 0);	// clear the command phase register
}

inline void hspi_setup_command_phase (uint8 cmd_bits, uint16 cmd_data)
{
	// Setup the command phase
	SET_PERI_REG_MASK (SPI_USER(HSPI), SPI_USR_COMMAND); // enable the command phase
	uint16 command = cmd_data << (16-cmd_bits); 		// align command data to the high bits
	command = ((command>>8)&0xff) | ((command<<8)&0xff00); //swap bytes... why again ?
	// I need to explain that next statement :
	SET_PERI_REG_MASK (SPI_USER2(HSPI), (((cmd_bits-1)&SPI_USR_COMMAND_BITLEN) <<SPI_USR_COMMAND_BITLEN_S) | command&SPI_USR_COMMAND_VALUE);
}

inline void hspi_setup_address_phase (uint32 addr_bits, uint32 addr_data)
{
	// Setup the number of bits for the address phase
	SET_PERI_REG_MASK (SPI_USER1(HSPI), ((addr_bits-1)&SPI_USR_ADDR_BITLEN)<<SPI_USR_ADDR_BITLEN_S);

	// Setup the address phase
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_ADDR); // enable the address phase
	WRITE_PERI_REG(SPI_ADDR(HSPI), addr_data<<(32-addr_bits)); // align address data to the high bits
}

// This is the write data phase, it can send up to 512 bits
// HOWEVER THIS IMPLEMENTATION IS LIMITED TO 32 BITS
// It remains useful because it applies to both little and big endian
inline void hspi_setup_write_phase (uint32 dout_bits, uint32 dout_data)
{
	// Setup the number of bits for the write phase
	SET_PERI_REG_MASK (SPI_USER1(HSPI), ((dout_bits-1)&SPI_USR_MOSI_BITLEN)<<SPI_USR_MOSI_BITLEN_S);

	// enable the transmission phase (MOSI)
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI);

	//copy data to W0 (depends on byte transmission order
	if (READ_PERI_REG(SPI_USER(HSPI))&SPI_WR_BYTE_ORDER)
		hspi_setup_write_short_BE (dout_bits, dout_data);
	else
		hspi_setup_write_short_LE (dout_bits, dout_data);
}

// More decomposition : this function can be called from other write setup functions
inline void hspi_setup_write_phase_length (uint32 dout_bits)
{
	// Setup the number of bits for the write phase
	SET_PERI_REG_MASK (SPI_USER1(HSPI), ((dout_bits-1)&SPI_USR_MOSI_BITLEN)<<SPI_USR_MOSI_BITLEN_S);

	// enable the transmission phase (MOSI)
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI);
}

// HSPI can read up to 512 bits. They will be stored in the HSPI buffer registers.
inline void hspi_setup_read_phase_length (uint32 din_bits)
{
	// Setup the number of bits for the read phase
	SET_PERI_REG_MASK (SPI_USER1(HSPI), ((din_bits-1)&SPI_USR_MISO_BITLEN)<<SPI_USR_MISO_BITLEN_S);
	// Enable the read phase
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO);
}

// Dummy bits are just zeroes sent out to provide more SCK cycles to the slave device
inline void hspi_setup_dummy_phase (uint32 dummy_bits)
{
	// Setup the number of bits for the dummy phase
	SET_PERI_REG_MASK (SPI_USER1(HSPI), ((dummy_bits-1)&SPI_USR_DUMMY_CYCLELEN)<<SPI_USR_DUMMY_CYCLELEN_S);
	// Enable the dummy phase
	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_DUMMY);
}

///////////// TRANSMISSION BUFFER SETUP HELPERS //////////////////////

// HSPI can send up to 512 bits in a single data phase. How to put that data
// into the buffer in the first place, is complicated because it depends on
// two parameters : byte ordering and how many bits are to be sent.
// The following functions are speed-optimized for different scenarios.

// Note on "little endian" (from Metal Phreak's comments)
// Data is sent out starting with the lowest BYTE, from
// MSB to LSB, followed by the second lowest BYTE, from
// MSB to LSB, followed by the second highest BYTE, from
// MSB to LSB, followed by the highest BYTE, from MSB to LSB
// 0xABCDEFGH would be sent as 0xGHEFCDAB

// Short write (32 bits or less) in big endian mode (MSB is sent first)
inline void hspi_setup_write_short_BE (uint32 dout_bits, uint32 dout_data)
{
	// Enable write phase and set its length
	hspi_setup_write_phase_length (dout_bits);

	WRITE_PERI_REG(SPI_W0(HSPI), dout_data << (32-dout_bits));
}

// Short write (32 bits or less) in little endian mode (LSB is sent first)
inline void hspi_setup_write_short_LE (uint32 dout_bits, uint32 dout_data)
{
	// Enable write phase and set its length
	hspi_setup_write_phase_length (dout_bits);

	//if your data isn't a byte multiple (8/16/24/32 bits)and you don't have SPI_WR_BYTE_ORDER set, you need this to move the non-8bit remainder to the MSBs
	//not sure if there's even a use case for this, but it's here if you need it...
	//for example, 0xDA4 12 bits without SPI_WR_BYTE_ORDER would usually be output as if it were 0x0DA4,
	//of which 0xA4, and then 0x0 would be shifted out (first 8 bits of low byte, then 4 MSB bits of high byte - ie reverse byte order).
	//The code below shifts it out as 0xA4 followed by 0xD as you might require.

	uint8 dout_extra_bits = dout_bits & 0x7; // "& 0x7" is equivalent to "% 8"

	if (dout_extra_bits)
		WRITE_PERI_REG(SPI_W0(HSPI), ((0xFFFFFFFF<<(dout_bits - dout_extra_bits)&dout_data)<<(8-dout_extra_bits) | (0xFFFFFFFF>>(32-(dout_bits - dout_extra_bits)))&dout_data));
	else
		WRITE_PERI_REG(SPI_W0(HSPI), dout_data);
}

// Long write (1 to 512 bits) in big endian mode (MSB is sent first)
// The data will be passed as a DWORD array
void hspi_setup_write_long_BE (uint32 dout_bits, uint32 *dout_data)
{
	// Enable write phase and set its length
	hspi_setup_write_phase_length (dout_bits);

	uint32_t* buff = (uint32_t*) SPI_W0(HSPI);	// pointer to the first word of the buffer

	// Copy 32 bits at a time. Loop will be skipped if there's less than 32 bits to send
	while (dout_bits >= 32)
	{
		*buff++ = *dout_data++;		// Copy 32 bits from argument to buffer
		//buff++;					// Increment both pointers
		//dout_data++;
		dout_bits -= 32;		// Decrement the bits counter
	}

	// If there's any data left, shift it before copying it
	if (dout_bits > 0)
		*buff = (*dout_data) << (32 - dout_bits);
}

// Same as above but in little endian mode. Difference is in the trailing bits.
void hspi_setup_write_long_LE (uint32 dout_bits, uint32 *dout_data)
{
	// Enable write phase and set its length
	hspi_setup_write_phase_length (dout_bits);

	uint32_t* buff = (uint32_t*) SPI_W0(HSPI);	// pointer to the first word of the buffer

	// Copy 32 bits at a time. Loop will be skipped if there's less than 32 bits to send
	while (dout_bits >= 32)
	{
		*buff++ = *dout_data++;		// Copy 32 bits from argument to buffer
		//buff++;					// Increment both pointers
		//dout_data++;
		dout_bits -= 32;		// Decrement the bits counter
	}

	// If there's any data left, shift it before copying it
	if (dout_bits > 0)
		*buff = *dout_data;
}



/////////////////////// TRANSACTION CONTROL //////////////////////

// Wait for the current transaction to complete. Call: :
// - before setting up a transaction
// - after starting a read transaction, or at least before reading its results
inline void hspi_wait_ready(void)
{
	while (hspi_busy);
}



////////////////// LEGACY API ////////////////////////////////////


// Send up to SPIFIFOSIZE x 4 = 64 bytes. Warning : sending more will overflow the HSPI
// Parameters are : pointer to a byte array, and number of bytes to send
// WARNING - BE SURE OF ENDIANNESS
// note : the "const" on the first argument is to allow the use of arrays stored in Flash
/*inline void hspi_send_data_old (const uint8_t * data, int8_t datasize)
{
	uint32_t *_data = (uint32_t*)data;	// recast data pointer from 8 bits to 32 bits (the width of the HSPI data registers)

	// Enable write phase and set its length. 8 * datasize (bytes to bits)
	hspi_setup_write_phase_length (datasize << 3);

	uint32_t* buff = (uint32_t*) SPI_W0(HSPI); // HSPI buffer start address

	while (datasize > 0)	// because this will go negative if datasize % 4 != 0
	{
		*buff++ = *_data++;
		datasize -= 4;	// because datasize is in bytes and I'm sending 4 bytes at a time
	}

	hspi_start_transaction;
}*/

// updating to new API.
// note : argument types not definitive, work in progress.
// In fact this function should be moved to the ILI 9341 library, for which
// it was originally intended.
inline void hspi_send_data (const uint8_t * data, int8_t datasize)
{
	// recast pointer to match HSPI buffer width and convert datasize from bytes to bits
	hspi_setup_write_long_LE (datasize << 3, (uint32_t*) data);
	// perform the transaction
	hspi_start_transaction;
}

/*
 * Nefastor : the following is a fairly heavy function but one that is universal
 * (it should cover all the possible types of SPI transaction).
 * My goal now is to create dedicated SPI read functions for specific SPI
 * slaves, to achieve the best performance.
 */

////////////////////////////////////////////////////////////////////////////////
// STOLEN BY NEFASTOR and adapted to / optimized for the HSPI
// Function Name: hspi_transaction
//   Description: HSPI transaction function
//    Parameters: cmd_bits - command phase : number of bits to transmit (0 disables this phase)
//				  cmd_data - command phase : the command to transmit
//				  addr_bits - address phase : number of bits to transmit (0 disables this phase)
//				  addr_data - address phase : the address to transmit
//				  dout_bits - transmit phase : number of bits to transmit (0 disables this phase)
//				  dout_data - transmit phase : the data to transmit
//				  din_bits - receive phase : number of bits to receive (0 disables this phase)
//				  dummy_bits - number of dummy bits to insert (0 disables this phase)
//
// 				  note that the address, dout and din phases can have up to 32 bits each
//
//		 Returns: data received - uint32 containing data read in (only if din_bits was not zero)
//				  0 - something went wrong (or data received was 0)
//				  1 - data sent ok (or data received was 1)
//				  Note: all data is assumed to be stored in the lower bits of
//				  the data variables (for anything <32 bits).
//
////////////////////////////////////////////////////////////////////////////////

uint32 hspi_transaction (uint8 cmd_bits, uint16 cmd_data,
						uint32 addr_bits, uint32 addr_data,
						uint32 dout_bits, uint32 dout_data,
						uint32 din_bits, uint32 dummy_bits)
{
	hspi_wait_ready (); //wait for SPI to be ready

	// disable all phases of the transaction in case they were previously set
	hspi_disable_all_phases;
	// reset and setup each phase of the transaction
	hspi_setup_clear ();
	if(cmd_bits) hspi_setup_command_phase (cmd_bits, cmd_data);
	if(addr_bits) hspi_setup_address_phase (addr_bits, addr_data);
	if(dout_bits) hspi_setup_write_phase (dout_bits, dout_data);
	if(din_bits) hspi_setup_read_phase_length (din_bits);
	if(dummy_bits) hspi_setup_dummy_phase (dummy_bits);

	// Start the SPI transaction
	hspi_start_transaction;

	// Return incoming (MISO) data
	if (din_bits)
	{
		hspi_wait_ready ();		//wait for SPI transaction to complete

		if (READ_PERI_REG(SPI_USER(HSPI))&SPI_RD_BYTE_ORDER)
			return READ_PERI_REG(SPI_W0(HSPI)) >> (32-din_bits); //Assuming data in is written to MSB. TBC
		else
			return READ_PERI_REG(SPI_W0(HSPI)); //Read in the same way as DOUT is sent. Note existing contents of SPI_W0 remain unless overwritten!
	}
	else
		return 1; //Transaction completed successfully
}

