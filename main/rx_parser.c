#include "include/main.h"
#include "include/lifx_struct_def.h"
#include <stdbool.h>
#include <stdlib.h>

static const char *default_label = "";
static const uint8_t *default_location = NULL;
static const char *default_location_label = "";
static uint64_t default_location_updated_at = 0;
static const char *default_echoing = "";
static uint16_t default_unhandled_type = 0;
static uint16_t default_hue = 0, default_saturation = 0, default_brightness = 0, default_kelvin = 0, default_light_power = 0;
static const char *default_light_label = "";
static const uint16_t *default_relay_powers = NULL;
static size_t default_relay_count = 0;

void listen_for_messages(void *pvParameters) {
    int sock = *(int*)pvParameters;  // Extract the socket file descriptor

    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    char buf[1024];  // Buffer size is sufficient for expected message size

    while (true) {
        ssize_t len = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &from_len);
        if (len > 0) {
            process_incoming_messages(buf, len, &from);
        } else if (len == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            ESP_LOGE("udp_rx", "recvfrom failed: errno %d", errno);
            break;  // Optionally, could attempt to restart the socket or exit the task
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Use portTICK_PERIOD_MS or pdMS_TO_TICKS for clarity
    }
}

void process_incoming_messages(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (is_get_state_response(data, len)) {
        handle_get_state_response(data, len, from);  // Pass 'from' to the handler
    } else if (is_discovery_response(data, len)) {
        handle_discovery_response(data, len, from);
    } else {
        ESP_LOGD("discard", "Discarded message from %s, length %d", inet_ntoa(from->sin_addr), len);
    }
}

bool is_discovery_response(const char* data, ssize_t len) {
    if (len < sizeof(lifx_protocol_header_t)) {
        return false;
    }

    const lifx_message_t* message = (const lifx_message_t*) data;
    return message->protocol_header.type == LIFX_STATE_SERVICE_MESSAGE_TYPE;
}

void handle_discovery_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_service_t)) {
        ESP_LOGE("handle_discovery_response", "Response too short to be valid");
        return;
    }

    const lifx_payload_state_service_t* service_payload = (const lifx_payload_state_service_t*)(data + sizeof(lifx_message_t));
    uint32_t port = ntohl(service_payload->port);
    ESP_LOGI("handle_discovery_response", "Device at %s:%lu - Service: %d", inet_ntoa(from->sin_addr), port, service_payload->service);
    
    lifx_device_t **head = NULL; // You need to define this appropriately based on your application's data structures
    add_or_update_device(head, inet_ntoa(from->sin_addr), service_payload->service, port,
                         default_label, default_location, default_location_label, default_location_updated_at,
                         default_echoing, default_unhandled_type, default_hue, default_saturation, default_brightness,
                         default_kelvin, default_light_power, default_light_label, default_relay_powers, default_relay_count);
}

bool is_get_state_response(const char* data, ssize_t len) {
    if (len < sizeof(lifx_message_t)) {
        return false;
    }

    const lifx_message_t* message = (const lifx_message_t*) data;
    uint16_t msg_type = message->protocol_header.type;

    switch (msg_type) {
        case LIFX_PACKET_ACKNOWLEDGEMENT:
        case LIFX_PACKET_STATE_SERVICE:
        case LIFX_PACKET_STATE_POWER:
        case LIFX_PACKET_STATE_LABEL:
        case LIFX_PACKET_STATE_LOCATION:
        case LIFX_PACKET_ECHO_RESPONSE:
        case LIFX_PACKET_STATE_UNHANDLED:
        case LIFX_PACKET_LIGHT_STATE:
        case LIFX_PACKET_STATE_LIGHT_POWER:
        case LIFX_PACKET_STATE_RPOWER:
            return true;
        default:
            return false;
    }
}

void handle_get_state_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    const lifx_message_t* message = (const lifx_message_t*) data;

    switch (message->protocol_header.type) {
        case LIFX_PACKET_STATE_POWER:
            handle_state_power_response(data, len, from);
            break;
        case LIFX_PACKET_STATE_LABEL:
            handle_state_label_response(data, len, from);
            break;
        case LIFX_PACKET_STATE_LOCATION:
            handle_state_location_response(data, len, from);
            break;
        case LIFX_PACKET_LIGHT_STATE:
            handle_light_state_response(data, len, from);
            break;
        case LIFX_PACKET_STATE_LIGHT_POWER:
            handle_state_light_power_response(data, len, from);
            break;
        case LIFX_PACKET_STATE_RPOWER:
            handle_state_rpower_response(data, len, from);
            break;
        default:
            ESP_LOGW("handle_get_state_response", "Unhandled state response type: %d", message->protocol_header.type);
            break;
    }
}

