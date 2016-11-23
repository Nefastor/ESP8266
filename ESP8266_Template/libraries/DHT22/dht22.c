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
 * Various notes:
 * - DHT22 data line may be glitchy and/or the sensor may time-out, so a time-out
 *   counter and test are necessary to ensure stability. In case of time-out, the
 *   current read is aborted.
 *
 * */

#include "esp_common.h"		// Espressif register names, etc...
#include "gpio.h"			// Nefastor's GPIO library
#include "dht22.h"			// Nefastor's DHT22 library
#include "ILI9341.h"		// Nefastor's ILI9341 library

// output from the sensor in decimal form
int sample_rh = 0;		// to do : change to uint16_t
int sample_t = 0;		// to do : change to uint16_t and interpret sign bit
int sample_valid = 0;	// value is 1 if the current sample is valid (checksum pass)

// raw binary storage for a sensor read (40 bits)
unsigned char samples[5];	// first data bit is in MSB of samples[0]

#ifdef __DHT22_DEBUG__
// debug : duration of each sensor bit's low and high state, including start bit
// these durations are expressed in "calls to os_delay_us(1)", which should be 1 탎
int bit_duration_hi[41];	// duration of low state for start bit and data bits
int bit_duration_lo[41];	// duration of high state for start bit and data bits
#endif // __DHT22_DEBUG__

// Decode sample[] into integer values (stored into globals)
// I made it a separate function because that part is common to all sensor reading functions
void dht22_sample_decoding()
{
	sample_rh = (samples[0] << 8) + samples[1];		// relative humidity

	sample_t = ((samples[2] & 0x7F) << 8) + samples[3];

	if (samples[2] & 0x80)
		sample_t = -sample_t;	// temperature is negative

	// Cheksum : accumulate the first four bytes on a word and keep only the LSB
	unsigned char chksum = (uint16_t)(samples[0] + samples[1] + samples[2] + samples[3]) & 0x00FF;

	sample_valid = (chksum == samples[4]) ? 1 : 0;
}

/* The following function is a dumb implementation, designed to verify sensor
 * operation prior to optimizing the code for multitasking support.
 *
 * Very stable, which is why I'm currently leaving it in. It is also useful for
 * learning how the sensor works (and checking that it does).
 */

void dht22_read (void)
{
	int k;		// bit index
	int t;		// bit duration counter (in microseconds)

	// clear previous results
	for (t=0;t<41;t++)
		bit_duration_hi[t] = bit_duration_lo[t] = 0;

	// Enter a critical section, preventing FreeRTOS from interrupting the read
	taskENTER_CRITICAL();

	// Send a read command to the sensor
	GPIO_OUTPUT_SET(DHT_PIN, 1);	// Start from a high state...
	os_delay_us (100);			// ... stay there for a short while (0.1 ms)...

	GPIO_OUTPUT_SET(DHT_PIN, 0);	// ... then hold the pin low for 1 ms...
	os_delay_us (1000);

	GPIO_OUTPUT_SET(DHT_PIN, 1);	// ... and finally, set it high for 40 탎
	os_delay_us(40);

	GPIO_DIS_OUTPUT(DHT_PIN);		// Set DHT_PIN pin as an input to wait for the sensor's data

	// Wait for the sensor to lower the pin (it should be immediate but the specs allow for an 80 탎 delay)
	t = 0;
	while ((GPIO_INPUT_GET(DHT_PIN) == 1) && (t++ < DHT_TIMEOUT))
		os_delay_us(1);

	if (t >= DHT_TIMEOUT)	return;	// The sensor failed to respond : abort

	// Receive 41 bits. Each is in low-then-high form with varying duty cycle, which
	// means the most robust reading method is measuring the waveform's timing.

	for (k = 0 ; k < 41 ; k++)		// 1 start bit + 40 data bits
	{
		// measure how long the pin stays low
		while (GPIO_INPUT_GET(DHT_PIN) == 0)
		{
			os_delay_us(1);			// The exact delay doesn't really matter since we're measuring relative durations
			bit_duration_lo[k]++;
			// timeout test
			if (bit_duration_lo[k] > DHT_TIMEOUT) break;	// something went wrong
		}
		if (bit_duration_lo[k] > DHT_TIMEOUT) break; // also break from outer loop

		// measure how long the pin stays high
		while (GPIO_INPUT_GET(DHT_PIN) == 1)
		{
			os_delay_us(1);
			bit_duration_hi[k]++;
			// timeout test
			if (bit_duration_lo[k] > DHT_TIMEOUT) break;	// something went wrong
		}
		if (bit_duration_lo[k] > DHT_TIMEOUT) break; // also break from outer loop
	}

	// Timing measurements are complete, we can leave the critical section.
	taskEXIT_CRITICAL();

	if (t > DHT_TIMEOUT) return;	// no valid data to decode

	// Decode measured bit durations into binary data
	t = bit_duration_hi[0] >> 1; 	// threshold = start bit "up" time divided by two
	unsigned char m = 0x80;			// bit mask
	unsigned char* p = samples;		// pointer to the bytes array

	for (k = 1 ; k < 41 ; k++)
	{
		if  (bit_duration_hi[k] > t)
			*p |= m;	// set the bit
		else
			*p &= ~m;	// clear the bit

		m = m >> 1;		// shift mask to next bit

		if (m == 0)
		{
			m = 0x80;	// reset the mask to MSBit
			p++; 		// move on to next byte
		}
	}

	// Decode the sensor reading's five bytes into temperature and
	dht22_sample_decoding();
}

