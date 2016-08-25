

#include "esp_common.h"		// Espressif register names, etc...
#include "gpio.h"			// Nefastor's GPIO library
#include "dht22.h"			// Nefastor's DHT22 library

// output from the sensor in decimal form
int sample_rh = 0;		// to do : change to uint16_t
int sample_t = 0;		// to do : change to uint16_t and interpret sign bit
int sample_valid = 0;	// value is 1 if the current sample is valid (checksum pass)

// raw binary storage for a sensor read (40 bits)
unsigned char samples[5];	// first data bit is in MSB of samples[0]

// debug : duration of each sensor bit's low and high state, including start bit
// these durations are expressed in "calls to os_delay_us(1)", which should be 1 µs
int bit_duration_hi[41];	// duration of low state for start bit and data bits
int bit_duration_lo[41];	// duration of high state for start bit and data bits

// decode sample[] into integer values (stored into globals)
void dht22_sample_decoding()
{
	sample_rh = (samples[0] << 8) + samples[1];
	sample_t = (samples[2] << 8) + samples[3];

	// Cheksum : accumulate the first four bytes on a word and keep only the LSB
	unsigned char chksum = (uint16_t)(samples[0] + samples[1] + samples[2] + samples[3]) & 0x00FF;

	sample_valid = (chksum == samples[4]) ? 1 : 0;
}

// call this after a read to turn intervals into usable data
void dht22_sample_decoding_thresh()
{
	int ix, bits;
	int threshold = bit_duration_hi[0] >> 1;	// Start bit duration divided by 2.
	// start bit is usually measured as 51-53, low bit at 16-17, high bit as 47-48

	// decoding the samples
	unsigned char b;		// bit mask
	int byte_cnt;			// samples[] index

	ix = 1;		// skip start bit
	for (byte_cnt = 0; byte_cnt < 5; byte_cnt++)
	{
		samples[byte_cnt] = 0;
		for (bits = 0, b = 0x80; bits < 8; bits++ , ix++)
		{
			if (bit_duration_hi[ix] > threshold)  // looking at high state durations only
				samples[byte_cnt] |= b;

			b = b >> 1;
		}
	}

	dht22_sample_decoding();	// turn samples[] into decimal values and check sum.
}

/* The following function is a dumb implementation, designed to verify sensor
 * operation prior to optimizing the code for multitasking support.
 *
 * Very stable, which is why I'm currently leaving it in. It is also useful for
 * learning how the sensor works (and checking that it does)
 */

void dht22_read (void)
{
	int k;		// bit index
	int t;		// bit duration counter (in microseconds)

	// given this function's long duration, feed the watchdog first
	system_soft_wdt_feed();

	// clear previous results
	for (t=0;t<41;t++)
		bit_duration_hi[t] = bit_duration_lo[t] = 0;

	// Send a read command to the sensor
	GPIO_OUTPUT_SET(DHT_PIN, 1);	// keep the pin high for 250 ms
	os_delay_us (50000);
	os_delay_us (50000);
	os_delay_us (50000);
	os_delay_us (50000);
	os_delay_us (50000);

	GPIO_OUTPUT_SET(DHT_PIN, 0);	// then hold it low for 20 ms
	os_delay_us (20000);

	GPIO_OUTPUT_SET(DHT_PIN, 1);	// and finally, high for 40 µs
	os_delay_us(40);

	GPIO_DIS_OUTPUT(DHT_PIN);		// set DHT_PIN pin as an input

	// at this point, the sensor should immediately pull down the line, and keep
	// it down for 80 us. Let's see if it does.

	// wait for pin to drop (loop should exit immediately)
	// tests show this works (duration : 0)

	t = 0;
	while ((GPIO_INPUT_GET(DHT_PIN) == 1) && (t++ < DHT_TIMEOUT))
		os_delay_us(1);

	if (t >= DHT_TIMEOUT)	return;	// The sensor failed to respond
	// in this case, the last sample received (if valid) is still available

	// getting here means we're going to receive 41 bits of data, each in the form
	// of a low state followed by a high state. Each bit's actual value is defined by
	// the relative length of each state, so we're going to measure those.

	// int bits;

	for (k = 0 ; k < 41 ; k++)
	{
		// measure low state
		while (GPIO_INPUT_GET(DHT_PIN) == 0)
		{
			os_delay_us(1);
			bit_duration_lo[k]++;
		}
		// measure high state
		while (GPIO_INPUT_GET(DHT_PIN) == 1)
		{
			os_delay_us(1);
			bit_duration_hi[k]++;
		}
	}

	dht22_sample_decoding_thresh();
}

