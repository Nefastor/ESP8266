

#include "unity.h"
#include <string.h>	// C string functions

// MCUnity state (mode)
int unity_mode = UNITY_MODE_INIT;					// three modes : init / setup / update

// Connection state

struct ip_addr unity_IP;    	// The IP address of whatever machine send the broadcast packet
struct udp_pcb *unity_pcb;		// Protocol Control Block

// GUI Setup Storage

void (*setup_operations)();		// Pointer to a firmware-specific function containing all calls to MCUnity setup operations

int* unity_variables_int[UNITY_MAX_VARIABLES];		// pointers to the variables shared with the Unity front-end
int	 unity_variables_int_occupancy = 0;				// index of the first available slot in the array above

void (*unity_variables_function[UNITY_MAX_VARIABLES])();	// pointers to firmware functions the user may want to trigger from the GUI
int	 unity_variables_function_occupancy = 0;				// index of the first available slot in the array above

// Utility functions for accessing packet data like a FIFO
// The idea is to cast the payload as a byte array

// pointer to a packet payload (for reading or writing)
uint8_t* payload = NULL;
int		 payload_index = 0;

inline void fifo_start() {payload_index = 0;}

inline uint8_t fifo_pop_byte()
{
	return payload[payload_index++];
}

inline uint8_t fifo_push_byte(uint8_t databyte)
{
	payload[payload_index++] = databyte; // I hope it returns the right one
	return databyte;
}

inline uint16_t fifo_pop_word_be()
{
	uint16_t rv = fifo_pop_byte();
	return (rv << 8) | fifo_pop_byte();
}

inline uint16_t fifo_push_word_be(uint16_t dataword)
{
	fifo_push_byte (dataword >> 8);
	fifo_push_byte (dataword & 0xFF);
	return dataword;
}

inline uint16_t fifo_pop_word_le()
{
	uint16_t rv = fifo_pop_byte() << 8;
	return rv | fifo_pop_byte();
}

inline uint16_t fifo_push_word_le(uint16_t dataword)
{
	fifo_push_byte (dataword & 0xFF);
	fifo_push_byte (dataword >> 8);
	return dataword;
}

inline int fifo_pop_int()
{
	uint32_t rv = fifo_pop_byte() << 8;
	rv = (rv | fifo_pop_byte()) << 8;
	rv = (rv | fifo_pop_byte()) << 8;
	return rv | fifo_pop_byte();
}

// note : the ESP is big-endian. This transmits in big-endian mode
inline int fifo_push_int(int data)
{
	uint32_t x = data;
	fifo_push_byte (x >> 24);
	fifo_push_byte ((x >> 16) & 0xFF);
	fifo_push_byte ((x >> 8) & 0xFF);
	fifo_push_byte (x & 0xFF);
	return data;
}

inline char* fifo_pop_string ()
{
	// this function assumes there's nothing after the string : don't update "payload_index"
	// create a char pointer to the first byte of the string in the payload, and return it
	return (char*) (payload + payload_index);
	// this function doesn't copy the data out of the packet : make sure to use the
	// string before releasing the packet
}

// push a null-terminated string to the packet payload
// for some reason, the ESP freezes if this is inlined
void fifo_push_string (char* datastring)
{
	int k = 0;	// index
	while (datastring[k] != 0)
		fifo_push_byte (datastring[k++]);
	//fifo_push_byte (0);		// Unity may or may not need it
	// don't send the null byte : I can always add it Unity-side if necessary
}

// GUI Setup Functions (only usable in UNITY_SETUP_MODE)

/*
 *
 */

