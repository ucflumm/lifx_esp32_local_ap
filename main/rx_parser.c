#include "include/main.h"
#include "include/lifx_struct_def.h"

void listen_for_messages(int sock) {
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    char buf[1024];  // Buffer size depends on expected message size

    while (true) {
        ssize_t len = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &from_len);
        if (len > 0) {
            parse_and_handle_message(buf, len, &from);
        } else if (len == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            ESP_LOGE("udp_rx", "recvfrom failed: errno %d", errno);
            // Handle error or exit
            break;
        }
        // Optionally, include a non-blocking delay or yield here if needed
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void parse_and_handle_message(const char* data, ssize_t len, const struct sockaddr_in* from) {
    // Example: Check if this is a discovery response
    if (is_discovery_response(data, len)) {
        handle_discovery_response(data, len, from);
    } else if (is_get_state_response(data, len)) {
        handle_get_state_response(data, len);
    }
}

bool is_discovery_response(const char* data, ssize_t len) {
    if (len < sizeof(lifx_protocol_header_t)) {
        return false;  // Not enough data to contain a valid LIFX message
    }

    // Assume the protocol header directly follows the frame and frame address
    const lifx_message_t* message = (const lifx_message_t*) data;

    // Check if the message type in the protocol header is for a StateService response
    return message->protocol_header.type == LIFX_STATE_SERVICE_MESSAGE_TYPE;
}

void handle_discovery_response(const char* data, ssize_t len, const struct sockaddr_in* from) {
    if (len < sizeof(lifx_message_t) + sizeof(lifx_payload_state_service_t)) {
        ESP_LOGE("handle_discovery_response", "Response too short to be valid");
        return;
    }

    const lifx_message_t* message = (const lifx_message_t*)data;
    const lifx_payload_state_service_t* service_payload = (const lifx_payload_state_service_t*)(message->payload);

    // Assuming the first byte after the protocol header is the service payload
    uint8_t service = service_payload->service;
    uint32_t port = ntohl(service_payload->port);  // Ensure network to host long conversion

    ESP_LOGI("handle_discovery_response", "Device at %s:%d - Service: %u", inet_ntoa(from->sin_addr), port, service);

    // Here you might store the device's info for later use
    // For instance, add the device to a list or database
    add_device_to_list(inet_ntoa(from->sin_addr), port, service);
}

void add_device_to_list(const char* ip, uint32_t port, uint8_t service) {
    // Add logic to store or update the device info in a list or database
    // This could involve dynamic memory allocation and list management
    ESP_LOGI("add_device_to_list", "Adding device with IP %s and port %d", ip, port);
}

bool is_get_state_response(const char* data, ssize_t len) {
    // Implement based on expected message format
    return true;  // Placeholder
}

void handle_get_state_response(const char* data, ssize_t len) {
    // Extract state info from the message
    ESP_LOGI("state", "Received state information");
    // Further processing such as updating state in the application
}

// void receive_responses() {
//     int sock = socket(AF_INET, SOCK_DGRAM, 0);
//     struct sockaddr_in addr;
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(LIFX_PORT);
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);

//     bind(sock, (struct sockaddr *)&addr, sizeof(addr));

//     while (1) {
//         lifx_message_t response;
//         struct sockaddr_in src_addr;
//         socklen_t src_addr_len = sizeof(src_addr);
//         int len = recvfrom(sock, &response, sizeof(response), 0, (struct sockaddr *)&src_addr, &src_addr_len);
//         if (len > 0) {
//             printf("Received a packet from %s\n", inet_ntoa(src_addr.sin_addr));
//             printf("Message Type: %d\n", ntohs(response.protocol_header.type));

//             if (ntohs(response.protocol_header.type) == 512) { // StateService
//                 lifx_state_service_t* service_info = (lifx_state_service_t*)response.payload;
//                 printf("Service: %d, Port: %u\n", service_info->service, ntohs(service_info->port));
//             }
//         }
//     }

//     close(sock);
// }