#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "include/lifx_struct_def.h"

#define LIFX_PORT 56700
#define BROADCAST_IP "255.255.255.255"

// Mutex protecting the LIFX device linked list (head).
// Shared across rx_parser, console_cmd, and data_structure.
extern SemaphoreHandle_t device_list_mutex;

void receive_responses(void *pvParameters);
void send_discovery_message(void *pvParameters);
void console_task(void *pvParameters);
void register_commands(void);
void log_hex(const void* data, size_t data_len);

lifx_device_t* find_or_create_device(struct in_addr ip);
void update_device_data(lifx_device_t *device, const MessageData *data, MessageType type);
void handle_message(struct in_addr ip, const MessageData *data, MessageType type);
void cleanup_devices();

#endif