#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "include/lifx_struct_def.h"
#include "esp_log.h"

#define LIFX_PORT 56700
#define BROADCAST_IP "255.255.255.255"

#define SSID "YOUR SSID"
#define PASSWORD "YOUR PASSWORD"

//glocal dec
lifx_device_t *head = NULL;

// rx parser funcs
void listen_for_messages(void *pvParameters);
void process_incoming_messages(const char* data, ssize_t len, const struct sockaddr_in* from);
bool is_discovery_response(const char* data, ssize_t len);
void handle_discovery_response(const char* data, ssize_t len, const struct sockaddr_in* from);
bool is_get_state_response(const char* data, ssize_t len);
void handle_get_state_response(const char* data, ssize_t len, const struct sockaddr_in* from);
void handle_state_power_response(const char* data, ssize_t len, const struct sockaddr_in* from);
void handle_state_label_response(const char* data, ssize_t len, const struct sockaddr_in* from);
void handle_state_location_response(const char* data, ssize_t len, const struct sockaddr_in* from);
void handle_light_state_response(const char* data, ssize_t len, const struct sockaddr_in* from);
void handle_state_light_power_response(const char* data, ssize_t len, const struct sockaddr_in* from);
void handle_state_rpower_response(const char* data, ssize_t len, const struct sockaddr_in* from);
char* log_device_info(const struct sockaddr_in* from, int level);

typedef void (*lifx_message_sender_t)(int sock, struct sockaddr_in *dest, const void *message, size_t msg_length);
lifx_payload_set_power_t* build_set_power_payload(uint16_t power, uint32_t duration);
lifx_message_t* build_lifx_message(uint8_t *target, uint16_t payload_size, void *payload, lifx_message_sender_t sender, int sock, struct sockaddr_in *dest);
void send_lifx_message(int sock, struct sockaddr_in *dest, const void *message, size_t msg_length);


int init_udp_socket(bool enable_broadcast, uint16_t port);
int cmd_light_control(int argc, char **argv);
void initialize_console(void);
void register_commands(void);

void remove_device(lifx_device_t **head, const char *ip);
lifx_device_t *find_device(lifx_device_t *head, const char *ip);
lifx_device_t *add_or_update_device(lifx_device_t **head, const char *ip, uint8_t service, uint32_t port,
                                    const char *label, const uint8_t *location, const char *location_label,
                                    uint64_t location_updated_at, const char *echoing, uint16_t unhandled_type,
                                    uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin,
                                    uint16_t light_power, const char *light_label, const uint16_t *relay_powers,
                                    size_t relay_count);
void cleanup_devices(lifx_device_t *head);
lifx_device_t *find_device_by_ip(lifx_device_t *head, const char *ip);

#endif