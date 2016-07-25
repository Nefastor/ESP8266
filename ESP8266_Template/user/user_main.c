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



#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio.h"

#define LED_GPIO 2
#define LED_GPIO_MUX PERIPHS_IO_MUX_GPIO2_U
#define LED_GPIO_FUNC FUNC_GPIO2

#define INCLUDE_vTaskDelay 1

void ICACHE_FLASH_ATTR
blink_task(void *pvParameters)
{
    uint8_t state = 0;
    
    while (1)
    {
    	state = 1 - state;
    	GPIO_OUTPUT_SET (LED_GPIO, state);
    	vTaskDelay (20);
    }
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void)
{
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);

    /* need to set opmode before you set config */
    wifi_set_opmode(STATION_MODE);

    xTaskCreate(blink_task, "blink_task", 256, NULL, 2, NULL);
}

