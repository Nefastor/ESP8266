

#include "esp_common.h"		// Espressif register names, etc...
#include "gpio.h"			// Nefastor's GPIO library
#include "dht22.h"			// Nefastor's DHT22 library


void dht22_init (void)
{
	// setup the I/O pin : enable pull-up and falling-edge interrupt
	PIN_PULLUP_EN (GPIO_PIN_REG (DHT_PIN));
	// gpio_pin_intr_state_set (DHT_PIN, GPIO_PIN_INTR_NEGEDGE);
	// gpio_pin_intr_state_set (DHT_PIN, 2); 	// temporary fix for double define
}

/* The following function is a dumb implementation, designed to verify sensor
 * operation prior to optimizing the code for multitasking support.
 *
 * Note : this function should not be called within the first second after powering
 * up the sensor.
 */

int	dht_busy = 0;		// value is 1 while the following function is working
int sample_rh = -100;
int sample_t = -100;
int sample_valid = 0;	// value is 1 if the current sample is valid (good checksum)
int bit_duration[82];	// store the duration of each bit, in microseconds
unsigned char samples[5];

void dht22_read (void)
{
	// os_delay_us(1);
	int cnt = 0;		// bit counter
	int i = 0;			// bit duration counter (in microseconds)

	for (i=0;i<82;i++) bit_duration[i] = 0;	// clear previous results

	// Start sequence
	// 250ms of high
	GPIO_OUTPUT_SET(DHT_PIN, 1);
	os_delay_us (50000);	system_soft_wdt_feed();
	os_delay_us (50000);	system_soft_wdt_feed();
	os_delay_us (50000);	system_soft_wdt_feed();
	os_delay_us (50000);	system_soft_wdt_feed();
	os_delay_us (50000);	system_soft_wdt_feed();
	// vTaskDelay (1);		// Also works
	// Hold low for 20ms
	GPIO_OUTPUT_SET(DHT_PIN, 0);
	os_delay_us (20000);	system_soft_wdt_feed();
	// vTaskDelay (1);		// Also works
	// High for 40us
	GPIO_OUTPUT_SET(DHT_PIN, 1);
	os_delay_us(40);
	// Set DHT_PIN pin as an input
	GPIO_DIS_OUTPUT(DHT_PIN);

	// at this point, the sensor should immediately pull down the line, and keep
	// it down for 80 us. Let's see if it does.

	// wait for pin to drop (loop should exit immediately)
	// tests show this works (duration : 0)
	while (GPIO_INPUT_GET(DHT_PIN) == 1 && bit_duration[0] < DHT_TIMEOUT)
	{
		os_delay_us(1);
		bit_duration[0]++;
	}

	if (bit_duration[0] == DHT_TIMEOUT) //	return;	// The sensor failed to respond, previous samples remain
	{
		bit_duration[99] = 777;		// arbitrary error code
		return;
	}

	// now count all long the pin stays low
	// should be 80 µs but gets reported as 40.
	// note : re-using bit_duration[0] since I'm storing low time, then high time
	while (GPIO_INPUT_GET(DHT_PIN) == 0 && bit_duration[0] < DHT_TIMEOUT)
	{
		os_delay_us(1);
		bit_duration[0]++;
	}

	// now measure the high period for the start bit
	while (GPIO_INPUT_GET(DHT_PIN) == 1 && bit_duration[1] < DHT_TIMEOUT)
	{
		os_delay_us(1);
		bit_duration[1]++;
	}

	// Now I should read 40 bits in similar fashion. Let's make a loop:
	int bits;
	int ix = 2;
	for (bits = 1; bits<41; bits++)
	{
		system_soft_wdt_feed();

		// measure low state
		while (GPIO_INPUT_GET(DHT_PIN) == 0)
		{
			os_delay_us(1);
			bit_duration[ix]++;
		}
		ix++;
		// measure high state
		while (GPIO_INPUT_GET(DHT_PIN) == 1)
		{
			os_delay_us(1);
			bit_duration[ix]++;
		}
		ix++;
	}

	// decoding the samples
	unsigned char b;
	int byte_cnt;

	ix = 3;		// looking at high state durations only, post start bit
	for (byte_cnt = 0; byte_cnt < 5; byte_cnt++)
	{
		samples[byte_cnt] = 0;
		for (bits = 0, b = 0x80; bits < 8; bits++ , ix += 2)
		{
			if (bit_duration[ix] > 30)
				samples[byte_cnt] |= b;

			b = b >> 1;
		}
	}

	sample_rh = (samples[0] << 8) + samples[1];
	sample_t = (samples[2] << 8) + samples[3];

	uint16_t chksum = samples[0];
	chksum += samples[1];
	chksum += samples[2];
	chksum += samples[3];
	chksum &= 0x00FF;

	// if (samples[0]+samples[1]+samples[2]+samples[3] == samples[4])
	if (chksum == (uint16_t) samples[4])
		sample_valid = 1;
	else
		sample_valid = 0;
}
