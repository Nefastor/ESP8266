/*
 * dht22.h
 *
 *  Created on: Aug 24, 2016
 *      Author: Nefastor
 */

#ifndef __DHT22_H__
#define __DHT22_H__

// Which pin is the sensor connected to ? Use only GPIO 0..15
// Note : the WeMos D1 Mini DHT22 shield uses pin D4 = GPIO2
#define 	DHT_PIN		2

// The sensor may not respond to the ESP8266, let's define a comfortable time-out
#define		DHT_TIMEOUT	1000		// Timeout in microseconds

// Global variables accessible to user code
extern int sample_rh;
extern int sample_t;
extern int sample_valid;
extern int bit_duration[82];
extern int bit_duration_up[41];
extern int bit_duration_down[41];
extern unsigned char samples[5];

// API

void dht22_init (void);		// Pin muxing and interrupt setup

void dht22_read (void);		// Blocking read function, VERY long (up to 275 ms)

void dht22_read_ed (void);	// Non-blocking read function, uses GPIO interrupt

#endif /* __DHT22_H__ */
