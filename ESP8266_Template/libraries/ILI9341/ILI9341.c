#include "ILI9341.h"
#include "mini-printf.h"
#include "gpio.h"

#define LOAD_FONT2 // Small font, needs ~3092 bytes in FLASH
#define LOAD_FONT4 // Medium font, needs ~8126 bytes in FLASH
#define LOAD_FONT6 // Large font, needs ~4404 bytes in FLASH
#define LOAD_FONT7 // 7-segment font, needs ~3652 bytes in FLASH

#ifdef LOAD_FONT2
#include "Font16.h"
#endif

#ifdef LOAD_FONT4
#include "Font32.h"
#endif

#ifdef LOAD_FONT6
#include "Font64.h"
#endif

#ifdef LOAD_FONT7
#include "Font7s.h"
#endif

int16_t		_width = ILI9341_TFTWIDTH;		// Display width and height as modified by current rotation
int16_t		_height = ILI9341_TFTHEIGHT;	// Initialized for rotation = 0
uint16_t	textcolor = 0xFFFF;				// White on black text
uint16_t	textbgcolor = 0x0000;

////////// Custom HSPI functions optimized for operating the ILI9341 /////////
// They serve two purposes : performance enhancement and source code readability

// Transmit 8 bits. Used to send command bytes.
// Followed by calls to transmitData (pixel color) or transmitRange (to select a display area)
inline void transmitCmd (uint8_t data)
{
	TFT_DC_COMMAND;
	// hspi_wait_ready (); // unnecessary because ILI9341 transmissions always start with a command byte transfer, so there shouldn't be a previous transfer still in progress when this function is called.
	hspi_setup_clear ();
	hspi_setup_write_phase_length (8);	// set data length to 8 bit
	WRITE_PERI_REG(SPI_W0(HSPI), data);	// direct to HSPI buffer (little-endian write)
	hspi_start_transaction;
	hspi_wait_ready();		// this one is vital, however
	TFT_DC_DATA;
}

// Transmit 16 bits. Typically used to send pixel data (16-bit colors)
inline void transmitData (uint16_t data)
{
	hspi_wait_ready (); // here however it's very necessary
	hspi_setup_clear ();
	hspi_setup_write_phase_length (16);	// set data length to 16 bit
	WRITE_PERI_REG(SPI_W0(HSPI), data);	// direct to HSPI buffer (little-endian write)
	hspi_start_transaction;
}

// Transmit 32 bits. Typically used to send display "window" coordinates
inline void transmitRange (uint32_t data)
{
	hspi_wait_ready (); // here however it's very necessary
	hspi_setup_clear ();
	hspi_setup_write_phase_length (32);	// set data length to 16 bit
	WRITE_PERI_REG(SPI_W0(HSPI), data);	// direct to HSPI buffer (little-endian write)
	hspi_start_transaction;
}

// Send a command byte followed by 1 to 64 data bytes
void transmitCmdDataBuf (uint8_t cmd, const uint8_t *data, uint8_t numDataByte)
{
	transmitCmd (cmd);
	hspi_send_data(data, numDataByte);
}

////////////// ILI-9341 COMMANDS //////////////////////////////////////
// Now that we have optimized low-level SPI functions, time to use them

