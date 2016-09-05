/*
 * unity.h
 *
 *  Created on: Aug 30, 2016
 *      Author: Nefastor
 */

#ifndef LIBRARIES_UNITY_UNITY_H_
#define LIBRARIES_UNITY_UNITY_H_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/udp.h"

// extern globals for debugging
extern struct ip_addr unity_IP;

// GUI Setup Storage
#define UNITY_MAX_VARIABLES		10		// max. 255 variables of each supported type

// Modes of operation
#define UNITY_MODE_INIT			0		// Firmware not connected to the application
#define UNITY_MODE_SETUP		1		// GUI setup phase (system initialization)
#define UNITY_MODE_UPDATE		2		// GUI update phase (system operating)

// NETWORK PACKET TYPES ARE UNIQUE ACROSS BOTH TX AND RX
// TX means ESP to Unity, RX means Unity to ESP

#define UNITY_TX_SETUP_FUNCTION	0x00		// Setup firmware function GUI element
#define UNITY_RX_BROADCAST		0x01		// Initial broadcast packet used to establish connection
#define UNITY_RX_FORCE_SETUP	0x02		// Execute the setup operations sequence
#define UNITY_RX_CALL_FUNCTION	0x03		// Call a function
#define UNITY_TX_SETUP_INT		0x04		// Setup integer variable GUI element
#define UNITY_RX_SET_INT		0x05		// Set the value of an "int" type variable
#define UNITY_TX_UPDATE_INT		0x06		// Send Unity the current value of all registered "int" variables (for GUI update)
#define UNITY_RX_REQUEST_INT	0x07		// Application request to send "int" values update

// Network parameters
#define UNITY_NETWORK_PORT		55555

////////////////// MACROS ///////////////////////////////////////

#define GUI_FLAGS(X, Y, W, H, C1, C2, R) ( \
	((X & 0xF) << 28) | \
	((Y & 0xF) << 24) | \
	((W & 0xF) << 20) | \
	((H & 0xF) << 16) | \
	((C1 & 0xF) << 12) | \
	((C2 & 0xF) << 8) | \
	(R & 0xFF) )

/////////////////////////// API /////////////////////////////////

// Overall Init Function

void unity_init (void (*setup_func)());		// library and socket initialization

// Test if MCUnity network connection is established, following unity_init

int unity_not_ready ();			// test repeatedly until it returns zero

// Overall GUI Setup Function

void unity_setup ();		// triggers the execution of the MCUnity setup operations

// GUI Setup operations

int unity_setup_function (void (*func)(), const char* name, uint32_t flags);
int unity_setup_int (int* variable, const char* name, int min, int max, uint32_t flags);

// GUI Update Functions

void unity_update_int (int offset, int count);

#endif /* LIBRARIES_UNITY_UNITY_H_ */
