#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

#define LIFX_PORT 56700
#define BROADCAST_IP "255.255.255.255"

#define SSID "YOUR SSID"
#define PASSWORD "YOUR PASSWORD"

void receive_responses();
void send_discovery_message();

#endif