/* "Partially event-driven" version
 *
 * "start function" returns non-zero if busy. The exact value is the number of bits
 * that still remain to be read.
 */

// globals
int bit_index = -1;

void dht22_read_ed (void)
{
	if (bit_index != -1)		// this value is used to indicate no read is in progress
		return;

	bit_index = 0;				// ammounts to indicating that a read is in progress

	// clear previous results
	samples[0] = samples[1] = samples[2] = samples[3] = samples[4] = 0;

	ETS_GPIO_INTR_DISABLE();	// so as not to self-interrupt while sending a read command

	// Send a read command :
	// 250ms of high
	GPIO_OUTPUT_SET(DHT_PIN, 1);
	vTaskDelay (25);		// 1 tick is 10 ms according to port comments
	// Hold low for 20ms
	GPIO_OUTPUT_SET(DHT_PIN, 0);
	vTaskDelay (2);
	// High for 40us
	GPIO_OUTPUT_SET(DHT_PIN, 1);
	os_delay_us(40);
	// Set DHT_PIN pin as an input
	GPIO_DIS_OUTPUT(DHT_PIN);

	// Enable GPIO interrupt
	ETS_GPIO_INTR_ENABLE();

	return;	// and we're done.
}

void dht22_ISR (uint32 mask, void* argument)
{
	// this ISR assumes there's no other GPIO interrupt source than the DHT22
	// it will trigger on I/O pin rising and measure how long it stays high, in µs.

	// this FreeRTOS port doesn't have a ISR-specific macro, not sure this works :
	//portENTER_CRITICAL();   	// Actually they seem to be causing hang-ups !

	static int theshold = 0; // doesn't seem to work
	int bit_timer = 0;

	if (GPIO_INPUT_GET(DHT_PIN) == 0)		// glitch protection
	{
		// "rearm" the interrupt (not sure this is actually necessary...)
		uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status );
		return;
	}

	// Time until pin goes low
	while (GPIO_INPUT_GET(DHT_PIN) == 1)
	{
		os_delay_us(1);
		bit_timer++;
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

	 // this port doesn't have a ISR-specific macro, not sure this works :
	 //portEXIT_CRITICAL();
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
	// experimental : display sensor start bit duration
	int wid;
	wid = drawNumber(bit_duration_lo[0],0,0,4);
	wid += drawString (" - ",wid,0,4);
	drawNumber(bit_duration_hi[0],wid,0,4);

	// experimental : display all sensor bit durations (low + high phases)
	int idx;
	int col = 0;	// 0 for left, 1 for center, 2 for right
	int line = 32;	// in pixels
	//for (idx = 2; idx < 82; idx+=2)
	for (idx = 1; idx < 41; idx++)
	{
		wid = col * 80;	// values : 0, 80, 160

		// wid += drawNumber(bit_duration[idx],wid,line,2);
		wid += drawNumber(bit_duration_lo[idx],wid,line,2);
		wid += drawString (" - ",wid,line,2);
		// drawNumber(bit_duration[idx + 1],wid,line,2);
		drawNumber(bit_duration_hi[idx],wid,line,2);

		col++;
		if (col == 3)
		{
			col = 0;
			line += 16;
		}
	}

	// displaying the samples
	line += 16;
	drawNumber(sample_rh,0,line,2);
	drawNumber(sample_t,80,line,2);
	drawNumber(sample_valid,160,line,2);

	line += 16;
	drawNumber(samples[0],0,line,2);
	drawNumber(samples[1],40,line,2);
	drawNumber(samples[2],80,line,2);
	drawNumber(samples[3],120,line,2);
	drawNumber(samples[4],160,line,2);
}
