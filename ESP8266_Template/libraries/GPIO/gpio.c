/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "espressif/esp_common.h"
#include "freertos/portmacro.h"

#include "gpio.h"



//////////////////////////// Nefastor's new GPIO functions /////////////////////////////////////

// GPIO MUXing LUT's
uint8 mux_offset[16] = {0x34, 0x18, 0x38, 0x14, 0x3C, 0x40, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x04, 0x08, 0x0C, 0x10};
//uint8 mux_func_nice[16] = {0, 3, 0, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}; // indexed by GPIO number
uint8 mux_func[16] = {0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30}; // as mux register bit fields
uint16 mux_func_to_mask[5] = {0x00, 0x10, 0x20, 0x30, 0x100};	// just because I can

// experimental : direct access to registers
uint8 mux_lut[16] = {12 ,5 ,13 ,4 ,14 ,15 ,6 ,7 ,8 ,9 ,10 ,11, 0 ,1 ,2 ,3 }; // GPIO number reordering. do ((mux_lut[n] + 1) << 2) to get the offset to the pin muxing register
// Note : the "+1" is unnecessary if the base address is that of the first pin : PERIPHS_IO_MUX_MTDI_U

#ifdef EXPERIMENTAL
// Mux registers as array:
//volatile uint32* mux = (volatile uint32 *)ETS_UNCACHED_ADDR(PERIPHS_IO_MUX_MTDI_U);
volatile PIN_MUX_register* mux = (volatile PIN_MUX_register *)ETS_UNCACHED_ADDR(PERIPHS_IO_MUX_MTDI_U);
// it should be possible to access the muxing registers using mux[mux_lut[gpio number]]

// same for pin registers:
volatile GPIO_PIN_register* gpio = (volatile GPIO_PIN_register *)ETS_UNCACHED_ADDR(PERIPHS_GPIO_BASEADDR + GPIO_PIN0_ADDRESS);
#endif

// GPIO MUXing
uint32 gpio_mux (int pin)
#ifdef EXPERIMENTAL
// easiest to read but takes even more space.
{
	int i = mux_lut[pin];
	mux[i].field.func_msb = 0;
	mux[i].field.func = mux_func[pin];
}
/* Easier to read but takes more RAM
{
	PIN_MUX_register mux_reg;
	mux_reg.rawVector = mux[mux_lut[pin]];
	// modify mux_reg fields
	mux_reg.field.func_msb = 0;			// GPIO function is either 0 or 3
	mux_reg.field.func = mux_func[pin];	// This table is ordered by GPIO pin number
	mux[mux_lut[pin]] = mux_reg.rawVector;
}*/
#else
// known good - this version takes less RAM (12 bytes less)
{
	// Writing a more economical pin MUXing function
	//PIN_FUNC_SELECT (PERIPHS_IO_MUX + mux_offset[pin], mux_func_nice[pin]);
	uint32 mux_addr = PERIPHS_IO_MUX + mux_offset[pin];
	// CLEAR_PERI_REG_MASK(mux_addr, 0x130);		// clear the FUNC field of the muxing register (because setting uses bitwise OR
	// SET_PERI_REG_MASK(mux_addr, mux_func[pin]);
	uint32 reg = READ_PERI_REG(mux_addr) & (~0x130);  // read and clear mux bits
	WRITE_PERI_REG(mux_addr, reg | mux_func[pin]);	  // OR and write back
	return mux_addr; // could be useful
}
#endif

/*// Old stuff : dedicated on/off functions for the pull-up. Waste of space.
// GPIO pull-up off
void inline gpio_pullup_off(uint32 pin_mux_reg_addr)
#ifdef EXPERIMENTAL
// uses 4 more bytes than the older version
{
	((volatile PIN_MUX_register*) pin_mux_reg_addr)->field.pull_up = 0;
}
#else
// known good
{
	uint32 reg = READ_PERI_REG(pin_mux_reg_addr) & (~BIT7);  // turn off the pull-up
	WRITE_PERI_REG(pin_mux_reg_addr, reg);
}
#endif

// GPIO pull-up on
// argument: PERIPHS_IO_MUX + mux_offset[pin_number]
void inline gpio_pullup_on(uint32 pin_mux_reg_addr)
#ifdef EXPERIMENTAL
{
	((volatile PIN_MUX_register*) pin_mux_reg_addr)->field.pull_up = 1;
}
#else
//known good
{
	uint32 reg = READ_PERI_REG(pin_mux_reg_addr);  // turn off the pull-up
	WRITE_PERI_REG(pin_mux_reg_addr, reg | BIT7);
}
#endif
*/

