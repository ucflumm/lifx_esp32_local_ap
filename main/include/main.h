#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lwip/sockets.h"
#include "lwip/inet.h"

#define LIFX_PORT 56700
#define BROADCAST_IP "255.255.255.255"


void receive_responses();
void send_discovery_message();
void console_task(void *pvParameters);
void register_commands(void);

#endif