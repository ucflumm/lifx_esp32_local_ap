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

lifx_device_t *add_device_to_list(lifx_device_t **head, const char *ip, uint32_t port, uint8_t service);
void remove_device(lifx_device_t **head, const char *ip);
lifx_device_t *find_device(lifx_device_t *head, const char *ip);

#endif