// value must be 0 or 1, nothing else
void inline gpio_set_pullup (uint32 pin_mux_reg_addr, uint32 value)
#ifdef EXPERIMENTAL
{
	((volatile PIN_MUX_register*) pin_mux_reg_addr)->field.pull_up = value;
}
#else
// This version takes less RAM but is harder to read
{
	uint32 reg = READ_PERI_REG(pin_mux_reg_addr) & (~BIT7);  // read and clear mux bit
	WRITE_PERI_REG(pin_mux_reg_addr, reg | (value << 7));	  // OR and write back to "value"
}
#endif

void inline gpio_setup_input (int pin)
{
	GPIO_REG_WRITE(GPIO_ENABLE_W1TC_ADDRESS, 0x1 << pin);	// just disable the pin
}

uint32 inline gpio_get_input (int pin)
{
	return (GPIO_REG_READ(GPIO_IN_ADDRESS) >> pin) & 0x1;
}

void gpio_setup_output (int pin, uint32 state)
{
	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, state << pin);
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, (1-state) << pin);
	GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, 0x1 << pin);
	// GPIO_REG_WRITE(GPIO_ENABLE_W1TC_ADDRESS, 0); // appears unnecessary
}

// Essentially the same function as "setup". Try to make this one faster.
// (Experimental fast version is UNTESTED)
void inline gpio_set_output (int pin, uint32 state)
{
	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, state << pin);
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, (1-state) << pin);
#ifndef EXPERIMENTAL
	GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, 0x1 << pin); // assume pin is enabled as output
#endif
	// GPIO_REG_WRITE(GPIO_ENABLE_W1TC_ADDRESS, 0); // appears unnecessary
}

// use 0 for totem-pole, 1 for open drain, all other values are deadly
void gpio_setup_drive_strength (int pin, int drive)
#ifdef EXPERIMENTAL
// very readable but takes more RAM (+8 bytes compared to the next best thing)
{
	gpio[pin].field.driver = drive;
}
#else
// known good
{
	// drive strength is setup in the pin register
	uint32 addr_pin = PERIPHS_GPIO_BASEADDR + GPIO_PIN0_ADDRESS + (pin << 2); // 0x60000300 + 0x28 + pin * 4

	GPIO_PIN_register reg;
	reg.rawVector = READ_PERI_REG(addr_pin);
	reg.field.driver = drive;	// if drive is larger than 1 bit, will C only take the LSB ?
	WRITE_PERI_REG(addr_pin, reg.rawVector);
}

// OLD VERSION, less readable
/*
{
	// drive strength is setup in the pin register
	uint32 addr_pin = PERIPHS_GPIO_BASEADDR + GPIO_PIN0_ADDRESS + (pin << 2); // 0x60000300 + 0x28 + pin * 4
	// specifically, gpio_register.h defines the fields GPIO_PIN0_DRIVER and GPIO_PIN0_DRIVER_S
	if (drive == 0)
		CLEAR_PERI_REG_MASK(addr_pin, BIT2); // Normal Drive : clear the field
	else
		SET_PERI_REG_MASK(addr_pin, BIT2);	// Open Drain : set the field
}*/
#endif

////////////////////////////////////////////////////////////////////////////////////////////////

// Nefastor : original function, very poorly coded.
// I'm keeping it as comment for future reference, but it's doomed.
/*
void gpio_config(GPIO_ConfigTypeDef *pGPIOConfig)
{
    uint16 gpio_pin_mask = pGPIOConfig->GPIO_Pin;
    uint32 io_reg;
    uint8 io_num = 0;
    uint32 pin_reg;

    if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Input) {
        GPIO_AS_INPUT(gpio_pin_mask);
    } else if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Output) {
        GPIO_AS_OUTPUT(gpio_pin_mask);
    }

    do {
        if ((gpio_pin_mask >> io_num) & 0x1) {
            io_reg = GPIO_PIN_REG(io_num);

            if ((0x1 << io_num) & (GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5)) {
                PIN_FUNC_SELECT(io_reg, 0);
            } else {
                PIN_FUNC_SELECT(io_reg, 3);
            }

            if (pGPIOConfig->GPIO_Pullup) {
                PIN_PULLUP_EN(io_reg);
            } else {
                PIN_PULLUP_DIS(io_reg);
            }

            if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Out_OD) {
                portENTER_CRITICAL();

                pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(io_num));
                pin_reg &= (~GPIO_PIN_DRIVER_MASK);
                pin_reg |= (GPIO_PAD_DRIVER_ENABLE << GPIO_PIN_DRIVER_LSB);
                GPIO_REG_WRITE(GPIO_PIN_ADDR(io_num), pin_reg);

                portEXIT_CRITICAL();
            } else if (pGPIOConfig->GPIO_Mode == GPIO_Mode_Sigma_Delta) {
                portENTER_CRITICAL();

                pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(io_num));
                pin_reg &= (~GPIO_PIN_SOURCE_MASK);
                pin_reg |= (0x1 << GPIO_PIN_SOURCE_LSB);
                GPIO_REG_WRITE(GPIO_PIN_ADDR(io_num), pin_reg);
                GPIO_REG_WRITE(GPIO_SIGMA_DELTA_ADDRESS, SIGMA_DELTA_ENABLE);

                portEXIT_CRITICAL();
            }

            gpio_pin_intr_state_set(io_num, pGPIOConfig->GPIO_IntrType);
        }

        io_num++;
    } while (io_num < 16);
}
*/

