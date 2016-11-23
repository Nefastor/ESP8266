#ifndef _ADAFRUIT_ILI9341H_
#define _ADAFRUIT_ILI9341H_


#include "hspi.h"

// new : let's try to use MetalPhreak's SPI driver instead of hpsi.c
//#include "spi.h"


#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0A
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/*
#define ILI9341_PWCTR6  0xFC

*/

// for setting rotation
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

// Color definitions. Swapped by Nefastor to remove a stupid byte-swap macro
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x1F00	// 0x001F
#define	ILI9341_RED     0x00F8	// 0xF800
#define	ILI9341_GREEN   0xE007	// 0x07E0
#define ILI9341_CYAN    0xFF07	// 0x07FF
#define ILI9341_MAGENTA 0x1FF8	// 0xF81F
#define ILI9341_YELLOW  0xE0FF	// 0xFFE0
#define ILI9341_WHITE   0xFFFF

// GPIO 16, which has limited functionality, will be used for the Command / Data signal
#define TFT_DC_DATA		gpio16_output_set(1)
#define TFT_DC_COMMAND	gpio16_output_set(0)
#define TFT_DC_INIT		gpio16_output_conf()

 void transmitCmdDataBuf(uint8_t cmd, const uint8_t *data, uint8_t numDataByte);
 inline void transmitData(uint16_t data); // {hspi_wait_ready(); hspi_send_uint16(data);}
 //inline void transmitCmdData(uint8_t cmd, uint32_t data); // {hspi_wait_ready(); TFT_DC_COMMAND; hspi_send_uint8(cmd); hspi_wait_ready(); TFT_DC_DATA; hspi_send_uint32(data);}
 inline void transmitCmd(uint8_t cmd); //{hspi_wait_ready(); TFT_DC_COMMAND; hspi_send_uint8(cmd);hspi_wait_ready(); TFT_DC_DATA;}


  void     begin(void),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
             uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(bool i);

  inline void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);



  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  int drawUnicode(uint16_t uniCode, uint16_t x, uint16_t y, uint8_t size);
  int drawChar(char c, uint16_t x, uint16_t y, uint8_t size);
  int drawNumber(long long_num,uint16_t poX, uint16_t poY, uint8_t size);
  int drawString(const char *string, uint16_t poX, uint16_t poY, uint8_t size);

  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,	uint16_t color);

  void setTextColor (uint16_t newColor);


  void hspi_send_uint16_r(const uint16_t data, int32_t repeats);

#endif
