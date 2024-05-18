#include <stdio.h>

#include "include/main.h"
#include "simple_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/wifi_cred.h"
#include "include/lifx_struct_def.h"

#include "esp_system.h"

void app_main(void)
{
    wifi_manager_init(SSID, PASSWORD);

    xTaskCreate(console_task, "console_task", 4096, NULL, 9, NULL);
    xTaskCreate(send_discovery_message, "send_discovery", 2048, NULL, 10, NULL);
    xTaskCreate(receive_responses, "receive_responses", 4096, NULL, 5, NULL);
}