#include <stdio.h>

#include "include/main.h"
#include "simple_wifi.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

void app_main(void)
{
    initialize_console();
    register_commands();
    wifi_manager_init(SSID, PASSWORD);
    // Initialize the UDP socket for broadcasting and listening
    int sock = init_udp_socket(true, 56700); // Enable broadcast on port 56700
    if (sock < 0) {
        ESP_LOGE("app_main", "Failed to initialize UDP socket");
        return; // Early return if socket initialization fails
    }

    // Start the task that listens for incoming messages
    xTaskCreate(listen_for_messages, "ListenTask", 2048, &sock, 5, NULL);

    // Main task can either be terminated or continue to perform other duties
    // For simplicity, we might just end the main task here. The listening task will continue running.
}