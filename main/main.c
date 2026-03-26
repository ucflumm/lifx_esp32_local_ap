#include <stdio.h>

#include "include/main.h"
#include "simple_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "include/wifi_cred.h"
#include "include/lifx_struct_def.h"

#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "main";

SemaphoreHandle_t device_list_mutex = NULL;

void app_main(void)
{
    device_list_mutex = xSemaphoreCreateMutex();
    if (device_list_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create device list mutex — aborting");
        return;
    }

    wifi_manager_init(SSID, PASSWORD);

    xTaskCreate(console_task,          "console_task",      4096, NULL, 9,  NULL);
    xTaskCreate(send_discovery_message,"send_discovery",    2048, NULL, 10, NULL);
    xTaskCreate(receive_responses,     "receive_responses", 4096, NULL, 5,  NULL);
}