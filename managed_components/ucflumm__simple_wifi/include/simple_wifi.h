#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

#define WIFI_CONNECTED_BIT BIT0

void wifi_manager_init(const char* ssid, const char* password);

#endif