int unity_setup_function (void (*func)(), const char* name)
{
	if (unity_variables_function_occupancy == UNITY_MAX_VARIABLES) return -3; // too many variables

	// store a pointer to the function
	unity_variables_function[unity_variables_function_occupancy] = func;

	// build packet:
	struct pbuf *transmission_pbuf;
	// compute packet payload length and allocate
	int tx_length = strlen(name) + 1 + 1;	// see below for detail of pushes
	transmission_pbuf = pbuf_alloc(PBUF_TRANSPORT,tx_length,PBUF_RAM);
	// initialize payload FIFO operations
	payload = (uint8_t*) transmission_pbuf->payload;
	fifo_start ();		// initialize payload FIFO mode
	// push the data
	fifo_push_byte (UNITY_TX_SETUP_FUNCTION);	// Command byte
	fifo_push_byte ((uint8_t) unity_variables_function_occupancy);		// Function's index / unique ID
	fifo_push_string ((char*) name);	// function name, to be shown in the GUI

	// send that packet !
	udp_sendto(unity_pcb, transmission_pbuf, &unity_IP, UNITY_NETWORK_PORT);
	pbuf_free(transmission_pbuf);

	// update occupancy and return the index for the function that has just been added
	return unity_variables_function_occupancy++;
}

int	 unity_setup_int (int* variable, const char* name, int min, int max, uint32_t flags)
{
	// this test can be omitted if proper design methodology is applied
	//if (unity_is_connected != UNITY_IS_CONNECTED) return -1;  // no connection to a host running the Unity app

	// this test can be omitted if proper design methodology is applied
	//if (unity_mode != UNITY_SETUP_MODE) return -2; // only usable in UNITY_SETUP_MODE

	// disabled for debugging only
	if (unity_variables_int_occupancy == UNITY_MAX_VARIABLES) return -3; // too many variables

	// store a pointer to the variable
	unity_variables_int[unity_variables_int_occupancy] = variable;

	// to do : create and send a packet to Unity to inform it of the new variable and its index
	// compute the length of the message (that of the "name" string", plus a fixed length for the other fields)

	//memcpy (transmission_pbuf->payload, name, name_len);	// start with the name
	// not sure how to add the index, min and max, and "look" fields (those are int's)

	// build packet:
	struct pbuf *transmission_pbuf;
	// compute packet payload length and allocate
	int tx_length = strlen(name) + 1 + 1 + 4 + 4 + 4 + 4;	// see below for detail of pushes
	transmission_pbuf = pbuf_alloc(PBUF_TRANSPORT,tx_length,PBUF_RAM);
	// initialize payload FIFO operations
	payload = (uint8_t*) transmission_pbuf->payload;
	fifo_start ();		// initialize payload FIFO mode
	// push the data
	fifo_push_byte (UNITY_TX_SETUP_INT);	// Command byte
	fifo_push_byte ((uint8_t) unity_variables_int_occupancy);		// Variable's index / unique ID
	fifo_push_int (*variable);		// send its current value
	fifo_push_int (min);			// send min and max boundaries
	fifo_push_int (max);
	fifo_push_int (flags);			// parameter to tell Unity how to display the variable
	fifo_push_string ((char*) name);// variable name, to be shown in the GUI

	// send that packet !
	udp_sendto(unity_pcb, transmission_pbuf, &unity_IP, UNITY_NETWORK_PORT);
	pbuf_free(transmission_pbuf);

	// update occupancy and return the index for the variable that has just been added
	return unity_variables_int_occupancy++;
}



// UDP callback : any packet sent by Unity will first go through here

