#ifndef INCLUDE_HSPI_H_
#define INCLUDE_HSPI_H_

#include <c_types.h>

#include <esp8266/esp8266.h>	// This includes the SDK's "spi_register.h" header
#include "spi_register_2.h"		// Additional SPI register definitions

// Raw clock pre-scaler. According to Kolban, controls the clock going into the SPI IP block
// This is for the raw clock into the SPI controller
// set at 1 it should mean that the SPI runs at the same speed as the CPU core
#define HSPI_PRESCALER 1	// actually 1 might be a tad too fast for ILI9341
// The ILI9341 datasheet rates SCLK at 10 MHz but much faster actual frequencies are known to work

#include <gpio.h>	// this was only included for one macro, which I duplicated here :
// Update : I've created my own GPIO header so I can use it now.
//#define GPIO_OUTPUT_SET(gpio_no, bit_value) \
    gpio_output_set((bit_value)<<gpio_no, ((~(bit_value))&0x01)<<gpio_no, 1<<gpio_no,0)


#define SPI         0  // This is the SPI used by the ESP8266 to access its external Flash
#define HSPI        1  // HSPI is the Hardware SPI
#define SPIFIFOSIZE 16 // HSPI has 16 32-bit data registers. Not sure it's really a FIFO, though

uint32_t *spi_fifo;

void hspi_init(void);

inline void hspi_wait_ready(void);

inline void hspi_prepare_tx(uint32_t bytecount);

inline void hspi_start_tx();

inline void hspi_send_uint8(uint8_t data);

inline void hspi_send_uint16(uint16_t data);

inline void hspi_send_uint32(uint32_t data);

void hspi_send_data(const uint8_t * data, int8_t datasize);

void hspi_send_uint16_r(const uint16_t data, int32_t repeats);

/*
 * NEFASTOR - new configuration macros
 */

#define hspi_enable_80Mhz	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x305)
#define hspi_enable_prediv	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105)

#define HSPI_FIFO ((uint32_t*) SPI_W0(HSPI))	// for use as an array in repeat transfer
//0x60000140


inline void hspi_init_gpio (void);





#endif /* INCLUDE_HSPI_H_ */
