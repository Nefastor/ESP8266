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

#define HSPI_DIVIDER 4


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

// enable / disable phases of an SPI transaction
#define hspi_enable_command_phase	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_COMMAND)
#define hspi_disable_command_phase 	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_COMMAND)

#define hspi_enable_data_phase		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI)
#define hspi_disable_data_phase 	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI)

#define hspi_enable_read_phase		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO)
#define hspi_disable_read_phase 	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MISO)

#define hspi_enable_addr_phase		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_ADDR)
#define hspi_disable_addr_phase 	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_ADDR)

#define hspi_enable_dummy_phase		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_DUMMY)
#define hspi_disable_dummy_phase 	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_DUMMY)

#define hspi_tx_byte_order_H_to_L	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_WR_BYTE_ORDER)
#define hspi_tx_byte_order_L_to_H	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_WR_BYTE_ORDER)

#define hspi_rx_byte_order_H_to_L	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_RD_BYTE_ORDER)
#define hspi_rx_byte_order_L_to_H	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_RD_BYTE_ORDER)

// NEFASTOR : Experimental : the following macros operate on undocumented fields
#define hspi_enable_ck_out_edge		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_CK_OUT_EDGE)
#define hspi_disable_ck_out_edge 	CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_CK_OUT_EDGE)
// (they appear to have no effect)

// also do SPI_CS_SETUP, SPI_CS_HOLD, SPI_FLASH_MODE

void hspi_mode(uint8 spi_cpha,uint8 spi_cpol);

inline void hspi_init_gpio (void);

void hspi_clock(uint16 prediv);

// stolen from spi.c and mutated into an HSPI-only function.
// I need this function in order to implement SPI reads and, of course, all transaction formats
uint32 hspi_transaction(uint8 cmd_bits, uint16 cmd_data, uint32 addr_bits, uint32 addr_data, uint32 dout_bits, uint32 dout_data,
				uint32 din_bits, uint32 dummy_bits);

// stolen from spi.h
// note : the original hspi.c transmission macros are likely faster,
// so what's really useful here are the macros for reading data
#define hspi_txd(bits, data) hspi_transaction(0, 0, 0, 0, bits, (uint32) data, 0, 0)
#define hspi_tx8(data)       hspi_transaction(0, 0, 0, 0, 8,    (uint32) data, 0, 0)
#define hspi_tx16(data)      hspi_transaction(0, 0, 0, 0, 16,   (uint32) data, 0, 0)
#define hspi_tx32(data)      hspi_transaction(0, 0, 0, 0, 32,   (uint32) data, 0, 0)

#define hspi_rxd(bits) hspi_transaction(0, 0, 0, 0, 0, 0, bits, 0)
#define hspi_rx8()       hspi_transaction(0, 0, 0, 0, 0, 0, 8,    0)
#define hspi_rx16()      hspi_transaction(0, 0, 0, 0, 0, 0, 16,   0)
#define hspi_rx32()      hspi_transaction(0, 0, 0, 0, 0, 0, 32,   0)


#endif /* INCLUDE_HSPI_H_ */
