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
#define UNITY_MAX_VARIABLES		10		// max. 255

// Modes of operation
#define UNITY_MODE_INIT			0		// Firmware not connected to the application
#define UNITY_MODE_SETUP		1		// GUI setup phase (system initialization)
#define UNITY_MODE_UPDATE		2		// GUI update phase (system operating)

// NETWORK PACKET TYPES ARE UNIQUE ACROSS BOTH TX AND RX
// TX means ESP to Unity, RX means Unity to ESP

#define UNITY_RX_BROADCAST		0x01		// Initial broadcast packet used to establish connection
#define UNITY_TX_SETUP_INT		0x02		// Setup integer variable GUI element
#define UNITY_RX_SET_INT		0x03		// Set the value of an "int" type variable


// Network parameters
#define UNITY_NETWORK_PORT		55555

/////////////////////////// API /////////////////////////////////

// Overall Init Function

void unity_init (void (*setup_func)());		// library and socket initialization

// Overall GUI Setup Function

void unity_setup ();		// triggers the execution of the MCUnity setup operations

// GUI Setup operations

int	 unity_setup_int (int* variable, const char* name, int min, int max, uint32_t flags);

// GUI Update Functions

#endif /* LIBRARIES_UNITY_UNITY_H_ */
