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
        lifx_message_t response;
        struct sockaddr_in src_addr;
        socklen_t src_addr_len = sizeof(src_addr);
        int len = recvfrom(sock, &response, sizeof(response), 0, (struct sockaddr *)&src_addr, &src_addr_len);
        if (len > 0) {
            printf("Received a packet from %s\n", inet_ntoa(src_addr.sin_addr));
            printf("Message Type: %d\n", ntohs(response.protocol_header.type));

            if (ntohs(response.protocol_header.type) == 512) { // StateService
                lifx_state_service_t* service_info = (lifx_state_service_t*)response.payload;
                printf("Service: %d, Port: %u\n", service_info->service, ntohs(service_info->port));
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    close(sock);
}