/* "Partially event-driven" version
 *
 * "start function" returns non-zero if busy. The exact value is the number of bits
 * that still remain to be read.
 */

// globals
int bit_index = -1;		// to do : find a way to eliminate this (static local variable)

void dht22_read_ed (void)
{
	if (bit_index != -1)		// this value is used to indicate no read is in progress
		return;

	bit_index = 0;				// ammounts to indicating that a read is in progress

	// clear previous results
	samples[0] = samples[1] = samples[2] = samples[3] = samples[4] = 0;

	ETS_GPIO_INTR_DISABLE();	// so as not to self-interrupt while sending a read command

	// Send a read command :
	GPIO_OUTPUT_SET(DHT_PIN, 1);	// set pin for 0.1 ms
	// vTaskDelay (1);
	os_delay_us (100);

	GPIO_OUTPUT_SET(DHT_PIN, 0);	// clear pin and leave it there for 1 to 10 ms
	vTaskDelay (1);					// while the pin stays low, let other tasks run

	GPIO_OUTPUT_SET(DHT_PIN, 1);	// set the pin again, for 40 탎
	os_delay_us(40);

	GPIO_DIS_OUTPUT(DHT_PIN);		// Set DHT_PIN pin as an input

	ETS_GPIO_INTR_ENABLE();			// Enable GPIO interrupt

	return;	// and we're done.
}

