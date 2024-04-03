extern "C" {
/// Proprietary SDK includes
#include "ets_sys.h"
#include "espconn.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espnow.h"
}

typedef struct {
	int x, y;
} Message;

Message m = {511, 511};

// Actual MAC of mega 0x2e,0xf4,0x32,0x4c,0xbe,0x11 
unsigned char server_mac [] = {0x2e,0xf4,0x32,0x4c,0xbe,0x11};

void on_data_sent(unsigned char *mac_addr, unsigned char status)
{
	printf("Sent with status: %d\r\n", status);
}

void setup() {
	Serial.begin(9600);
	pinMode(A0, INPUT);
	wifi_set_opmode(STATION_MODE);

	if (esp_now_init())
		Serial.print("ERROR with esp_now_init\r\n");
	else
		Serial.print("ESP now init success\r\n");

	esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
	esp_now_register_send_cb(on_data_sent);

	if (esp_now_add_peer(server_mac, ESP_NOW_ROLE_SLAVE, 0, NULL, 16))
		Serial.print("Failed to add peer\n");
	else
		Serial.printf("Success adding peer\r\n");
}

void loop() {
	m.y = system_adc_read();
	Serial.printf("Sent %d, %d\r\n", m.x, m.y);
	if (esp_now_send(server_mac, (unsigned char*)&m, sizeof(Message)))
		Serial.print("SUCCESS\r\n");
	else
		Serial.print("FAILED\r\n");
	delay(200);
}
