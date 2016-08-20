/*
 * MAX6675.h
 *
 *  Created on: Aug 20, 2016
 *      Author: Nefastor
 */

#ifndef __MAX6675_H__
#define __MAX6675_H__

#include <c_types.h>

// pinout defintions
#define	MAX6675_SO		4		// GPIO 4 is NodeMCU's D2
#define MAX6675_CS		0		// GPIO 0 is NodeMCU's D3
#define MAX6675_SCK		2		// GPIO 2 is NodeMCU's D4

void max6675_init (void);

uint16 max6675_read (void);

#endif /* __MAX6675_H__ */