// ISR (to do : check if it can be interrupted, as this FreeRTOS port doesn't have ISR critical section macros => they might not be required)
void dht22_ISR (uint32 mask, void* argument)
{
	// this ISR assumes there's no other GPIO interrupt source than the DHT22
	// it will trigger on I/O pin rising and measure how long it stays high, in 탎.

	static int theshold = 0;
	int bit_timer = 0;

	if (GPIO_INPUT_GET(DHT_PIN) == 0)		// glitch protection
	{
		// "rearm" the interrupt (not sure this is actually necessary...)
		uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
		return;
	}

	// Measure time until pin goes low
	while (GPIO_INPUT_GET(DHT_PIN) == 1)
	{
		os_delay_us(1);
		bit_timer++;
		if (bit_timer > DHT_TIMEOUT)
		{
			// end the reading process
			bit_index = -1;		// indicates no read is in progress
			ETS_GPIO_INTR_DISABLE();	// also disable GPIO interrupts
			return;
		}
	}

	if (bit_index == 0)		// then we just measured the start bit
	{
		theshold = bit_timer >> 1;	// anything longer than that is a "one"

		// for debugging
		bit_duration_hi[0] = bit_timer;
	}
	else
	{
		// got a data bit : decode and store
		// sample byte index for this bit :
		int samplebyte = (bit_index - 1) >> 3; // each byte holds 8 bits
		// "- 1" to get rid of start bit and get in the 0..39 range
		// which bit in the byte ? we receive MSB to LSB
		int bitinbyte = 7 - ((bit_index - 1) & 0x07);
		// mask for that bit
		unsigned char bitmsk = 1 << bitinbyte;
		// is the bit a one ? (longer than threshold ?)
		if (bit_timer > theshold)
			samples[samplebyte] |= bitmsk;

		// Also store the timing data for debugging
		bit_duration_hi[bit_index] = bit_timer;

		// Also store the bits as integers in the timing array, for tests
		if (bit_timer > theshold)
			bit_duration_lo[bit_index] = 1;
		else
			bit_duration_lo[bit_index] = 0;

	}

	// Detect read completion
	if (bit_index == 40)
	{
		bit_index = -1;		// indicates no read is in progress

		ETS_GPIO_INTR_DISABLE();	// also disable GPIO interrupts

		dht22_sample_decoding();	// call function to decode the results
	}
	else
	{
		bit_index++;	// update index

		// "rearm" the interrupt (not sure this is actually necessary...)
		uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
	}
}

// returns 1 until a complete sample has been read
int dht22_read_ed_busy (void)
{
	return (bit_index == -1) ? 0 : 1;
}

void dht22_init (void)
{
	// setup the I/O pin : enable pull-up and rising-edge interrupt
	PIN_PULLUP_EN (GPIO_PIN_REG (DHT_PIN));
	gpio_pin_intr_state_set (DHT_PIN, GPIO_PIN_INTR_POSEDGE);

	// Set the GPIO ISR (note : interrupt enable is handled in dht22_read_ed function)
	gpio_intr_handler_register(dht22_ISR, NULL);
}


// ********************* DEBUG FUNCTIONS ********************************

// display sample data on an ILI9341 LCD module
// uses the ILI9341 library

void dht22_sample_display ()
{
	// vanity
	setTextColor (ILI9341_GREEN);
	drawString(" Nefastor DHT22",0,0,4);

	// experimental : display sensor start bit duration
	int wid;
	//wid = drawNumber(bit_duration_lo[0],0,0,4);
	//wid += drawString (" - ",wid,0,4);
	//drawNumber(bit_duration_hi[0],wid,0,4);

	// display all sensor bit durations (low + high phases)
	int idx;
	int col = 0;	// 0 for left, 1 for center, 2 for right
	int line = 28;	// in pixels
	for (idx = 0; idx < 41; idx++)
	{
		if (idx == 0)
			setTextColor (ILI9341_RED);
		else
			setTextColor (ILI9341_WHITE);

		wid = col * 80;	// values : 0, 80, 160

		wid += drawNumber(bit_duration_lo[idx],wid,line,2);
		wid += drawString (" - ",wid,line,2);
		drawNumber(bit_duration_hi[idx],wid,line,2);

		col++;
		if (col == 3)
		{
			col = 0;
			line += 16;
		}
	}

	// display the samples as five bytes
	line += 16;
	setTextColor (ILI9341_YELLOW);
	drawNumber(samples[0],0,line,2);
	drawNumber(samples[1],40,line,2);
	drawNumber(samples[2],80,line,2);
	drawNumber(samples[3],120,line,2);
	drawNumber(samples[4],160,line,2);

	// display the samples as decoded words and validity flags
	line += 16;
	setTextColor (ILI9341_CYAN);
	drawNumber(sample_rh,0,line,2);
	drawNumber(sample_t,80,line,2);
	drawNumber(sample_valid,160,line,2);

	// display the fully-decoded sample as floating point data
	char payload[20];
	sprintf (payload, "%i.%i%% / %i.%iC",sample_rh / 10, sample_rh % 10, sample_t / 10, sample_t % 10);
	setTextColor (ILI9341_MAGENTA);
	drawString(payload,0,290,4);
}
