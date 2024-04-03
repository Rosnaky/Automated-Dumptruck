extern "C" {
/// Proprietary SDK includes
#include "ets_sys.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "espnow.h"
#include "web_server.h"
}

void memcpy(unsigned char* dest, unsigned char* source, unsigned int len)
{
	while (len--)
		*dest++ = *source++;
}

// Actual MAC of mega 0x2e,0xf4,0x32,0x4c,0xbe,0x11 
unsigned char server_mac [] = {0x2e,0xf4,0x32,0x4c,0xbe,0x11};

// Structure for message
typedef struct {
	int x, y;
} Message;

// Will run when data is received
void on_data_recv(uint8_t* mac, uint8_t* data, unsigned char len)
{
	// Copy data onto separate struct on stack
	Message m;
	// Casts because -fpermissive
	memcpy((unsigned char*)&m, (unsigned char*)data, len);
	// Print out data received
	Serial.write(m.x);
	Serial.write(m.y);
	//Serial.printf("MAC_ADDR: %x:%x:%x:%x\r\n", mac[0], mac[1], mac[2], mac[3]);
	//Serial.printf("Length: %d\r\n", len);
	//Serial.printf("DATA: %d, %d\r\n", m.x, m.y);
}

void setup()
{
	// Initialize uart at 9600 baud
	Serial.begin(9600);
	// Make sure we set the mac address to the same one that is available to sender
	wifi_set_macaddr(STATION_IF, (unsigned char*)&server_mac);
	web_server_init("ESP WIFI!", "", 6, false);
	// Initialize esp now
	if (esp_now_init())
		Serial.println("Error initializing ESP-NOW");
	Serial.println("The connection works!");
	// Set role as slave (joystick esp is the controller)
	esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
	// Set the callback function
	esp_now_register_recv_cb(on_data_recv);
}

void loop()
{

}