// First, let's initialize the ILI9341... it's only polite.
void lcd_init ()
{
	hspi_setup_pins();		// HSPI pin-muxing and (default) little-endianness
	hspi_setup_clock(0);	// HSPI bit-rate : 0 for 80 MHz, or N for 40/N MHz

	TFT_DC_INIT;	// Initialize the GPIO pin that will drive the ILI9341's command / data signal

	uint8_t data[15] = {0};	// Buffer used for init command parameter storage

	// Power Control A
	data[0] = 0x39;
	data[1] = 0x2C;
	data[2] = 0x00;
	data[3] = 0x34;
	data[4] = 0x02;
	transmitCmdDataBuf(0xCB, data, 5);

	// Power Control B
	data[0] = 0x00;
	data[1] = 0XC1;
	data[2] = 0X30;
	transmitCmdDataBuf(0xCF, data, 3);

	// Driver Timing Control A
	data[0] = 0x85;
	data[1] = 0x00;
	data[2] = 0x78;
	transmitCmdDataBuf(0xE8, data, 3);

	// Driver Timing Control B
	data[0] = 0x00;
	data[1] = 0x00;
	transmitCmdDataBuf(0xEA, data, 2);

	// Power On Sequence Control
	data[0] = 0x64;
	data[1] = 0x03;
	data[2] = 0X12;
	data[3] = 0X81;
	transmitCmdDataBuf(0xED, data, 4);

	// Pump Ratio Control
	data[0] = 0x20;
	transmitCmdDataBuf(0xF7, data, 1);

	// Power Control 1
	data[0] = 0x23;   	//VRH[5:0]
	transmitCmdDataBuf(0xC0, data, 1);

	// Power Control 2
	data[0] = 0x10;   	//SAP[2:0];BT[3:0]
	transmitCmdDataBuf(0xC1, data, 1);

	// VCOM Control 1
	data[0] = 0x3e;   	//Contrast
	data[1] = 0x28;
	transmitCmdDataBuf(0xC5, data, 2);

	// VCOM Control 2
	data[0] = 0x86;  	 //--
	transmitCmdDataBuf(0xC7, data, 1);

	// Memory Access Control : this defines the scanning...
	// data[0] = 0x48;  	//C8. 48 means MX + BGR
	data[0] = 0x00;
	// data[0] |= 0x80;	// MY seems to decide the direction of the Y axis : don't change
	data[0] |= 0x40;	// MX seems to decide the direction of the X axis : don't change
	// data[0] |= 0x20;	// MV
	// data[0] |= 0x10;	// ML seems to be the direction of auto increment for Y
	data[0] |= 0x08;	// BGR. Seems I need to keep it
	// data[0] |= 0x04;	// MH seems to be the direction of auto increment for X
	// data[0] |= 0x02;	// nothing
	// data[0] |= 0x01;	// nothing
	transmitCmdDataBuf(0x36, data, 1);

	// COLMOD : Pixel Format Set
	data[0] = 0x55;
	transmitCmdDataBuf(0x3A, data, 1);

	// Frame Rate Control (page 155)
	data[0] = 0x00;
	data[1] = 0x18;		// RTNA = 24 clocks => 79 Hz frame rate
	transmitCmdDataBuf(0xB1, data, 2);

	// Display Function Control : I shouldn't have to touch this
	data[0] = 0x08;
	data[1] = 0x82;
	data[2] = 0x27;
	transmitCmdDataBuf(0xB6, data, 3);

	// 3Gamma Function : Disable
	data[0] = 0x00;
	transmitCmdDataBuf(0xF2, data, 1);

	// Gamma curve selected
	data[0] = 0x01;
	transmitCmdDataBuf(0x26, data, 1);

	data[0] = 0x0F;
	data[1] = 0x31;
	data[2] = 0x2B;
	data[3] = 0x0C;
	data[4] = 0x0E;
	data[5] = 0x08;
	data[6] = 0x4E;
	data[7] = 0xF1;
	data[8] = 0x37;
	data[9] = 0x07;
	data[10] = 0x10;
	data[11] = 0x03;
	data[12] = 0x0E;
	data[13] = 0x09;
	data[14] = 0x00;
	transmitCmdDataBuf(0xE0, data, 15);    	//Set Gamma

	data[0] = 0x00;
	data[1] = 0x0E;
	data[2] = 0x14;
	data[3] = 0x03;
	data[4] = 0x11;
	data[5] = 0x07;
	data[6] = 0x31;
	data[7] = 0xC1;
	data[8] = 0x48;
	data[9] = 0x08;
	data[10] = 0x0F;
	data[11] = 0x0C;
	data[12] = 0x31;
	data[13] = 0x36;
	data[14] = 0x0F;
	transmitCmdDataBuf(0xE1, data, 15);    	// Set Gamma

	transmitCmd(0x11);

	transmitCmd(0x29);    // Display on
	transmitCmd(0x2c);
}

// Set the area of the display where pixel colors will be set
// Note : there's an endianness issue that's begging to be optimized-out...
inline void setAddrWindow (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	x0 = ((x0 & 0xFF) << 8) | (x0 >> 8); // byte swap
	x1 = ((x1 & 0xFF) << 8) | (x1 >> 8); // byte swap
	y0 = ((y0 & 0xFF) << 8) | (y0 >> 8); // byte swap
	y1 = ((y1 & 0xFF) << 8) | (y1 >> 8); // byte swap

	transmitCmd (ILI9341_CASET);
	transmitRange ((x1 << 16) | x0 ); // word swap
	transmitCmd (ILI9341_PASET);
	transmitRange ((y1 << 16) | y0 ); // word swap
	transmitCmd (ILI9341_RAMWR); // write to RAM : 16-bit pixel colors can be written right after this function returns
}


// The following function is very generic and slow. Try to avoid using it.
void lcd_draw_pixel (int16_t x, int16_t y, uint16_t color) {

	// Nefastor : the following safety test reduces performance.
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
	// if this safety is relied upon, find something else, such as:
	// - moving to unsigned arguments
	// - bounding the arguments with bitwise logical functions
	// I'm guessing this test is necessary because of geometric functions that don't perform it themselves.
	// (once again, it's typical Adafruit crap coding)
	// note : if the tests are necessary they could at least be cut into 4 tests
	// to allow for early returns on first failure.
	setAddrWindow(x,y,x+1,y+1);	// not even sure I need the +1's
	transmitData(color);
}

