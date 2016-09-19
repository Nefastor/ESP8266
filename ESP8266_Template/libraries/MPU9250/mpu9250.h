/*
 * mpu9250.h
 *
 *  Created on: Sep 14, 2016
 *      Author: Nefastor
 */

#ifndef _MPU9250_H_
#define _MPU9250_H_

#include <c_types.h>

void mpu9250_init();

void mpu9250_hspi_clock(uint16 prediv, uint8 cntdiv);

uint8 mpu9250_read (uint8 addr);


#endif /* _MPU9250_H_ */
