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

uint8 mpu9250_read (uint8 addr);


////////////// I2C Only /////////////////////

// MPI9250's AD0 on GPIO12
#define AD0_MUX PERIPHS_IO_MUX_MTDI_U
#define AD0_FUNC FUNC_GPIO12
#define AD0_PIN 12
// MPI9250's nCS on GPIO15
#define nCS_MUX PERIPHS_IO_MUX_MTDO_U
#define nCS_FUNC FUNC_GPIO15
#define nCS_PIN 15
// MPI9250's FSYNC on GPIO5
#define FSYNC_MUX PERIPHS_IO_MUX_GPIO5_U
#define FSYNC_FUNC FUNC_GPIO5
#define FSYNC_PIN 5

void mpu9250_i2c_init ();


#endif /* _MPU9250_H_ */
