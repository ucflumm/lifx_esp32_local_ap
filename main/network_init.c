#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>  // For close()

// ESP-IDF specific headers may be needed for error logging or task handling
#include "esp_log.h"
#include "esp_err.h"

int init_udp_socket(bool enable_broadcast, uint16_t port) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE("udp_socket", "Failed to create socket: errno %d", errno);
        return -1;
    }

    if (enable_broadcast) {
        int broadcast = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
            ESP_LOGE("udp_socket", "Failed to set broadcast: errno %d", errno);
            close(sock);
            return -1;
        }
    }

    // Optionally bind to a specific local port if necessary
    if (port != 0) {
        struct sockaddr_in local_addr;
        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(port);
        local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
            ESP_LOGE("udp_socket", "Failed to bind socket: errno %d", errno);
            close(sock);
            return -1;
        }
    }

    return sock;
}