void handle_state_power_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_power_t)) {
        ESP_LOGE("state_power", "Invalid message length");
        return;
    }
    const lifx_payload_state_power_t* payload = (const lifx_payload_state_power_t*)(data + sizeof(lifx_message_t));
    
    // Retrieve IP address and log the information
    char* ip_str = log_device_info(from, payload->level);
    if (ip_str != NULL) {
        lifx_device_t *device = add_or_update_device(&head, ip_str, from->sin_port, payload->service, payload->level,
                                                 payload->label, payload->location, payload->location_label,
                                                 payload->location_updated_at, "", 0, 0, 0, 0, 0, "", payload->relay_powers, 4);
        free(ip_str);  // Free the allocated IP string when done
    }
}

void handle_state_label_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_label_t)) {
        ESP_LOGE("state_label", "Invalid message length");
        return;
    }
    const lifx_payload_state_label_t* payload = (const lifx_payload_state_label_t*)(data + sizeof(lifx_message_t));
    ESP_LOGI("state_label", "Label: %s", payload->label);
}

void handle_state_location_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_location_t)) {
        ESP_LOGE("state_location", "Invalid message length");
        return;
    }
    const lifx_payload_state_location_t* payload = (const lifx_payload_state_location_t*)(data + sizeof(lifx_message_t));
    char updated_at_str[32];
    strftime(updated_at_str, sizeof(updated_at_str), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&payload->updated_at));
    ESP_LOGI("state_location", "Location UUID: %s, Label: %s, Updated At: %s", payload->location, payload->label, updated_at_str);

    // Convert IP to string for identification and storing
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, ip_str, INET_ADDRSTRLEN);

    // Convert UUID to a string representation for easier handling
    char location_uuid_str[33];
    for (int i = 0; i < 16; ++i) {
        sprintf(&location_uuid_str[i * 2], "%02x", payload->location[i]);
    }

    // Update the linked list with this information
    lifx_device_t *device = add_or_update_device(&head, ip_str, from->sin_port, payload->service, payload->power_level,
                                                 payload->label, location_uuid_str, payload->label,
                                                 payload->updated_at, "", 0, 0, 0, 0, 0, "", payload->relay_powers, 4);
}

void handle_light_state_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_light_state_t)) {
        ESP_LOGE("light_state", "Invalid message length");
        return;
    }
    const lifx_payload_light_state_t* payload = (const lifx_payload_light_state_t*)(data + sizeof(lifx_message_t));
    ESP_LOGI("light_state", "Hue: %d, Saturation: %d, Brightness: %d, Kelvin: %d", payload->hue, payload->saturation, payload->brightness, payload->kelvin);

    // Convert IP to string for identification and storing
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, ip_str, INET_ADDRSTRLEN);

    // Update the linked list with this information
    lifx_device_t *device = add_or_update_device(&head, ip_str, from->sin_port, payload->service, payload->power_level,
                                                 payload->label, payload->location, payload->location_label,
                                                 payload->location_updated_at, "", 0, payload->hue, payload->saturation,
                                                 payload->brightness, payload->kelvin, "", payload->relay_powers, 4);
}

void handle_state_light_power_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_light_power_t)) {
        ESP_LOGE("state_light_power", "Invalid message length");
        return;
    }
    const lifx_payload_state_light_power_t* payload = (const lifx_payload_state_light_power_t*)(data + sizeof(lifx_message_t));
    ESP_LOGI("state_light_power", "Light Power: %d", payload->level);

    // Convert IP to string for identification and storing
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, ip_str, INET_ADDRSTRLEN);

    // Update the linked list with this information

    lifx_device_t *device = add_or_update_device(&head, ip_str, from->sin_port, payload->service, payload->level,
                                                 payload->label, payload->location, payload->location_label,
                                                 payload->location_updated_at, "", 0, 0, 0, 0, 0, "", payload->relay_powers, 4);
}

void handle_state_rpower_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_rpower_t)) {
        ESP_LOGE("state_rpower", "Invalid message length");
        return;
    }
    const lifx_payload_state_rpower_t* payload = (const lifx_payload_state_rpower_t*)(data + sizeof(lifx_message_t));
    ESP_LOGI("state_rpower", "Relay Power: %d", payload->level);

    // Convert IP to string for identification and storing
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from->sin_addr, ip_str, INET_ADDRSTRLEN);

    // Update the linked list with this information
    lifx_device_t *device = add_or_update_device(&head, ip_str, from->sin_port, payload->service, payload->level,
                                                 payload->label, payload->location, payload->location_label,
                                                 payload->location_updated_at, "", 0, 0, 0, 0, 0, "", payload->relay_powers, 4);
}

char* log_device_info(const struct sockaddr_in* from, int level) {
    char* ip_str = malloc(INET_ADDRSTRLEN);
    if (ip_str == NULL) {
        ESP_LOGE("log_device_info", "Failed to allocate memory for IP string");
        return NULL;
    }
    
    inet_ntop(AF_INET, &from->sin_addr, ip_str, INET_ADDRSTRLEN);
    ESP_LOGI("Device Info", "Device at %s reports level: %d", ip_str, level);
    return ip_str;  // Return the allocated IP string
}