void lcd_fast_v_line (int16_t x, int16_t y, int16_t h, uint16_t color) {

	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;

	if((y+h-1) >= _height)
		h = _height-y;

	setAddrWindow(x, y, x, y+h-1);
	hspi_send_uint16_r(color, h);
}

void lcd_fast_h_line (int16_t x, int16_t y, int16_t w, uint16_t color) {

	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((x+w-1) >= _width)  w = _width-x;
	setAddrWindow(x, y, x+w-1, y);
	hspi_send_uint16_r(color, w);
}

//ICACHE_FLASH_ATTR
void lcd_fill_screen (uint16_t color)
{
	lcd_fill_rect (0, 0,  _width, _height, color);
}

// fill a rectangle
void lcd_fill_rect (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	setAddrWindow(x, y, x+w-1, y+h-1);
	hspi_send_uint16_r(color, h*w);	// multiply is unavoidable here : a call loop would cost more
}

// Convert 8-bit (each) R,G,B into 16-bit packed color
inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Argument must be 0 to 3
void lcd_set_rotation (uint8_t m)
{
	uint8_t madctl;

	switch (m)
	{
		case 0:
			madctl = MADCTL_MX | MADCTL_BGR;
			_width  = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 1:
			madctl = MADCTL_MV | MADCTL_BGR;
			_width  = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
		case 2:
			madctl = MADCTL_MY | MADCTL_BGR;
			_width  = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 3:
			madctl = MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR;
			_width  = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
		default:
			break;
	}

	transmitCmdDataBuf(ILI9341_MADCTL, &madctl, 1);
}


inline void invertDisplay(bool i)
{
	transmitCmd(i ? ILI9341_INVON : ILI9341_INVOFF);
}

//ICACHE_FLASH_ATTR : I think this is no longer required
int drawUnicode(uint16_t uniCode, uint16_t x, uint16_t y, uint8_t size)
{
	// Nefastor : the following tests reduce performance. Single-font functions might be faster
	// Or maybe use const arrays indexed by the "size" argument to initialize the variables

	uniCode -= 32;		// The first 32 character codes are non-printable

	uint16_t width;		// Width of the character in pixels, not counting any horizontal spacing
	uint16_t height;	// Height of the character in pixels : specific to the font
	const uint8_t *flash_address;	// Address of the character's bitmap in the font array
	int8_t spacing = 0;  // 1 if the font requires horizontal spacing of characters

	switch (size)
	{
#ifdef LOAD_FONT2
		case 2:
			flash_address = chrtbl_f16[uniCode];
			width = *(widtbl_f16+uniCode);	// Each character's width is stored in a table
			height = chr_hgt_f16;
			spacing = 1;	// this font requires spacing
			break;
#endif
#ifdef LOAD_FONT4
		case 4:
			flash_address = chrtbl_f32[uniCode];
			width = *(widtbl_f32+uniCode);
			height = chr_hgt_f32;
			break;
#endif
#ifdef LOAD_FONT6
		case 6:
			flash_address = chrtbl_f64[uniCode];
			width = *(widtbl_f64+uniCode);
			height = chr_hgt_f64;
			break;
#endif
#ifdef LOAD_FONT7
		case 7:
			flash_address = chrtbl_f7s[uniCode];
			width = *(widtbl_f7s+uniCode);
			height = chr_hgt_f7s;
			break;
#endif
		default:	// just in case no font is loaded for some reason
			break;
	}

	uint16_t w = (width + 7) >> 3;
	// consider this : 7 is "111" binary, >>3 will disappear those...
	// ... and all that's going to be left is a carry.

	uint16_t color   = 0;
	uint8_t line = 0;

	// First, set the window directly to the full size of the character, to benefit from self-incrementation
	setAddrWindow(x,y,x+width-1,y+height); // -1 necessary to convert width into limit (n pixels range is from 0 to n-1)

	int ii = 0;		// ii counts in bytes of pixels on a given line
	int i;			// i counts in lines of pixels
	for(i=0; i<height; i++, ii+=w)	// ii avoids a multiplication such as i*w
	{
		int wid = width;	// count-down to stop sending pixels
		int k;
		for (k = 0;k < w; k++)
		{
			// This can probably be further optimized.
			line = *(flash_address + ii + k);
			// optimization 1 : leave the loop as soon as "wid" == 0
			if (wid-- == 0) break;
				if(line & 0x80) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x40) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x20) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x10) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x8) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x4) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x2) transmitData(textcolor); else transmitData(textbgcolor);
			if (wid-- == 0) break;
				if(line & 0x1) transmitData(textcolor); else transmitData(textbgcolor);

			/*  // That's a code size optimization but I don't think it's faster : I'm adding a shift
			while (wid--)
			{
				if(line & 0x80) transmitData(textcolor); else transmitData(textbgcolor);
				line = line << 1;
			}
			*/

			// optimization idea : loop and shift "line"
			// optimization idea : create a window for the whole line, fill the FIFO
			// ... and send it in one go ! Could happen at the outer loop !
			// ... and it might also use the shifting idea
			// I could also recode the fonts.
			/*
			 * Note : HSPI buffer is 64 bytes, enough for 32 pixels, but some characters
			 * are wider than that.
			 */
		}

	}

	// at this point I should add some horizontal spacing.
	if (spacing)
	{
		setAddrWindow(x+width, y, x+width ,y+height);	// blank space : a column of pixels
		hspi_send_uint16_r(textbgcolor, height);	   // clear it
	}

	return width + spacing;  // returns the width displayed, in pixels (useful for displaying strings)
}

