#include "include/main.h"
#include "include/lifx_struct_def.h"

void receive_responses() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LIFX_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    while (1) {
        char buffer[1024]; // Buffer large enough for the expected messages
        lifx_message_t *response;
        struct sockaddr_in src_addr;
        socklen_t src_addr_len = sizeof(src_addr);
        int len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&src_addr, &src_addr_len);

        if (len > 0) {
            printf("Raw data: ");
            for (int i = 0; i < len; i++) {
                printf("%02x ", (unsigned char)buffer[i]);
            }
            printf("\n");
            response = (lifx_message_t *)buffer;
            printf("Received a packet from %s\n", inet_ntoa(src_addr.sin_addr));
            printf("Message Type: %d\n", ntohs(response->protocol_header.type));

            MessageData data;
            memset(&data, 0, sizeof(data)); // Clear the data union

            switch (ntohs(response->protocol_header.type)) {
                case 512: // StateService
                    {
                        lifx_state_service_t *service_info = (lifx_state_service_t *)response->payload;
                        printf("Service: %d, Port: %u\n", service_info->service, ntohs(service_info->port));
                        data.service.service = service_info->service;
                        data.service.port = ntohs(service_info->port);
                        handle_message(src_addr.sin_addr, &data, TYPE_SERVICE);
                    }
                    break;
                // Add cases for other expected message types
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    close(sock);
}