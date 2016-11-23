/*
The MIT License (MIT)
Copyright (c) 2016 Nefastor Online

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * dht22.h
 *
 *  Created on: Aug 24, 2016
 *      Author: Nefastor
 */

#ifndef __DHT22_H__
#define __DHT22_H__

// enables code that's only useful for learning and debugging
#define __DHT22_DEBUG__

// Which pin is the sensor connected to ? Use only GPIO 0..15
// Note : the WeMos D1 Mini DHT22 shield uses pin D4 = GPIO2
#define 	DHT_PIN		2

// The sensor may not respond to the ESP8266, let's define a comfortable time-out
#define		DHT_TIMEOUT	1000		// Timeout in microseconds

// Global variables accessible to user code
extern int sample_rh;
extern int sample_t;
extern int sample_valid;

#ifdef __DHT22_DEBUG__
extern int bit_duration_hi[41];
extern int bit_duration_lo[41];
extern unsigned char samples[5];
#endif // __DHT22_DEBUG__

// ********************* API *******************************************

void dht22_init (void);		// Pin muxing and interrupt setup

// Blocking API
void dht22_read (void);		// Blocking read function, VERY long (up to 275 ms)

// Non-blocking API ("ed" stands for "event-driven", a.k.a. "interrupt based"
void dht22_read_ed (void);	// Non-blocking read function, uses GPIO interrupt

int dht22_read_ed_busy (void);	// Returns 1 if a read is in progress

// ********************* DEBUG FUNCTIONS ********************************

#ifdef __DHT22_DEBUG__
void dht22_sample_display ();	// display sample data on an ILI9341 LCD module
#endif // __DHT22_DEBUG__

#endif /* __DHT22_H__ */
