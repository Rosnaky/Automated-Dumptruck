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
	Serial.printf("MAC_ADDR: %x:%x:%x:%x\r\n", mac[0], mac[1], mac[2], mac[3]);
	Serial.printf("Length: %d\r\n", len);
	Serial.printf("DATA: %d, %d\r\n", m.x, m.y);
}

// Print debug information on wifi event
void wifi_handle_event(System_Event_t* evt)
{
	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			Serial.printf("connect to ssid %s, channel %d\n",
					evt->event_info.connected.ssid,
					evt->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:
			Serial.printf("disconnect from ssid %s, reason %d\n",
					evt->event_info.disconnected.ssid,
					evt->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			Serial.printf("mode: %d -> %d\n",
					evt->event_info.auth_change.old_mode,
					evt->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			Serial.printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
					IP2STR(&evt->event_info.got_ip.ip),
					IP2STR(&evt->event_info.got_ip.mask),
					IP2STR(&evt->event_info.got_ip.gw));
			Serial.printf("\n");
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			Serial.printf("station: " MACSTR "join, AID = %d\n",
					MAC2STR(evt->event_info.sta_connected.mac),
					evt->event_info.sta_connected.aid);
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			Serial.printf("station: " MACSTR "leave, AID = %d\n",
					MAC2STR(evt->event_info.sta_disconnected.mac),
					evt->event_info.sta_disconnected.aid);
			break;
		default:
			break;
	}
}

void wifi_set_up(const char* ssid, const char* passwd, uint8_t channel, bool static_ip)
{
	// We need AP mode to create our own wifi
	wifi_set_opmode(SOFTAP_MODE);
	wifi_softap_dhcps_stop();

	struct softap_config apconfig;
	// Fill in the structure with ssid and password
	if (wifi_softap_get_config(&apconfig)) {
		strncpy((char*)apconfig.ssid, ssid, 32);
		strncpy((char*)apconfig.password, passwd, 64);
		os_memcpy(apconfig.password, passwd, os_strlen(passwd));
		apconfig.authmode = AUTH_OPEN;
		apconfig.ssid_hidden = 0;
		apconfig.max_connection = 4;
		// apconfig.channel=7;
		if (!wifi_softap_set_config(&apconfig))
			Serial.print("ESP8266 not set ap config!\r\n");
	}
	// Set the event callback function
	wifi_set_event_handler_cb(wifi_handle_event);
	// Set our static ip
	static struct ip_info info;
	IP4_ADDR(&info.ip, 192, 168, 22, 1);
	IP4_ADDR(&info.gw, 192, 168, 22, 1);
	IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	wifi_set_ip_info(SOFTAP_IF, &info);

	// Initialize dhcp
	struct dhcps_lease dhcp_lease;
	IP4_ADDR(&dhcp_lease.start_ip, 192, 168, 22, 2);
	IP4_ADDR(&dhcp_lease.end_ip, 192, 168, 22, 5);
	wifi_softap_set_dhcps_lease(&dhcp_lease);

	wifi_softap_dhcps_start();

	//Serial.print("SOFTAP Status:%d\r\n", wifi_softap_dhcps_status());
	//Serial.print("Size of ESP8266: %d\r\n", sizeof(apconfig.ssid));
	//Serial.print("Length of ESP8266: %d\r\n", os_strlen(apconfig.ssid));

	static struct espconn esp_conn;
	static esp_tcp esptcp;
	// Fill the connection structure, including "listen" port
	esp_conn.type = ESPCONN_TCP;
	esp_conn.state = ESPCONN_NONE;
	esp_conn.proto.tcp = &esptcp;
	esp_conn.proto.tcp->local_port = 80;
	esp_conn.recv_callback = NULL;
	esp_conn.sent_callback = NULL;
	esp_conn.reverse = NULL;
	// Register the connection timeout(-1=no timeout)
	espconn_regist_time(&esp_conn, 0, 0);
	// Register connection callback
	//espconn_regist_connectcb(&esp_conn, web_server_listen);
	// Start listening for connections
	espconn_accept(&esp_conn);
	Serial.print("Web Server initialized\n");
}

void setup()
{
	// Initialize uart at 9600 baud
	Serial.begin(9600);
	// Make sure we set the mac address to the same one that is available to sender
	wifi_set_macaddr(STATION_IF, (unsigned char*)&server_mac);
	wifi_set_up("ESP WIFI!", "", 6, false);
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
