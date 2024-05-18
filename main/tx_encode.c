#include "include/main.h"
#include "include/lifx_struct_def.h"
#include "esp_log.h"

void send_discovery_message() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("Failed to create socket\n");
        return;
    }

    // Allow broadcast
    int broadcast = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LIFX_PORT);
    inet_pton(AF_INET, BROADCAST_IP, &addr.sin_addr);

    lifx_message_t message;
    memset(&message, 0, sizeof(message));
    message.frame.size = htons(sizeof(message));
    message.frame.protocol = htons(1024);
    message.frame.addressable = 1;
    message.frame.tagged = 1;
    message.frame.origin = 0;
    message.frame.source = htonl(1);
    message.protocol_header.type = htons(2);  // MessageType: GetService

    ESP_LOGI("TX_ENCODE", "Sending discovery message");
    ESP_LOGI("TX_ENCODE", "Message size: %d", sizeof(message));
    ESP_LOGI("TX_ENCODE", "Message protocol: %d", ntohs(message.frame.protocol));
    ESP_LOGI("TX_ENCODE", "Message addressable: %d", message.frame.addressable);
    ESP_LOGI("TX_ENCODE", "Message tagged: %d", message.frame.tagged);
    ESP_LOGI("TX_ENCODE", "Message origin: %d", message.frame.origin);
    ESP_LOGI("TX_ENCODE", "Message source: %d", ntohl(message.frame.source));
    ESP_LOGI("TX_ENCODE", "Message type: %d", ntohs(message.protocol_header.type));
    ESP_LOGI("TX_ENCODE", "Message payload: %s", message.payload);
    

    if (sendto(sock, &message, sizeof(message), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Failed to send message\n");
    }

    close(sock);
    vTaskDelete(NULL);
}