/*
 * dht22.h
 *
 *  Created on: Aug 24, 2016
 *      Author: Nefastor
 */

#ifndef __DHT22_H__
#define __DHT22_H__

// from pin_mux_register.h
//#define PIN_PULLUP_DIS(PIN_NAME)         CLEAR_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLUP)
//#define PIN_PULLUP_EN(PIN_NAME)          SET_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLUP)


//#define 	DHT_PIN		GPIO_D4		// WeMos DHT22 shield uses pin D4
//#define		DHT_PIN_NUMBER

extern int dht_busy;			// value is 1 while the following function is working
extern int sample_rh;
extern int sample_t;
extern int sample_valid;		// value is 1 if the current sample is valid (good checksum)
extern int bit_duration[82];	// store the duration of each bit, in microseconds
extern unsigned char samples[5];

#define 	DHT_PIN		2			// WeMos DHT22 shield uses pin D4 = GPIO2

#define		DHT_TIMEOUT	1000		// Timeout in microseconds

void dht22_init (void);

void dht22_read (void);


#endif /* __DHT22_H__ */
