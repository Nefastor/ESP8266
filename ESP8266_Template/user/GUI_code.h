/*
 * GUI_code.h
 *
 *  Created on: Sep 5, 2016
 *      Author: Nefastor
 */

#ifndef USER_GUI_CODE_H_
#define USER_GUI_CODE_H_

// it may be necessary to duplicate some code :
#define LED_GPIO 		2		// ESP8266 GPIO pin number, NOT module D pin number


// global variables I want to expose on the Unity front-end
int	exposed_variable = 0x12345678;
int adc;
int test = -120;
int led = 1;


// example of functions designed to be called from the GUI
void increment_counter ()
{
	test++;
}

void toggle_led ()
{
	led = 1 - led;
	GPIO_OUTPUT_SET (LED_GPIO, led);
}



// example of GUI flags definitions, centralized for clarity
// each macro defines a GUI element's position (X, Y), size (W, H), two colors and a reserved byte
// position and dimension are expressed in 1/16th of a display's dimensions.
/*// large GUI tiles
#define	GUI1	GUI_FLAGS(0,0,4,3,0,0,0)
#define	GUI2	GUI_FLAGS(4,0,4,3,0,0,0)
#define	GUI3	GUI_FLAGS(0,3,4,3,0,0,0)
#define	GUI4	GUI_FLAGS(4,3,4,3,0,0,0)
#define	GUI5	GUI_FLAGS(0,6,4,3,0,0,0)
#define	GUI6	GUI_FLAGS(4,6,4,3,0,0,0)
*/
// small GUI tiles (1 x 1)
/*
#define	GUI1	GUI_FLAGS(0,0,1,1,0,0,0)
#define	GUI2	GUI_FLAGS(1,0,1,1,0,0,0)
#define	GUI3	GUI_FLAGS(2,0,1,1,0,0,0)
#define	GUI4	GUI_FLAGS(3,0,1,1,0,0,0)
#define	GUI5	GUI_FLAGS(4,0,1,1,0,0,0)
#define	GUI6	GUI_FLAGS(5,0,1,1,0,0,0)
*/
// larger GUI tiles (2 x 2)
#define	GUI1	GUI_FLAGS(0,0,2,2,0,0,0)
#define	GUI2	GUI_FLAGS(2,0,2,2,0,0,0)
#define	GUI3	GUI_FLAGS(4,0,2,2,0,0,0)
#define	GUI4	GUI_FLAGS(6,0,2,2,0,0,0)
#define	GUI5	GUI_FLAGS(8,0,2,2,0,0,0)
#define	GUI6	GUI_FLAGS(10,0,2,2,0,0,0)
#define	GUI7	GUI_FLAGS(12,0,2,2,0,0,0)
#define	GUI8	GUI_FLAGS(14,0,2,2,0,0,0)



#endif /* USER_GUI_CODE_H_ */
