#ifndef _NEFASTOR_ILI9341_
#define _NEFASTOR_ILI9341_

#include "hspi.h"

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C

#define ILI9341_MADCTL  0x36

// for setting rotation
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08

// Color definitions. Manually byte-swapped by Nefastor to eliminate a stupid macro
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x1F00
#define	ILI9341_RED     0x00F8
#define	ILI9341_GREEN   0xE007
#define ILI9341_CYAN    0xFF07
#define ILI9341_MAGENTA 0x1FF8
#define ILI9341_YELLOW  0xE0FF
#define ILI9341_WHITE   0xFFFF

// GPIO 16, which has limited functionality, will be used for the Command / Data signal
#define TFT_DC_INIT		gpio16_output_conf()
#define TFT_DC_DATA		gpio16_output_set(1)
#define TFT_DC_COMMAND	gpio16_output_set(0)

////////////////// API /////////////////////////////////////////////

// ILI9341-optimized HSPI functions
inline void transmitCmd (uint8_t data);
inline void transmitData (uint16_t data);
inline void transmitRange (uint32_t data);
void transmitCmdDataBuf (uint8_t cmd, const uint8_t *data, uint8_t numDataByte);

// ILI9341 init
void lcd_init ();
void lcd_set_rotation (uint8_t r);

// ILI9341 operation
void lcd_fill_screen (uint16_t color);
void lcd_fill_rect (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void lcd_draw_pixel (int16_t x, int16_t y, uint16_t color);
void lcd_fast_v_line (int16_t x, int16_t y, int16_t h, uint16_t color);
void lcd_fast_h_line (int16_t x, int16_t y, int16_t w, uint16_t color);


/////////// NEED RENAMING AND REFACTORING ///////////////////////////////
void invertDisplay(bool i);
inline void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

int drawUnicode(uint16_t uniCode, uint16_t x, uint16_t y, uint8_t size);
int drawNumber(long long_num,uint16_t poX, uint16_t poY, uint8_t size);
int drawString(const char *string, uint16_t poX, uint16_t poY, uint8_t size);

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,	uint16_t color);

void setTextColor (uint16_t newColor);

void hspi_send_uint16_r(const uint16_t data, int32_t repeats);

#endif // _NEFASTOR_ILI9341_
