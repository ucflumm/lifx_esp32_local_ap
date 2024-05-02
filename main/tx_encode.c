#include "include/main.h"
#include "include/lifx_struct_def.h"

lifx_payload_set_power_t* build_set_power_payload(uint16_t power, uint32_t duration) {
    lifx_payload_set_power_t* payload = (lifx_payload_set_power_t*) malloc(sizeof(lifx_payload_set_power_t));
    if (payload != NULL) {
        payload->level = power ? 65535 : 0;  // 65535 for ON, 0 for OFF
        payload->duration = duration;
    }
    return payload;
}

lifx_message_t* build_lifx_message(uint8_t *target, uint16_t payload_size, void *payload, lifx_message_sender_t sender, int sock, struct sockaddr_in *dest) {
    size_t total_size = sizeof(lifx_message_t) + payload_size;
    lifx_message_t *message = (lifx_message_t *)malloc(total_size);

    // Initialize and populate the message fields
    memset(message, 0, total_size);
    message->frame.size = total_size;
    message->frame.protocol = 1024;
    message->frame.addressable = 1;
    message->frame.tagged = 0;
    message->frame.source = rand();

    memcpy(message->frame_address.target, target, 8);
    message->frame_address.ack_required = 1;
    message->frame_address.sequence = rand();

    message->protocol_header.type = MSG_SET_POWER;  // This can be parameterized as needed

    // Attach the payload
    if (payload != NULL && payload_size > 0) {
        memcpy(message->payload, payload, payload_size);
    }

    // If a sender function is provided, call it
    if (sender != NULL) {
        sender(sock, dest, message);
    }

    return message;
}

// void send_discovery_message() {
//     int sock = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sock < 0) {
//         printf("Failed to create socket\n");
//         return;
//     }

//     // Allow broadcast
//     int broadcast = 1;
//     setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

//     struct sockaddr_in addr;
//     memset(&addr, 0, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(LIFX_PORT);
//     inet_pton(AF_INET, BROADCAST_IP, &addr.sin_addr);

//     lifx_message_t message;
//     memset(&message, 0, sizeof(message));
//     message.frame.size = htons(sizeof(message));
//     message.frame.protocol = htons(1024);
//     message.frame.addressable = 1;
//     message.frame.tagged = 1;
//     message.frame.origin = 0;
//     message.frame.source = htonl(1);
//     message.protocol_header.type = htons(2);  // MessageType: GetService

//     if (sendto(sock, &message, sizeof(message), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//         printf("Failed to send message\n");
//     }

//     close(sock);
//     vTaskDelete(NULL);
// }