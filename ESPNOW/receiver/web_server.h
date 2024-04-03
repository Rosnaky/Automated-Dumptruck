#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "osapi.h"

void web_server_init(const char* ssid, const char* passwd, uint8_t channel, bool static_ip);

#endif /* WEB_SERVER_H */
