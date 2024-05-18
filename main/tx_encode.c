#include "include/main.h"
#include "include/lifx_struct_def.h"
#include "esp_log.h"

static const char* TAG = "tx_encode";
static uint8_t seq_counter = 0;

void send_discovery_message(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while (1) {
        
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            printf("Failed to create socket\n");
            return;
        }

        ESP_LOGI(TAG, "Socket created in tx\n");

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
        message.frame.size = sizeof(message);
        message.frame.protocol = 1024;
        message.frame.addressable = 1;
        message.frame.tagged = 1;
        message.frame.origin = 0;
        message.frame.source = 4096;
        memset(&message.frame_address.target, 0, sizeof(message.frame_address.target));
        message.frame_address.res_required = 0;
        message.frame_address.ack_required = 1;
        if (seq_counter == 255) seq_counter = 0; // Reset sequence number (8-bit)
        message.frame_address.sequence = seq_counter++;
        message.protocol_header.type = 2;
          // MessageType: GetService
        
        log_hex(&message, sizeof(message));

        if (sendto(sock, &message, sizeof(message), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            printf("Failed to send message\n");
        } else {
            ESP_LOGI(TAG, "Sent discovery message\n");
        }

        close(sock);
        vTaskDelayUntil(&xLastWakeTime, 30000 / portTICK_PERIOD_MS);
    }
    
}

void log_hex(const void* data, size_t data_len) {
    const unsigned char* byte_data = (const unsigned char*)data;
    printf("Raw data: ");
    for (size_t i = 0; i < data_len; ++i) {
        printf("%02X ", byte_data[i]);
    }
    printf("\n");
}