void unity_udp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  if (p != NULL)
  {
	// initialize FIFO operation to read the packet payload :
	payload = (uint8_t*)p->payload; // cast the payload pointer to a byte array pointer
	fifo_start();	// initialize payload FIFO index

	// pop the first payload byte, which indicates the payload's type
	uint8_t packet_type = fifo_pop_byte();

	// getting here means the ESP is connected to Unity, time to parse that packet !
	// the next two bytes are the payload size, let's get that first, then switch/case
	// the packet type:

	uint8_t buf8 = 0x0;			// single-byte buffer
	uint32_t buf32 = 0x0;		// buffer for 32-bit field recomposition

	switch (packet_type)
	{
		case UNITY_RX_BROADCAST:	// update the IP address to the application
			unity_IP = *addr;		// copy the IP address of the remote host running the Unity app
			if (unity_mode == UNITY_MODE_INIT)
				unity_mode = UNITY_MODE_SETUP;	// mode transition, only if necessary
			// No need to send a reply : Unity will accept GUI setup packets from an ESP as a connection handshake
			break;
		case UNITY_RX_FORCE_SETUP:	// trigger a call to the MCUnity custom setup function
			unity_variables_int_occupancy = 0;	// clear previously-registered variables of each supported type
			unity_variables_function_occupancy = 0;
			setup_operations();		// call the custom setup function that was registered by the firmware
			break;
		case UNITY_RX_CALL_FUNCTION:	// call a firmware function
			unity_variables_function[fifo_pop_byte()]();	// pop function index from incoming packet and use it to call the correct function pointer
			break;
		case UNITY_RX_SET_INT:		// set "int" type variable
			buf8 = fifo_pop_byte(); // payload byte 2 is the variable's index
			buf32 = fifo_pop_int();		// pop its new value
			*unity_variables_int[buf8] = (int) buf32;
			break;
		case UNITY_RX_REQUEST_INT:	// application requests update of "int" variables
			unity_update_int (0, 0);	// send the value of all "int" variables
			break;
		default:	// unsupported packet type
			break;	// nothing to do : we're going straight to pubf deallocation and return
	}

    // ... and finish by deallocating the packet buffer
	pbuf_free(p);
  }
}


// Unity interface initialization : this must be called ASAP following connection to the WiFi network

void unity_init (void (*setup_func)())
{
	// Setup UDP packet listener
	unity_pcb = udp_new();		// allocate the PCB
	udp_bind(unity_pcb, IP_ADDR_ANY, UNITY_NETWORK_PORT);
	udp_recv(unity_pcb, unity_udp_recv, NULL);

	// Save handle of setup task, in case Unity app requests a re-setup
	setup_operations = setup_func;
}

void unity_setup ()
{
	// TO DO - MCUnity STATE TRANSITIONS !!!
	// Actually, the "setup" mode is now superfluous if all setup happens here...
	// ... unless this function is interrupted by another task
	unity_mode = UNITY_MODE_SETUP;

	// TO DO - reset any MCUnity GUI state information on the firmware side
	// (since the setup operations will otherwise duplicate it)
	unity_variables_int_occupancy = 0;		// clear "int" variables storage

	// using a function pointer instead:
	(*setup_operations)();		// call the firmware-specific GUI setup function

	// finally, transition MCUnity to "update" state
	unity_mode = UNITY_MODE_UPDATE;
}

// This is used to let the firmware handle the wait for connection anyway it sees fit
int unity_not_ready ()
{
	return (unity_IP.addr == 0) ? 1 : 0;
}


// GUI Update Functions

// These functions send the application the current value of registered variables of the same type
// * offset is the index of the variable to be sent to the application
// * count indicates how many variables should be sent
// if count is zero, all variables from offset to the last are sent


void unity_update_int (int offset, int count)
{
	//if (unity_not_ready ()) return;		// always returns for some reason

	if (unity_mode != UNITY_MODE_UPDATE) return;

	struct pbuf *transmission_pbuf;		// start creating a packet

	// compute packet payload length and allocate
	int cnt = count;	// how many variables are going to be sent ?
	if (count == 0)
		cnt = unity_variables_int_occupancy - offset;

	// what size is the packet payload going to be
	int tx_length = 2 + (cnt << 2);	// fixed length part + variables in bytes
	// "<<2" is a cheap "*4" to turn count into a number of bytes

	transmission_pbuf = pbuf_alloc(PBUF_TRANSPORT,tx_length,PBUF_RAM);

	// initialize payload FIFO operations
	payload = (uint8_t*) transmission_pbuf->payload;
	fifo_start ();		// initialize payload FIFO mode

	// push the fixed part
	fifo_push_byte (UNITY_TX_UPDATE_INT);	// Command byte
	fifo_push_byte ((uint8_t) offset);		// Variable index / unique ID offset

	// push the variable-length part
	int j;
	for (j = 0; j < cnt; j++)
		fifo_push_int (*unity_variables_int[offset + j]);

	// send that packet !
	udp_sendto(unity_pcb, transmission_pbuf, &unity_IP, UNITY_NETWORK_PORT);
	pbuf_free(transmission_pbuf);
}