/***************************************************************************************
 ** Function name:           drawNumber unsigned with size
 ** Descriptions:            drawNumber
 ***************************************************************************************/
ICACHE_FLASH_ATTR int drawNumber(long long_num,uint16_t poX, uint16_t poY, uint8_t size)
{
	char tmp[10];
	if (long_num < 0) snprintf(tmp, sizeof(tmp), "%d", long_num);
	else snprintf(tmp, sizeof(tmp), "%u", long_num);
	return drawString(tmp, poX, poY, size);
}


/***************************************************************************************
 ** Function name:           drawString
 ** Descriptions:            draw string
 ***************************************************************************************/
ICACHE_FLASH_ATTR int drawString(const char *string, uint16_t poX, uint16_t poY, uint8_t size)
{
	uint16_t sumX = 0;

	while(*string)	// interesting : will loop until the NULL character occurs
	{
		uint16_t xPlus = drawUnicode(*string, poX, poY, size);

		sumX += xPlus;
		string++;

		// if(poX < 264)
		poX += xPlus;   // Move cursor right
	}
	return sumX;
}

// NEW : set text color
void setTextColor (uint16_t newColor)
{
	textcolor = newColor;
}


// Draw a rectangle
//
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	lcd_fast_h_line(x, y, w, color);
	lcd_fast_h_line(x, y+h-1, w, color);
	lcd_fast_v_line(x, y, h, color);
	lcd_fast_v_line(x+w-1, y, h, color);

	// possible optimization : this code overwrites the corner pixels.
	// solution : shorten the lines.
}

// SPI acceleration function (makes use of the ESP8266's 512-bit SPI buffer)
// Send the same 16-bit value multiple times.
// Useful for filling pixels with the same color on 16bpp displays.
void hspi_send_uint16_r(uint16_t data, int32_t repeats)
{
	uint32_t i;
	uint32_t bts;	// bytes to send
	uint32_t word = data << 16 | data;	// SPI data registers are 32-bit : send two words at once !

	while (repeats > 0)		// outer loop for transfers up to 512 bits
	{
		hspi_wait_ready();	// wait for previous transfer to complete

		// determine how many bytes will be sent during this iteration
		if (repeats >= 32)	// 512 bits = 32 x 16 bit words. Sending 32 or more means a full transfer right now
			bts = SPIFIFOSIZE << 2;		// cheap multiply by four
		else
			bts = repeats << 1;			// cheap multiply by two

		i = 0;	// FIFO index
		while ((repeats > 0) && (i < SPIFIFOSIZE))    // now fill the FIFO and update "repeats"
		{
			repeats -= 2;	// because I'm sending two 16-bit words at once
			HSPI_FIFO[i++] = word;
		}

		// perform the transfer : (hspi_prepare_tx(bts)) and send :
		WRITE_PERI_REG(SPI_USER1(HSPI), (((bts << 3) - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);
		SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // send
	}
}

/* Nefastor : TO DO : Bresenham line algo
 * Optimization idea : reduce the number of coordinate settings on line segments
 * longer than one pixels, to save on SPI bandwidth
 * Take inspiration from the lcd_fast_h_line and lcd_fast_v_line functions.
 * */


// Nefastor - Port this, it looks interesting
/*
void Adafruit_GFX_AS::drawBitmap(int16_t x, int16_t y,
		const uint16_t *bitmap, int16_t w, int16_t h) {


	for(uint16_t j=0; j<h; j++) {
		for(uint16_t i=0; i<w; i++ ) {
			lcd_draw_pixel(x+i, y+j, bitmap[j * w + i]);
		}
	}
}
*/