/*
 * Change GPIO pin output by setting, clearing, or disabling pins.
 * In general, it is expected that a bit will be set in at most one
 * of these masks.  If a bit is clear in all masks, the output state
 * remains unchanged.
 *
 * There is no particular ordering guaranteed; so if the order of
 * writes is significant, calling code should divide a single call
 * into multiple calls.
 */
/* Nefastor says :
 * The "enable" TC register doesn't seem necessary here.
 * Function kept for reference only (I have better)
 */
void gpio_output_conf(uint32 set_mask, uint32 clear_mask, uint32 enable_mask, uint32 disable_mask)
{
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, set_mask);
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clear_mask);
    GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, enable_mask);
    GPIO_REG_WRITE(GPIO_ENABLE_W1TC_ADDRESS, disable_mask);
}

/*
 * Sample the value of GPIO input pins and returns a bitmask.
 * TO DO : inline or turn into a macro
 */
uint32 gpio_input_get(void)
{
    return GPIO_REG_READ(GPIO_IN_ADDRESS);
}

/*
 * Register an application-specific interrupt handler for GPIO pin
 * interrupts.  Once the interrupt handler is called, it will not
 * be called again until after a call to gpio_intr_ack.  Any GPIO
 * interrupts that occur during the interim are masked.
 *
 * The application-specific handler is called with a mask of
 * pending GPIO interrupts.  After processing pin interrupts, the
 * application-specific handler may wish to use gpio_intr_pending
 * to check for any additional pending interrupts before it returns.
 */
void gpio_intr_handler_register(void *fn, void *arg)
{
    _xt_isr_attach(ETS_GPIO_INUM, fn, arg);
}

/*
  only highlevel and lowlevel intr can use for wakeup
*/
void gpio_pin_wakeup_enable(uint32 i, GPIO_INT_TYPE intr_state)
{
    uint32 pin_reg;

    if ((intr_state == GPIO_PIN_INTR_LOLEVEL) || (intr_state == GPIO_PIN_INTR_HILEVEL)) {
        portENTER_CRITICAL();

        pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));
        pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
        pin_reg |= (intr_state << GPIO_PIN_INT_TYPE_LSB);
        pin_reg |= GPIO_PIN_WAKEUP_ENABLE_SET(GPIO_WAKEUP_ENABLE);
        GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);

        portEXIT_CRITICAL();
    }
}

void gpio_pin_wakeup_disable(void)
{
    uint8  i;
    uint32 pin_reg;

    for (i = 0; i < GPIO_PIN_COUNT; i++) {
        pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));

        if (pin_reg & GPIO_PIN_WAKEUP_ENABLE_MASK) {
            pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
            pin_reg |= (GPIO_PIN_INTR_DISABLE << GPIO_PIN_INT_TYPE_LSB);
            pin_reg &= ~(GPIO_PIN_WAKEUP_ENABLE_SET(GPIO_WAKEUP_ENABLE));
            GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);
        }
    }
}

void gpio_pin_intr_state_set(uint32 i, GPIO_INT_TYPE intr_state)
{
    uint32 pin_reg;

    portENTER_CRITICAL();

    pin_reg = GPIO_REG_READ(GPIO_PIN_ADDR(i));
    pin_reg &= (~GPIO_PIN_INT_TYPE_MASK);
    pin_reg |= (intr_state << GPIO_PIN_INT_TYPE_LSB);
    GPIO_REG_WRITE(GPIO_PIN_ADDR(i), pin_reg);

    portEXIT_CRITICAL();
}

////////////////// GPIO 16 API ////////////////////////////////////////////

// Nefastor - TO DO : make the read and write functions inline

void gpio16_output_conf(void)
{
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF,
                   (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1); 	// mux configuration for XPD_DCDC to output rtc_gpio0

    WRITE_PERI_REG(RTC_GPIO_CONF,
                   (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);	//mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
                   (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);	//out enable
}

void gpio16_output_set(uint8 value)
{
    WRITE_PERI_REG(RTC_GPIO_OUT,
                   (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe) | (uint32)(value & 1));
}

void gpio16_input_conf(void)
{
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF,
                   (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1); 	// mux configuration for XPD_DCDC and rtc_gpio0 connection

    WRITE_PERI_REG(RTC_GPIO_CONF,
                   (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);	//mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
                   READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe);	//out disable
}

uint8 gpio16_input_get(void)
{
    return (uint8)(READ_PERI_REG(RTC_GPIO_IN_DATA) & 1);
}
