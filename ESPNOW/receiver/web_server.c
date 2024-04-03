/// Proprietary SDK includes
#include "ets_sys.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

// Our header
#include "web_server.h"

#define DEBUG 1
#if !DEBUG
	#define printf(...) 
#endif

// Print debug information on wifi event
void wifi_handle_event(System_Event_t* evt)
{
	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			printf("connect to ssid %s, channel %d\n",
					evt->event_info.connected.ssid,
					evt->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:
			printf("disconnect from ssid %s, reason %d\n",
					evt->event_info.disconnected.ssid,
					evt->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			printf("mode: %d -> %d\n",
					evt->event_info.auth_change.old_mode,
					evt->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
					IP2STR(&evt->event_info.got_ip.ip),
					IP2STR(&evt->event_info.got_ip.mask),
					IP2STR(&evt->event_info.got_ip.gw));
			printf("\n");
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			printf("station: " MACSTR "join, AID = %d\n",
					MAC2STR(evt->event_info.sta_connected.mac),
					evt->event_info.sta_connected.aid);
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			printf("station: " MACSTR "leave, AID = %d\n",
					MAC2STR(evt->event_info.sta_disconnected.mac),
					evt->event_info.sta_disconnected.aid);
			break;
		default:
			break;
	}
}

// Will run when data is received
static void ICACHE_FLASH_ATTR
web_server_receive(void* arg, char* pusrdata, unsigned short length)
{
	// arg is a pointer to our esp connection
	struct espconn* esp_conn = arg;
	// We want to reuse this ip address
	espconn_set_opt(esp_conn, ESPCONN_REUSEADDR);
	printf("Received data:\r\n%s\r\n", pusrdata); // not sure if null terminated
	                                              // Send a one through uart to tell the atmega to send it's sensor readings
	// Some basic html for a webpage
	char* header_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: keep-alive\r\nContent-Length: 5\r\n\r\nhello\r\n\r\n";
	printf("Sending now\r\n");
	// Send the html to the client
	espconn_sent(esp_conn, header_html, sizeof(header_html));
}

// Will run if TCP connection is closed
static void ICACHE_FLASH_ATTR
web_server_disconnect(void* arg)
{
	struct espconn* esp_conn = arg;

	printf("%d.%d.%d.%d:%d disconnect\n", esp_conn->proto.tcp->remote_ip[0],
			esp_conn->proto.tcp->remote_ip[1],esp_conn->proto.tcp->remote_ip[2],
			esp_conn->proto.tcp->remote_ip[3],esp_conn->proto.tcp->remote_port);
}

// Will run if TCP disconnects
static void ICACHE_FLASH_ATTR
web_server_reconnect(void* arg, char err)
{
	struct espconn* esp_conn = arg;

	printf("%d.%d.%d.%d:%d err %d reconnect\n", esp_conn->proto.tcp->remote_ip[0],
		esp_conn->proto.tcp->remote_ip[1],esp_conn->proto.tcp->remote_ip[2],
		esp_conn->proto.tcp->remote_ip[3],esp_conn->proto.tcp->remote_port, err);
}

// Sets callback once espconn_accept is called
static void ICACHE_FLASH_ATTR
web_server_listen(void* arg)
{
 	struct espconn* esp_conn = arg;
	printf("Listening\n");
	espconn_regist_recvcb(esp_conn, web_server_receive);
	espconn_regist_reconcb(esp_conn, web_server_reconnect);
	espconn_regist_disconcb(esp_conn, web_server_disconnect);
}

void web_server_init(const char* ssid, const char* passwd, uint8_t channel, bool static_ip)
{
	// We need AP mode to create our own wifi
	wifi_set_opmode(SOFTAP_MODE);
	wifi_softap_dhcps_stop();

	struct softap_config apconfig;
	// Fill in the structure with ssid and password
	if (wifi_softap_get_config(&apconfig)) {
		strncpy(apconfig.ssid, ssid, 32);
		strncpy(apconfig.password, passwd, 64);
		os_memcpy(apconfig.password, passwd, os_strlen(passwd));
		apconfig.authmode = AUTH_OPEN;
		apconfig.ssid_hidden = 0;
		apconfig.max_connection = 4;
		// apconfig.channel=7;
		if (!wifi_softap_set_config(&apconfig))
			printf("ESP8266 not set ap config!\r\n");
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
	printf("Web Server initialized\n");
}
