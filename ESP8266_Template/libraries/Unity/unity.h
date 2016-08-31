/*
 * unity.h
 *
 *  Created on: Aug 30, 2016
 *      Author: Nefastor
 */

#ifndef LIBRARIES_UNITY_UNITY_H_
#define LIBRARIES_UNITY_UNITY_H_

#include "lwip/udp.h"

// extern globals for debugging
extern struct ip_addr unity_IP;

// GUI Setup Storage
#define UNITY_MAX_VARIABLES		10		// max. 255

// Connection status
#define UNITY_NOT_CONNECTED		0
#define UNITY_IS_CONNECTED		1

// Modes of operation
#define UNITY_SETUP_MODE		0
#define UNITY_UPDATE_MODE		1

// Network packet types (8-bit) FROM UNITY TO ESP
#define UNITY_RX_BROADCAST		0x01		// Initial broadcast packet used to establish connection
#define UNITY_RX_SET_INT		0x02		// Set the value of an "int" type variable

// Network packet types (8-bit) FROM ESP TO UNITY
#define UNITY_TX_SETUP_INT		0x01		// Setup integer variable

// Network parameters
#define UNITY_NETWORK_PORT		55555

void unity_init ();		// library and socket initialization

int	 unity_register_int (int* variable, const char* name, int min, int max, int look);

// UDP callback - only used internally
// void unity_udp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

#endif /* LIBRARIES_UNITY_UNITY_H_ */
