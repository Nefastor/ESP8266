/*
 * MAX6675.h
 *
 *  Created on: Aug 20, 2016
 *      Author: Nefastor
 *
*/

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

#ifndef __MAX6675_H__
#define __MAX6675_H__

#include <c_types.h>

// pinout defintions
#define	MAX6675_SO		4		// GPIO 4 is NodeMCU's D2
#define MAX6675_CS		0		// GPIO 0 is NodeMCU's D3
#define MAX6675_SCK		2		// GPIO 2 is NodeMCU's D4

inline void max6675_init (void);		// mux and set I/O pins for MAX6675 operation

inline uint16 max6675_read (void);		// read 16 bits from a MAX6675

#endif /* __MAX6675_H__ */
